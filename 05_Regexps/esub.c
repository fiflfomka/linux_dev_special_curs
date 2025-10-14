#include <regex.h>
#include <stdio.h>
#include <string.h>

#define ERR_LEN 255

#define is_digit(i) (i >= '0' && i <= '9')

int main(int argc, char** argv) {
    if (argc != 4) {
        printf("expected 3 args: REGEXP, subst, text; found %d\n", argc - 1);
        return 1;
    }

#ifdef DEBUG
    printf("REGEXP: %s\n", argv[1]);
    printf("SUBST:  %s\n", argv[2]);
    printf("TEXT:   %s\n", argv[3]);
#endif

    regex_t regex;
    regmatch_t bags[10];
    int errcode;

    // парсить регулярку, не исполняя
    errcode = regcomp(&regex, argv[1], REG_EXTENDED);

    if (errcode != 0) {
        // ошибка парсинга регулярки
        char errbuf[ERR_LEN+1];
        regerror(errcode, &regex, errbuf, ERR_LEN);
        fprintf(stderr, "%.*s\n", ERR_LEN, errbuf);
        regfree(&regex);
        return errcode;
    }

    // найти значение
    errcode = regexec(&regex, argv[3], 10, bags, 0);
    if (errcode != 0) {
        printf("%s\n", argv[3]);
        regfree(&regex);
        return errcode;
    }

    printf("%.*s", bags[0].rm_so, argv[3]);

    for (int i = 0; i < strlen(argv[2]); i++) {
        if (argv[2][i] == '\\' && is_digit(argv[2][i+1])) {
            regmatch_t *b = &bags[argv[2][i + 1] - '0'];
            if (b->rm_eo == 0) {
                putchar('\\');
            } else {
                i++;
                printf("%.*s", b->rm_eo - b->rm_so, argv[3] + b->rm_so);
            }
        } else {
            putchar(argv[2][i]);
        }
    }

    printf("%s\n", argv[3] + bags[0].rm_eo);

    regfree(&regex);
    return 0;
}
