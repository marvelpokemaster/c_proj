#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libpq-fe.h>

void view_vehicle_details(PGconn *conn, int vehicle_id);
void book_vehicle(PGconn *conn, int vehicle_id);
void show_vehicles(PGconn *conn);
void manage_sales(PGconn *conn);
void vehicle_management(PGconn *conn);
void manage_customers(PGconn *conn);  // Function prototype

void exit_with_error(PGconn *conn) {
    fprintf(stderr, "Error: %s\n", PQerrorMessage(conn));
    PQfinish(conn);
    exit(1);
}

void show_vehicles(PGconn *conn) {
    PGresult *res = PQexec(conn, "SELECT vehicle_id, name FROM vehicles");
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "Error fetching vehicles: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return;
    }

    printf("Available Vehicles:\n");
    for (int i = 0; i < PQntuples(res); i++) {
        printf("%s. %s\n", PQgetvalue(res, i, 0), PQgetvalue(res, i, 1));
    }

    PQclear(res);

    int proceed;
    printf("\nWould you like to book a vehicle? (1 for Yes, 0 for No): ");
    scanf("%d", &proceed);

    if (proceed == 1) {
        int vehicle_id;
        printf("Enter the Vehicle ID to book: ");
        scanf("%d", &vehicle_id);
        view_vehicle_details(conn, vehicle_id);

        char confirm;
        printf("Do you want to proceed with booking this vehicle? (y/n): ");
        scanf(" %c", &confirm);

        if (confirm == 'y' || confirm == 'Y') {
            book_vehicle(conn, vehicle_id);
        } else {
            printf("Booking canceled.\n");
        }
    }
}

void view_vehicle_details(PGconn *conn, int vehicle_id) {
    char query[256];
    snprintf(query, sizeof(query), "SELECT * FROM vehicles WHERE vehicle_id = %d", vehicle_id);
    PGresult *res = PQexec(conn, query);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "Error fetching vehicle details: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return;
    }

    if (PQntuples(res) == 0) {
        printf("No details found for vehicle ID %d.\n", vehicle_id);
    } else {
        printf("Details:\nName: %s\nDetails: %s\nPrice: %s\n",
               PQgetvalue(res, 0, 1), PQgetvalue(res, 0, 2), PQgetvalue(res, 0, 3));
    }

    PQclear(res);
}

void book_vehicle(PGconn *conn, int vehicle_id) {
    char name[100], address[200], phone[15], query[512];
    double amount_paid;
    char full_payment_choice;

    printf("Enter Customer Details:\n");
    printf("Name: ");
    scanf(" %[^\n]", name);
    printf("Address: ");
    scanf(" %[^\n]", address);
    printf("Phone: ");
    scanf(" %s", phone);

    snprintf(query, sizeof(query),
             "INSERT INTO customers (name, address, phone) VALUES ('%s', '%s', '%s') RETURNING customer_id",
             name, address, phone);

    PGresult *res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_COMMAND_OK && PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "Error inserting customer: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return;
    }

    int customer_id = atoi(PQgetvalue(res, 0, 0));
    PQclear(res);

    printf("Enter Amount Paid: ");
    scanf("%lf", &amount_paid);
    printf("Full Payment? (y/n): ");
    scanf(" %c", &full_payment_choice);

    snprintf(query, sizeof(query),
             "INSERT INTO sales (vehicle_id, customer_id, amount_paid, full_payment) "
             "VALUES (%d, %d, %lf, %s)",
             vehicle_id, customer_id, amount_paid, (full_payment_choice == 'y' ? "TRUE" : "FALSE"));

    res = PQexec(conn, query);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "Error creating sale: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return;
    }

    printf("Vehicle booked successfully!\n");
    PQclear(res);
}

void manage_sales(PGconn *conn) {
    printf("Sales Management:\n1. View Sales\n2. Delete Sale\nChoice: ");
    int choice;
    scanf("%d", &choice);

    if (choice == 1) {
        PGresult *res = PQexec(conn, "SELECT * FROM sales");
        if (PQresultStatus(res) != PGRES_TUPLES_OK) {
            fprintf(stderr, "Error fetching sales: %s\n", PQerrorMessage(conn));
            PQclear(res);
            return;
        }

        printf("Sales Records:\n");
        for (int i = 0; i < PQntuples(res); i++) {
            printf("Sale ID: %s | Vehicle ID: %s | Customer ID: %s | Amount Paid: %s | Full Payment: %s\n",
                   PQgetvalue(res, i, 0), PQgetvalue(res, i, 1), PQgetvalue(res, i, 2),
                   PQgetvalue(res, i, 3), PQgetvalue(res, i, 4));
        }

        PQclear(res);
    } else if (choice == 2) {
        int sale_id;
        printf("Enter Sale ID to delete: ");
        scanf("%d", &sale_id);

        char query[256];
        snprintf(query, sizeof(query), "DELETE FROM sales WHERE sale_id = %d", sale_id);
        PGresult *res = PQexec(conn, query);

        if (PQresultStatus(res) != PGRES_COMMAND_OK) {
            fprintf(stderr, "Error deleting sale: %s\n", PQerrorMessage(conn));
            PQclear(res);
            return;
        }

        printf("Sale deleted successfully!\n");
        PQclear(res);
    }
}

void manage_customers(PGconn *conn) {
    printf("Customer Management:\n1. View Customers\n2. Delete Customer\nChoice: ");
    int choice;
    scanf("%d", &choice);

    if (choice == 1) {
        PGresult *res = PQexec(conn, "SELECT * FROM customers");
        if (PQresultStatus(res) != PGRES_TUPLES_OK) {
            fprintf(stderr, "Error fetching customers: %s\n", PQerrorMessage(conn));
            PQclear(res);
            return;
        }

        printf("Customer Records:\n");
        for (int i = 0; i < PQntuples(res); i++) {
            printf("Customer ID: %s | Name: %s | Address: %s | Phone: %s\n",
                   PQgetvalue(res, i, 0), PQgetvalue(res, i, 1), PQgetvalue(res, i, 2),
                   PQgetvalue(res, i, 3));
        }

        PQclear(res);
    } else if (choice == 2) {
        int customer_id;
        printf("Enter Customer ID to delete: ");
        scanf("%d", &customer_id);

        char query[256];
        snprintf(query, sizeof(query), "DELETE FROM customers WHERE customer_id = %d", customer_id);
        PGresult *res = PQexec(conn, query);

        if (PQresultStatus(res) != PGRES_COMMAND_OK) {
            fprintf(stderr, "Error deleting customer: %s\n", PQerrorMessage(conn));
            PQclear(res);
            return;
        }

        printf("Customer deleted successfully!\n");
        PQclear(res);
    }
}

void vehicle_management(PGconn *conn) {
    printf("Vehicle Management:\n1. Add Vehicle\n2. View Vehicles\n3. Delete Vehicle\n4. Update Vehicle\n5. Exit\nChoice: ");
    int choice;
    scanf("%d", &choice);

    if (choice == 1) {
        // Add Vehicle logic
        char name[100], type[50], color[50];
        double price, height, width;

        printf("Enter Vehicle Name: ");
        scanf(" %[^\n]", name);
        printf("Enter Vehicle Type: ");
        scanf(" %[^\n]", type);
        printf("Enter Vehicle Color: ");
        scanf(" %[^\n]", color);
        printf("Enter Price: ");
        scanf("%lf", &price);
        printf("Enter Vehicle Height: ");
        scanf("%lf", &height);
        printf("Enter Vehicle Width: ");
        scanf("%lf", &width);

        char query[512];
        snprintf(query, sizeof(query),
                 "INSERT INTO vehicles (name, type, color, price, height, width) "
                 "VALUES ('%s', '%s', '%s', %lf, %lf, %lf)",
                 name, type, color, price, height, width);

        PGresult *res = PQexec(conn, query);
        if (PQresultStatus(res) != PGRES_COMMAND_OK) {
            fprintf(stderr, "Error adding vehicle: %s\n", PQerrorMessage(conn));
            PQclear(res);
            return;
        }

        printf("Vehicle added successfully!\n");
        PQclear(res);
    } else if (choice == 2) {
        // View Vehicles logic
        show_vehicles(conn);
    } else if (choice == 3) {
        // Delete Vehicle logic
        int vehicle_id;
        printf("Enter Vehicle ID to delete: ");
        scanf("%d", &vehicle_id);

        char query[256];
        snprintf(query, sizeof(query), "DELETE FROM vehicles WHERE vehicle_id = %d", vehicle_id);
        PGresult *res = PQexec(conn, query);

        if (PQresultStatus(res) != PGRES_COMMAND_OK) {
            fprintf(stderr, "Error deleting vehicle: %s\n", PQerrorMessage(conn));
            PQclear(res);
            return;
        }

        printf("Vehicle deleted successfully!\n");
        PQclear(res);
    } else if (choice == 4) {
        // Update Vehicle logic
        int vehicle_id;
        printf("Enter Vehicle ID to update: ");
        scanf("%d", &vehicle_id);

        // Get the existing vehicle details
        char query[256];
        snprintf(query, sizeof(query), "SELECT * FROM vehicles WHERE vehicle_id = %d", vehicle_id);
        PGresult *res = PQexec(conn, query);

        if (PQresultStatus(res) != PGRES_TUPLES_OK) {
            fprintf(stderr, "Error fetching vehicle details: %s\n", PQerrorMessage(conn));
            PQclear(res);
            return;
        }

        if (PQntuples(res) == 0) {
            printf("No vehicle found with ID %d.\n", vehicle_id);
            PQclear(res);
            return;
        }

        // Display the current details of the vehicle
        printf("Current details for Vehicle ID %d:\n", vehicle_id);
        printf("Name: %s\n", PQgetvalue(res, 0, 1));
        printf("Type: %s\n", PQgetvalue(res, 0, 2));
        printf("Color: %s\n", PQgetvalue(res, 0, 3));
        printf("Price: %s\n", PQgetvalue(res, 0, 4));
        printf("Height: %s\n", PQgetvalue(res, 0, 5));
        printf("Width: %s\n", PQgetvalue(res, 0, 6));

        // Ask for updated details
        char name[100], type[50], color[50];
        double price, height, width;

        printf("\nEnter new Vehicle Name (leave blank to keep current): ");
        scanf(" %[^\n]", name);
        printf("Enter new Vehicle Type (leave blank to keep current): ");
        scanf(" %[^\n]", type);
        printf("Enter new Vehicle Color (leave blank to keep current): ");
        scanf(" %[^\n]", color);
        printf("Enter new Price (leave blank to keep current): ");
        scanf("%lf", &price);
        printf("Enter new Vehicle Height (leave blank to keep current): ");
        scanf("%lf", &height);
        printf("Enter new Vehicle Width (leave blank to keep current): ");
        scanf("%lf", &width);

        // Construct the update query
        snprintf(query, sizeof(query),
                 "UPDATE vehicles SET "
                 "name = COALESCE(NULLIF('%s', ''), name), "
                 "type = COALESCE(NULLIF('%s', ''), type), "
                 "color = COALESCE(NULLIF('%s', ''), color), "
                 "price = COALESCE(NULLIF(%lf, 0), price), "
                 "height = COALESCE(NULLIF(%lf, 0), height), "
                 "width = COALESCE(NULLIF(%lf, 0), width) "
                 "WHERE vehicle_id = %d",
                 name, type, color, price, height, width, vehicle_id);

        res = PQexec(conn, query);
        if (PQresultStatus(res) != PGRES_COMMAND_OK) {
            fprintf(stderr, "Error updating vehicle: %s\n", PQerrorMessage(conn));
            PQclear(res);
            return;
        }

        printf("Vehicle updated successfully!\n");
        PQclear(res);
    }
}

int main() {
    PGconn *conn = PQconnectdb("user=postgres dbname=automobile_db password=your_password");

    if (PQstatus(conn) != CONNECTION_OK) {
        fprintf(stderr, "Connection to database failed: %s\n", PQerrorMessage(conn));
        PQfinish(conn);
        exit(1);
    }

    int choice;
    do {
        printf("\nMain Menu:\n1. Vehicle Management\n2. Sales Management\n3. Customer Management\n4. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                vehicle_management(conn);
                break;
            case 2:
                manage_sales(conn);
                break;
            case 3:
                manage_customers(conn);
                break;
            case 4:
                break;
            default:
                printf("Invalid choice, please try again.\n");
        }
    } while (choice != 4);

    PQfinish(conn);
    return 0;
}
