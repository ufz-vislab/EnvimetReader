cmake_minimum_required(VERSION 2.8)

find_package(ParaView REQUIRED)
include(${PARAVIEW_USE_FILE})

ADD_PARAVIEW_PLUGIN(EnvimetReader "0.1"
  SERVER_MANAGER_XML EnvimetReader.xml
  SERVER_MANAGER_SOURCES EnvimetReader.cxx
  REQUIRED_ON_SERVER
)
