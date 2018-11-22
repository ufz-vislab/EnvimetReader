add_library(EnvimetReaderLib STATIC
    Helper.cxx
    EnvimetV4XmlParser.cxx
)

target_link_libraries(EnvimetReaderLib INTERFACE vtkCommonCore vtkIOXMLParser)
