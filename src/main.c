#include <stdio.h>
#include <string.h>
#include <math.h>

#include "types.h"
#include "consumer.h"
#include "billing.h"
#include "file_handler.h"

static void clearInputBuffer(void) {
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF) {
    }
}

static int readInt(int *value) {
    if (scanf("%d", value) != 1) {
        clearInputBuffer();
        return 0;
    }
    clearInputBuffer();
    return 1;
}

static int readDouble(double *value) {
    if (scanf("%lf", value) != 1) {
        clearInputBuffer();
        return 0;
    }
    clearInputBuffer();

    if (!isfinite(*value)) {
        return 0;
    }
    return 1;
}

static int readName(char name[MAX_NAME_LENGTH + 1]) {
    char input[MAX_NAME_LENGTH + 2];
    size_t length;
    int ch;

    if (fgets(input, sizeof(input), stdin) == NULL) {
        return 0;
    }

    length = strlen(input);

    if (length > 0 && input[length - 1] == '\n') {
        input[length - 1] = '\0';
    } else if (!feof(stdin)) {
        while ((ch = getchar()) != '\n' && ch != EOF) {
        }
        return 0;
    }

    length = strlen(input);

    if (length == 0 || length > MAX_NAME_LENGTH ||
        strchr(input, '|') != NULL ||
        strchr(input, '\r') != NULL) {
        return 0;
    }

    strcpy(name, input);
    return 1;
}

static void displayStatus(StatusCode status) {
    switch (status) {
        case STATUS_OK:
            printf("Operation successful.\n");
            break;
        case STATUS_INVALID_INPUT:
            printf("Invalid input. Please check the entered values.\n");
            break;
        case STATUS_DUPLICATE:
            printf("This record already exists.\n");
            break;
        case STATUS_NOT_FOUND:
            printf("Record not found.\n");
            break;
        case STATUS_CAPACITY_FULL:
            printf("Storage capacity is full.\n");
            break;
        case STATUS_READING_DECREASED:
            printf("Current meter reading cannot be less than previous reading.\n");
            break;
        case STATUS_FILE_ERROR:
            printf("Error while accessing a data file.\n");
            break;
        case STATUS_CORRUPTED_DATA:
            printf("Stored data is corrupted.\n");
            break;
        case STATUS_NULL_POINTER:
            printf("Internal error: NULL pointer detected.\n");
            break;
        default:
            printf("Unknown error.\n");
    }
}

static void displayBill(Bill bill) {
    printf("\n---------- ELECTRICITY BILL ----------\n");
    printf("Units consumed : %.2f kWh\n", bill.units);
    printf("Energy charge  : Rs. %.2f\n", bill.energyCharge);
    printf("Fixed charge   : Rs. %.2f\n", bill.fixedCharge);
    printf("Tax            : Rs. %.2f\n", bill.tax);
    printf("--------------------------------------\n");
    printf("Total payable  : Rs. %.2f\n", bill.total);
    printf("--------------------------------------\n");
}

static void displayPastBill(BillRecord record) {
    printf("\n---------- PAST BILL ----------\n");
    printf("Consumer ID       : %d\n", record.consumerId);
    printf("Billing month     : %d\n", record.month);
    printf("Billing year      : %d\n", record.year);
    printf("Previous reading  : %.2f kWh\n", record.previousReading);
    printf("Current reading   : %.2f kWh\n", record.currentReading);
    displayBill(record.bill);
}

static void registerConsumer(
    Consumer consumers[],
    int *consumerCount
) {
    int id;
    double initialReading;
    char name[MAX_NAME_LENGTH + 1];
    StatusCode status;

    printf("\nEnter consumer ID: ");
    if (!readInt(&id)) {
        printf("Invalid ID.\n");
        return;
    }

    printf("Enter consumer name: ");
    if (!readName(name)) {
        printf("Invalid name. Use 1 to 50 characters and do not use '|'.\n");
        return;
    }

    printf("Enter initial meter reading (kWh): ");
    if (!readDouble(&initialReading)) {
        printf("Invalid meter reading.\n");
        return;
    }

    status = addConsumer(consumers, consumerCount, id, name, initialReading);
    displayStatus(status);

    if (status == STATUS_OK) {
        status = saveConsumers(consumers, *consumerCount);
        if (status != STATUS_OK) {
           
            (*consumerCount)--;
            displayStatus(status);
        }
    }
}

static void displayConsumers(
    const Consumer consumers[],
    int consumerCount
) {
    int i;

    if (consumerCount == 0) {
        printf("\nNo consumers registered.\n");
        return;
    }

    printf("\n----------- CONSUMER LIST -----------\n");
    for (i = 0; i < consumerCount; i++) {
        printf("\nConsumer ID     : %d\n", consumers[i].id);
        printf("Consumer Name   : %s\n", consumers[i].name);
        printf("Last Reading    : %.2f kWh\n", consumers[i].lastReading);
        printf("Last Units Used : %.2f kWh\n", consumers[i].lastUnits);
    }
}

static void generateBill(
    Consumer consumers[],
    int consumerCount,
    BillRecord bills[],
    int *billCount
) {
    int id;
    int month;
    int year;
    int i;
    double currentReading;
    double units;
    Consumer *consumer;
    Consumer oldConsumer;
    Bill bill;
    BillRecord record;
    UsageLevel usage;
    StatusCode status;

    if (*billCount >= MAX_BILLS) {
        printf("Billing history is full.\n");
        return;
    }

    printf("\nEnter consumer ID: ");
    if (!readInt(&id)) {
        printf("Invalid consumer ID.\n");
        return;
    }

    status = findConsumer(consumers, consumerCount, id, &consumer);
    if (status != STATUS_OK) {
        displayStatus(status);
        return;
    }

    printf("Enter billing month (1-12): ");
    if (!readInt(&month) ||
        month < MIN_BILLING_MONTH || month > MAX_BILLING_MONTH) {
        printf("Invalid billing month.\n");
        return;
    }

    printf("Enter billing year (2000-2100): ");
    if (!readInt(&year) ||
        year < MIN_BILLING_YEAR || year > MAX_BILLING_YEAR) {
        printf("Invalid billing year.\n");
        return;
    }

    for (i = 0; i < *billCount; i++) {
        if (bills[i].consumerId == id &&
            bills[i].month == month &&
            bills[i].year == year) {
            printf("A bill already exists for this billing period.\n");
            return;
        }
    }

    printf("Enter current meter reading (kWh): ");
    if (!readDouble(&currentReading)) {
        printf("Invalid meter reading.\n");
        return;
    }

    status = calculateConsumption(consumer->lastReading,
                                  currentReading, &units);
    if (status != STATUS_OK) {
        displayStatus(status);
        return;
    }

    status = calculateBill(units, &bill);
    if (status != STATUS_OK) {
        displayStatus(status);
        return;
    }

    status = checkHighUsage(units, consumer->lastUnits, &usage);
    if (status != STATUS_OK) {
        displayStatus(status);
        return;
    }

    record.consumerId = id;
    record.month = month;
    record.year = year;
    record.previousReading = consumer->lastReading;
    record.currentReading = currentReading;
    record.bill = bill;

    oldConsumer = *consumer;

    
    bills[*billCount] = record;
    (*billCount)++;

    status = saveBills(bills, *billCount);
    if (status != STATUS_OK) {
        (*billCount)--;
        displayStatus(status);
        return;
    }

    
    consumer->lastReading = currentReading;
    consumer->lastUnits = units;

    status = saveConsumers(consumers, consumerCount);
    if (status != STATUS_OK) {
        
        *consumer = oldConsumer;
        (*billCount)--;

        if (saveBills(bills, *billCount) != STATUS_OK) {
            printf("Warning: Could not restore the previous billing history file.\n");
        }

        displayStatus(status);
        return;
    }

    printf("\nBill generated successfully!\n");
    displayBill(bill);

    if (usage == USAGE_HIGH) {
        printf("\nWARNING: High electricity consumption detected!\n");
    }
}

static void retrievePastBill(BillRecord bills[], int billCount) {
    int id;
    int month;
    int year;
    BillRecord foundBill;
    StatusCode status;

    printf("\nEnter consumer ID: ");
    if (!readInt(&id)) {
        printf("Invalid consumer ID.\n");
        return;
    }

    printf("Enter billing year: ");
    if (!readInt(&year)) {
        printf("Invalid year.\n");
        return;
    }

    printf("Enter billing month: ");
    if (!readInt(&month)) {
        printf("Invalid month.\n");
        return;
    }

    status = findPastBill(bills, billCount, id, year, month, &foundBill);
    if (status == STATUS_OK) {
        displayPastBill(foundBill);
    } else {
        displayStatus(status);
    }
}

static void updateConsumer(
    Consumer consumers[],
    int consumerCount
) {
    int id;
    char newName[MAX_NAME_LENGTH + 1];
    char oldName[MAX_NAME_LENGTH + 1];
    Consumer *consumer;
    StatusCode status;

    printf("\nEnter consumer ID: ");
    if (!readInt(&id)) {
        printf("Invalid consumer ID.\n");
        return;
    }

    status = findConsumer(consumers, consumerCount, id, &consumer);
    if (status != STATUS_OK) {
        displayStatus(status);
        return;
    }

    printf("Enter new consumer name: ");
    if (!readName(newName)) {
        printf("Invalid name. Use 1 to 50 characters and do not use '|'.\n");
        return;
    }

    strcpy(oldName, consumer->name);

    status = updateConsumerName(consumers, consumerCount, id, newName);
    if (status != STATUS_OK) {
        displayStatus(status);
        return;
    }

    status = saveConsumers(consumers, consumerCount);
    if (status != STATUS_OK) {
        strcpy(consumer->name, oldName);
        displayStatus(status);
        return;
    }

    displayStatus(STATUS_OK);
}

int main(void) {
    Consumer consumers[MAX_CONSUMERS];
    BillRecord bills[MAX_BILLS];
    int consumerCount = 0;
    int billCount = 0;
    int choice = 0;
    StatusCode status;

    status = loadConsumers(consumers, &consumerCount);
    if (status != STATUS_OK) {
        displayStatus(status);
        return 1;
    }

    status = loadBills(bills, &billCount);
    if (status != STATUS_OK) {
        displayStatus(status);
        return 1;
    }

    printf("========================================\n");
    printf(" ELECTRICAL ENERGY CONSUMPTION AND BILLING\n");
    printf("========================================\n");

    do {
        printf("\n--------------- MAIN MENU ---------------\n");
        printf("1. Register new consumer\n");
        printf("2. View all consumers\n");
        printf("3. Generate electricity bill\n");
        printf("4. Retrieve past bill\n");
        printf("5. Update consumer name\n");
        printf("6. Exit\n");
        printf("------------------------------------------\n");
        printf("Enter your choice: ");

        if (!readInt(&choice)) {
            printf("Invalid choice. Please enter a number.\n");
            choice = 0;
            continue;
        }

        switch (choice) {
            case 1:
                registerConsumer(consumers, &consumerCount);
                break;
            case 2:
                displayConsumers(consumers, consumerCount);
                break;
            case 3:
                generateBill(consumers, consumerCount, bills, &billCount);
                break;
            case 4:
                retrievePastBill(bills, billCount);
                break;
            case 5:
                updateConsumer(consumers, consumerCount);
                break;
            case 6:
                printf("\nThank you for using the billing system!\n");
                break;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 6);

    return 0;
}
