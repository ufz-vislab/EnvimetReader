#include "EnvimetV4XmlReader.h"

#include "lib/EnvimetV4XmlParser.h"
#include "lib/Helper.h"

#include <vtkObjectFactory.h>
#include <vtkStreamingDemandDrivenPipeline.h>
#include <vtkInformationVector.h>
#include <vtkInformation.h>
#include <vtkSmartPointer.h>
#include <vtkPointData.h>
#include <vtkDataArray.h>
#include <vtkFloatArray.h>
#include <vtkStringArray.h>
#include <vtkDataArraySelection.h>
#include <vtkCallbackCommand.h>

vtkStandardNewMacro(EnvimetV4XmlReader);

EnvimetV4XmlReader::EnvimetV4XmlReader()
	: _infoFileRead(false)
{
	FileName = NULL;
	NumberOfNestingCells = 12;
	SetNumberOfInputPorts(0);
	SetNumberOfOutputPorts(1);
	PointDataArraySelection = vtkDataArraySelection::New();
	SelectionObserver = vtkCallbackCommand::New();
	SelectionObserver->SetCallback(&EnvimetV4XmlReader::SelectionModifiedCallback);
	SelectionObserver->SetClientData(this);
	PointDataArraySelection->AddObserver(vtkCommand::ModifiedEvent, this->SelectionObserver);
	XCoordinates = vtkFloatArray::New();
	YCoordinates = vtkFloatArray::New();
	ZCoordinates = vtkFloatArray::New();

	Parser = EnvimetV4XmlParser::New();
}

EnvimetV4XmlReader::~EnvimetV4XmlReader()
{
	XCoordinates->Delete();
	YCoordinates->Delete();
	ZCoordinates->Delete();
	PointDataArraySelection->RemoveObserver(this->SelectionObserver);
	SelectionObserver->Delete();
	PointDataArraySelection->Delete();

	Parser->Delete();
}

int EnvimetV4XmlReader::RequestInformation(
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

	if(!Helper::CanReadFile(FileName, ".EDX"))
		return -1;

	Parser->SetFileName(FileName);
	if(Parser->Parse() != 1)
		return -1;

	for(int i = 0; i < Parser->VariableNames->GetNumberOfTuples(); i++)
		PointDataArraySelection->AddArray(Parser->VariableNames->GetValue(i));

	std::cout << "Num arrays: " << PointDataArraySelection->GetNumberOfArrays() << std::endl;

	if(PointDataArraySelection->GetNumberOfArrays() != Parser->VariableNames->GetNumberOfTuples())
	vtkErrorMacro(<< "Mismatch of read data arrays (" << PointDataArraySelection->GetNumberOfArrays() <<
					  ") to number of data arrays given in EDX-file(" << Parser->VariableNames->GetNumberOfValues() << ")");

	XCoordinates->Reset();
	XCoordinates->InsertNextValue(0.0f);
	for(int i = 0; i < Parser->XDimension; i++)
		XCoordinates->InsertNextValue(XCoordinates->GetValue(i) + Parser->XSpacing->GetValue(i));

	YCoordinates->Reset();
	YCoordinates->InsertNextValue(0.0f);
	for(int i = 0; i < Parser->YDimension; i++)
		YCoordinates->InsertNextValue(YCoordinates->GetValue(i) + Parser->YSpacing->GetValue(i));

	ZCoordinates->Reset();
	ZCoordinates->InsertNextValue(0.0f);
	for(int i = 0; i < Parser->ZDimension; i++)
		ZCoordinates->InsertNextValue(ZCoordinates->GetValue(i) + Parser->ZSpacing->GetValue(i));

	_infoFileRead = true;

	int ext[6] = { NumberOfNestingCells, Parser->XDimension - NumberOfNestingCells - 1,
				   NumberOfNestingCells, Parser->YDimension - NumberOfNestingCells - 1,
				   0, Parser->ZDimension - 1 };
	double origin[3] = {0, 0, 0};

	outInfo->Set(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(), ext, 6);
	outInfo->Set(vtkDataObject::ORIGIN(), origin, 3);
	vtkDataObject::SetPointDataActiveScalarInfo(outInfo, VTK_FLOAT, 1);

	return 1;
}

int EnvimetV4XmlReader::RequestData(
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
	output->SetDimensions(Parser->XDimension, Parser->YDimension, Parser->ZDimension);

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

	const vtkIdType numTuples = Parser->XDimension * Parser->YDimension * Parser->ZDimension;
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

void EnvimetV4XmlReader::SelectionModifiedCallback(vtkObject*, unsigned long, void* clientdata, void*)
{
	static_cast<EnvimetV4XmlReader*>(clientdata)->Modified();
}

int EnvimetV4XmlReader::GetNumberOfPointArrays()
{
	return PointDataArraySelection->GetNumberOfArrays();
}

const char * EnvimetV4XmlReader::GetPointArrayName(int index)
{
	return PointDataArraySelection->GetArrayName(index);
}

int EnvimetV4XmlReader::GetPointArrayStatus(const char *name)
{
	return PointDataArraySelection->ArrayIsEnabled(name);
}

void EnvimetV4XmlReader::SetPointArrayStatus(const char *name, int status)
{
	if(status)
		PointDataArraySelection->EnableArray(name);
	else
		PointDataArraySelection->DisableArray(name);
}

void EnvimetV4XmlReader::PrintSelf(ostream& os, vtkIndent indent)
{
	Superclass::PrintSelf(os,indent);

	os << indent << "File Name: "
	<< (FileName ? FileName : "(none)") << "\n";
}
