#pragma once
#include "testcase.h"

struct testcase_signal : testcase {
    virtual ~testcase_signal()
    {
    }

    virtual bool parent_init() override
    {
        if (!testcase::parent_init()) {
            return false;
        }

        return true;
    }

    virtual bool parent_tick() override
    {
        if (!testcase::parent_tick()) {
            return false;
        }

        return true;
    }

    virtual bool child_init() override
    {
        if (!testcase::child_init()) {
            return false;
        }
        return true;
    }

    virtual bool child_tick() override
    {
        if (!testcase::child_tick()) {
            return false;
        }
        return true;
    }
};