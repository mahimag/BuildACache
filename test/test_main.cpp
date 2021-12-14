#include <gtest/gtest.h>

extern "C" {
#include "cache.h"
}

store_t store;
cache_t cache;
void LocalSetup(int store_size, int cache_size, policy_t policy) {
    // Create a store
    store_init(&store, store_size);

    // Initialize the pages to have a known pattern
    char val = 0;
    for (int i = 0; i < store_size; i++) {
        page_t* page = store_get(&store, i * PAGESIZE);
        for (int j = 0; j < PAGESIZE; j++) {
            page_set(page, j, (char) val++);
        }
    }
    // Create a cache
    cache_init(&cache, cache_size, &store, policy);
}

unsigned char ExpectedChar(int location) {
    return location;
}

TEST(Cache, Creating) {
    LocalSetup(16, 4, FIFO);
}

TEST(Cache, FIFO) {
    LocalSetup(16, 4, FIFO);
    int latency = 0;
    cache_get(&cache, 12, &latency);    // page 3
    printf("LATENCY: %d\n", latency);
    ASSERT_EQ(STORE_LATENCY, latency);
    cache_get(&cache, 13, &latency); 
    ASSERT_EQ(CACHE_LATENCY, latency); 
    cache_get(&cache, 16, &latency);    // page 4
    cache_get(&cache, 22, &latency);    // page 5
    cache_get(&cache, 27, &latency);    // page 6

    ASSERT_TRUE(cache_contains(&cache, 3));
    ASSERT_TRUE(cache_contains(&cache, 4));
    ASSERT_TRUE(cache_contains(&cache, 5));
    ASSERT_TRUE(cache_contains(&cache, 6));
    ASSERT_FALSE(cache_contains(&cache, 1));

    unsigned char val = cache_get(&cache, 35, &latency); // page 8
    ASSERT_TRUE(cache_contains(&cache, 8)); // it does contain page 8
    ASSERT_FALSE(cache_contains(&cache, 3));
    ASSERT_EQ(ExpectedChar(35), val);

    cache_get(&cache, 36, &latency);
    ASSERT_TRUE(cache_contains(&cache, 9));
    ASSERT_FALSE(cache_contains(&cache, 4));
}

TEST(Cache, RANDOM) {
    LocalSetup(16, 4, FIFO);
}

TEST(Cache, LRU) {
    LocalSetup(16, 4, LRU);

    int latency = 0;
    cache_get(&cache, 12, &latency);    // page 3
    ASSERT_EQ(STORE_LATENCY, latency);
    cache_get(&cache, 13, &latency);
    ASSERT_EQ(CACHE_LATENCY, latency);
    cache_get(&cache, 16, &latency);    // page 4
    cache_get(&cache, 22, &latency);    // page 5
    cache_get(&cache, 27, &latency);    // page 6

    ASSERT_TRUE(cache_contains(&cache, 3));
    ASSERT_TRUE(cache_contains(&cache, 4));
    ASSERT_TRUE(cache_contains(&cache, 5));
    ASSERT_TRUE(cache_contains(&cache, 6));
    ASSERT_FALSE(cache_contains(&cache, 1));
    //cache - 3 4 5 6 // 3 is LRU
    unsigned char val = cache_get(&cache, 13, &latency);          // page 3
    ASSERT_EQ(ExpectedChar(13), val);
    //cache - 3 4 5 6 // 4 becomes LRU
    cache_get(&cache, 35, &latency);          // page 8
    ASSERT_TRUE(cache_contains(&cache, 8));
    ASSERT_FALSE(cache_contains(&cache, 4));
    //cache - 3 8 5 6
    cache_get(&cache, 36, &latency);
    ASSERT_TRUE(cache_contains(&cache, 9));   // page 9
    ASSERT_FALSE(cache_contains(&cache, 5));
    //cache - 3 8 9 6
}

TEST(Cache, MRU) {
    LocalSetup(16, 4, MRU);

    int latency = 0;
    cache_get(&cache, 12, &latency);    // page 3
    ASSERT_EQ(STORE_LATENCY, latency);
    cache_get(&cache, 13, &latency);
    ASSERT_EQ(CACHE_LATENCY, latency);
    cache_get(&cache, 16, &latency);    // page 4
    cache_get(&cache, 22, &latency);    // page 5
    unsigned char val = cache_get(&cache, 27, &latency);    // page 6
    ASSERT_EQ(ExpectedChar(27), val); //works!

    ASSERT_TRUE(cache_contains(&cache, 3));
    ASSERT_TRUE(cache_contains(&cache, 4));
    ASSERT_TRUE(cache_contains(&cache, 5));
    ASSERT_TRUE(cache_contains(&cache, 6));
    ASSERT_FALSE(cache_contains(&cache, 1));

    cache_get(&cache, 23, &latency);          // page 5 
    ASSERT_EQ(CACHE_LATENCY, latency);
    cache_get(&cache, 35, &latency);          // page 8
    ASSERT_EQ(STORE_LATENCY, latency);
    ASSERT_TRUE(cache_contains(&cache, 8));
    ASSERT_FALSE(cache_contains(&cache, 5));
//cache - 3 4 8 6
    cache_get(&cache, 36, &latency);          //page 9
    ASSERT_EQ(STORE_LATENCY, latency);
    ASSERT_TRUE(cache_contains(&cache, 9));   // page 9
    ASSERT_FALSE(cache_contains(&cache, 8));
//cache - 3 4 9 6
}


int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
