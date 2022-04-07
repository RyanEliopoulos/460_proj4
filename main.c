#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>


#define FIFO 1
#define LRU 2

void vet_args(int, char *[], FILE *);



unsigned int max_pages = 0;
unsigned int page_count = 0;
int alg = 0;


int main(int argc, char *argv[]) {

    // Process args
    unsigned int page_count = 0;
    FILE *file = NULL;
    vet_args(argc, argv, file);
}

void vet_args(int argc, char *argv[], FILE *file) {

    // Arg count
    if(argc != 4) {
        printf("usage: ./simulate [ALGORITHM] [PAGE COUNT] [PAGEREF]\n");
        exit(1);
    }
    // Checking page count
    int ret = sscanf(argv[2], "%u", &max_pages);
    if(ret != 1 || max_pages == 0 || argv[2][0] == '-') {
        printf("page count must be a number > 0\n");
        exit(1);
    }
    // Opening pageref file
    file = fopen(argv[3], "r");
    if(file == NULL) {
        printf("page ref error: %s\n", strerror(errno));
        exit(1);
    }
    // algorithm check
    if(!strcmp("FIFO", argv[1]))  alg = FIFO;
    if(!strcmp("LRU", argv[1])) alg = LRU;


    if(alg == 0) {
        // Failed algorithm check
        printf("usage: ./simulate [ALGORITHM] [PAGE COUNT] [PAGEREF]\n");
        exit(1);
    }

}
