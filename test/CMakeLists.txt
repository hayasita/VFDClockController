cmake_minimum_required(VERSION 3.14)
project(gtestSub)

# GoogleTestはC++11で動かす必要がある
set(CMAKE_CXX_STANDARD 11)

# cmakeのFetchContent機能をinclude
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
# 上記で ./build/_deps 以下にgoogletestのソースとバイナリが入る。
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# テスト有効化
enable_testing()

add_executable(
    #Target name
    test000
    #Source file
    test000.cpp
)

target_link_libraries(
    #Target name
    test000

    #libraries
    func
    driver_sub
    gtest_main
)

add_executable(
    #Target name
    test_driver_sub
    #Source file
    test_driver_sub.cpp
)

target_link_libraries(
    #Target name
    test_driver_sub

    #libraries
    driver_sub
    gtest_main
)


add_executable(
    #Target name
    test_mode_ctrl
    #Source file
    test_mode_ctrl.cpp
)

target_link_libraries(
    #Target name
    test_mode_ctrl

    #libraries
    mode_ctrl
    gtest_main
)

# mock
add_executable(
    test_monitor
    test_monitor.cpp
)
target_link_libraries(
    test_monitor
    monitor
    gmock_main
    gtest_main
)


add_subdirectory(./main)

include(GoogleTest)
gtest_discover_tests(test000)
