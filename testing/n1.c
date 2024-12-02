#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <libpq-fe.h>

void view_vehicle_details(PGconn *conn, int vehicle_id);
void book_vehicle(PGconn *conn, int vehicle_id);
void show_vehicles(PGconn *conn);
void manage_sales(PGconn *conn);
void vehicle_management(PGconn *conn);
void manage_customers(PGconn *conn);

void exit_with_error(PGconn *conn) {
    fprintf(stderr, "Error: %s\n", PQerrorMessage(conn));
    PQfinish(conn);
    exit(1);
}

void show_vehicles(PGconn *conn) {
    PGresult *res = PQexec(conn, "SELECT vehicle_id, name, type, color, price FROM vehicles");
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "Error fetching vehicles: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return;
    }

    printf("Available Vehicles:\n");
    for (int i = 0; i < PQntuples(res); i++) {
        printf("%s. %s | Type: %s | Color: %s | Price: %s\n",
               PQgetvalue(res, i, 0), PQgetvalue(res, i, 1), PQgetvalue(res, i, 2),
               PQgetvalue(res, i, 3), PQgetvalue(res, i, 4));
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
        printf("Details:\nName: %s\nType: %s\nColor: %s\nPrice: %s\nHeight: %s\nWidth: %s\n",
               PQgetvalue(res, 0, 1), PQgetvalue(res, 0, 2), PQgetvalue(res, 0, 3),
               PQgetvalue(res, 0, 4), PQgetvalue(res, 0, 5), PQgetvalue(res, 0, 6));
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

    // Validate phone number
    for (int i = 0; i < strlen(phone); i++) {
        if (!isdigit(phone[i])) {
            printf("Error: Phone number must contain only digits.\n");
            return;
        }
    }

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

    if (amount_paid <= 0) {
        printf("Error: Amount paid must be positive.\n");
        return;
    }

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
        char name[100], type[50], color[50];
        double price, height, width;

        printf("Enter Vehicle Name: ");
        scanf(" %[^\n]", name);
        printf("Enter Vehicle Type: ");
        scanf(" %[^\n]", type);
        printf("Enter Color: ");
        scanf(" %[^\n]", color);
        printf("Enter Price: ");
        scanf("%lf", &price);
        printf("Enter Height: ");
        scanf("%lf", &height);
        printf("Enter Width: ");
        scanf("%lf", &width);

        if (price <= 0 || height <= 0 || width <= 0) {
            printf("Error: Price, height, and width must be positive.\n");
            return;
        }

        char query[512];
        snprintf(query, sizeof(query),
                 "INSERT INTO vehicles (name, type, color, price, height, width) "
                 "VALUES ('%s', '%s', '%s', %lf, %lf, %lf)",
                 name, type, color, price, height, width);

        PGresult *res = PQexec(conn, query);
        if (PQresultStatus(res) != PGRES_COMMAND_OK) {
            fprintf(stderr, "Error inserting vehicle: %s\n", PQerrorMessage(conn));
            PQclear(res);
            return;
        }

        printf("Vehicle added successfully!\n");
        PQclear(res);
    } else if (choice == 2) {
        show_vehicles(conn);
    } else if (choice == 3) {
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
        int vehicle_id;
        printf("Enter Vehicle ID to update: ");
        scanf("%d", &vehicle_id);

        char name[100], type[50], color[50];
        double price, height, width;

        printf("Enter new Name (or '-' to skip): ");
        scanf(" %[^\n]", name);
        printf("Enter new Type (or '-' to skip): ");
        scanf(" %[^\n]", type);
        printf("Enter new Color (or '-' to skip): ");
        scanf(" %[^\n]", color);
        printf("Enter new Price (or -1 to skip): ");
        scanf("%lf", &price);
        printf("Enter new Height (or -1 to skip): ");
        scanf("%lf", &height);
        printf("Enter new Width (or -1 to skip): ");
        scanf("%lf", &width);

        char query[512];
        snprintf(query, sizeof(query),
                 "UPDATE vehicles SET "
                 "name = CASE WHEN '%s' <> '-' THEN '%s' ELSE name END, "
                 "type = CASE WHEN '%s' <> '-' THEN '%s' ELSE type END, "
                 "color = CASE WHEN '%s' <> '-' THEN '%s' ELSE color END, "
                 "price = CASE WHEN %lf > 0 THEN %lf ELSE price END, "
                 "height = CASE WHEN %lf > 0 THEN %lf ELSE height END, "
                 "width = CASE WHEN %lf > 0 THEN %lf ELSE width END "
                 "WHERE vehicle_id = %d",
                 name, name, type, type, color, color, price, price, height, height, width, width, vehicle_id);

        PGresult *res = PQexec(conn, query);
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
    PGconn *conn = PQconnectdb("dbname=automobile_db user=postgres password=your_password");
    if (PQstatus(conn) == CONNECTION_BAD) {
        exit_with_error(conn);
    }

    int choice;
    do {
        printf("\nVehicle Management System:\n");
        printf("1. View Vehicles\n");
        printf("2. Manage Sales\n");
        printf("3. Manage Vehicles\n");
        printf("4. Manage Customers\n");
        printf("5. Exit\n");
        printf("Choice: ");
        scanf("%d", &choice);

        switch (choice) {
        case 1:
            show_vehicles(conn);
            break;
        case 2:
            manage_sales(conn);
            break;
        case 3:
            vehicle_management(conn);
            break;
        case 4:
            manage_customers(conn);
            break;
        case 5:
            printf("Exiting...\n");
            break;
        default:
            printf("Invalid choice. Try again.\n");
        }
    } while (choice != 5);

    PQfinish(conn);
    return 0;
}
