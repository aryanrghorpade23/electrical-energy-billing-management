#ifndef BILLING_H
#define BILLING_H

#include "types.h"

StatusCode calculateConsumption(
    double previousReading,
    double currentReading,
    double *units
);

StatusCode calculateBill(
    double units,
    Bill *bill
);

StatusCode checkHighUsage(
    double currentUnits,
    double previousUnits,
    UsageLevel *usage
);

#endif
