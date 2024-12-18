get_filename_component(CURRENT_SOURCE_DIR_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
set(APP_NAME ${CURRENT_SOURCE_DIR_NAME}) # 工程名, 默认文件夹名

file(GLOB_RECURSE SOURCE_FILES 
    *.h
    *.cpp
    *.cc
    *.c
)

include_directories(${CMAKE_SOURCE_DIR})

add_executable(${APP_NAME} ${SOURCE_FILES})
set_target_properties(${APP_NAME} PROPERTIES LINK_FLAGS -rdynamic)

if(APPLE)
    target_link_libraries(${APP_NAME} "-framework CoreFoundation")
endif(APPLE)
