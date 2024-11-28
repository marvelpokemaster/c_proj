#include <stdio.h>
#include <string.h>


#define MAX_CARS 100

typedef struct {
    char model[50];
    char manufacturer[50];
    int year;
    float price;
    int stock;
} Car;

Car inventory[MAX_CARS];
int carCount = 0;

void addCar() {
    if (carCount >= MAX_CARS) {
        printf("Inventory full! Cannot add more cars.\n");
        return;
    }
    Car newCar;
    printf("Enter car model: ");
    scanf("%s", newCar.model);
    printf("Enter car manufacturer: ");
    scanf("%s", newCar.manufacturer);
    printf("Enter car year: ");
    scanf("%d", &newCar.year);
    printf("Enter car price: ");
    scanf("%f", &newCar.price);
    printf("Enter car stock: ");
    scanf("%d", &newCar.stock);

    inventory[carCount++] = newCar;
    printf("Car added successfully!\n");
}

void viewCars() {
    if (carCount == 0) {
        printf("No cars available in the inventory.\n");
        return;
    }
    printf("\nAvailable Cars:\n");
    for (int i = 0; i < carCount; i++) {
        printf("%d. %s %s (%d) - $%.2f [%d in stock]\n", 
               i + 1, inventory[i].manufacturer, inventory[i].model, 
               inventory[i].year, inventory[i].price, inventory[i].stock);
    }
}

void sellCar() {
    if (carCount == 0) {
        printf("No cars available for sale.\n");
        return;
    }
    viewCars();
    int choice;
    printf("Enter the number of the car you want to sell: ");
    scanf("%d", &choice);
    if (choice < 1 || choice > carCount) {
        printf("Invalid choice!\n");
        return;
    }
    if (inventory[choice - 1].stock > 0) {
        inventory[choice - 1].stock--;
        printf("Car sold successfully!\n");
        if (inventory[choice - 1].stock == 0) {
            printf("Stock for this car is now empty.\n");
        }
    } else {
        printf("Sorry, this car is out of stock.\n");
    }
}

int main() {
    int choice;
    do {
        printf("\nAutomobile Dealership System\n");
        printf("1. Add Car\n");
        printf("2. View Cars\n");
        printf("3. Sell Car\n");
        printf("4. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: addCar(); break;
            case 2: viewCars(); break;
            case 3: sellCar(); break;
            case 4: printf("Exiting the system. Goodbye!\n"); break;
            default: printf("Invalid choice. Try again.\n");
        }
    } while (choice != 4);

    return 0;
}
