cmake_minimum_required(VERSION 3.1)

project(SingleArray)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif()


message(STATUS "BUILD_TYPE          is  ${CMAKE_BUILD_TYPE}")
message(STATUS "CXX_STANDARD        is  ${CMAKE_CXX_STANDARD}")
message(STATUS "CXX_FLAGS           are ${CMAKE_CXX_FLAGS}")
message(STATUS "CXX_FLAGS_DEBUG     are ${CMAKE_CXX_FLAGS_DEBUG}")
message(STATUS "CXX_FLAGS_RELEASE   are ${CMAKE_CXX_FLAGS_RELEASE}")
message(STATUS "CXX_COMPILER_ID     is  ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "BUILTIN_POPCNT      is  ${BUILTIN_POPCNT}")

add_executable(main main.cpp)

set(BOOST_ROOT "C:/boost_1_70_0")

add_subdirectory(SimpleDataStructure EXCLUDE_FROM_ALL)
#target_link_libraries(main PRIVATE sim_ds sdsl)

target_link_libraries(main sim_ds)


#find_package(Boost 1.53.0 REQUIRED)
#    COMPONENTS unit_test_framework iostreams system thread filesystem)
#message(STATUS "include_dir  ${Boost_INCLUDE_DIRS}")
#target_include_directories(main PUBLIC ${Boost_INCLUDE_DIRS}