#include <vtkSmartPointer.h>

#include "vtkSink.h"
#include "vtkPODInt.h"

int main(int, char *[])
{
	vtkSink* source = vtkSink::New();
	source->Update();

	vtkPODInt* test = source->GetOutput();
	std::cout << test->GetValue() << std::endl;

	return EXIT_SUCCESS;
}
