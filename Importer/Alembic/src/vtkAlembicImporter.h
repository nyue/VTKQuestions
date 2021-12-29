/*=========================================================================
  Program:   Visualization Toolkit
  Module:    vtkAlembicImporter.h
  Copyright (c) Nicholas Yue
  All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notice for more information.
  =========================================================================*/
/**
 * @class   vtkABCImporter
 * @brief   import from .abc Alembic files
 *
 * @sa
 *  vtkImporter
 */

#ifndef vtkAlembicImporter_h
#define vtkAlembicImporter_h

#include "vtkIOImportModule.h" // For export macro
#include "vtkImporter.h"
#include "vtkSmartPointer.h" // for ivars

#include <string>            // for string

#include <Alembic/Abc/All.h>
#include <Alembic/AbcGeom/All.h>

class vtkRenderWindow;
class vtkRenderer;
class vtkPolydata;

typedef std::vector<std::string> PathList;

class VTKIOIMPORT_EXPORT vtkAlembicImporter : public vtkImporter
{
 public:
	static vtkAlembicImporter* New();

	vtkTypeMacro(vtkAlembicImporter, vtkImporter);
	void PrintSelf(ostream& os, vtkIndent indent) override;

	///@{
	/**
	 * Specify the name of the file to read.
	 */
	void SetFileName(VTK_FILEPATH const char* arg);
	VTK_FILEPATH const char* GetFileName() const;
	///@}

	/**
	 * Get a printable string describing all outputs
	 */
	std::string GetOutputsDescription() override;

	/**
	 * Get a string describing an output
	 */
	std::string GetOutputDescription(int idx);

 protected:
	vtkAlembicImporter();
	~vtkAlembicImporter() override;

	int ImportBegin() override /*override*/;
	void ImportEnd() override /*override*/;
	void ReadData() override /* override */;

 private:
	vtkAlembicImporter(const vtkAlembicImporter&) = delete;
	void operator=(const vtkAlembicImporter&) = delete;

	void ProcessIPolyMesh(const Alembic::AbcGeom::IPolyMesh& pmesh);
	void IterateIObject(const Alembic::Abc::IObject &parent,
						const Alembic::Abc::ObjectHeader &ohead);

	std::string alembic_filename;


};

#endif
