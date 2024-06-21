#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#include "utils.h"

size_t count_spaces(const char *str) {
    size_t rv = 0;

    while(*str) {
        if(isspace(*str++))
            ++rv;
    }

    return rv;
}

void flush_input(FILE *fp) {
    int c;

    /* Read until we get an EOF character or a newline. */
    while(1) {
        c = fgetc(fp);

        if(c == '\n' || c == EOF)
            return;
    }
}

char *unescape(const char *str, FILE *errf) {
    size_t len = strlen(str);
    char *unesc, *rv;
    char cur;
    char quoted = 0;

    /* Allocate space for the new string. Since only potentially removing
       characters, it will be no larger than the original string... */
    if(!(rv = (char *)malloc(len + 1))) {
        fprintf(errf, "shell error: %s\n", strerror(errno));
        return NULL;
    }

    unesc = rv;

    /* Scan through the string... */
    while(*str) {
        cur = *str++;

        /* Is this the beginning of an escape sequence? */
        if(!quoted && cur == '\\') {
            cur = *str++;

            switch(cur) {
                case '\0':
                    fprintf(errf, "shell error: illegal escape sequence\n");
                    free(rv);
                    return NULL;

                case 'n':
                    *unesc++ = '\n';
                    continue;
                case 'a':
                    *unesc++ = '\a';
                    continue;
                case 'b':
                    *unesc++ = '\b';
                    continue;
                case 'r':
                    *unesc++ = '\r';
                    continue;
                case '\\':
                    *unesc++ = '\\';
                    continue;
                case 'f':
                    *unesc++ = '\f';
                    continue;
                case 'v':
                    *unesc++ = '\v';
                    continue;
                case '\'':
                    *unesc++ = '\'';
                    continue;
                case '"':
                    *unesc++ = '"';
                    continue;
                case '?':
                    *unesc++ = '?';
                    continue;
                case '*':
                    *unesc++ = '*';
                    continue;
                case '$':
                    *unesc++ = '$';
                    continue;
                case 't':
                    *unesc++ = '\t';
                    continue;
                case ' ':
                    *unesc++ = ' ';
                    continue;
                case '!':
                    *unesc++ = '!';
                    continue;

                /* Ugh... Octal. */
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                {
                    int tmp = (cur - '0') << 6;

                    cur = *str++;
                    if(cur < '0' || cur > '7') {
                        free(rv);
                        return NULL;
                    }

                    tmp |= (cur - '0') << 3;

                    cur = *str++;
                    if(cur < '0' || cur > '7') {
                        free(rv);
                        return NULL;
                    }

                    tmp |= (cur - '0');
                    *unesc++ = (char)tmp;
                    continue;
                }

                /* And, for more fun, hex! */
                case 'x':
                case 'X':
                {
                    int tmp;

                    cur = *str++;
                    if(cur >= '0' && cur <= '9')
                        tmp = (cur - '0') << 4;
                    else if(cur >= 'a' && cur <= 'f')
                        tmp = (cur - 'a' + 10) << 4;
                    else if(cur >= 'A' && cur <= 'F')
                        tmp = (cur - 'A' + 10) << 4;
                    else {
                        free(rv);
                        return NULL;
                    }

                    cur = *str++;
                    if(cur >= '0' && cur <= '9')
                        tmp |= (cur - '0');
                    else if(cur >= 'a' && cur <= 'f')
                        tmp |= (cur - 'a' + 10);
                    else if(cur >= 'A' && cur <= 'F')
                        tmp |= (cur - 'A' + 10);
                    else {
                        free(rv);
                        return NULL;
                    }

                    *unesc++ = (char)tmp;
                    continue;
                }

                default:
                    *unesc++ = cur;
            }

            continue;
        }
        /* Escape sequences in quoted strings are very limited... */
        else if(quoted && cur == '\\') {
            cur = *str++;

            if(!cur) {
                fprintf(errf, "shell error: invalid escape sequence\n");
                free(rv);
                return NULL;
            }

            if(cur != quoted)
                *unesc++ = '\\';
        }
        /* Is this the beginning of a quoted string? */
        else if(!quoted && (cur == '\'' || cur == '"')) {
            quoted = cur;
            continue;
        }
        else if(quoted && cur == quoted) {
            quoted = 0;
            continue;
        }

        /* If we get here, it's not part of an escape, so copy it directly. */
        *unesc++ = cur;
    }

    /* Did we terminate any quotes that we started? */
    if(quoted) {
        fprintf(errf, "shell error: unterminated quote\n");
        free(rv);
        return NULL;
    }

    *unesc = 0;

    return rv;
}

int first_unquoted_space(const char *str) {
    int pos = 0;
    const char *tmp = str;
    char quoted = 0, last = 0, cur = 0;

    while(*tmp) {
        last = cur;
        cur = *tmp;

        if(last != '\\') {
            if(!quoted && (cur == '\'' || cur == '"'))
                quoted = cur;
            else if(quoted && cur == quoted)
                quoted = 0;
            if(!quoted && isspace(cur))
                return pos;
        }

        ++pos;
        ++tmp;
    }

    return -1;
}
