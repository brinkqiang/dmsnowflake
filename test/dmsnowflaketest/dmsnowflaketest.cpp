
#include "snowflake.hpp"

#include "gtest.h"

#include <unordered_set>

TEST(id_generator, id_generator) {
    id_generator id_gen;

    int c = 1000000;
    std::unordered_set<uint64_t> keys;
    for (int i = 0; i < c; ++i)
    {
        uint64_t id = id_gen.get_nextid();

        auto it = keys.find(id);
        ASSERT_TRUE(it == keys.end());
        keys.insert(id);
    }
}


