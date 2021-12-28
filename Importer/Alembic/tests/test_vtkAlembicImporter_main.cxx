#include <vtkNew.h>
#include <vtkAlembicImporter.h>
#include <string>

int main(int argc, char** argv)
{
	vtkNew<vtkAlembicImporter> abc;

	std::string filename(argv[1]);
	abc->SetFileName(filename.c_str());

	abc->Update();

	return 0;
}

