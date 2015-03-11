#ifndef __EnvimetReader_h
#define __EnvimetReader_h

#include "vtkRectilinearGridAlgorithm.h"


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

protected:
	EnvimetReader();
	~EnvimetReader(){}

	float x_spacing;
	float y_spacing;
	int x_dim;
	int y_dim;
	int z_dim;

	int RequestInformation(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
	int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);

	//vtkDelimitedTextReader* Reader;

private:
	EnvimetReader(const EnvimetReader&);  // Not implemented.
	void operator=(const EnvimetReader&);  // Not implemented.

	char* FileName;
};

#endif
