# Setup project
cmake_minimum_required(VERSION 3.5.1)
project(cLinkedList)

# Setup compiler
set(CMAKE_CXX_STANDARD 11)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall")

# Setup compile heirarchy
set(CLINKEDLIST_INCLUDE_PATH ${CMAKE_SOURCE_DIR})
set(CLINKEDLIST_SOURCE_PATH ${CMAKE_SOURCE_DIR}/sources)

file(GLOB_RECURSE C_SOURCE_FILES  "${CLINKEDLIST_SOURCE_PATH}/*.c")
file(GLOB_RECURSE C_INCLUDE_FILES "${CLINKEDLIST_INCLUDE_PATH}/*.h")

# Include header files
include_directories(${CLINKEDLIST_INCLUDE_PATH})

# Add sources to compiler
add_executable(${PROJECT_NAME} ${C_SOURCE_FILES} ${C_INCLUDE_FILES})
