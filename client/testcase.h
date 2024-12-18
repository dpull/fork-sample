#pragma once
#include <cassert>

struct testcase {
    virtual ~testcase() { }

    virtual bool parent_init()
    {
        child_loop_times = 0;
        return true;
    }

    virtual bool parent_tick()
    {
        assert(child_loop_times == 0);
        return true;
    }

    virtual bool child_init()
    {
        child_loop_times = 1;
        return true;
    }

    virtual bool child_tick()
    {
        child_loop_times++;
        return child_loop_times < 32;
    }

    bool is_child() { return child_loop_times != 0; }

    int child_loop_times = 0;
};
