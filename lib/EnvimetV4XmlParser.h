#ifndef __EnvimetV4XmlParser_h
#define __EnvimetV4XmlParser_h

#include <vtkObject.h>

class vtkFloatArray;
class vtkStringArray;
class vtkXMLDataParser;

class EnvimetV4XmlParser : public vtkObject
{
	vtkTypeMacro(EnvimetV4XmlParser,vtkObject);

public:
	static EnvimetV4XmlParser *New();

	int Parse();
	const char* FileName;
	vtkSetStringMacro(FileName);

	// Header


	// Data description
	int NumberOfSpatialDimensions;
	int XDimension;
	int YDimension;
	int ZDimension;

	vtkFloatArray *XSpacing;
	vtkFloatArray *YSpacing;
	vtkFloatArray *ZSpacing;

	// Variables
	int NumberOfVariables;
	vtkStringArray *VariableNames;

	// Model description
	//std::string ModelTitle;
	std::string SimulationBaseName;
	std::string SimulationDate;
	std::string SimulationTime;
	std::string ProjectName;
	std::string LocationName;
	float ModelRotation;
	double LocationGeorefX;
	double LocationGeorefY;

	// Envimet reference

	// Additional info
protected:
	EnvimetV4XmlParser();
	~EnvimetV4XmlParser();

	vtkXMLDataParser *Parser;

private:
	EnvimetV4XmlParser(const EnvimetV4XmlParser&);  // Not implemented.
	void operator=(const EnvimetV4XmlParser&);  // Not implemented.

	int StringToInt(const char *string);
	float StringToFloat(const char *string);
	double StringToDouble(const char *string);
	vtkFloatArray* StringToFloatArray(const char *string, const char *delimiter);
	vtkStringArray* StringToStringArray(const char *string, const char *delimiter);
};

#endif // __EnvimetV4XmlParser_h
