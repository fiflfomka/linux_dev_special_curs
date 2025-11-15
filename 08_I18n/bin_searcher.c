#include <libintl.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"

#define MAX_NUM 100u

inline static int str_equal(char* s1, char* s2) {
    return strcmp(s1, s2) == 0;
}

typedef unsigned int UINT32;

int main(void) {
    setlocale(LC_ALL, "");
    bindtextdomain(PACKAGE, LOCALEDIR);
    textdomain(PACKAGE);

    printf(gettext("Pick a number in range 1..%u (include 1 and %u).\n"), MAX_NUM, MAX_NUM);
    printf(gettext("Use only Yes/yes/Y/y and No/no/N/n to answer.\n"));

    UINT32 l = 0, r = MAX_NUM, center;

    while (l != r - 1) {
        center = (r + l) / 2;
        printf(gettext("Is the number above than %u?\n"), center);
        char* user_answer = NULL;
        size_t buf_size = 0;
        size_t line_len = getline(&user_answer, &buf_size, stdin);

        if (line_len == -1) {
            printf(gettext("Stopped. Your number is %d <= ? <= %d.\n"), l, r - 1);
            return 0;
        }

        if (str_equal(user_answer, "Yes\n") || str_equal(user_answer, "yes\n") || str_equal(user_answer, "Y\n") ||
            str_equal(user_answer, "y\n")) {
            l = center;
        } else if (str_equal(user_answer, "No\n") || str_equal(user_answer, "no\n") || str_equal(user_answer, "N\n") ||
                   str_equal(user_answer, "n\n")) {
            r = center;
        } else {
            printf(gettext("Use only Yes/yes/Y/y and No/no/N/n to answer.\n"));
        }
        free(user_answer);
    }

    printf(gettext("The number is %u!\n"), r);

    return 0;
}
