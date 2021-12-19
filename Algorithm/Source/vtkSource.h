#pragma once

#include <vtkAlgorithm.h>
#include "vtkPODInt.h"

class vtkSource: public vtkAlgorithm
{
public:
    vtkTypeMacro(vtkSource,vtkAlgorithm);
    void PrintSelf(ostream& os, vtkIndent indent);

    static vtkSource *New();

    // Description:
    // Get the output data object for a port on this algorithm.
    vtkPODInt* GetOutput();
    vtkPODInt* GetOutput(int);
    virtual void SetOutput(vtkDataObject* d);

    // Description:
    // see vtkAlgorithm for details
    virtual int ProcessRequest(vtkInformation*,
                               vtkInformationVector**,
                               vtkInformationVector*) override;

protected:
    vtkSource();
    ~vtkSource();

    // int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
    // Description:
    // This is called by the superclass.
    // This is the method you should override.
    virtual int RequestDataObject(
      vtkInformation* request,
      vtkInformationVector** inputVector,
      vtkInformationVector* outputVector );

    // convenience method
    virtual int RequestInformation(
      vtkInformation* request,
      vtkInformationVector** inputVector,
      vtkInformationVector* outputVector );

    // Description:
    // This is called by the superclass.
    // This is the method you should override.
    virtual int RequestData(
      vtkInformation* request,
      vtkInformationVector** inputVector,
      vtkInformationVector* outputVector );

    // Description:
    // This is called by the superclass.
    // This is the method you should override.
    virtual int RequestUpdateExtent(
      vtkInformation*,
      vtkInformationVector**,
      vtkInformationVector* );

    virtual int FillOutputPortInformation( int port, vtkInformation* info ) override;

private:
    vtkSource(const vtkSource&);  // Not implemented.
    void operator=(const vtkSource&);  // Not implemented.

};
