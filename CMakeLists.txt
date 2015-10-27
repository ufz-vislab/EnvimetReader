project(EnvimetReader CXX C)
cmake_minimum_required(VERSION 2.8)
cmake_policy(SET CMP0022 NEW)
cmake_policy(SET CMP0042 NEW)

find_package(ParaView REQUIRED)
include(${PARAVIEW_USE_FILE})

add_subdirectory(lib)

ADD_PARAVIEW_PLUGIN(EnvimetReader "1.1"
        SERVER_MANAGER_XML EnvimetReader.xml
        SERVER_MANAGER_SOURCES EnvimetReader.cxx
        # SOURCES lib/EnvimetV4XmlParser.cxx lib/Helper.cxx
        REQUIRED_ON_SERVER
        )
target_link_libraries(EnvimetReader PRIVATE EnvimetReaderLib)

add_subdirectory(tests)
