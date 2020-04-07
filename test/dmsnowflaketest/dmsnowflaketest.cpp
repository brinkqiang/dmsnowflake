
#include "dmsnowflake.h"

#include "gtest.h"

#include <unordered_set>

TEST(CDMIDGenerator, CDMIDGenerator) {
    CDMIDGenerator oID;

    int c = 1000000;
    std::unordered_set<uint64_t> keys;
    for (int i = 0; i < c; ++i)
    {
        uint64_t id = oID.GetNextID();

        auto it = keys.find(id);
        ASSERT_TRUE(it == keys.end());
        keys.insert(id);
    }
}


