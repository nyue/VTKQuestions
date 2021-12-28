/*=========================================================================
  Program:   Visualization Toolkit
  Module:    vtkAlembicImporter.cxx
  Copyright (c) Nicholas Yue
  All rights reserved.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.
=========================================================================*/

#include "vtkAlembicImporter.h"

#include "vtkActor.h"
#include "vtkCellArray.h"
#include "vtkFloatArray.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkProperty.h"
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"
#include "vtkSmartPointer.h"
#include "vtksys/SystemTools.hxx"

#include <cctype>
#include <cstdio>
#include <list>
#include <map>
#include <memory>
#include <set>
#include <sstream>

#include <Alembic/AbcGeom/All.h>
// #include <Alembic/AbcCoreHDF5/All.h>
#include <Alembic/AbcCoreOgawa/All.h>
#include <Alembic/Util/All.h>
#include <Alembic/Abc/All.h>
#include <Alembic/AbcCoreAbstract/All.h>
#include <Alembic/AbcCoreFactory/All.h>

vtkStandardNewMacro(vtkAlembicImporter);

//------------------------------------------------------------------------------
vtkAlembicImporter::vtkAlembicImporter()
{
  // this->Impl = vtkSmartPointer<vtkOBJPolyDataProcessor>::New();
}

//------------------------------------------------------------------------------
vtkAlembicImporter::~vtkAlembicImporter() = default;

int CanReadFile(vtkObject* that, const std::string& fname)
{
	printf("CanReadFile %s\n",fname.c_str());
  FILE* fileFD = vtksys::SystemTools::Fopen(fname, "rb");
  if (fileFD == nullptr)
  {
    vtkErrorWithObjectMacro(that, << "Unable to open file: " << fname.c_str());
    return 0;
  }
  fclose(fileFD);
  return 1;
}

int vtkAlembicImporter::ImportBegin()
{
  if (!CanReadFile(this, this->GetFileName()))
  {
    return 0;
  }
  /*
  if (!std::string(GetFileNameMTL()).empty() && !CanReadFile(this, this->GetFileNameMTL()))
  {
    return 0;
  }
  */
  return 1;
}

//------------------------------------------------------------------------------
void vtkAlembicImporter::ImportEnd()
{
  vtkDebugMacro("Done with " << this->GetClassName() << "::" << __FUNCTION__);
}

//------------------------------------------------------------------------------
void vtkAlembicImporter::ReadData()
{
	Alembic::AbcCoreFactory::IFactory factory;
	Alembic::AbcCoreFactory::IFactory::CoreType core_type;

	Alembic::Abc::IArchive archive = factory.getArchive(this->GetFileName(), core_type);

	switch (core_type)
	{
	    case Alembic::AbcCoreFactory::IFactory::kHDF5:
	        // RENDERER_LOG_INFO("        core type: HDF5");
	        break;
	    case Alembic::AbcCoreFactory::IFactory::kOgawa:
	        // RENDERER_LOG_INFO("        core type: Ogawa");
	        break;
	    case Alembic::AbcCoreFactory::IFactory::kLayer:
	        // RENDERER_LOG_INFO("        core type: Layer");
	        break;
	    case Alembic::AbcCoreFactory::IFactory::kUnknown:
	        // RENDERER_LOG_WARNING("        core type: Unknown");
	        break;
	    // assert_otherwise;
	}
	Alembic::Abc::IObject top = archive.getTop();
	size_t top_num_children = top.getNumChildren();

	//   vtkDebugMacro("Done with " << this->GetClassName() << "::" << __FUNCTION__);
	std::cout <<  top_num_children << std::endl;

  /*
  this->Impl->Update();
  if (Impl->GetSuccessParsingFiles())
  {
    bindTexturedPolydataToRenderWindow(this->RenderWindow, this->Renderer, Impl);
  }
  */
}

//------------------------------------------------------------------------------
void vtkAlembicImporter::PrintSelf(std::ostream& os, vtkIndent indent)
{
  vtkImporter::PrintSelf(os, indent);
}

void vtkAlembicImporter::SetFileName(const char* arg)
{
	alembic_filename = arg;
  // this->Impl->SetFileName(arg);
}

const char* vtkAlembicImporter::GetFileName() const
{
	return alembic_filename.c_str();
  // return this->Impl->GetFileName().data();
}

//------------------------------------------------------------------------------
std::string vtkAlembicImporter::GetOutputsDescription()
{
  std::stringstream ss;
  /*
  for (int i = 0; i < this->Impl->GetNumberOfOutputs(); i++)
  {
    ss << this->GetOutputDescription(i) << std::endl;
  }
  */
  return ss.str();
}

//------------------------------------------------------------------------------
std::string vtkAlembicImporter::GetOutputDescription(int idx)
{
	  std::stringstream ss;
	  /*
  vtkOBJImportedMaterial* mtl = this->Impl->GetMaterial(idx);
  ss << "data output " << idx;
  if (mtl)
  {
    ss << " with material named " << mtl->name << " texture file "
       << (mtl->texture_filename[0] == '\0' ? "none" : mtl->texture_filename) << " diffuse color ("
       << mtl->diff[0] << ", " << mtl->diff[1] << ", " << mtl->diff[2] << ")"
       << " ambient color (" << mtl->amb[0] << ", " << mtl->amb[1] << ", " << mtl->amb[2] << ")"
       << " specular color (" << mtl->spec[0] << ", " << mtl->spec[1] << ", " << mtl->spec[2] << ")"
       << " specular power " << mtl->specularPower << " opacity " << mtl->trans;
  }
  else
  {
    ss << " with no material";
  }
*/
  return ss.str();
}
