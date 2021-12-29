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
#include "vtkPoints.h"
#include "vtkNew.h"
#include "vtkSphereSource.h"

#include <cctype>
#include <cstdio>
#include <list>
#include <map>
#include <memory>
#include <set>
#include <sstream>

//#include <Alembic/AbcGeom/All.h>
// #include <Alembic/AbcCoreHDF5/All.h>
#include <Alembic/AbcCoreOgawa/All.h>
#include <Alembic/Util/All.h>
// #include <Alembic/Abc/All.h>
#include <Alembic/AbcCoreAbstract/All.h>
#include <Alembic/AbcCoreFactory/All.h>

typedef std::map<Alembic::AbcGeom::PlainOldDataType,std::string> PODStringMap;
typedef std::map<Alembic::AbcGeom::PropertyType,std::string> PropertyTypeStringMap;


void CreatePropertyTypeStringMap(PropertyTypeStringMap& propertyTypeStringMap)
{
	propertyTypeStringMap[Alembic::AbcGeom::kCompoundProperty] = "kCompoundProperty";
	propertyTypeStringMap[Alembic::AbcGeom::kScalarProperty] = "kScalarProperty";
	propertyTypeStringMap[Alembic::AbcGeom::kArrayProperty] = "kArrayProperty";
}

void CreatePODStringMap(PODStringMap& podStringMap)
{
	podStringMap[Alembic::AbcGeom::kBooleanPOD] = "kBooleanPOD";
	podStringMap[Alembic::AbcGeom::kUint8POD] = "kUint8POD";
	podStringMap[Alembic::AbcGeom::kInt8POD] = "kInt8POD";
	podStringMap[Alembic::AbcGeom::kUint16POD] = "kUint16POD";
	podStringMap[Alembic::AbcGeom::kInt16POD] = "kInt16POD";
	podStringMap[Alembic::AbcGeom::kUint32POD] = "kUint32POD";
	podStringMap[Alembic::AbcGeom::kInt32POD] = "kInt32POD";
	podStringMap[Alembic::AbcGeom::kUint64POD] = "kUint64POD";
	podStringMap[Alembic::AbcGeom::kInt64POD] = "kInt64POD";
	podStringMap[Alembic::AbcGeom::kFloat16POD] = "kFloat16POD";
	podStringMap[Alembic::AbcGeom::kFloat32POD] = "kFloat32POD";
	podStringMap[Alembic::AbcGeom::kFloat64POD] = "kFloat64POD";
	podStringMap[Alembic::AbcGeom::kStringPOD] = "kStringPOD";
	podStringMap[Alembic::AbcGeom::kWstringPOD] = "kWstringPOD";
	podStringMap[Alembic::AbcGeom::kNumPlainOldDataTypes] = "kNumPlainOldDataTypes";
	podStringMap[Alembic::AbcGeom::kUnknownPOD] = "kUnknownPOD";
}

#ifdef REQUIRES_BOOST
void TokenizePath( const std::string &path, PathList& result )
{
    typedef boost::char_separator<char> Separator;
    typedef boost::tokenizer<Separator> Tokenizer;

    Tokenizer tokenizer( path, Separator( "/" ) );

    for ( Tokenizer::iterator iter = tokenizer.begin() ; iter != tokenizer.end() ;
          ++iter )
    {
		if ( (*iter).empty() )
		{
			continue;
		}
		std::cout << "*iter = " << *iter << std::endl;
		result.push_back( *iter );
    }
}
#endif // REQUIRES_BOOST

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
void vtkAlembicImporter::ProcessIPolyMesh(const Alembic::AbcGeom::IPolyMesh& pmesh)
{
  vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
  vtkSmartPointer<vtkCellArray> polys = vtkSmartPointer<vtkCellArray>::New();
  vtkSmartPointer<vtkPolyData> polydata = vtkSmartPointer<vtkPolyData>::New();

    Alembic::AbcGeom::IPolyMeshSchema::Sample samp;
    if ( pmesh.getSchema().getNumSamples() > 0 )
    {
        pmesh.getSchema().get( samp );

        Alembic::AbcGeom::P3fArraySamplePtr P = samp.getPositions();
        Alembic::AbcGeom::Int32ArraySamplePtr indices = samp.getFaceIndices();
        Alembic::AbcGeom::Int32ArraySamplePtr counts = samp.getFaceCounts();
        Alembic::AbcGeom::V3fArraySamplePtr v = samp.getVelocities();

        size_t P_size = P->size();
        size_t indices_size = indices->size();
        size_t counts_size = counts->size();

        std::cout << "P size = " << P_size << std::endl;
        std::cout << "indices size = " << indices_size << std::endl;
        std::cout << "counts size = " << counts_size << std::endl;
        if (v != 0)
        {
            size_t v_size = v->size();
            std::cout << "v size = " << v_size << std::endl;
        }

        for (size_t i=0;i<P_size;i++)
        {
            std::cout << "P[" << i << "] = {"
                      << P->get()[i].x << ","
                      << P->get()[i].y << ","
                      << P->get()[i].z << "}"
                      << std::endl;
            points->InsertNextPoint(P->get()[i].x, P->get()[i].y, P->get()[i].z);
        }

        for (size_t i=0;i<indices_size;i++)
        {
            std::cout << "indices[" << i << "] = {"
                      << indices->get()[i] << "}"
                      << std::endl;
        }

        for (size_t i=0;i<counts_size;i++)
        {
            std::cout << "counts[" << i << "] = {"
                      << counts->get()[i] << "}"
                      << std::endl;
        }

        size_t face_index = 0;
        for (size_t i=0;i<counts_size;i++)
        {
        	size_t polyface_vertex_count = counts->get()[i];
            polys->InsertNextCell(polyface_vertex_count);
            for (size_t j=0;j<polyface_vertex_count;j++)
            {
            	printf("face_index = %d\n",face_index);
                polys->InsertCellPoint(indices->get()[face_index++]);
            }
        }

        const Alembic::AbcGeom::IPolyMeshSchema &ipms = pmesh.getSchema();

        PODStringMap podStringMap;
        PropertyTypeStringMap propertyTypeStringMap;
        CreatePODStringMap(podStringMap);
        CreatePropertyTypeStringMap(propertyTypeStringMap);

        // Texture coordinates
        {
            Alembic::AbcGeom::IV2fGeomParam uvParams = ipms.getUVsParam();
            if (uvParams.valid())
            {
                printf("Have UVs\n");
                Alembic::AbcGeom::IV2fGeomParam::Sample uvValue = uvParams.getIndexedValue();
                if (uvValue.valid())
                {
                    Alembic::AbcGeom::UInt32ArraySamplePtr indices = uvValue.getIndices();
                    Alembic::AbcGeom::V2fArraySamplePtr uvs = uvValue.getVals();
                    for (size_t i=0;i<P_size;i++)
                    {
                        // (*(uvsamp.getVals()))
                        Alembic::AbcGeom::V2f uv = (*(uvValue.getVals()))[i];
                        printf("UV[%lu] = {%f,%f}\n",i,uv[0],uv[1]);
                    }
                }
            }
        }
        // Custom and Arbitrary data stuff
        {
            // UserProperties stuff
            Alembic::AbcGeom::ICompoundProperty userProps = ipms.getUserProperties();
            if (userProps.valid())
            {
                size_t numUserProps = userProps.getNumProperties();
                for (size_t i=0;i<numUserProps;i++)
                {
                    std::string propertyTypeName = propertyTypeStringMap[userProps.getPropertyHeader(i).getPropertyType()];
                    printf("UserProps[%ld] = %s of type [%s]\n",
                           i,
                           userProps.getPropertyHeader(i).getName().c_str(),
                           propertyTypeName.c_str()
                           );
                    if (userProps.getPropertyHeader(i).getPropertyType() != Alembic::AbcGeom::kCompoundProperty)
                    {
                        std::string podName = podStringMap[userProps.getPropertyHeader(i).getDataType().getPod()];
                        printf("\tProperty has type %s\n",podName.c_str());
                    }
                }
            }
        }

        {
            // ArbGeomParams stuff

            Alembic::AbcGeom::ICompoundProperty arbParams = ipms.getArbGeomParams();
            if (arbParams.valid())
            {
                size_t numArbParams = arbParams.getNumProperties();
                for (size_t i=0;i<numArbParams;i++)
                {
                    std::string propertyTypeName = propertyTypeStringMap[arbParams.getPropertyHeader(i).getPropertyType()];
                    printf("ArbParams[%ld] = %s of type [%s]\n",
                           i,
                           arbParams.getPropertyHeader(i).getName().c_str(),
                           propertyTypeName.c_str()
                           );
                    if (arbParams.getPropertyHeader(i).getPropertyType() != Alembic::AbcGeom::kCompoundProperty)
                    {
                        std::string podName = podStringMap[arbParams.getPropertyHeader(i).getDataType().getPod()];
                        printf("\tProperty has type %s\n",podName.c_str());

                        if (arbParams.getPropertyHeader(i).getName().compare("grid_id")==0)
                        {
                            Alembic::AbcGeom::IInt32GeomParam grid_id(arbParams,"grid_id");
                            Alembic::AbcGeom::Int32ArraySamplePtr intArray = grid_id.getValueProperty().getValue();
                            int32_t *gridID = (int32_t*)(intArray.get()->getData());
                            printf("\t\tFound Grid Animation ID = %d\n",(*gridID));
                        }
                        else if (arbParams.getPropertyHeader(i).getName().compare("mtoa_")>0)
                        {
                            printf("Found arbgeomparams matching mtoa_* - START\n");
                            switch (arbParams.getPropertyHeader(i).getDataType().getPod())
                            {
                            case Alembic::Util::kBooleanPOD :
                                {
                                    Alembic::AbcGeom::IBoolGeomParam mtoa_(arbParams,arbParams.getPropertyHeader(i).getName());
                                    Alembic::AbcGeom::BoolArraySamplePtr boolArray = mtoa_.getValueProperty().getValue();
                                    bool *mtoa_Value = (bool*)(boolArray.get()->getData());
                                    printf("\t\tFound mtoa_* value %d\n",*mtoa_Value);
                                }
                                break;
                            case Alembic::Util::kUint8POD :
                                break;
                            case Alembic::Util::kInt8POD :
                                break;
                            case Alembic::Util::kUint16POD :
                                break;
                            case Alembic::Util::kInt16POD :
                                break;
                            case Alembic::Util::kUint32POD :
                                break;
                            case Alembic::Util::kInt32POD :
                                {
                                    Alembic::AbcGeom::IInt32GeomParam mtoa_(arbParams,arbParams.getPropertyHeader(i).getName());
                                    Alembic::AbcGeom::Int32ArraySamplePtr intArray = mtoa_.getValueProperty().getValue();
                                    int32_t *mtoa_Value = (int32_t*)(intArray.get()->getData());
                                    printf("\t\tFound mtoa_* value %d\n",*mtoa_Value);
                                }
                                break;
                            case Alembic::Util::kUint64POD :
                                break;
                            case Alembic::Util::kInt64POD :
                                break;
                            case Alembic::Util::kFloat16POD :
                                break;
                            case Alembic::Util::kFloat32POD :
								{
									Alembic::AbcGeom::IFloatGeomParam mtoa_(arbParams,arbParams.getPropertyHeader(i).getName());
									Alembic::AbcGeom::FloatArraySamplePtr floatArray = mtoa_.getValueProperty().getValue();
									float *mtoa_Value = (float*)(floatArray.get()->getData());
									std::string interpretation = arbParams.getPropertyHeader(i).getMetaData().get("interpretation");
									if (interpretation.compare("rgb")==0)
									{
										printf("\t\tFound mtoa_* value COLOR[%f,%f,%f]\n",mtoa_Value[0],mtoa_Value[1],mtoa_Value[2]);
									}
									else if (interpretation.compare("vector")==0)
									{
										uint8_t extent = arbParams.getPropertyHeader(i).getDataType().getExtent();
										/*
										  printf("\t\tFound mtoa_* value PNT extent = %u\n",extent);
										  printf("\t\tFound mtoa_* value PNT rank = %lu\n",floatArray.get()->getDimensions().rank());
										  printf("\t\tFound mtoa_* value PNT numPoints = %lu\n",floatArray.get()->getDimensions().numPoints());
										  printf("\t\tFound mtoa_* value PNT size = %lu\n",floatArray.get()->size());
										*/
										switch (extent)
										{
										case 2:
											printf("\t\tFound mtoa_* value PNT[%f,%f]\n",mtoa_Value[0],mtoa_Value[1]);
											break;
										case 3:
											printf("\t\tFound mtoa_* value VECTOR[%f,%f,%f]\n",mtoa_Value[0],mtoa_Value[1],mtoa_Value[2]);
											break;
										}
									}
								}
                                break;
                            case Alembic::Util::kFloat64POD :
								{
									Alembic::AbcGeom::IDoubleGeomParam mtoa_(arbParams,arbParams.getPropertyHeader(i).getName());
									Alembic::AbcGeom::DoubleArraySamplePtr doubleArray = mtoa_.getValueProperty().getValue();
									double *mtoa_Value = (double*)(doubleArray.get()->getData());
									std::string interpretation = arbParams.getPropertyHeader(i).getMetaData().get("interpretation");
									if (interpretation.compare("vector")==0)
									{
										printf("\t\tFound mtoa_* value VECTOR[%e,%e,%e]\n",mtoa_Value[0],mtoa_Value[1],mtoa_Value[2]);
									}
								}
                                break;
                            case Alembic::Util::kStringPOD :
								{
									Alembic::AbcGeom::IStringGeomParam mtoa_(arbParams,arbParams.getPropertyHeader(i).getName());
									Alembic::AbcGeom::StringArraySamplePtr stringArray = mtoa_.getValueProperty().getValue();
									char** mtoa_Value = (char **)(stringArray.get()->getData());
									printf("\t\tFound mtoa_* value STRING [%s]\n",mtoa_Value[0]);
								}
                                break;
                            case Alembic::Util::kWstringPOD :
                                break;
                            }
                            printf("Found arbgeomparams matching mtoa_* - END\n");
                        }
                    }
                }
            }
        }
    }
    polydata->SetPoints(points);
    polydata->SetPolys(polys);
    {

  	  // Create a mapper and actor
  	  vtkNew<vtkPolyDataMapper> polyMapper;

  	  polyMapper->SetInputData(polydata);

  	  vtkNew<vtkActor> polyActor;
  	polyActor->SetMapper(polyMapper);
  	  /*
  	  sphereActor->GetProperty()->SetColor(
  	      colors->GetColor3d("MistyRose").GetData());
  	      */
  	  this->Renderer->AddActor(polyActor);
    }
}

//------------------------------------------------------------------------------
void vtkAlembicImporter::IterateIObject(const Alembic::Abc::IObject &parent,
										const Alembic::Abc::ObjectHeader &ohead)
{
	std::cout << "Parent Full name " << parent.getFullName() << std::endl;
	std::cout << "ohead Full name " << ohead.getFullName() << std::endl;

	//set this if we should continue traversing
	Alembic::Abc::IObject nextParentObject;

	if ( Alembic::AbcGeom::IXform::matches( ohead ) )
	{
		std::cout << "iterate_iobject match IXform" << std::endl;
		Alembic::AbcGeom::IXform xform( parent, ohead.getName() );

		nextParentObject = xform;

	}
	else if ( Alembic::AbcGeom::ISubD::matches( ohead ) )
	{
		std::cout << "iterate_iobject match ISubD" << std::endl;
		Alembic::AbcGeom::ISubD subd( parent, ohead.getName() );
		nextParentObject = subd;

	}
	else if ( Alembic::AbcGeom::IPolyMesh::matches( ohead ) )
	{
		std::cout << "iterate_iobject match IPolyMesh" << std::endl;
		Alembic::AbcGeom::IPolyMesh polymesh( parent, ohead.getName() );
		ProcessIPolyMesh(polymesh);
		nextParentObject = polymesh;

	}
	else if ( Alembic::AbcGeom::INuPatch::matches( ohead ) )
	{
		std::cout << "iterate_iobject match INuPatch" << std::endl;

	}
	else if ( Alembic::AbcGeom::IPoints::matches( ohead ) )
	{
		std::cout << "iterate_iobject match IPoints" << std::endl;
		Alembic::AbcGeom::IPoints points( parent, ohead.getName() );
		// ProcessIPoints(points);
		nextParentObject = points;
	}
	else if ( Alembic::AbcGeom::ICurves::matches( ohead ) )
	{
		std::cout << "iterate_iobject match ICurves" << std::endl;

	}
	else if ( Alembic::AbcGeom::IFaceSet::matches( ohead ) )
	{
		std::cout << "iterate_iobject match IFaceSet" << std::endl;
		std::cerr << "DOH !" << std::endl;
	}

	// Recursion
	if ( nextParentObject.valid() )
	{
		for ( size_t i = 0; i < nextParentObject.getNumChildren() ; ++i )
		{
			IterateIObject( nextParentObject, nextParentObject.getChildHeader( i ));
		}
	}
}
//------------------------------------------------------------------------------
void vtkAlembicImporter::ReadData()
{
	Alembic::AbcCoreFactory::IFactory factory;
	Alembic::AbcCoreFactory::IFactory::CoreType core_type;

	Alembic::Abc::IArchive archive = factory.getArchive(this->GetFileName(), core_type);

	Alembic::Abc::IObject top = archive.getTop();
	size_t top_num_children = top.getNumChildren();

	//   vtkDebugMacro("Done with " << this->GetClassName() << "::" << __FUNCTION__);
	std::cout <<  top_num_children << std::endl;

    for ( size_t i = 0; i < top.getNumChildren(); ++i )
    {
    	IterateIObject( top, top.getChildHeader(i));
    }

    if (false){
    	  // vtkNew<vtkNamedColors> colors;

    	  // Create a sphere
    	  vtkNew<vtkSphereSource> sphereSource;
    	  sphereSource->SetCenter(1.0, 0.0, 0.0);
    	  sphereSource->Update();

    	  // Create a mapper and actor
    	  vtkNew<vtkPolyDataMapper> sphereMapper;
    	  sphereMapper->SetInputConnection(sphereSource->GetOutputPort());

    	  vtkNew<vtkActor> sphereActor;
    	  sphereActor->SetMapper(sphereMapper);
    	  /*
    	  sphereActor->GetProperty()->SetColor(
    	      colors->GetColor3d("MistyRose").GetData());
    	      */
    	  this->Renderer->AddActor(sphereActor);

    }

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
