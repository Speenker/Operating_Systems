cmake_minimum_required(VERSION 3.10)
project(DistributedSystem)

set(CMAKE_CXX_STANDARD 14)

## load in pkg-config support
find_package(PkgConfig)
## use pkg-config to get hints for 0mq locations
pkg_check_modules(PC_ZeroMQ QUIET zmq)

## use the hint from above to find where 'zmq.hpp' is located
find_path(ZeroMQ_INCLUDE_DIR
        NAMES zmq.hpp
        PATHS ${PC_ZeroMQ_INCLUDE_DIRS}
)

## use the hint from above to find the location of libzmq
find_library(ZeroMQ_LIBRARY
        NAMES zmq
        PATHS ${PC_ZeroMQ_LIBRARY_DIRS}
)

# Добавляем исполняемый файл main
add_executable(main parent.cpp)

# Подключаем каталоги и библиотеки ZeroMQ
target_include_directories(main PRIVATE ${ZeroMQ_INCLUDE_DIR})
target_link_libraries(main PRIVATE ${ZeroMQ_LIBRARY})

add_executable(child child.cpp)

# Подключаем каталоги и библиотеки ZeroMQ
target_include_directories(child PRIVATE ${ZeroMQ_INCLUDE_DIR})
target_link_libraries(child PRIVATE ${ZeroMQ_LIBRARY})