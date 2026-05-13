#ifndef REPORT_H
#define REPORT_H

#include <time.h>

typedef struct {
    int report_id;               
    int severity_level;          
    double latitude;             
    double longitude;            
    time_t timestamp;            
    char inspector_name[48];     
    char issue_category[32];     
    char description[96];        
} Report;                        

#endif