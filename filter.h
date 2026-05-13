#ifndef FILTER_H
#define FILTER_H

#include "report.h"

int parse_condition(const char *input, char *field, char *op, char *value);
int match_condition(Report *r, const char *field, const char *op, const char *value);

#endif