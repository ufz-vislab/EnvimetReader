add_subdirectory(gtest)

include_directories(${gtest_SOURCE_DIR}/include)

add_executable(EnvimetReaderTests
    EnvimetV4XmlParserTests.cxx)

target_link_libraries(EnvimetReaderTests gtest gtest_main EnvimetReaderLib)
