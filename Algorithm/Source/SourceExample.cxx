#include <vtkSmartPointer.h>

#include "vtkSource.h"
#include "vtkPODInt.h"

int main(int, char *[])
{
	vtkSource* source = vtkSource::New();
	source->Update();

	vtkPODInt* test = source->GetOutput();
	std::cout << test->GetValue() << std::endl;

	return EXIT_SUCCESS;
}
