cmake_minimum_required(VERSION 3.5.0)
project(Server VERSION 0.1.0 LANGUAGES C CXX)

include(CTest)
enable_testing()

add_executable(${PROJECT_NAME} main.cpp)

target_include_directories(${PROJECT_NAME}
        PUBLIC
            Networking
)

target_link_libraries(${PROJECT_NAME}
        PUBLIC
            Networking
)

