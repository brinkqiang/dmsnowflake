
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

 // �ṹΪ��
 //
 // 0---0000000000 0000000000 0000000000 0000000000 0 --- 00000 ---00000 ---0000000000 00
 // ��������ַ����У���һλΪδʹ�ã�ʵ����Ҳ����Ϊlong�ķ���λ������������41λΪ���뼶ʱ�䣬Ȼ��5λdatacenter��ʶλ��
 // 5λ����ID���������ʶ����ʵ����Ϊ�̱߳�ʶ����Ȼ��12λ�ú����ڵĵ�ǰ�����ڵļ������������պ�64λ��Ϊһ��Long�͡�
 //
 // �����ĺô��ǣ������ϰ���ʱ���������򣬲��������ֲ�ʽϵͳ�ڲ������ID��ײ����datacenter�ͻ���ID�����֣���
 // ����Ч�ʽϸߣ������ԣ�snowflakeÿ���ܹ�����26��ID���ң���ȫ������Ҫ��
 //
class IdWorkerUnThreadSafe {
public:
    // TODO(@benqi): ���worker_id��data_center_id
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