#include "EnvimetReader.h"

#include "lib/Helper.h"

#include <vtkObjectFactory.h>
#include <vtkStreamingDemandDrivenPipeline.h>
#include <vtkInformationVector.h>
#include <vtkInformation.h>
#include <vtkSmartPointer.h>
#include <vtkPointData.h>
#include <vtkDataArray.h>
#include <vtkFloatArray.h>
#include <vtkDataArraySelection.h>
#include <vtkCallbackCommand.h>

#include <algorithm>

vtkStandardNewMacro(EnvimetReader);

EnvimetReader::EnvimetReader()
: _infoFileRead(false)
{
	FileName = NULL;
	NumberOfNestingCells = 12;
	SetNumberOfInputPorts(0);
	SetNumberOfOutputPorts(1);
	PointDataArraySelection = vtkDataArraySelection::New();
	SelectionObserver = vtkCallbackCommand::New();
	SelectionObserver->SetCallback(&EnvimetReader::SelectionModifiedCallback);
	SelectionObserver->SetClientData(this);
	PointDataArraySelection->AddObserver(vtkCommand::ModifiedEvent, this->SelectionObserver);
	XCoordinates = vtkFloatArray::New();
	YCoordinates = vtkFloatArray::New();
	ZCoordinates = vtkFloatArray::New();
}

EnvimetReader::~EnvimetReader()
{
	XCoordinates->Delete();
	YCoordinates->Delete();
	ZCoordinates->Delete();
	PointDataArraySelection->RemoveObserver(this->SelectionObserver);
	SelectionObserver->Delete();
	PointDataArraySelection->Delete();
}

int EnvimetReader::RequestInformation(
	vtkInformation *vtkNotUsed(request),
	vtkInformationVector **vtkNotUsed(inputVector),
	vtkInformationVector *outputVector)
{

	if(_infoFileRead)
		return 1;

	vtkInformation *outInfo = outputVector->GetInformationObject(0);


	if(!FileName)
	{
		vtkErrorMacro(<< "A FileName must be specified.");
		return -1;
	}

	if(!Helper::CanReadFile(FileName, ".EDI"))
		return -1;

	std::ifstream in (FileName, std::ifstream::in);
	if(!in.is_open())
	{
		vtkErrorMacro(<< "File " << this->FileName << " could not be opened");
		return -1;
	}
	std::string line;
	std::getline(in, line);
	// TODO "Waldstr 07:00:00 07.09.2013"
	std::getline(in, line);
	XDimension = atoi(line.c_str());
	std::getline(in, line);
	YDimension = atoi(line.c_str());
	std::getline(in, line);
	ZDimension = atoi(line.c_str());
	std::getline(in, line);
	const int numVars = atoi(line.c_str());

	// Array names
	while(line.find("Gridspacing") == std::string::npos)
	{
		std::getline(in, line);
		std::replace(line.begin(), line.end(), '(', '-');                          // Replace braces
		std::replace(line.begin(), line.end(), ')', ' ');                          // Replace braces
		line.erase(std::remove_if(line.begin(), line.end(), isspace), line.end()); // Remove whitespace
		line.erase(remove_if(line.begin(),line.end(), Helper::invalidChar), line.end());   // Remove non-ascii chars
		if(line.find("Gridspacing") == std::string::npos)
			PointDataArraySelection->AddArray(line.c_str());
	}
	if(PointDataArraySelection->GetNumberOfArrays() != numVars)
		vtkErrorMacro(<< "Mismatch of read data arrays (" << PointDataArraySelection->GetNumberOfArrays() <<
			") to number of data arrays given in EDI-file line 5 (" << numVars << ")");

	XCoordinates->Reset();
	XCoordinates->InsertNextValue(0.0f);
	for(int i = 0; i < XDimension; i++)
	{
		std::getline(in, line);
		const float cellSize = atof(line.c_str());
		XCoordinates->InsertNextValue(XCoordinates->GetValue(i) + cellSize);
	}

	YCoordinates->Reset();
	YCoordinates->InsertNextValue(0.0f);
	for(int i = 0; i < YDimension; i++)
	{
		std::getline(in, line);
		const float cellSize = atof(line.c_str());
		YCoordinates->InsertNextValue(YCoordinates->GetValue(i) + cellSize);
	}

	ZCoordinates->Reset();
	ZCoordinates->InsertNextValue(0.0f);
	for(int i = 0; i < ZDimension; i++)
	{
		std::getline(in, line);
		const float cellSize = atof(line.c_str());
		ZCoordinates->InsertNextValue(ZCoordinates->GetValue(i) + cellSize);
	}

	in.close();
	_infoFileRead = true;

	int ext[6] = { NumberOfNestingCells, XDimension - NumberOfNestingCells - 1,
	               NumberOfNestingCells, YDimension - NumberOfNestingCells - 1,
	               0, ZDimension - 1 };
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
	output->SetDimensions(XDimension, YDimension, ZDimension);

	output->SetXCoordinates(XCoordinates);
	output->SetYCoordinates(YCoordinates);
	output->SetZCoordinates(ZCoordinates);

	if(PointDataArraySelection->GetNumberOfArraysEnabled() == 0)
	{
		vtkErrorMacro(<< "No data arrays enabled. Only the mesh itself was loaded.")
		return 1;
	}

	std::string asciiFileName = std::string(FileName);
	std::string binaryFileName = asciiFileName.substr(0, asciiFileName.size() - 3).append("EDT");

	if(!Helper::CanReadFile(binaryFileName.c_str(), ".EDT"))
		return -1;

	std::ifstream in (binaryFileName.c_str(), std::ifstream::in | std::ios::binary);
	if(!in.is_open())
	{
		vtkErrorMacro(<< "File " << binaryFileName.c_str() << " could not be opened");
		return -1;
	}

	const vtkIdType numTuples = XDimension * YDimension * ZDimension;
	const int numArrays = PointDataArraySelection->GetNumberOfArraysEnabled();
	int numArraysRead = 0;

	UpdateProgress(0.0f);
	for(int arrayIndex = 0; arrayIndex < PointDataArraySelection->GetNumberOfArrays(); arrayIndex++)
	{
		if(PointDataArraySelection->GetArraySetting(arrayIndex) &&
			!output->GetPointData()->HasArray(GetPointArrayName(arrayIndex)))
		{
			// Read into vtk array
			float *floats = new float[numTuples];
			in.read(reinterpret_cast<char *>(&floats[0]), sizeof(float) * numTuples);

			vtkFloatArray *floatArray = vtkFloatArray::New();
			floatArray->SetName(PointDataArraySelection->GetArrayName(arrayIndex));
			floatArray->SetNumberOfTuples(numTuples);
			floatArray->SetArray(floats, numTuples, 0); // Ownership of floats is handed over to floatArray
			output->GetPointData()->AddArray(floatArray);

			UpdateProgress(numArraysRead * 1 / numArrays);

			if(numArraysRead == numArrays)
				break;
		}
		else
			// Move on to the next array
			in.seekg(sizeof(float) * numTuples, std::ios_base::cur);
	}
	UpdateProgress(1.0f);

	in.close();

	return 1;
}

void EnvimetReader::SelectionModifiedCallback(vtkObject*, unsigned long, void* clientdata, void*)
{
	static_cast<EnvimetReader*>(clientdata)->Modified();
}

int EnvimetReader::GetNumberOfPointArrays()
{
	return PointDataArraySelection->GetNumberOfArrays();
}

const char * EnvimetReader::GetPointArrayName(int index)
{
	return PointDataArraySelection->GetArrayName(index);
}

int EnvimetReader::GetPointArrayStatus(const char *name)
{
	return PointDataArraySelection->ArrayIsEnabled(name);
}

void EnvimetReader::SetPointArrayStatus(const char *name, int status)
{
	if(status)
		PointDataArraySelection->EnableArray(name);
	else
		PointDataArraySelection->DisableArray(name);
}

void EnvimetReader::PrintSelf(ostream& os, vtkIndent indent)
{
	Superclass::PrintSelf(os,indent);

	os << indent << "File Name: "
		<< (FileName ? FileName : "(none)") << "\n";
}
