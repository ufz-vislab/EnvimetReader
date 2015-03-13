#ifndef __EnvimetReader_h
#define __EnvimetReader_h

#include "vtkRectilinearGridAlgorithm.h"

class vtkDataArraySelection;
class vtkFloatArray;
class vtkCallbackCommand;

class EnvimetReader : public vtkRectilinearGridAlgorithm
{
public:
	vtkTypeMacro(EnvimetReader,vtkRectilinearGridAlgorithm);
	void PrintSelf(ostream& os, vtkIndent indent);

	static EnvimetReader *New();

	// Description:
	// Specify file name of the .abc file.
	vtkSetStringMacro(FileName);
	vtkGetStringMacro(FileName);

	// Description:
	// Test whether the file with the given name can be read by this reader.
	int CanReadFile(const char* name, const char* extension);

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
	EnvimetReader();
	~EnvimetReader();

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

	// The number of cells in each dimension.
	int XDimension;
	int YDimension;
	int ZDimension;

private:
	EnvimetReader(const EnvimetReader&);  // Not implemented.
	void operator=(const EnvimetReader&);  // Not implemented.

	bool _infoFileRead;
};

#endif
