
#include "snowflake.hpp"

#include "snowflake2.hpp"

#include "gtest.h"

#include <unordered_set>
#include <mutex>

TEST(snowflake_hpp, snowflake_hpp) {
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


TEST(snowflake2_hpp, snowflake2_hpp) {
	snowflake<std::mutex> id_gen;
	id_gen.init(0, 0);
	int c = 1000000;
	std::unordered_set<uint64_t> keys;
	for (int i = 0; i < c; ++i)
	{
		uint64_t id = id_gen.nextid();

		auto it = keys.find(id);
		ASSERT_TRUE(it == keys.end());
		keys.insert(id);
	}
}


