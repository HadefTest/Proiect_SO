#ifndef REPORT_H
#define REPORT_H

#include <time.h>

#define MAX_PATH 256
#define MAX_STR 24 

typedef struct {
    int report_id;          
    char inspector[24];     
    float latitude;         
    float longitude;        
    char category[24];      
    int severity;           
    time_t timestamp;       
    char description[136];  
} Report;                       

#endif