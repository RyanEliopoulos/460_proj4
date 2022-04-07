#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>


#define FIFO 1
#define LRU 2

void vet_args(int, char *[], FILE **);
void process(FILE *file);



unsigned int max_pages = 0;
unsigned int page_count = 0;
int alg = 0;


int main(int argc, char *argv[]) {

    // Process args
    FILE *file = NULL;
    vet_args(argc, argv, &file);
    // Args valid. Beginning
    process(file);
   
}

void vet_args(int argc, char *argv[], FILE **file) {

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
    *file = fopen(argv[3], "r");
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


void process(FILE *file) {
    // Read each line of the pageref file 
    // and take action as needed

    char *line = NULL;
    size_t linelen = 20;
    while((getline(&line, &linelen, file)) != -1) {
        printf("here\n");
        char mode; // r or w
        unsigned int page; 
        int ret = sscanf(line, "%c %u", &mode, &page);
        if(ret != 2) {
            printf("ret does not equal 2\n");
        }
        else {
            printf("%c: %u\n", mode, page);
        }        
        free(line);
        line = NULL;
    }
    printf("here though\n");
}
