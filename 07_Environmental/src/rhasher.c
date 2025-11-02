#include <ctype.h>
#include <errno.h>
#include <rhash.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"

#ifdef READLINE
#include <readline/history.h>
#include <readline/readline.h>
#endif

int parse_algo(char* input, int* output_mode, int* err) {
    *err = 0;
    if (input[0] == toupper(input[0])) {
        *output_mode = RHPR_HEX;
    } else {
        *output_mode = RHPR_BASE64;
    }
    // полагаюсь на ленивую логику
    if (toupper(input[0]) == 'M' && toupper(input[1]) == 'D' && input[2] == '5' && input[3] == ' ') {
        return RHASH_MD5;
    }
    if (toupper(input[0]) == 'T' && toupper(input[1]) == 'T' && toupper(input[2]) == 'H' && input[3] == ' ') {
        return RHASH_TTH;
    }
    if (toupper(input[0]) == 'S' && toupper(input[1]) == 'H' && toupper(input[2]) == 'A' && input[3] == '1' &&
        input[4] == ' ') {
        return RHASH_SHA1;
    }
    *err = 1;
    return -1;
}

int main(int argc, char* argv[]) {
    rhash_library_init();
    char* input;

    while (1) {
#ifdef READLINE
        if ((input = readline("")) == NULL) {
            break;
        }
#else
        size_t line_len = 0;
        if (getline(&input, &line_len, stdin) == -1) {
            break;
        }
#endif
        // распознать первое слово: имя алгоритма
        int hash_algo, output_mode, err = 0;
        hash_algo = parse_algo(input, &output_mode, &err);
        if (err) {
            fprintf(stderr, "ERROR: Unknown protocol!\n");
            free(input);
            continue;
        }

        // перейти ко второму слову: строка или имя файла
        char* word = input;
        while (*word != ' ' && *word != '\0') {
            word++;
        }
        if (*word == '\0') {
            fprintf(stderr, "ERROR: No file or string to make hash!\n");
            free(input);
            continue;
        }
        word++;

        // перейти к следующему слову (разбить по пробелам)
        char* word_end = word;
        while (*word_end != '\n' && *word_end != ' ' && *word_end != '\0') {
            word_end++;
        }
        *word_end = '\0';

        // вычислить хэш
        int res;
        unsigned char buf_rhash[8193];
        if (word[0] == '"') {
            res = rhash_msg(hash_algo, word + 1, word_end - word - 1, buf_rhash);
        } else {
            res = rhash_file(hash_algo, word, buf_rhash);
        }
        if (res < 0) {
            fprintf(stderr, "rhash ERROR: %s\n", strerror(errno));
        } else {
            char rhash_output[4097];
            rhash_print_bytes(rhash_output, buf_rhash, rhash_get_digest_size(hash_algo), output_mode);
            printf("%s\n", rhash_output);
        }
        free(input);
    }

    free(input);
    return 0;
}
