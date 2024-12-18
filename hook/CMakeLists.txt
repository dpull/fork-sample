get_filename_component(CURRENT_SOURCE_DIR_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
set(LIB_NAME ${CURRENT_SOURCE_DIR_NAME}) # 工程名, 默认文件夹名

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC -fvisibility=hidden -Wl,--exclude-libs=ALL -ffunction-sections")
add_definitions(-D_GNU_SOURCE) 

file(GLOB_RECURSE SOURCE_FILES 
    *.h
    *.cpp
    *.cc
    *.c
)
add_library(${LIB_NAME} SHARED ${SOURCE_FILES})

if(LINUX)
    target_link_libraries(${LIB_NAME} dl)
    target_link_libraries(${LIB_NAME} pthread)
endif(LINUX)
