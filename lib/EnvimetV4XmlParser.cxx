#include "EnvimetV4XmlParser.h"

#include "Helper.h"

#include <vtkObjectFactory.h>

#include <iostream>
#include <sstream>

vtkStandardNewMacro(EnvimetV4XmlParser);

EnvimetV4XmlParser::EnvimetV4XmlParser() :
	SpatialDim(0)
{

}

EnvimetV4XmlParser::~EnvimetV4XmlParser()
{

}

int EnvimetV4XmlParser::Parse()
{
	if(!Helper::CanReadFile(FileName, ".EDX"))
		return -1;

	return this->Superclass::Parse();
}

void EnvimetV4XmlParser::StartElement(const char *name, const char **atts)
{
	this->Superclass::StartElement(name, atts);
	std::cout << name << std::endl;
	if(std::strcmp(name, "data_spatial_dim") == 0)
		SpatialDim = 0;
}

void EnvimetV4XmlParser::EndElement(const char *name)
{
	this->Superclass::EndElement(name);
	std::cout << name << std::endl;
}
