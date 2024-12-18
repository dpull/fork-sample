#pragma once
#include "testcase.h"
#include "tcp_client.h"

struct testcase_tcp : testcase {
    virtual ~testcase_tcp()
    {
        if (!is_child()) {
            client.close(true);
        }
    }

    virtual bool parent_init() override
    {
        if (!testcase::parent_init()) {
            return false;
        }

        if (!client.connect("127.0.0.1", 8241)) {
            log("error", "connect server failed");
            return false;
        }
        return true;
    }

    virtual bool parent_tick() override
    {
        if (!testcase::parent_tick()) {
            return false;
        }

        client.update(false);
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
        client.update(true);
        return true;
    }

    tcp_client client;
};