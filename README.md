
LD_PRELOAD=./build/hook/libhook.so ./build/client/client 

LD_PRELOAD=/root/code/fork-test/build/hook/libhook.so  ./XXXXXX -nothreading -ansimalloc 

addr2line -e XXXXXX -a 0xac6cee1