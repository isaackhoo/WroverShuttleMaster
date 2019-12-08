#pragma once

#ifndef HELPER_H
#define HELPER_H

// Cut c string
extern bool strcut(char *, char *, int, int, bool);
extern bool strcut(char *, char *, int, int);

// Convert int to 2 char digit string, i.e. int 1 -> "01"
extern void GET_TWO_DIGIT_STRING(char *, int);

// Search c string
extern int findIndex(char *, char);

#endif