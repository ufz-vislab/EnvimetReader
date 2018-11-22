cmake_minimum_required(VERSION 3.3)
project(EnvimetReader)

option(BUILD_TESTING "Build tests")

# Find
if(ParaView_SOURCE_DIR)
  #building within the ParaView source tree
  include_directories(${VTK_INCLUDE_DIRS})
else()
  #building outside of the source tree
  find_package(ParaView REQUIRED)
  include(${PARAVIEW_USE_FILE})
endif()

add_subdirectory(lib)

ADD_PARAVIEW_PLUGIN(EnvimetReader "1.2.1"
        SERVER_MANAGER_XML EnvimetReader.xml
        SERVER_MANAGER_SOURCES EnvimetReader.cxx EnvimetV4XmlReader.cxx
        REQUIRED_ON_SERVER
        )
target_link_libraries(EnvimetReader PRIVATE EnvimetReaderLib)

if(BUILD_TESTING)
	add_subdirectory(tests)
endif()
