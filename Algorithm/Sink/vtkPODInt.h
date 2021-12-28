#pragma once

#include <vtkDataObject.h>

class vtkPODInt: public vtkDataObject
{
public:
  static vtkPODInt* New();
  vtkTypeMacro(vtkPODInt,vtkDataObject);
  void PrintSelf( ostream& os, vtkIndent indent ) override;

  vtkGetMacro(Value, int);

protected:
  vtkPODInt();
  ~vtkPODInt();

private:
  vtkPODInt( const vtkPODInt& ); // Not implemented.
  void operator = ( const vtkPODInt& ); // Not implemented.

  int Value;
};
