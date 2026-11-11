#include <gtest/gtest.h>

extern "C" {
#include "billing.h"
}

TEST(BillingTest, CalculateConsumption) {
    double units = 0.0;

    StatusCode status = calculateConsumption(100.0, 250.0, &units);

    EXPECT_EQ(status, STATUS_OK);
    EXPECT_DOUBLE_EQ(units, 150.0);
}

TEST(BillingTest, DecreasedMeterReading) {
    double units = 0.0;

    StatusCode status = calculateConsumption(250.0, 100.0, &units);

    EXPECT_EQ(status, STATUS_READING_DECREASED);
}

TEST(BillingTest, CalculateBillUsingTariffSlabs) {
    Bill bill;

    StatusCode status = calculateBill(230.0, &bill);

    EXPECT_EQ(status, STATUS_OK);
    EXPECT_DOUBLE_EQ(bill.units, 230.0);
    EXPECT_DOUBLE_EQ(bill.energyCharge, 1010.0);
    EXPECT_DOUBLE_EQ(bill.fixedCharge, 100.0);
    EXPECT_DOUBLE_EQ(bill.tax, 55.5);
    EXPECT_DOUBLE_EQ(bill.total, 1165.5);
}

TEST(BillingTest, NegativeConsumptionIsRejected) {
    Bill bill;

    EXPECT_EQ(calculateBill(-50.0, &bill), STATUS_INVALID_INPUT);
}

TEST(BillingTest, HighConsumptionDetected) {
    UsageLevel usage = USAGE_NORMAL;

    StatusCode status = checkHighUsage(160.0, 100.0, &usage);

    EXPECT_EQ(status, STATUS_OK);
    EXPECT_EQ(usage, USAGE_HIGH);
}

TEST(BillingTest, NormalConsumptionDetected) {
    UsageLevel usage = USAGE_HIGH;

    StatusCode status = checkHighUsage(120.0, 100.0, &usage);

    EXPECT_EQ(status, STATUS_OK);
    EXPECT_EQ(usage, USAGE_NORMAL);
}

TEST(BillingTest, NullOutputPointerIsRejected) {
    EXPECT_EQ(calculateBill(100.0, nullptr), STATUS_NULL_POINTER);
}
