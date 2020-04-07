
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

#ifndef __DMSNOWFLAKE_H_INCLUDE__
#define __DMSNOWFLAKE_H_INCLUDE__

#include <stdint.h>
#include <mutex>

 // 结构为：
 //
 // 0---0000000000 0000000000 0000000000 0000000000 0 --- 00000 ---00000 ---0000000000 00
 // 在上面的字符串中，第一位为未使用（实际上也可作为long的符号位），接下来的41位为毫秒级时间，然后5位datacenter标识位，
 // 5位机器ID（并不算标识符，实际是为线程标识），然后12位该毫秒内的当前毫秒内的计数，加起来刚好64位，为一个Long型。
 //
 // 这样的好处是，整体上按照时间自增排序，并且整个分布式系统内不会产生ID碰撞（由datacenter和机器ID作区分），
 // 并且效率较高，经测试，snowflake每秒能够产生26万ID左右，完全满足需要。
 //
class IdWorkerUnThreadSafe {
public:
    // TODO(@benqi): 检查worker_id和data_center_id
    IdWorkerUnThreadSafe(uint16_t worker_id, uint16_t data_center_id)
        : worker_id_(worker_id),
        data_center_id_(data_center_id) {}

    uint64_t GetNextID();

protected:
    uint16_t worker_id_{ 0 };
    uint16_t data_center_id_{ 0 };
    uint64_t last_timestamp_{ 0 };
    uint32_t sequence_{ 0 };
};

class IdWorkerThreadSafe {
public:
    IdWorkerThreadSafe(uint16_t worker_id, uint16_t data_center_id)
        : id_worker_(worker_id, data_center_id) {}

    uint64_t GetNextID() {
        std::lock_guard<std::mutex> g(lock_);
        return id_worker_.GetNextID();
    }

protected:
    IdWorkerUnThreadSafe id_worker_;
    std::mutex lock_;
};

#endif // __DMSNOWFLAKE_H_INCLUDE__