#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libpq-fe.h>

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
        printf("Enter Vehicle Height (in meters): ");
        scanf("%lf", &height);
        printf("Enter Vehicle Width (in meters): ");
        scanf("%lf", &width);
        printf("Enter Vehicle Color: ");
        scanf(" %[^\n]", color);
        printf("Enter Vehicle Price: ");
        scanf("%lf", &price);

        char query[512];
        snprintf(query, sizeof(query),
                 "INSERT INTO vehicles (name, type, height, width, color, price) "
                 "VALUES ('%s', '%s', %lf, %lf, '%s', %lf)",
                 name, type, height, width, color, price);
        PGresult *res = PQexec(conn, query);

        if (PQresultStatus(res) != PGRES_COMMAND_OK) {
            fprintf(stderr, "Error adding vehicle: %s\n", PQerrorMessage(conn));
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
        char name[100], type[50], color[50];
        double price, height, width;

        printf("Enter Vehicle ID to update: ");
        scanf("%d", &vehicle_id);
        printf("Enter New Vehicle Name: ");
        scanf(" %[^\n]", name);
        printf("Enter New Vehicle Type: ");
        scanf(" %[^\n]", type);
        printf("Enter New Vehicle Height (in meters): ");
        scanf("%lf", &height);
        printf("Enter New Vehicle Width (in meters): ");
        scanf("%lf", &width);
        printf("Enter New Vehicle Color: ");
        scanf(" %[^\n]", color);
        printf("Enter New Vehicle Price: ");
        scanf("%lf", &price);

        char query[512];
        snprintf(query, sizeof(query),
                 "UPDATE vehicles SET name = '%s', type = '%s', height = %lf, width = %lf, color = '%s', price = %lf WHERE vehicle_id = %d",
                 name, type, height, width, color, price, vehicle_id);
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
    PGconn *conn = PQconnectdb("user=postgres dbname=automobile_db password=yourpassword");
    if (PQstatus(conn) != CONNECTION_OK) {
        exit_with_error(conn);
    }

    int choice;
    do {
        printf("\nAutomobile Management System\n");
        printf("1. Vehicle\n2. Customer\n3. Sales\n4. Help\n5. Exit\nChoice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                vehicle_management(conn);
                break;
            case 3:
                manage_sales(conn);
                break;
            case 4:
                printf("Help: This is the manual.\n");
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
