#include <stdio.h>
#include <stdlib.h>

struct Range {
    long start, stop, step, cur;
};
typedef struct Range range;

void range_init(range *r) {
    r->cur = r->start;
}

int range_run(range *r) {
    return r->cur < r->stop;
}

void range_next(range *r) {
    r->cur += r->step;
}

long range_get(range *r) {
    return r->cur;
}

int argparse(int argc, char **argv, range *r) {
    if (argc == 1) {
        printf("Program works like python range() iterator:\n");
        printf("range A    : print 0   1   .. (A-2) (A-1)\n");
        printf("range A B  : print A (A+1) .. (B-2) (B-1)\n");
        printf("range A B C: print A (A+C) .. A+K*C for all K where\n");
        printf("             (A+K*C in range A B) if C > 0 or\n");
        printf("             (A+K*C in range B A) if C < 0\n");
        return 1;
    }
    if (argc == 2) {
        r->start = 0;
        r->stop = strtol(argv[1], NULL, 10);
        r->step = 1;
    } else if (argc == 3) {
        r->start = strtol(argv[1], NULL, 10);
        r->stop = strtol(argv[2], NULL, 10);
        r->step = 1;
    } else if (argc == 4) {
        r->start = strtol(argv[1], NULL, 10);
        r->stop = strtol(argv[2], NULL, 10);
        r->step = strtol(argv[3], NULL, 10);
    } else {
        return 2;
    }
    return 0;
}

int main(int argc, char *argv[]) {
        range I;
        int ret_code = argparse(argc, argv, &I);
        if (ret_code) {
            return ret_code == 1 ? 0 : ret_code;
        }

        for(range_init(&I); range_run(&I); range_next(&I)) {
            printf("%ld\n", range_get(&I));
        }

        return 0;
}
