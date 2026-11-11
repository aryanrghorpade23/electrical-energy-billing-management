#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

#include "types.h"

StatusCode saveConsumers(
    const Consumer consumers[],
    int count
);

StatusCode loadConsumers(
    Consumer consumers[],
    int *count
);

StatusCode saveBills(
    const BillRecord bills[],
    int count
);

StatusCode loadBills(
    BillRecord bills[],
    int *count
);

StatusCode findPastBill(
    BillRecord bills[],
    int count,
    int consumerId,
    int year,
    int month,
    BillRecord *foundBill
);

#endif
