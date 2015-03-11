#include "EnvimetReader.h"

#include "vtkObjectFactory.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkInformationVector.h"
#include "vtkInformation.h"
#include "vtkDataObject.h"
#include "vtkSmartPointer.h"
#include "vtkPointData.h"
#include <vtkDataArray.h>

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vtkFloatArray.h>

vtkStandardNewMacro(EnvimetReader);

EnvimetReader::EnvimetReader()
{
	this->FileName = NULL;
	this->SetNumberOfInputPorts(0);
	this->SetNumberOfOutputPorts(1);
}

int EnvimetReader::RequestInformation(
	vtkInformation *vtkNotUsed(request),
	vtkInformationVector **vtkNotUsed(inputVector),
	vtkInformationVector *outputVector)
{
	vtkInformation *outInfo = outputVector->GetInformationObject(0);

	if(!this->FileName)
	{
		vtkErrorMacro(<< "A FileName must be specified.");
		return -1;
	}

	std::ifstream in (this->FileName, std::ios::in);
	if(!in.is_open())
	{
		vtkErrorMacro(<< "File " << this->FileName << " not found");
		return -1;
	}

	vtkDebugMacro (<< "reading seismic header");
	// TODO:
	x_spacing = 5;
	y_spacing = 5;

	x_dim = 249;
	y_dim = 249;
	z_dim = 30;

	int ext[6] = {0, x_dim, 0, y_dim, 0, z_dim};
	double origin[3] = {0, 0, 0};

	outInfo->Set(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(), ext, 6);
	outInfo->Set(vtkDataObject::ORIGIN(), origin, 3);
	vtkDataObject::SetPointDataActiveScalarInfo(outInfo, VTK_FLOAT, 1);

	return 1;
}

int EnvimetReader::RequestData(
	vtkInformation *vtkNotUsed(request),
	vtkInformationVector **vtkNotUsed(inputVector),
	vtkInformationVector *outputVector)
{
	vtkInformation *outInfo = outputVector->GetInformationObject(0);
	vtkDataObject::SetPointDataActiveScalarInfo(outInfo, VTK_FLOAT, 1);

	// get the ouptut
	vtkRectilinearGrid *output = vtkRectilinearGrid::SafeDownCast(
		outInfo->Get(vtkDataObject::DATA_OBJECT()));

	output->SetExtent(
		outInfo->Get(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT()));

	output->SetDimensions(x_dim, y_dim, z_dim);

	vtkFloatArray *xCoords = vtkFloatArray::New();
	for(int i = 0; i < x_dim; i++) xCoords->InsertNextValue(i*x_spacing);
	vtkFloatArray *yCoords = vtkFloatArray::New();
	for(int i = 0; i < y_dim; i++) yCoords->InsertNextValue(i*y_spacing);
	vtkFloatArray *zCoords = vtkFloatArray::New();
	zCoords->InsertNextValue(1.f);
	for(int i = 1; i < z_dim; i++) zCoords->InsertNextValue(1.2f * zCoords->GetValue(i-1));

	output->SetXCoordinates(xCoords);
	output->SetYCoordinates(yCoords);
	output->SetZCoordinates(zCoords);

	return 1;
}

void EnvimetReader::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os,indent);

	os << indent << "File Name: "
		<< (this->FileName ? this->FileName : "(none)") << "\n";
}
