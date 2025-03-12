#pragma once

#include <vtkPythonAlgorithm.h>

class vtkPythonSource : public vtkPythonAlgorithm
{
public:
    vtkTypeMacro(vtkPythonSource,vtkPythonAlgorithm);

    static vtkPythonSource* New();
protected:
    vtkPythonSource();
    ~vtkPythonSource();
};
