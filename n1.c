#include <stdio.h>
#include <string.h>

#define MAX_CUSTOMERS 100

typedef struct {
    char name[50];
    char phone[15];
    char address[100];
    char email[50];
    char carModel[50];
    float budget;
} Customer;

Customer customers[MAX_CUSTOMERS];
int customerCount = 0;

void addCustomer() {
    if (customerCount >= MAX_CUSTOMERS) {
        printf("Customer database is full! Cannot add more customers.\n");
        return;
    }

    Customer newCustomer;
    printf("Enter customer name: ");
    scanf(" %[^\n]", newCustomer.name);  // Accepts full name with spaces
    printf("Enter phone number: ");
    scanf("%s", newCustomer.phone);
    printf("Enter address: ");
    scanf(" %[^\n]", newCustomer.address);  // Accepts address with spaces
    printf("Enter email: ");
    scanf("%s", newCustomer.email);
    printf("Enter desired car model: ");
    scanf(" %[^\n]", newCustomer.carModel);
    printf("Enter budget: ");
    scanf("%f", &newCustomer.budget);

    customers[customerCount++] = newCustomer;
    printf("Customer details added successfully!\n");
}

void viewCustomers() {
    if (customerCount == 0) {
        printf("No customer details available.\n");
        return;
    }

    printf("\nCustomer Details:\n");
    for (int i = 0; i < customerCount; i++) {
        printf("\nCustomer %d:\n", i + 1);
        printf("Name: %s\n", customers[i].name);
        printf("Phone: %s\n", customers[i].phone);
        printf("Address: %s\n", customers[i].address);
        printf("Email: %s\n", customers[i].email);
        printf("Desired Car Model: %s\n", customers[i].carModel);
        printf("Budget: $%.2f\n", customers[i].budget);
    }
}

int main() {
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
                addCustomer();
                break;
            case 2:
                viewCustomers();
                break;
            case 3:
                printf("Exiting the system. Goodbye!\n");
                break;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 3);

    return 0;
}