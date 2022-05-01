/* Ryan Paulos and Brandon Huang    
 *  CS 460 Project 4
 *
 *
 *   read_op is responsible for shifting the linked list around to represent the correct LRU state
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>


#define FIFO 1
#define LRU 2


void print_stats();
void vet_args(int, char *[], FILE **);
void process(FILE *file);
void add(unsigned int);
void load(unsigned int);
void evict();
void read_op(unsigned int);
void write_op(unsigned int);
void print_list();

struct pageNode {
    struct pageNode *prev_node;
    struct pageNode *nxt_node;
    int dirty;
    unsigned int page;
};

// page/cache info  
struct pageNode *page_head = NULL;
unsigned int max_pages = 0;  // Updated when parsing args
unsigned int page_count = 0;  // pages currently in RAM.  Add(), evict()
unsigned int page_refs = 0;  // Rows in the pageref text basically. process()
unsigned int page_misses = 0;  // add()
unsigned int pm_time = 0;  // Page miss cumulative time. add()
unsigned int wb_time = 0;  // write back cumulative time. evict()
int alg = 0;  // Updatd when parsing args


int main(int argc, char *argv[]) {

    // Process args
    FILE *file = NULL;
    vet_args(argc, argv, &file);
    //printf("max count: %u\n", max_pages);
    // Args valid. Beginning
    process(file);
    //print_list();
    //printf("This is the current wb_time: %u\n", wb_time);
    // Flushing remaining dirty pages
    while(page_head != NULL) {
        evict();
    }
    fclose(file);
    // Printing metadata 
    print_stats();
}

void print_stats() {
    printf("Page references: %u\n", page_refs);
    printf("Page misses: %u\n", page_misses);
    printf("Page miss time units: %u\n", pm_time);
    printf("Write back time units: %u\n", wb_time);
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
    // Updates page_refs

    char *line = NULL;
    size_t linelen = 20;
    while((getline(&line, &linelen, file)) != -1) {
        // Read a line from the pageref file
        //printf("read new line\n");
        char mode; // r or w
        unsigned int page; 
        int ret = sscanf(line, "%c %u", &mode, &page);
        if(ret == 2) {
            page_refs++;
        }
        // Mode and page number retrieved. Now perform the action..

        if(mode == 'R') {
            //printf("%c %u\n", mode, page);
            read_op(page);
        }
        else if(mode == 'W') {
            //printf("%c %u\n", mode, page);
            write_op(page);
        }
        else {
            //printf("neither_mode"); 
        }
        //print_list();
        free(line);
        line = NULL;
    }
    free(line);
}

struct pageNode *new_pn(unsigned int page) {
    struct pageNode *pn = malloc(sizeof(struct pageNode));
    pn->page = page;
    pn->dirty = 0;
    return pn;
}

int cached(unsigned int page) {
    // Returns 1 if the given page is in memory, else 0
    //printf("Analyze cached values\n");
    struct pageNode *pn = page_head;
    while(pn != NULL) {
        if(pn->page == page) {
            //printf("cached: %u\n", page);
            return 1;
        }
        //printf("Cached: %u\n", pn->page);
        pn = pn->nxt_node;
    }
    //printf("not cached: %u\n", page);
    return 0;
}

void load(unsigned int page) {
    // Pulls the requested page from disk to RAM.
    // Updates time_units, page_misses 
    //
    //printf("loading\n");
    if(page_count == max_pages) {
        evict();
    }
    add(page);
    page_misses++;
    pm_time = pm_time + 5;
}

void add(unsigned int page) {
    // Caller checks if there is room for the addition
    // Newest node goes at the end of the list (furthest from page_head)
    // Updates page_count
      
    //printf("in add\n");    
    if(page_head == NULL) {
        page_head = new_pn(page);
        page_head->prev_node = NULL;
        page_head->nxt_node = NULL;
        page_count++;
    }
    //else if(alg == FIFO) {
    else if(alg == alg) {
        if(page_count == max_pages) return; 
        //printf("BEFORE NEW NODE node\n");
        //print_list(); 
        struct pageNode *tmp_node = page_head;
        while(tmp_node->nxt_node != NULL) {
            tmp_node = tmp_node->nxt_node;
        }
        struct pageNode *new_node = new_pn(page);
        new_node->prev_node = tmp_node;
        new_node->nxt_node = NULL;
        tmp_node->nxt_node = new_node; 
        page_count++;
        //printf("AFTER NEW NODE node\n");
        //print_list(); 
    }
}

void evict() {
    // Evicts the obsolete page based on the algorithm
    // Caller's responsibility to check if there is something to evict
    // Updates wb_time and page_count
    //printf("Evicting\n");
    //if(alg == FIFO) {
    if(alg == alg) {
        if(page_head->dirty) {
            wb_time = wb_time + 10;
            //printf("Evicting %u with bit set: %d\n", page_head->page, page_head->dirty);
        }
        // Cutting head
        if(page_head->nxt_node == NULL) {
            free(page_head);
            page_head = NULL;
            
        }
        else {
            struct pageNode *new_head = page_head->nxt_node;
            free(page_head);
            new_head->prev_node = NULL;
            page_head = new_head;
           
        }
        page_count--;
    }
}

void read_op(unsigned int page) {
    if(alg == FIFO) {
        if(cached(page)) return;
        // Page miss
        load(page);
    }
    else if(alg == LRU) {
        if(cached(page)) {
            // Have to rearrange the pages. page_head == oldest
            if(max_pages == 1) return;   // No rearranging necessary

            struct pageNode *tmp = page_head;
            // Extract current node from the list 
            while(1) {
                if(tmp->page == page) {
                    if(tmp->prev_node != NULL) {
                        tmp->prev_node->nxt_node = tmp->nxt_node;
                        //printf("page: %d, prev_node not null\n", page);
                    }
                    if(tmp->nxt_node != NULL) {
                        tmp->nxt_node->prev_node = tmp->prev_node;
                        //printf("page: %d, next_node not null\n", page);
                    }
                    break;
                }
                tmp = tmp->nxt_node;
            }   

            // Appending to end of list
            struct pageNode *end = page_head;
            while(end->nxt_node != NULL) {
                end = end->nxt_node;
            }
            //printf("end page: %d\n", end->page);
            
            if(end->page == tmp->page) { // If the current node is also the last node, no updates are needed
                return;
            }
            if(tmp->page == page_head->page) {  // Need to update the page head
                page_head = tmp->nxt_node;
            }
            // Appending
            end->nxt_node = tmp;
            tmp->prev_node = end;
            tmp->nxt_node = NULL;
        }
        else {
            // Page miss
            load(page);
        }
    }
}

void write_op(unsigned int page) {
    //printf("In write op\n");
    if(cached(page)) {
        // Marking dirty bit
        struct pageNode *tmp = page_head;
       // printf("About to be clever\n");
        while(tmp->page != page) tmp = tmp->nxt_node;
        //printf("Done being clever\n");
        tmp->dirty = 1;
    }
    else {  // page is not in RAM
        //printf("Write: Not cached\n");
        load(page);
        write_op(page);
    }
}

void print_list() {
    struct pageNode *node = page_head;
    int i = 0;
    while(node != NULL) {
        printf("-----node %d: ", i);
        printf(",page %d: ", node->page);
        printf(", dirty: %d", node->dirty);
        if(node->prev_node != NULL) {
            printf("prev page: %d", node->prev_node->page);
        }
        else {
            printf("prev page NULL");
        }
        if(node->nxt_node != NULL) {
            printf(", next page: %d", node->nxt_node->page);
        }
        else {
            printf("Nxt page NULL");
        }
        printf("\n");    
        i++;
        node = node->nxt_node; 
    }
}
