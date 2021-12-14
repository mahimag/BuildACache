#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <limits.h>
#include "cache.h"

// Initialize a cache of the given size number of pages.
// The cache is backed by the store and is governed with the given cache
// replacement policy.
void cache_init(cache_t* cache, int size, store_t* store, policy_t policy) {
    cache->size = size;
    cache->store = store;
    cache->policy = policy;
    cache->entries = calloc(size, sizeof(entry_t)); //allocate mem for each entry (same size as page)

    cache->entries->count = 0; // metadata
    cache->entries->old = 0;

    for(int i=0; i<size; i++){ // loop thru entries
        cache->entries[i].index = i; //set last accessed index to 0 initially
        cache->entries[i].page = calloc(1, sizeof(page_t)); // allocate mem for page in each entry
        cache->entries[i].page = NULL;
    }
}

// Return the value in memory corresponding to the given memory location.
// This function looks up whether the page exists in the cache first.
// If it does, it retrieves the Page from the cache, and updates the cache,
// and returns the value.
// If the Page is not present, it goes to the store to retrieve the page,
// updates the cache and returns the value.
// The time required for the get is set in the pointer latency.
unsigned char cache_get(cache_t* cache, int memory_location, int* latency) {

    int policy = cache->policy;
    printf("FIFO is: %d\n", policy);
    int size = cache->size;
    printf("size is: %d\n", size);

    //does page exist in cache
    int pagenum = memory_location / PAGESIZE;
    printf("pagenum is %d\n", pagenum);
    bool check = cache_contains(cache, pagenum); // does page # exist in cache already
    if(check){ // if page exists in cache
        printf("check goes thru\n");

        cache->entries->index--;
        int index = cache->entries->index;
        printf("index is %d\n", index);

        //retrieve page from cache
        page_t pg = cache->entries[index].page[0]; 
        int offset = (memory_location % PAGESIZE);
        *latency = CACHE_LATENCY;

        for(int i=0; i<size; i++){ //check which entry has pagenum
            if(cache->entries[i].page != NULL){
                if(cache->entries[i].page->pageno == pagenum){
                    cache->entries[i].count = 1;
                    cache->entries[i].old = size;
                    pg = cache->entries[i].page[0];
                }
                else{
                    cache->entries[i].count = 0;
                    if(cache->entries[i].old != 1){
                        cache->entries[i].old = (cache->entries[i].old)-1;
                    }
                }
            }
        }
        
        printf("count : %d\n", cache->entries[index].count);
        cache->entries->index++;
        printf("index is %d\n", cache->entries->index);

        return page_get(&pg, offset);
    }
    else{
        printf("check doesnt go thru\n");

        int index = cache->entries->index; // initially 0
        printf("index is %d\n", index);
        //updates cache

        int save = 0; //saving page num

        for(int i=0; i<size; i++){
            if(cache->entries[i].old == 1 && policy == 1){
                save = i;
                printf("save : %d\n", save);
            }
            if(cache->entries[i].count == 1 && policy == 2){
                //save = &(cache->entries[i].page); //saving last used
                save = i;
                printf("save : %d\n", save);
            }
            cache->entries[i].count = 0;
        }
        cache->entries[index].count = 1; //increment count of entry 0
        cache->entries[index].old = (index % size)+1; // 0 % 4
        printf("count : %d\n", cache->entries[index].count);
        printf("old : %d\n", cache->entries[index].old);

        if(!isFull(cache)){ //if cache is not full of pages
            //page_t* pg = cache->entries[index].page;
            cache->entries[index].page = &cache->store->pages[pagenum];
            cache->entries->index++;
            printf("index : %d\n", cache->entries->index);
        }
        else{
            //implement FIFO policy - if policy == 0
            if(policy == 0){
                printf("DOING FIFO METHOD\n");
                cache->entries[0].page = cache->entries[1].page; //p4 replaces p3
                cache->entries[1].page = cache->entries[2].page; //p5 replaces p4
                cache->entries[2].page = cache->entries[3].page; //p6 replaces p5
                cache->entries[3].page = &cache->store->pages[pagenum]; //p8 replaces 
            }
            else if(policy == 1){
                printf("DOING LRU METHOD\n");
                cache->entries[save].page = &cache->store->pages[pagenum];
                for(int i = 0; i<size; i++){
                    if(i == save){
                        cache->entries[i].old = size;
                    }
                    else{
                        cache->entries[i].old = (cache->entries[i].old)-1;
                    }
                }
              
            }
            else if(policy == 2){
                printf("DOING MRU METHOD\n");
                cache->entries[save].page = &cache->store->pages[pagenum]; 
                cache->entries[save].count = 1;
            
            }
            else{
                printf("INVALID POLICY\n");
        
            }
        }

        // return page 3 from store
        page_t* pg = &cache->store->pages[pagenum];
        int offset = (memory_location % PAGESIZE);
        *latency = STORE_LATENCY;
        return page_get(pg, offset);
    }
    return 0;
}

// Return true if the page with page_no is present in the cache
bool cache_contains(cache_t* cache, int page_no) {

    int size = cache->size; // 4 if full
    bool flag = false;

    for(int i=0; i<size; i++){
        if(cache->entries[i].page != NULL){ //does page exist
            if(cache->entries[i].page->pageno == page_no){ //is this page equal to page 3 
                flag = true;
            }
        }
    }
    return flag;
    /*
    if(cache->entries[page_no].page != NULL){ //if page exists
        return true;
    }
    else{
        return false;
    }
    */
}

bool isFull(cache_t* cache){
    int size = cache->size;
    bool flag = true;
    for(int i=0; i<size; i++){
        if(cache->entries[i].page == NULL){
            flag = false;
        }
    }
    return flag;
}