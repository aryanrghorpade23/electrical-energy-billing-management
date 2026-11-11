#include <string.h>
#include <math.h>

#include "consumer.h"


static int isValidName(const char name[]) {
    size_t length;
    size_t i;

    if (name == NULL) {
        return 0;
    }

    length = strlen(name);

    if (length == 0 || length > MAX_NAME_LENGTH) {
        return 0;
    }

    
    for (i = 0; i < length; i++) {
        if (name[i] == '|' || name[i] == '\n' || name[i] == '\r') {
            return 0;
        }
    }

    return 1;
}



StatusCode addConsumer(
    Consumer consumers[],
    int *count,
    int id,
    const char name[],
    double initialReading
) {
    int i;

    if (consumers == NULL || count == NULL || name == NULL) {
        return STATUS_NULL_POINTER;
    }

    if (*count < 0 || *count > MAX_CONSUMERS) {
        return STATUS_INVALID_INPUT;
    }

    if (*count == MAX_CONSUMERS) {
        return STATUS_CAPACITY_FULL;
    }

    if (id < MIN_CONSUMER_ID || id > MAX_CONSUMER_ID ||
        !isValidName(name) ||
        !isfinite(initialReading) ||
        initialReading < 0 ||
        initialReading > MAX_METER_READING) {
        return STATUS_INVALID_INPUT;
    }

    for (i = 0; i < *count; i++) {
        if (consumers[i].id == id) {
            return STATUS_DUPLICATE;
        }
    }

    consumers[*count].id = id;
    strcpy(consumers[*count].name, name);
    consumers[*count].lastReading = initialReading;
    consumers[*count].lastUnits = 0.0;

    (*count)++;

    return STATUS_OK;
}


StatusCode findConsumer(
    Consumer consumers[],
    int count,
    int id,
    Consumer **foundConsumer
) {
    int i;

    if (consumers == NULL || foundConsumer == NULL) {
        return STATUS_NULL_POINTER;
    }

    *foundConsumer = NULL;

    if (count < 0 || count > MAX_CONSUMERS ||
        id < MIN_CONSUMER_ID || id > MAX_CONSUMER_ID) {
        return STATUS_INVALID_INPUT;
    }

    for (i = 0; i < count; i++) {
        if (consumers[i].id == id) {
            *foundConsumer = &consumers[i];
            return STATUS_OK;
        }
    }

    return STATUS_NOT_FOUND;
}


StatusCode updateConsumerName(
    Consumer consumers[],
    int count,
    int id,
    const char newName[]
) {
    Consumer *foundConsumer;
    StatusCode status;

    if (newName == NULL) {
        return STATUS_NULL_POINTER;
    }

    if (!isValidName(newName)) {
        return STATUS_INVALID_INPUT;
    }

    status = findConsumer(consumers, count, id, &foundConsumer);

    if (status != STATUS_OK) {
        return status;
    }

    strcpy(foundConsumer->name, newName);

    return STATUS_OK;
}
