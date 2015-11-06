#include "EnvimetV4XmlParser.h"

#include "Helper.h"

#include <vtkFloatArray.h>
#include <vtkObjectFactory.h>
#include <vtkStringArray.h>
#include <vtkVariant.h>
#include <vtkXMLDataParser.h>

#include <algorithm>
#include <iostream>
#include <string>

vtkStandardNewMacro(EnvimetV4XmlParser);

EnvimetV4XmlParser::EnvimetV4XmlParser() :
	NumberOfSpatialDimensions(0),
	XDimension(0),
	YDimension(1),
	ZDimension(2),
	ModelRotation(0.0f),
	LocationGeorefX(0.0),
	LocationGeorefY(0.0),
	NumberOfNestingCells(0)
{
	Parser = vtkXMLDataParser::New();
	// because files are in Windows 1252 encoding
	// See https://en.wikipedia.org/wiki/Windows-1252
	Parser->SetEncoding("iso-8859-1");
	Parser->SetIgnoreCharacterData(0); // Enables reading of character data.
}

EnvimetV4XmlParser::~EnvimetV4XmlParser()
{
	Parser->Delete();
}

int EnvimetV4XmlParser::Parse()
{
	if(!Helper::CanReadFile(FileName, ".EDX"))
		return -1;

	Parser->SetFileName(FileName);
	//Parser->SetAttributesEncoding(VTK_ENCODING_UNICODE);

	if(Parser->Parse() != 1)
		return -1;

	vtkXMLDataElement *root = Parser->GetRootElement();
	NumberOfSpatialDimensions = Helper::StringToInt(
		root->LookupElementWithName("data_spatial_dim")->GetCharacterData());
	XDimension = Helper::StringToInt(
		root->LookupElementWithName("nr_xdata")->GetCharacterData());
	YDimension = Helper::StringToInt(
		root->LookupElementWithName("nr_ydata")->GetCharacterData());
	if(NumberOfSpatialDimensions > 2)
		ZDimension = Helper::StringToInt(
			root->LookupElementWithName("nr_zdata")->GetCharacterData());

	const char *delim = ",";
	XSpacing = StringToFloatArray(
		root->LookupElementWithName("spacing_x")->GetCharacterData(), delim);
	YSpacing = StringToFloatArray(
		root->LookupElementWithName("spacing_y")->GetCharacterData(), delim);
	ZSpacing = StringToFloatArray(
		root->LookupElementWithName("spacing_z")->GetCharacterData(), delim);

	NumberOfVariables = Helper::StringToInt(
		root->LookupElementWithName("nr_variables")->GetCharacterData());
	VariableNames = StringToStringArray(
		root->LookupElementWithName("name_variables")->GetCharacterData(), delim);

	SimulationBaseName = Helper::trim(
		root->LookupElementWithName("simulation_basename")->GetCharacterData());
	SimulationDate = Helper::trim(
		root->LookupElementWithName("simulation_date")->GetCharacterData());
	SimulationTime = Helper::trim(
		root->LookupElementWithName("simulation_time")->GetCharacterData());
	ProjectName = Helper::trim(
		root->LookupElementWithName("projectname")->GetCharacterData());
	ModelRotation = StringToFloat(
		root->LookupElementWithName("model_rotation")->GetCharacterData());
	LocationGeorefX = StringToDouble(
		root->LookupElementWithName("location_georef_x")->GetCharacterData());
	LocationGeorefY = StringToDouble(
		root->LookupElementWithName("location_georef_y")->GetCharacterData());

	std::cout << FileName << std::endl;
	std::string SimulationDirectory = Helper::getDirectory(FileName);
	std::string NestingFileName = SimulationDirectory + SimulationBaseName + "nesting.txt";
	std::ifstream in (NestingFileName, std::ifstream::in);
	if(in.is_open())
	{
		std::string line;
		std::getline(in, line);
		NumberOfNestingCells = Helper::StringToInt(line.c_str());
		in.close();
	}

	return 1;
}

float EnvimetV4XmlParser::StringToFloat(const char *string)
{
	vtkVariant variant(string);
	return variant.ToFloat();
}

double EnvimetV4XmlParser::StringToDouble(const char *string)
{
	vtkVariant variant(string);
	return variant.ToDouble();
}

vtkFloatArray* EnvimetV4XmlParser::StringToFloatArray(const char *string, const char *delimiter)
{
	vtkFloatArray *floatArray = vtkFloatArray::New();
	floatArray->SetNumberOfComponents(1);
	std::string s(string);
	std::string delim(delimiter);

	unsigned int start = 0U;
	std::string::size_type end = s.find(delim);
	while(end != std::string::npos)
	{
		floatArray->InsertNextTuple1(StringToFloat((s.substr(start, end-start)).c_str()));
		start = end + delim.length();
		end = s.find(delim, start);
	}
	// Read last value
	floatArray->InsertNextTuple1(StringToFloat((s.substr(start, s.length()-start)).c_str()));

	return floatArray;
}

vtkStringArray* EnvimetV4XmlParser::StringToStringArray(const char *string, const char *delimiter)
{
	vtkStringArray *array = vtkStringArray::New();
	array->SetNumberOfComponents(1);
	std::string s(string);
	std::string delim(delimiter);

	unsigned int start = 0U;
	std::string::size_type end = s.find(delim);
	while(end != std::string::npos)
	{
		array->InsertNextValue((Helper::trim(s.substr(start, end-start))).c_str());
		start = end + delim.length();
		end = s.find(delim, start);
	}
	// Read last value
	array->InsertNextValue((Helper::trim(s.substr(start, s.length()-start))).c_str());

	return array;
}
