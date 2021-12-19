#include "vtkSource.h"
#include "vtkPODInt.h"

#include <vtkCommand.h>
#include <vtkInformation.h>
#include <vtkInformationVector.h>
#include <vtkObjectFactory.h>
#include <vtkStreamingDemandDrivenPipeline.h>

vtkStandardNewMacro(vtkSource);

//----------------------------------------------------------------------------
vtkSource::vtkSource()
{
	this->SetNumberOfInputPorts( 0 );
	this->SetNumberOfOutputPorts( 1 );
}

//----------------------------------------------------------------------------
vtkSource::~vtkSource()
{
}

//----------------------------------------------------------------------------
void vtkSource::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
vtkPODInt* vtkSource::GetOutput()
{
	return this->GetOutput(0);
}

//----------------------------------------------------------------------------
vtkPODInt* vtkSource::GetOutput(int port)
{
	return dynamic_cast<vtkPODInt*>(this->GetOutputDataObject(port));
}

//----------------------------------------------------------------------------
void vtkSource::SetOutput(vtkDataObject* d)
{
	this->GetExecutive()->SetOutputData(0, d);
}


//----------------------------------------------------------------------------
int vtkSource::ProcessRequest(vtkInformation* request,
		vtkInformationVector** inputVector,
		vtkInformationVector* outputVector)
{
	// Create an output object of the correct type.
	if(request->Has(vtkDemandDrivenPipeline::REQUEST_DATA_OBJECT()))
	{
		return this->RequestDataObject(request, inputVector, outputVector);
	}
	// generate the data
	if(request->Has(vtkDemandDrivenPipeline::REQUEST_DATA()))
	{
		return this->RequestData(request, inputVector, outputVector);
	}

	if(request->Has(vtkStreamingDemandDrivenPipeline::REQUEST_UPDATE_EXTENT()))
	{
		return this->RequestUpdateExtent(request, inputVector, outputVector);
	}

	// execute information
	if(request->Has(vtkDemandDrivenPipeline::REQUEST_INFORMATION()))
	{
		return this->RequestInformation(request, inputVector, outputVector);
	}

	return this->Superclass::ProcessRequest(request, inputVector, outputVector);
}

//----------------------------------------------------------------------------
int vtkSource::FillOutputPortInformation(
		int vtkNotUsed(port), vtkInformation* info)
{
	// now add our info
	info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkPODInt");
	return 1;
}


//----------------------------------------------------------------------------
int vtkSource::RequestDataObject(
		vtkInformation* vtkNotUsed(request),
		vtkInformationVector** vtkNotUsed(inputVector),
		vtkInformationVector* outputVector )
{
	for ( int i = 0; i < this->GetNumberOfOutputPorts(); ++i )
	{
		vtkInformation* outInfo = outputVector->GetInformationObject( i );
		vtkPODInt* output = dynamic_cast<vtkPODInt*>(
				outInfo->Get( vtkDataObject::DATA_OBJECT() ) );
		if ( ! output )
		{
			output = vtkPODInt::New();
			outInfo->Set( vtkDataObject::DATA_OBJECT(), output );
			output->FastDelete();
			this->GetOutputPortInformation( i )->Set(
					vtkDataObject::DATA_EXTENT_TYPE(), output->GetExtentType() );
		}
	}
	return 1;
}

//----------------------------------------------------------------------------
int vtkSource::RequestInformation(
		vtkInformation* vtkNotUsed(request),
		vtkInformationVector** vtkNotUsed(inputVector),
		vtkInformationVector* vtkNotUsed(outputVector))
{
	// do nothing let subclasses handle it
	return 1;
}

//----------------------------------------------------------------------------
int vtkSource::RequestUpdateExtent(
		vtkInformation* vtkNotUsed(request),
		vtkInformationVector** inputVector,
		vtkInformationVector* vtkNotUsed(outputVector))
{
	int numInputPorts = this->GetNumberOfInputPorts();
	for (int i=0; i<numInputPorts; i++)
	{
		int numInputConnections = this->GetNumberOfInputConnections(i);
		for (int j=0; j<numInputConnections; j++)
		{
			vtkInformation* inputInfo = inputVector[i]->GetInformationObject(j);
			inputInfo->Set(vtkStreamingDemandDrivenPipeline::EXACT_EXTENT(), 1);
		}
	}
	return 1;
}

//----------------------------------------------------------------------------
// This is the superclasses style of Execute method.  Convert it into
// an imaging style Execute method.
int vtkSource::RequestData(
		vtkInformation* vtkNotUsed(request),
		vtkInformationVector** vtkNotUsed( inputVector ),
		vtkInformationVector* vtkNotUsed(outputVector) )
{
	// do nothing let subclasses handle it
	return 1;
}
