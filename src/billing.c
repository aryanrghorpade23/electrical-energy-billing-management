#include <math.h>
#include <stddef.h>

#include "billing.h"

/* Calculate energy consumed from two meter readings. */
StatusCode calculateConsumption(
    double previousReading,
    double currentReading,
    double *units
) {
    if (units == NULL) {
        return STATUS_NULL_POINTER;
    }

    if (!isfinite(previousReading) || !isfinite(currentReading) ||
        previousReading < 0 || currentReading < 0 ||
        previousReading > MAX_METER_READING ||
        currentReading > MAX_METER_READING) {
        return STATUS_INVALID_INPUT;
    }

    if (currentReading < previousReading) {
        return STATUS_READING_DECREASED;
    }

    *units = currentReading - previousReading;

    return STATUS_OK;
}



StatusCode calculateBill(
    double units,
    Bill *bill
) {
    double energyCharge;
    double tax;

    if (bill == NULL) {
        return STATUS_NULL_POINTER;
    }

    if (!isfinite(units) || units < 0 || units > MAX_METER_READING) {
        return STATUS_INVALID_INPUT;
    }

    if (units <= 100.0) {
        energyCharge = units * 3.0;
    } else if (units <= 200.0) {
        energyCharge = (100.0 * 3.0) +
                       ((units - 100.0) * 5.0);
    } else {
        energyCharge = (100.0 * 3.0) +
                       (100.0 * 5.0) +
                       ((units - 200.0) * 7.0);
    }

    bill->units = units;
    bill->energyCharge = energyCharge;
    bill->fixedCharge = 100.0;

    tax = (energyCharge + bill->fixedCharge) * 0.05;
    bill->tax = tax;
    bill->total = energyCharge + bill->fixedCharge + tax;

    return STATUS_OK;
}



StatusCode checkHighUsage(
    double currentUnits,
    double previousUnits,
    UsageLevel *usage
) {
    if (usage == NULL) {
        return STATUS_NULL_POINTER;
    }

    if (!isfinite(currentUnits) || !isfinite(previousUnits) ||
        currentUnits < 0 || previousUnits < 0) {
        return STATUS_INVALID_INPUT;
    }

    if (previousUnits == 0.0) {
        *usage = (currentUnits > 0.0) ? USAGE_HIGH : USAGE_NORMAL;
    } else if (currentUnits > previousUnits * 1.5) {
        *usage = USAGE_HIGH;
    } else {
        *usage = USAGE_NORMAL;
    }

    return STATUS_OK;
}
