#ifndef ENVIMETREADER_ENVIMETV4XMLREADER_H
#define ENVIMETREADER_ENVIMETV4XMLREADER_H

#include "vtkRectilinearGridAlgorithm.h"

class vtkDataArraySelection;
class vtkFloatArray;
class vtkCallbackCommand;
class EnvimetV4XmlParser;

class EnvimetV4XmlReader : public vtkRectilinearGridAlgorithm
{
public:
	vtkTypeMacro(EnvimetV4XmlReader,vtkRectilinearGridAlgorithm);
	void PrintSelf(ostream& os, vtkIndent indent);

	static EnvimetV4XmlReader *New();

	// Description:
	// Specify file name of the .abc file.
	vtkSetStringMacro(FileName);
	vtkGetStringMacro(FileName);

	// Description:
	// Get the data array selection tables used to configure which data
	// arrays are loaded by the reader.
	vtkGetObjectMacro(PointDataArraySelection, vtkDataArraySelection);

	// Description:
	// Get the number of point or cell arrays available in the input.
	int GetNumberOfPointArrays();

	// Description:
	// Get the name of the point or cell array with the given index in the input.
	const char * GetPointArrayName(int index);

	// Description:
	// Get/Set whether the point or cell array with the given name is to be read.
	int GetPointArrayStatus(const char *name);
	void SetPointArrayStatus(const char *name, int status);

protected:
	EnvimetV4XmlReader();
	~EnvimetV4XmlReader();

	int RequestInformation(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
	int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);

	// Callback registered with the SelectionObserver.
	static void SelectionModifiedCallback(vtkObject* caller, unsigned long eid,
										  void* clientdata, void* calldata);

	// The input file's name.
	char* FileName;

	// The number of nesting cells in x and y direction.
	unsigned int NumberOfNestingCells;

	// The observer to modify this object when the array selections are modified.
	vtkCallbackCommand* SelectionObserver;

	// The array selections
	vtkDataArraySelection* PointDataArraySelection;

	// The cell coordinates (sizes).
	vtkFloatArray *XCoordinates;
	vtkFloatArray *YCoordinates;
	vtkFloatArray *ZCoordinates;

	EnvimetV4XmlParser *Parser;

private:
	EnvimetV4XmlReader(const EnvimetV4XmlReader&);  // Not implemented.
	void operator=(const EnvimetV4XmlReader&);  // Not implemented.

	bool _infoFileRead;
};

#endif //ENVIMETREADER_ENVIMETV4XMLREADER_H
