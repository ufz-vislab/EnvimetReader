add_library(EnvimetReaderLib
    Helper.cxx
    EnvimetV4XmlParser.cxx
)

target_link_libraries(EnvimetReaderLib INTERFACE vtkCommonCore vtkIOXMLParser)
target_compile_features(EnvimetReaderLib PRIVATE cxx_auto_type)
