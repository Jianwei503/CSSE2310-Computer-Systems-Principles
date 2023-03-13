cmake_minimum_required(VERSION 3.16)
project(a4 C)

#find_package(Threads REQUIRED)
#target_link_libraries(${PROJECT_NAME} pthread)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

#add_executable(a4 utilities.c utilities.h airport.c airport.h mapper.c mapper.h aircraft.c aircraft.h)
add_executable(a4 utilities.c utilities.h mapper.c mapper.h)
#add_executable(a4 utilities.c utilities.h aircraft.c aircraft.h)