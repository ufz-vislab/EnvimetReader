project(EnvimetReader CXX C)
cmake_minimum_required(VERSION 2.8)
cmake_policy(SET CMP0022 NEW)
cmake_policy(SET CMP0042 NEW)
cmake_policy(SET CMP0054 NEW)

if(${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-tautological-constant-out-of-range-compare")
endif()

find_package(ParaView REQUIRED)
include(${PARAVIEW_USE_FILE})

add_subdirectory(lib)

ADD_PARAVIEW_PLUGIN(EnvimetReader "1.2"
        SERVER_MANAGER_XML EnvimetReader.xml
        SERVER_MANAGER_SOURCES EnvimetReader.cxx EnvimetV4XmlReader.cxx
        REQUIRED_ON_SERVER
        )
target_link_libraries(EnvimetReader PRIVATE EnvimetReaderLib)

if(BUILD_TESTING)
	add_subdirectory(tests)
endif()