#include <stdlib.h>
#include "store.h"
#include <stdio.h>

// Initialize a memory store with the given number of pages
// This creates numpages of Pages in the store and initializes
// the content of each Page to 0
void store_init(store_t* store, int numpages) {
    store->numpages = numpages; //num of pages assigned to numpages initally
    //initialize contents of each page to 0
    store->pages = calloc(numpages, sizeof(page_t)); //allocate mem for _ # of pages
    //memset(store->pages, 0, numpages * sizeof(page_t*)); // setting each to 0
    for(int i=0; i<numpages; i++){
        page_t* pg = &store->pages[i]; //pointer to page i
        store->pages[i].pageno = i; // setting each pageno to i
        int pgnum = page_pageno(&store->pages[i]); //page num at pages[i]
        //printf("pgnum is %d\n", pgnum); //print it out
        page_init(pg, pgnum, pg->bytes); // initializing bytes in page
    }
}

// Return the page corresponding to the given memory location
page_t* store_get(store_t* store, int location) { 
    //int num = store->pages->pageno;

    int index = location / PAGESIZE;
    page_t* page = &store->pages[index];
    return page;

    /*int pages = store->numpages;
    for(int i=0; i<pages; i++){
        int target = i * PAGESIZE; // targetting the location
        //location is 8 aka bytes[2]
        page_t* result = &store->pages[target];
        if(location == target){
            return result;
        }
    }
    return NULL;
    */
}