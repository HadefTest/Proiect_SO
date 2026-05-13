#include "filter.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int parse_condition(const char *input, char *field, char *op, char *value) {
    if (!input || !field || !op || !value) return 0;
    int matches = sscanf(input, "%[^:]:%[^:]:%[^\n]", field, op, value);
    return (matches == 3);
}

int match_condition(Report *r, const char *field, const char *op, const char *value) {
    if (!r || !field || !op || !value) return 0;

    if (strcmp(field, "severity") == 0) {
        int val_int = atoi(value);
        if (strcmp(op, "==") == 0) return r->severity_level == val_int;
        if (strcmp(op, "!=") == 0) return r->severity_level != val_int;
        if (strcmp(op, "<") == 0)  return r->severity_level < val_int;
        if (strcmp(op, "<=") == 0) return r->severity_level <= val_int;
        if (strcmp(op, ">") == 0)  return r->severity_level > val_int;
        if (strcmp(op, ">=") == 0) return r->severity_level >= val_int;
    }
    else if (strcmp(field, "timestamp") == 0) {
        long long val_time = atoll(value);
        if (strcmp(op, "==") == 0) return r->timestamp == val_time;
        if (strcmp(op, "!=") == 0) return r->timestamp != val_time;
        if (strcmp(op, "<") == 0)  return r->timestamp < val_time;
        if (strcmp(op, "<=") == 0) return r->timestamp <= val_time;
        if (strcmp(op, ">") == 0)  return r->timestamp > val_time;
        if (strcmp(op, ">=") == 0) return r->timestamp >= val_time;
    }
    else if (strcmp(field, "category") == 0) {
        int cmp = strcmp(r->issue_category, value);
        if (strcmp(op, "==") == 0) return cmp == 0;
        if (strcmp(op, "!=") == 0) return cmp != 0;
    }
    else if (strcmp(field, "inspector") == 0) {
        int cmp = strcmp(r->inspector_name, value);
        if (strcmp(op, "==") == 0) return cmp == 0;
        if (strcmp(op, "!=") == 0) return cmp != 0;
    }
    return 0;
}