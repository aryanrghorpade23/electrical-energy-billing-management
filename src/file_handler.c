#include <stdio.h>
#include <string.h>
#include <math.h>
#include <errno.h>

#include "file_handler.h"
#include "billing.h"

#define DATA_LINE_LENGTH 512


static int hasOnlyTrailingWhitespace(const char *text) {
    while (*text != '\0') {
        if (*text != ' ' && *text != '\t' &&
            *text != '\n' && *text != '\r') {
            return 0;
        }
        text++;
    }

    return 1;
}


static int readDataLine(FILE *file, char line[], size_t size) {
    size_t length;

    if (fgets(line, (int)size, file) == NULL) {
        return 0;
    }

    length = strlen(line);

    if (length > 0 && line[length - 1] == '\n') {
        return 1;
    }

    if (!feof(file)) {
        int ch;
        while ((ch = fgetc(file)) != '\n' && ch != EOF) {
        }
        return -1;
    }

    return 1;
}


StatusCode saveConsumers(const Consumer consumers[], int count) {
    FILE *file;
    int i;

    if (consumers == NULL) {
        return STATUS_NULL_POINTER;
    }

    if (count < 0 || count > MAX_CONSUMERS) {
        return STATUS_INVALID_INPUT;
    }

    file = fopen("data/consumers.txt", "w");

    if (file == NULL) {
        return STATUS_FILE_ERROR;
    }

    for (i = 0; i < count; i++) {
        if (fprintf(file, "%d|%s|%.2f|%.2f\n",
                    consumers[i].id,
                    consumers[i].name,
                    consumers[i].lastReading,
                    consumers[i].lastUnits) < 0) {
            fclose(file);
            return STATUS_FILE_ERROR;
        }
    }

    if (fclose(file) != 0) {
        return STATUS_FILE_ERROR;
    }

    return STATUS_OK;
}


StatusCode loadConsumers(Consumer consumers[], int *count) {
    FILE *file;
    char line[DATA_LINE_LENGTH];
    int lineStatus;
    int i;

    if (consumers == NULL || count == NULL) {
        return STATUS_NULL_POINTER;
    }

    *count = 0;
    errno = 0;
    file = fopen("data/consumers.txt", "r");

    if (file == NULL) {
        return (errno == ENOENT) ? STATUS_OK : STATUS_FILE_ERROR;
    }

    while ((lineStatus = readDataLine(file, line, sizeof(line))) != 0) {
        Consumer temp;
        int consumed = 0;
        int fields;

        if (lineStatus < 0) {
            fclose(file);
            *count = 0;
            return STATUS_CORRUPTED_DATA;
        }

        fields = sscanf(line, "%d|%50[^|]|%lf|%lf %n",
                        &temp.id,
                        temp.name,
                        &temp.lastReading,
                        &temp.lastUnits,
                        &consumed);

        if (fields != 4 || !hasOnlyTrailingWhitespace(line + consumed) ||
            *count >= MAX_CONSUMERS ||
            temp.id < MIN_CONSUMER_ID || temp.id > MAX_CONSUMER_ID ||
            temp.name[0] == '\0' ||
            !isfinite(temp.lastReading) ||
            !isfinite(temp.lastUnits) ||
            temp.lastReading < 0 ||
            temp.lastReading > MAX_METER_READING ||
            temp.lastUnits < 0 ||
            temp.lastUnits > MAX_METER_READING) {
            fclose(file);
            *count = 0;
            return STATUS_CORRUPTED_DATA;
        }

        for (i = 0; i < *count; i++) {
            if (consumers[i].id == temp.id) {
                fclose(file);
                *count = 0;
                return STATUS_CORRUPTED_DATA;
            }
        }

        consumers[*count] = temp;
        (*count)++;
    }

    if (ferror(file)) {
        fclose(file);
        *count = 0;
        return STATUS_FILE_ERROR;
    }

    fclose(file);
    return STATUS_OK;
}


StatusCode saveBills(const BillRecord bills[], int count) {
    FILE *file;
    int i;

    if (bills == NULL) {
        return STATUS_NULL_POINTER;
    }

    if (count < 0 || count > MAX_BILLS) {
        return STATUS_INVALID_INPUT;
    }

    file = fopen("data/billing_history.txt", "w");

    if (file == NULL) {
        return STATUS_FILE_ERROR;
    }

    for (i = 0; i < count; i++) {
        const BillRecord *r = &bills[i];

        if (fprintf(file,
                    "%d|%d|%d|%.2f|%.2f|%.2f|%.2f|%.2f|%.2f|%.2f\n",
                    r->consumerId, r->month, r->year,
                    r->previousReading, r->currentReading,
                    r->bill.units, r->bill.energyCharge,
                    r->bill.fixedCharge, r->bill.tax,
                    r->bill.total) < 0) {
            fclose(file);
            return STATUS_FILE_ERROR;
        }
    }

    if (fclose(file) != 0) {
        return STATUS_FILE_ERROR;
    }

    return STATUS_OK;
}


StatusCode loadBills(BillRecord bills[], int *count) {
    FILE *file;
    char line[DATA_LINE_LENGTH];
    int lineStatus;
    int i;

    if (bills == NULL || count == NULL) {
        return STATUS_NULL_POINTER;
    }

    *count = 0;
    errno = 0;
    file = fopen("data/billing_history.txt", "r");

    if (file == NULL) {
        return (errno == ENOENT) ? STATUS_OK : STATUS_FILE_ERROR;
    }

    while ((lineStatus = readDataLine(file, line, sizeof(line))) != 0) {
        BillRecord temp;
        Bill expectedBill;
        int consumed = 0;
        int fields;
        StatusCode billStatus;

        if (lineStatus < 0) {
            fclose(file);
            *count = 0;
            return STATUS_CORRUPTED_DATA;
        }

        fields = sscanf(line,
                        "%d|%d|%d|%lf|%lf|%lf|%lf|%lf|%lf|%lf %n",
                        &temp.consumerId,
                        &temp.month,
                        &temp.year,
                        &temp.previousReading,
                        &temp.currentReading,
                        &temp.bill.units,
                        &temp.bill.energyCharge,
                        &temp.bill.fixedCharge,
                        &temp.bill.tax,
                        &temp.bill.total,
                        &consumed);

        if (fields != 10 || !hasOnlyTrailingWhitespace(line + consumed) ||
            *count >= MAX_BILLS ||
            temp.consumerId < MIN_CONSUMER_ID ||
            temp.consumerId > MAX_CONSUMER_ID ||
            temp.month < MIN_BILLING_MONTH ||
            temp.month > MAX_BILLING_MONTH ||
            temp.year < MIN_BILLING_YEAR ||
            temp.year > MAX_BILLING_YEAR ||
            !isfinite(temp.previousReading) ||
            !isfinite(temp.currentReading) ||
            !isfinite(temp.bill.units) ||
            !isfinite(temp.bill.energyCharge) ||
            !isfinite(temp.bill.fixedCharge) ||
            !isfinite(temp.bill.tax) ||
            !isfinite(temp.bill.total) ||
            temp.previousReading < 0 ||
            temp.currentReading < temp.previousReading ||
            temp.currentReading > MAX_METER_READING ||
            temp.bill.units < 0 ||
            temp.bill.energyCharge < 0 ||
            temp.bill.fixedCharge < 0 ||
            temp.bill.tax < 0 ||
            temp.bill.total < 0 ||
            fabs(temp.bill.units -
                 (temp.currentReading - temp.previousReading)) > 0.011) {
            fclose(file);
            *count = 0;
            return STATUS_CORRUPTED_DATA;
        }

        billStatus = calculateBill(temp.bill.units, &expectedBill);

        if (billStatus != STATUS_OK ||
            fabs(temp.bill.energyCharge - expectedBill.energyCharge) > 0.011 ||
            fabs(temp.bill.fixedCharge - expectedBill.fixedCharge) > 0.011 ||
            fabs(temp.bill.tax - expectedBill.tax) > 0.011 ||
            fabs(temp.bill.total - expectedBill.total) > 0.011) {
            fclose(file);
            *count = 0;
            return STATUS_CORRUPTED_DATA;
        }

        for (i = 0; i < *count; i++) {
            if (bills[i].consumerId == temp.consumerId &&
                bills[i].year == temp.year &&
                bills[i].month == temp.month) {
                fclose(file);
                *count = 0;
                return STATUS_CORRUPTED_DATA;
            }
        }

        bills[*count] = temp;
        (*count)++;
    }

    if (ferror(file)) {
        fclose(file);
        *count = 0;
        return STATUS_FILE_ERROR;
    }

    fclose(file);
    return STATUS_OK;
}


StatusCode findPastBill(
    BillRecord bills[],
    int count,
    int consumerId,
    int year,
    int month,
    BillRecord *foundBill
) {
    int i;

    if (bills == NULL || foundBill == NULL) {
        return STATUS_NULL_POINTER;
    }

    if (count < 0 || count > MAX_BILLS ||
        consumerId < MIN_CONSUMER_ID || consumerId > MAX_CONSUMER_ID ||
        year < MIN_BILLING_YEAR || year > MAX_BILLING_YEAR ||
        month < MIN_BILLING_MONTH || month > MAX_BILLING_MONTH) {
        return STATUS_INVALID_INPUT;
    }

    for (i = 0; i < count; i++) {
        if (bills[i].consumerId == consumerId &&
            bills[i].year == year &&
            bills[i].month == month) {
            *foundBill = bills[i];
            return STATUS_OK;
        }
    }

    return STATUS_NOT_FOUND;
}
