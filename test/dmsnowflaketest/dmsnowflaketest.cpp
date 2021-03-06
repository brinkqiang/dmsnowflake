
#include "dmsnowflake.hpp"

#include "gtest.h"

#include <unordered_set>

int gen_count = 1000000;

TEST(dmsnowflake_h, dmsnowflake_h) {
	CDMIDGenerator id_gen(0, 0);

	std::unordered_set<uint64_t> keys;
	for (int i = 0; i < gen_count; ++i)
	{
		uint64_t id = id_gen.GetNextID();

		auto it = keys.find(id);
		ASSERT_TRUE(it == keys.end());
		keys.insert(id);
	}
	ASSERT_TRUE(keys.size() == gen_count);
}
