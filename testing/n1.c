#include <stdio.h>
#include <stdlib.h>
#include <libpq-fe.h>

void addCustomer(PGconn *conn) {
    char name[50], phone[15], address[100], email[50], carModel[50];
    float budget;

    printf("Enter customer name: ");
    scanf(" %[^\n]", name);
    printf("Enter phone number: ");
    scanf("%s", phone);
    printf("Enter address: ");
    scanf(" %[^\n]", address);
    printf("Enter email: ");
    scanf("%s", email);
    printf("Enter desired car model: ");
    scanf(" %[^\n]", carModel);
    printf("Enter budget: ");
    scanf("%f", &budget);

    char query[512];
    snprintf(query, sizeof(query),
             "INSERT INTO customers (name, phone, address, email, car_model, budget) VALUES ('%s', '%s', '%s', '%s', '%s', %.2f);",
             name, phone, address, email, carModel, budget);

    PGresult *res = PQexec(conn, query);

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "Error inserting customer: %s\n", PQerrorMessage(conn));
    } else {
        printf("Customer added successfully!\n");
    }

    PQclear(res);
}

void viewCustomers(PGconn *conn) {
    PGresult *res = PQexec(conn, "SELECT id, name, phone, address, email, car_model, budget FROM customers;");

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "Error fetching customers: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return;
    }

    int rows = PQntuples(res);

    if (rows == 0) {
        printf("No customer details available.\n");
    } else {
        printf("\nCustomer Details:\n");
        for (int i = 0; i < rows; i++) {
            printf("\nCustomer %s:\n", PQgetvalue(res, i, 0)); // ID
            printf("Name: %s\n", PQgetvalue(res, i, 1));       // Name
            printf("Phone: %s\n", PQgetvalue(res, i, 2));      // Phone
            printf("Address: %s\n", PQgetvalue(res, i, 3));    // Address
            printf("Email: %s\n", PQgetvalue(res, i, 4));      // Email
            printf("Car Model: %s\n", PQgetvalue(res, i, 5));  // Car Model
            printf("Budget: $%s\n", PQgetvalue(res, i, 6));    // Budget
        }
    }

    PQclear(res);
}

int main() {
    const char *conninfo = "dbname=dealership user=postgres password=postgres host=localhost port=5432";

    // Connect to the database
    PGconn *conn = PQconnectdb(conninfo);

    if (PQstatus(conn) != CONNECTION_OK) {
        fprintf(stderr, "Connection to database failed: %s\n", PQerrorMessage(conn));
        PQfinish(conn);
        return 1;
    }

    printf("Connected to the database successfully!\n");

    int choice;
    do {
        printf("\nCar Dealership - Customer Management System\n");
        printf("1. Add Customer Details\n");
        printf("2. View Customer Details\n");
        printf("3. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                addCustomer(conn);
                break;
            case 2:
                viewCustomers(conn);
                break;
            case 3:
                printf("Exiting the system. Goodbye!\n");
                break;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 3);

    PQfinish(conn);
    return 0;
}
