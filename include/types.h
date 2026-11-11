

#ifndef TYPES_H
#define TYPES_H



#define MAX_CONSUMERS       100    
#define MAX_BILLS           1200   
#define MAX_NAME_LENGTH     50      


#define MIN_CONSUMER_ID     1
#define MAX_CONSUMER_ID     999999

#define MIN_BILLING_MONTH   1
#define MAX_BILLING_MONTH   12
#define MIN_BILLING_YEAR    2000
#define MAX_BILLING_YEAR    2100

#define MAX_METER_READING   9999999.0   



typedef enum {
    STATUS_OK = 0,
    STATUS_NULL_POINTER,       
    STATUS_INVALID_INPUT,       
    STATUS_DUPLICATE,          
    STATUS_NOT_FOUND,           
    STATUS_CAPACITY_FULL,       
    STATUS_READING_DECREASED,   
    STATUS_FILE_ERROR,          
    STATUS_CORRUPTED_DATA       
} StatusCode;


typedef enum {
    USAGE_NORMAL = 0,
    USAGE_HIGH
} UsageLevel;



typedef struct {
    int id;                             
    char name[MAX_NAME_LENGTH + 1];     
    double lastReading;                 
                                        
    double lastUnits;                   
                                        
} Consumer;


typedef struct {
    double units;                       
    double energyCharge;                
    double fixedCharge;                
    double tax;                        
    double total;                       
} Bill;


typedef struct {
    int consumerId;                   
    int month;                         
    int year;                          
    double previousReading;             
    double currentReading;              
    Bill bill;                         
} BillRecord;

#endif
