#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "cache.h"
#include <ctype.h>
//prompt user to enter mem locations 

//exit(0);
//global variables
store_t store;
cache_t cache;
int first = 0;
int second = 0;
int third = 0;
int valS = 16;
int valC = 4;
int valP = 0;
int storesize = 16; //
int cachesize = 4; //
int policy = 0;

//function that initializes everything - identical to localSetUp inside test_main.cpp
void init(int store_size, int cache_size, int policy_no){
    //create a store
    store_init(&store, store_size);

    // Initialize the pages to have a known pattern
    char val = 0;
    for (int i = 0; i < store_size; i++) {
        page_t* page = store_get(&store, i * PAGESIZE);
        for (int j = 0; j < PAGESIZE; j++) {
            page_set(page, j, (char) val++);
        }
    }
    //initialize a cache
    cache_init(&cache, cache_size, &store, policy_no);
}

void usage(){
    printf("Usage: ./cache_sim [-s storesize] [-c cachesize] [-p policy]\n");
}

int main(int argc, char** argv) {
    // keep a counter of cache latency
    int total_latency = 0;

    int option, iflag, fflag, jflag = 0;

    while((option = getopt(argc, argv, "s:c:p:")) != -1) {
        switch(option){
            case 's':
                printf("ARGC: %d\n", argc);
                if(argc == 3){
                    valS = atoi(argv[2]);
                    //init(valS, 4, 0);
                    //exit(0);
                }
                else if(argc == 5 || argc == 7){
                    if(first == 0){
                        first = 1;
                        valS = atoi(argv[2]);
                
                    }
                    else if(second == 0){
                        second = 1;
                        valS = atoi(argv[4]);
                    }
                    else{
                        third = 1;
                        valS = atoi(argv[6]);
                    }
                }
                else{
                    //print message
                    usage();
                    exit(0);
                }
                iflag = 1;
                break;

            case 'c':
                printf("ARGC: %d\n", argc);
                if(argc == 3){
                    valC = atoi(argv[2]);
                    //init(16, valC, 0);
                    //exit(0);
                }
                else if(argc == 5 || argc == 7){
                    if(first == 0){
                        first = 1;
                        valC = atoi(argv[2]);
                    }
                    else if(second == 0){
                        second = 1;
                        valC = atoi(argv[4]);
                    }
                    else{
                        third = 1;
                        valC = atoi(argv[6]);
                    }
                }
                else{
                    //print message
                    usage();
                    exit(0);
                }
                fflag = 1;
                break;

            case 'p':
                //policy
                printf("ARGC: %d\n", argc);
                if(argc == 3){
                    valP = atoi(argv[2]);
                    //init(16, 4, valP);
                    //exit(0);
                }
                else if(argc == 5 || argc == 7){
                    if(first == 0){
                        first = 1;
                        char* temp = argv[2];
                        if(strcmp(temp, "fifo") == 0){
                            valP = FIFO; //atoi(argv[2]);
                        }
                        else if(strcmp(temp, "lru") == 0){
                            valP = LRU;
                        }
                        else if(strcmp(temp, "mru") == 0){
                            valP = MRU;
                        }
                        printf("valP isssss: %d\n", valP);
                    }
                    else if(second == 0){
                        second = 1;
                        valP = atoi(argv[4]);
                    }
                    else{
                        third = 1;
                        valP = atoi(argv[6]);
                    }
                }
                else{
                    //print message
                    usage();
                    exit(0);
                }
                jflag = 1;
                break;
            default:
                printf("this is default case.\n");
        }
        //init(valS, valC, valP);
    }
    //}
    printf("\n\n checking: %d %d %d \n", valS, valC, valP);
    init(valS, valC, valP); // (16, 4, 0)

    printf("\n\n\n\n");

    

    
    //read from the file
    FILE* fp;
    fp = fopen("test/test_input.txt", "r"); // 0 1
    char line[100];
    int latency = 0;
    while(!feof(fp)){ //goes through file until EOF
        fgets(line, 100, fp); //gets one line
        int val = atoi(line);
        //printf("val : %d\n", val);
        cache_get(&cache, val, &latency);
        printf("LATENCYYYYYYYYYY : %d\n", latency);
        total_latency+=latency;
    }
    

    // print out total latency at the end
    printf("Total latency=%d\n", total_latency);
}