
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

#ifndef __SNOWFLAKE2_H_INCLUDE__
#define __SNOWFLAKE2_H_INCLUDE__

#include <cstdint>
#include <chrono>
#include <stdexcept>
#include <mutex>

class snowflake_nonlock
{
public:
	void lock()
	{
	}
	void unlock()
	{
	}
};

template<typename TLock = snowflake_nonlock>
class snowflake
{
	using lock_type = TLock;
	static constexpr int64_t TWEPOCH = 1534832906275L;
	static constexpr int64_t WORKER_ID_BITS = 5L;
	static constexpr int64_t DATACENTER_ID_BITS = 5L;
	static constexpr int64_t MAX_WORKER_ID = -1L ^ (-1L << WORKER_ID_BITS);
	static constexpr int64_t MAX_DATACENTER_ID = -1L ^ (-1L << DATACENTER_ID_BITS);
	static constexpr int64_t SEQUENCE_BITS = 12L;
	static constexpr int64_t WORKER_ID_SHIFT = SEQUENCE_BITS;
	static constexpr int64_t DATACENTER_ID_SHIFT = SEQUENCE_BITS + WORKER_ID_BITS;
	static constexpr int64_t TIMESTAMP_LEFT_SHIFT = SEQUENCE_BITS + WORKER_ID_BITS + DATACENTER_ID_BITS;
	static constexpr int64_t SEQUENCE_MASK = -1L ^ (-1L << SEQUENCE_BITS);

	using time_point = std::chrono::time_point<std::chrono::steady_clock>;

	time_point start_time_point_ = std::chrono::steady_clock::now();
	int64_t start_millsecond_ = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

	int64_t last_timestamp_ = -1;
	int64_t workerid_ = 0;
	int64_t datacenterid_ = 0;
	int64_t sequence_ = 0;
	lock_type lock_;
public:
	snowflake(){}
	snowflake(const snowflake&) = delete;

	snowflake& operator=(const snowflake&) = delete;

	void init(int64_t workerid, int64_t datacenterid)
	{
		if (workerid > MAX_WORKER_ID || workerid < 0) {
			throw std::runtime_error("worker Id can't be greater than 31 or less than 0");
		}

		if (datacenterid > MAX_DATACENTER_ID || datacenterid < 0) {
			throw std::runtime_error("datacenter Id can't be greater than 31 or less than 0");
		}

		workerid_ = workerid;
		datacenterid_ = datacenterid;
	}

	int64_t nextid()
	{
		std::unique_lock<lock_type> lock(lock_);
		//std::chrono::steady_clock  cannot decrease as physical time moves forward
		auto timestamp = millsecond();
		if (last_timestamp_ == timestamp)
		{
			sequence_ = (sequence_ + 1) & SEQUENCE_MASK;
			if (sequence_ == 0)
			{
				timestamp = wait_next_millis(last_timestamp_);
			}
		}
		else
		{
			sequence_ = 0;
		}

		last_timestamp_ = timestamp;

		return ((timestamp - TWEPOCH) << TIMESTAMP_LEFT_SHIFT)
			| (datacenterid_ << DATACENTER_ID_SHIFT)
			| (workerid_ << WORKER_ID_SHIFT)
			| sequence_;
	}

private:
	int64_t millsecond() const noexcept
	{
		auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start_time_point_);
		return start_millsecond_ + diff.count();
	}

	int64_t wait_next_millis(int64_t last) const noexcept
	{
		auto timestamp = millsecond();
		while (timestamp <= last)
		{
			timestamp = millsecond();
		}
		return timestamp;
	}
};

#endif // __SNOWFLAKE2_H_INCLUDE__