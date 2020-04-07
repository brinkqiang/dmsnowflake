
// Copyright (c) 2018 brinkqiang (brink.qiang@gmail.com)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "dmsnowflake.h"
#include "dmos.h"

static inline uint32_t GetTickCount32() {
    //auto now = std::chrono::system_clock::now();
    //auto now = std::chrono::steady_clock::now();
    //auto now = std::chrono::high_resolution_clock::now();
    //return std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
#ifdef _MSC_VER
    return ::GetTickCount();
#else
    struct timespec ts = { 0 };
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
#endif
}


uint64_t WaitUntilNextMillis(uint64_t last_timestamp) {
    uint64_t timestamp = GetTickCount32();
    while (timestamp <= last_timestamp) {
        timestamp = GetTickCount32();
    }
    return timestamp;
}


uint64_t IdWorkerUnThreadSafe::GetNextID() {
    uint64_t timestamp = GetTickCount32();

    // 在当前秒内
    if (last_timestamp_ == timestamp) {
        sequence_ = (sequence_ + 1) & 0xFFF;
        if (sequence_ == 0) {
            timestamp = WaitUntilNextMillis(last_timestamp_);
        }
    }
    else {
        sequence_ = 0;
    }

    last_timestamp_ = timestamp;
    return ((timestamp & 0x1FFFFFF) << 22 |
        (data_center_id_ & 0x1F) << 17 |
        (worker_id_ & 0x1F) << 12 |
        (sequence_ & 0xFFF));

}

