cmake_minimum_required(VERSION 4.1)
project(yazino_bot VERSION 0.1)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

add_executable(${PROJECT_NAME} src/main.cpp
        src/modules/User.cpp
        src/modules/headers/User.h
        src/services/economy/Games.cpp
        src/services/economy/headers/Games.h
        src/modules/headers/UserException.h
        src/modules/DatabaseConnection.cpp
        src/modules/headers/DatabaseConnection.h
)

find_package(DPP REQUIRED)

find_package(Boost REQUIRED)

find_package(SQLite3 REQUIRED)

target_link_libraries(${PROJECT_NAME} PRIVATE
        ${DPP_LIBRARIES}
        Boost::boost
        SQLite::SQLite3
)

# Include the DPP directories.
target_include_directories(${PROJECT_NAME} PRIVATE
        ${DPP_INCLUDE_DIR}
)


target_compile_options(${PROJECT_NAME} PRIVATE
        -Wall
        -Wextra
)

# Set C++ version
set_target_properties(${PROJECT_NAME} PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
)