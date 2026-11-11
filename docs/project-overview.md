# Project Overview

## Objective
Build a simple C application to manage electricity consumers and their monthly energy bills.

## Main modules

- **Consumer management:** add consumers, search by consumer ID, and update names.
- **Billing:** calculate units from meter readings, apply the tariff slabs, and identify high consumption.
- **File handling:** save/load consumer data and billing history, and retrieve a bill for a selected consumer and billing period.
- **Main program:** provide the menu-driven interface and coordinate the modules.

## Billing assumptions

- 0–100 units: ₹3/unit
- 101–200 units: ₹5/unit
- Above 200 units: ₹7/unit
- Fixed charge: ₹100
- Tax: 5% of energy charge plus fixed charge
- High usage: current usage greater than 150% of previous usage. If previous usage is zero, any positive current usage is treated as high.

These are illustrative academic-project values, not an official utility tariff.
