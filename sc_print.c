#include <stdio.h>
#include <ctype.h>
#include "sc_object.h"
#include "sc_print.h"
#include "sc_log.h"

static int write_character(object *val) {
    char v = obj_cv(val);
    
    printf("%s", "#\\");
    switch (v) {
        case NUL:
            printf("%s", "nul");
            break;

        case TAB:
            printf("%s", "tab");
            break;

        /* LINEFEED is equal to NEWLINE */
        case NEWLINE:
            printf("%s", "newline");
            break;

        case RETURN:
            printf("%s", "return");
            break;

        case SPACE:
            printf("%s", "space");
            break;

        default:
            printf("%c", v);
            if (!isgraph(v)) {
                char msg[64];
                sprintf(msg, "character not printable `%d\n", v);
                sc_log(msg);
            }
            break;
    }
    return 0;
}

static int write_string(object *val) {
    char *str = obj_sv(val);

    fputc('"', stdout);
    while (*str != '\0') {
       switch (*str) {
           case '\a':
               printf("%s", "\\a");
               break;
           case '\b':
               printf("%s", "\\b");
               break;
           case '\t':
               printf("%s", "\\t");
               break;
           case '\n':
               printf("%s", "\\n");
               break;
           case '\v':
               printf("%s", "\\v");
               break;
           case '\f':
               printf("%s", "\\f");
               break;
           case '\r':
               printf("%s", "\\r");
               break;
           case '"':
               printf("%s", "\\\"");
               break;
           case '\\':
               printf("%s", "\\\\");
               break;
           default:
               fputc(*str, stdout);
               break;
       }
       str++;
    }
    fputc('"', stdout);

    return 0;
}

static int write_pair(object *val) {
    object *car_obj, *cdr_obj;

    car_obj = car(val);
    cdr_obj = cdr(val);

    sc_write(car_obj);
    if (is_pair(cdr_obj)) {
        printf(" ");
        write_pair(cdr_obj);
    } else if (is_empty_list(cdr_obj)) {
        return 0;
    } else {
        printf(" . ");
        sc_write(cdr_obj);
    }
    return 0;
}

int sc_write(object *val) {
    int ret = 0;

    if (val == NULL) {
        sc_log("Cannot print NULL val\n");
        return -1;
    }

    if (is_fixnum(val)) {
        printf("%ld", obj_nv(val));
    } else if (is_boolean(val)) {
        char v = is_true(val) ? 't' : 'f';
        printf("#%c", v);
    } else if (is_character(val)) {
        ret = write_character(val);
    } else if (is_string(val)) {
        ret = write_string(val);
    } else if (is_empty_list(val)) {
        printf("%s", "()");
    } else if (is_pair(val)) {
        printf("(");
        ret = write_pair(val);
        printf(")");
    } else {
        fprintf(stderr,
                "unknown type, cannot print\n");
        ret = 1;
    }

    return ret;
}

