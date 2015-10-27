#ifndef __EnvimetV4XmlParser_h
#define __EnvimetV4XmlParser_h

#include <vtkXMLDataParser.h>

class vtkFloatArray;

class EnvimetV4XmlParser : public vtkXMLDataParser
{
	vtkTypeMacro(EnvimetV4XmlParser,vtkXMLDataParser);

public:
	static EnvimetV4XmlParser *New();

	virtual int Parse();

	// Header


	// Data description
	int SpatialDim;
	int Num_XData;
	int Num_YData;
	int Num_ZData;

	vtkFloatArray *XCoordinates;
	vtkFloatArray *YCoordinates;
	vtkFloatArray *ZCoordinates;

	// Variables

	// Model description

	// Envimet reference

	// Additional info
protected:
	EnvimetV4XmlParser();
	~EnvimetV4XmlParser();

	virtual void StartElement(const char *name, const char **atts);
	virtual void EndElement(const char *name);

private:
	EnvimetV4XmlParser(const EnvimetV4XmlParser&);  // Not implemented.
	void operator=(const EnvimetV4XmlParser&);  // Not implemented.
};

#endif // __EnvimetV4XmlParser_h
