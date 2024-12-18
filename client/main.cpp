#include "testcase_mmap.h"
#include "testcase_tcp.h"
#include "util.h"
#include <cstdio>
#include <memory>

testcase* create(int argc, char* argv[])
{
    // return new testcase_mmap;
    return new testcase_tcp;
}

void fork_test(int argc, char* argv[])
{
    std::unique_ptr<testcase> test(create(argc, argv));
    if (!test) {
        log("error", "create testcase failed");
        return;
    }

    if (!test->parent_init()) {
        log("error", "testcase parent_init failed");
        return;
    }

    int child_count = 0;
    int child_count_limit = 4;

    while (true) {
        os::sleep(100);

        int term_pid = os::waitpid();
        if (term_pid != -1 && term_pid != 0) {
            child_count--;
            log("info", "child process terminal, pid=%d child_count=%d", term_pid, child_count);
        }

        if (!test->is_child()) {
            test->parent_tick();
        } else {
            test->child_tick();
        }

        if (child_count > child_count_limit) {
            continue;
        }

        int ret = os::fork();
        if (ret < 0) {
            log("error", "fork failed");
            break;
        }

        if (ret == 0) {
            test->child_init();
            continue;
        }

        log("info", "fork child process success, pid=%d child_count=%d", ret, child_count);
        child_count++;
    }
}

int main(int argc, char* argv[])
{
    if (!log_init()) {
        printf("open log file failed\n");
        return 1;
    }

    fork_test(argc, argv);

    log_close();
    return 0;
}
