#include "EnvimetReader.h"

#include <vtkObjectFactory.h>
#include <vtkStreamingDemandDrivenPipeline.h>
#include <vtkInformationVector.h>
#include <vtkInformation.h>
#include <vtkSmartPointer.h>
#include <vtkPointData.h>
#include <vtkDataArray.h>
#include <vtkFloatArray.h>
#include <vtkDataArraySelection.h>

#include <iostream>
#include <fstream>
#include <string>

vtkStandardNewMacro(EnvimetReader);

EnvimetReader::EnvimetReader()
{
	this->FileName = NULL;
	this->SetNumberOfInputPorts(0);
	this->SetNumberOfOutputPorts(1);
	_arraySelection = vtkDataArraySelection::New();
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

	vtkDebugMacro (<< "reading seismic header");
	// TODO: read this info from EDI file
	x_spacing = 5;
	y_spacing = 5;

	x_dim = 249;
	y_dim = 249;
	z_dim = 30;

	_arraySelection->AddArray("z");
	_arraySelection->AddArray("Classed LAD");
	_arraySelection->AddArray("Flow u");
	_arraySelection->AddArray("Flow v");
	_arraySelection->AddArray("Flow w");
	_arraySelection->EnableAllArrays();

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

	vtkRectilinearGrid *output = vtkRectilinearGrid::SafeDownCast(
		outInfo->Get(vtkDataObject::DATA_OBJECT()));

	output->SetExtent(
		outInfo->Get(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT()));

	// Points
	output->SetDimensions(x_dim, y_dim, z_dim);
	vtkFloatArray *xCoords = vtkFloatArray::New();
	for(int i = 0; i < x_dim; i++) xCoords->InsertNextValue(i*x_spacing);
	vtkFloatArray *yCoords = vtkFloatArray::New();
	for(int i = 0; i < y_dim; i++) yCoords->InsertNextValue(i*y_spacing);
	vtkFloatArray *zCoords = vtkFloatArray::New();
	zCoords->InsertNextValue(0.f);
	zCoords->InsertNextValue(1.f);
	float z_sum = 1;
	for(int i = 2; i < z_dim; i++)
	{
		float lastCellHeight = zCoords->GetValue(i-1)-zCoords->GetValue(i-2);
		float currentCellHeight = 1.2f * lastCellHeight;
		z_sum += currentCellHeight;
		zCoords->InsertNextValue(z_sum);
	}

	output->SetXCoordinates(xCoords);
	output->SetYCoordinates(yCoords);
	output->SetZCoordinates(zCoords);

	// data arrays
	std::string arrayNames[] = {
		"z",
		"Classed LAD",
		"Flow u",
		"Flow v",
		"Flow w",
		"Wind speed",
		"Wind speed change",
		"Wind direction",
		"Pressure perturb",
		"Pot. temperature",
		"Pot. temperature (Diff K)",
		"Pot. temperature Change K/h",
		"Spec. humidity",
		"Relative humidity",
		"Turbulent kinetic energy",
		"Dissipitation",
		"Vertical exchange coef.",
		"Horizontal exchange coef.",
		"Absoule LAD",
		"Direct SW radiation",
		"Diffuse SW radiation",
		"Reflected SW radiation",
		"Longwave rad. environment",
		"Sky-view-factor buildings",
		"Sky-view-factor buildings + vegetation",
		"Temperature flux",
		"Vapour flux",
		"Water on leafs",
		"Wall temperature x",
		"Wall temperature y",
		"Wall temperature z",
		"Leaf temperature",
		"Local mixing length",
		"PMV",
		"Percentage people dissatisfied",
		"Mean radiant temperature",
		"Gas/particle concentration",
		"Gas/particle source",
		"Deposition velocities",
		"Total deposed mass",
		"Deposed mass time averaged",
		"TKE normalised 1D",
		"Dissipitaion normalised 1D",
		"km normalised 1D",
		"TKE mechanical prod",
		"Stomata resistance",
		"CO2",
		"CO2 ppm",
		"Plant CO2 flux",
		"Div Rlw Temp change",
		"Local mass budget"
	};

	std::ifstream in (this->FileName, std::ifstream::in | std::ios::binary);
	if(!in.is_open())
	{
		vtkErrorMacro(<< "File " << this->FileName << " could not be opened");
		return -1;
	}

	// TODO: read all vars
	const vtkIdType numTuples = x_dim*y_dim*z_dim;

	for(int arrayIndex = 0; arrayIndex < _arraySelection->GetNumberOfArrays(); arrayIndex++)
	{
		if((bool)_arraySelection->GetArraySetting(arrayIndex))
		{
			// Read into vtk array
			vtkFloatArray *floatArray = vtkFloatArray::New();
			floatArray->SetName(arrayNames[arrayIndex].c_str());
			floatArray->SetNumberOfTuples(numTuples);

			float *floats = new float[numTuples];
			in.read(reinterpret_cast<char *>(&floats[0]), sizeof(float) * numTuples);
			// Ownership of floats is handed over to floatArray
			floatArray->SetArray(floats, numTuples, 0);

			output->GetPointData()->AddArray(floatArray);
		}
		else
			// Move on to the next array
			in.seekg(sizeof(float) * numTuples, std::ios_base::cur);
	}

	in.close();


	return 1;
}

int EnvimetReader::GetNumberOfPointArrays() const
{
	return _arraySelection->GetNumberOfArrays();
}

const char * EnvimetReader::GetPointArrayName(int index) const
{
	// data arrays
	std::string arrayNames[] = {
		"z",
		"Classed LAD",
		"Flow u",
		"Flow v",
		"Flow w",
		"Wind speed",
		"Wind speed change",
		"Wind direction",
		"Pressure perturb",
		"Pot. temperature",
		"Pot. temperature (Diff K)",
		"Pot. temperature Change K/h",
		"Spec. humidity",
		"Relative humidity",
		"Turbulent kinetic energy",
		"Dissipitation",
		"Vertical exchange coef.",
		"Horizontal exchange coef.",
		"Absoule LAD",
		"Direct SW radiation",
		"Diffuse SW radiation",
		"Reflected SW radiation",
		"Longwave rad. environment",
		"Sky-view-factor buildings",
		"Sky-view-factor buildings + vegetation",
		"Temperature flux",
		"Vapour flux",
		"Water on leafs",
		"Wall temperature x",
		"Wall temperature y",
		"Wall temperature z",
		"Leaf temperature",
		"Local mixing length",
		"PMV",
		"Percentage people dissatisfied",
		"Mean radiant temperature",
		"Gas/particle concentration",
		"Gas/particle source",
		"Deposition velocities",
		"Total deposed mass",
		"Deposed mass time averaged",
		"TKE normalised 1D",
		"Dissipitaion normalised 1D",
		"km normalised 1D",
		"TKE mechanical prod",
		"Stomata resistance",
		"CO2",
		"CO2 ppm",
		"Plant CO2 flux",
		"Div Rlw Temp change",
		"Local mass budget"
	};

	return arrayNames[index].c_str();
}

int EnvimetReader::GetPointArrayStatus(const char *name) const
{
	return _arraySelection->ArrayIsEnabled(name);
}

void EnvimetReader::SetPointArrayStatus(const char *name, int status)
{
	if((bool)status)
		_arraySelection->EnableArray(name);
	else
		_arraySelection->DisableArray(name);
}

void EnvimetReader::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os,indent);

	os << indent << "File Name: "
		<< (this->FileName ? this->FileName : "(none)") << "\n";
}
