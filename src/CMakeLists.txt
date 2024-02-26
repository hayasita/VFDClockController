set(SRC_FILES 
    #main.cpp
    func.cpp
    driver_sub.cpp
    # 追加のソースファイル
)

add_library(func func.cpp)

#driver_sub Test
add_library(driver_sub driver_sub.cpp)

add_library(mode_ctrl mode_ctrl.cpp)

add_library(
    monitor

    monitor.cpp
    monitor_serial.cpp
)
