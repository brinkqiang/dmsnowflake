
#include "dmsnowflake.h"

#include "gtest.h"


TEST(CDMIDGenerator, CDMIDGenerator) {
    CDMIDGenerator oID;

    int c = 1000000;

    for (int i = 0; i < c; ++i)
    {
        oID.GetNextID();
    }
}



