
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

#ifndef __SNOWFLAKE_H_INCLUDE__
#define __SNOWFLAKE_H_INCLUDE__

#include <cstdint>
#include <mutex>
#include <chrono>
#include <cassert>

class id_generator_impl;

class id_generator
{
public:
	id_generator(int region_id = 0, int worker_id = 0);
	~id_generator();

	uint64_t get_nextid();

private:
	std::unique_ptr< id_generator_impl> m_oImpl;
};


// the timestamp in milliseconds of the start of the custom epoch
#define SNOWFLAKE_EPOCH 1388534400000 //Midnight January 1, 2014

#define SNOWFLAKE_TIME_BITS 41
#define SNOWFLAKE_REGIONID_BITS 4
#define SNOWFLAKE_WORKERID_BITS 10
#define SNOWFLAKE_SEQUENCE_BITS 8

typedef struct {
    // milliseconds since SNOWFLAKE_EPOCH
    uint64_t time;
    uint64_t seq_max;
    uint64_t worker_id;
    uint64_t region_id;
    uint64_t seq;
    uint64_t time_shift_bits;
    uint64_t region_shift_bits;
    uint64_t worker_shift_bits;
} snowflake_state;

typedef struct {
    time_t started_at;
    char* version;
    uint64_t ids;
    uint64_t waits;
    uint64_t seq_max;
    int region_id;
    int worker_id;
    uint64_t seq_cap;
} app_stats;

static inline uint64_t snowflake_gettime() {
  return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - SNOWFLAKE_EPOCH;
}

class id_generator_impl
{
public:
    id_generator_impl(int region_id, int worker_id) { 
        std::lock_guard guard(lock);
        snowflake_init(region_id, worker_id);
    }

    ~id_generator_impl() {}

    uint64_t get_nextid() {
        std::lock_guard guard(lock);
        return snowflake_id();
    }

private:
    uint64_t snowflake_id() {
        uint64_t millisecs = snowflake_gettime();
        uint64_t id = 0;

        // Catch NTP clock adjustment that rolls time backwards and sequence number overflow
        if ((snowflake_global_state.seq > snowflake_global_state.seq_max) || snowflake_global_state.time > millisecs) {
            ++app_stats.waits;
            while (snowflake_global_state.time >= millisecs) {
                millisecs = snowflake_gettime();
            }
        }

        if (snowflake_global_state.time < millisecs) {
            snowflake_global_state.time = millisecs;
            snowflake_global_state.seq = 0L;
        }


        id = (millisecs << snowflake_global_state.time_shift_bits)
            | (snowflake_global_state.region_id << snowflake_global_state.region_shift_bits)
            | (snowflake_global_state.worker_id << snowflake_global_state.worker_shift_bits)
            | (snowflake_global_state.seq++);

        if (app_stats.seq_max < snowflake_global_state.seq)
            app_stats.seq_max = snowflake_global_state.seq;

        ++app_stats.ids;
        return id;
    }


    int snowflake_init(int region_id, int worker_id) {
        const int max_region_id = (1 << SNOWFLAKE_REGIONID_BITS) - 1;
        if (region_id < 0 || region_id > max_region_id) {
            printf("Region ID must be in the range : 0-%d\n", max_region_id);
            assert(0);
            return -1;
        }
        const int max_worker_id = (1 << SNOWFLAKE_WORKERID_BITS) - 1;
        if (worker_id < 0 || worker_id > max_worker_id) {
            printf("Worker ID must be in the range: 0-%d\n", max_worker_id);
            assert(0);
            return -1;
        }

        snowflake_global_state.time_shift_bits = SNOWFLAKE_REGIONID_BITS + SNOWFLAKE_WORKERID_BITS + SNOWFLAKE_SEQUENCE_BITS;
        snowflake_global_state.region_shift_bits = SNOWFLAKE_WORKERID_BITS + SNOWFLAKE_SEQUENCE_BITS;
        snowflake_global_state.worker_shift_bits = SNOWFLAKE_SEQUENCE_BITS;

        snowflake_global_state.worker_id = worker_id;
        snowflake_global_state.region_id = region_id;
        snowflake_global_state.seq_max = (1L << SNOWFLAKE_SEQUENCE_BITS) - 1;
        snowflake_global_state.seq = 0L;
        snowflake_global_state.time = 0L;

        app_stats.seq_cap = snowflake_global_state.seq_max;
        app_stats.waits = 0L;
        app_stats.seq_max = 0L;
        app_stats.ids = 0L;
        app_stats.region_id = region_id;
        app_stats.worker_id = worker_id;
        return 1;
    }
private:
    snowflake_state snowflake_global_state;
    app_stats       app_stats;
    std::mutex      lock;
};


id_generator::id_generator(int region_id, int worker_id)
    : m_oImpl(new id_generator_impl(region_id, worker_id))
{

}

id_generator::~id_generator()
{
}

uint64_t id_generator::get_nextid()
{
    return m_oImpl->get_nextid();
}

#endif // __SNOWFLAKE_H_INCLUDE__