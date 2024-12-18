#pragma once
#include "testcase.h"
#include "util.h"

namespace testcase_mmap_tls {
thread_local int pid = 0;
}


struct testcase_mmap : testcase {
    const int mmap_mem_len = 64;

    virtual ~testcase_mmap()
    {
        if (mmap_mem) {
            os::memory_free(mmap_mem, sizeof(int) * mmap_mem_len);
        }
    }

    virtual bool parent_init() override
    {
        mmap_mem = static_cast<int*>(os::memory_alloc(sizeof(int) * mmap_mem_len));
        for (int i = 0; i < mmap_mem_len; ++i) {
            mmap_mem[i] = os::getpid();
        }
        testcase_mmap_tls::pid = os::getpid();
        return true;
    }
    virtual bool parent_tick() override
    {
        test_mem();
        return true;
    }

    virtual bool child_init() override
    {
        if (!testcase::child_init()) {
            return false;
        }

        for (int i = 0; i < mmap_mem_len; ++i) {
            mmap_mem[i] = os::getpid();
        }

        if (testcase_mmap_tls::pid != os::getpid()) {
            // log("info", "tls pid=%d", testcase_mmap_tls::pid);
        }
        return true;
    }

    virtual bool child_tick() override
    {
        if (!testcase::child_tick()) {
            return false;
        }

        test_mem();
        return true;
    }

    void test_mem()
    {
        for (int i = 0; i < mmap_mem_len; ++i) {
            auto v = mmap_mem[i];
            if (v != os::getpid()) {
                log("error", "mmap_mem %d=%d", i, v);
                return;
            }
        }
        log("info", "mmap_mem ok");
    }

    int* mmap_mem = nullptr;
};