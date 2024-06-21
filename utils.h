#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Count the space characters in a given string.
   Note: This counts anything that would return true to isspace(). */
extern size_t count_spaces(const char *str);

/* Flush the given input stream up to EOF or the first newline character. */
extern void flush_input(FILE *fp);

/* Unescape a given string, transforming all escape sequences and quoted strings
   as appropriate.
   Note: You are responsible for freeing the string returned by this function */
extern char *unescape(const char *str, FILE *errf);

/* Find the first unquoted/unescaped space character in a given string.
   Note: Running this on a string returned by unescape will not do what you want
   in all likelihood. */
extern int first_unquoted_space(const char *str);

#ifdef __cplusplus
}
#endif

#endif /* !UTILS_H */
