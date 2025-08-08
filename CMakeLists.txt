cmake_minimum_required(VERSION 3.10)

# Project name and language
project(HashTableProject C)

# Set C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

# Add source files
set(SOURCES
    src/hash_table.c
    src/main.c
)

# Create executable
add_executable(hash_table ${SOURCES})

# Include headers
target_include_directories(hash_table PRIVATE ${PROJECT_SOURCE_DIR}/src)

