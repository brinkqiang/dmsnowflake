
#include "dmsnowflake.h"

#include "gtest.h"


TEST(IdWorkerUnThreadSafe, IdWorkerUnThreadSafe) {
    IdWorkerUnThreadSafe id(0, 0);

    int c = 1000000;

    for (int i = 0; i < c; ++i)
    {
        id.GetNextID();
    }
}

TEST(IdWorkerThreadSafe, IdWorkerThreadSafe) {
    IdWorkerThreadSafe id(0, 0);

    int c = 1000000;

    for (int i = 0; i < c; ++i)
    {
        id.GetNextID();
    }
}



