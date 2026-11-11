#ifndef CONSUMER_H
#define CONSUMER_H

#include "types.h"

StatusCode addConsumer(
    Consumer consumers[],
    int *count,
    int id,
    const char name[],
    double initialReading
);

StatusCode findConsumer(
    Consumer consumers[],
    int count,
    int id,
    Consumer **foundConsumer
);

StatusCode updateConsumerName(
    Consumer consumers[],
    int count,
    int id,
    const char newName[]
);

#endif
