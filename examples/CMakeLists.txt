include_directories(${TINYSM_INCLUDE_DIRS})

add_executable(led led.cpp)
add_executable(clock clock.cpp)
add_executable(elevator elevator.cpp)
add_executable(elevator_with_driver elevator_with_driver.cpp)