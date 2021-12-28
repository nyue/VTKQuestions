#include "vtkPODInt.h"

#include <vtkObjectFactory.h>

vtkStandardNewMacro(vtkPODInt);

vtkPODInt::vtkPODInt()
{
  this->Value = 42;
}

vtkPODInt::~vtkPODInt()
{

}

void vtkPODInt::PrintSelf( ostream& os, vtkIndent indent )
{
  this->Superclass::PrintSelf( os, indent );
}
