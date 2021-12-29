find_package(Alembic CONFIG REQUIRED)

add_library(vtkAlembic
  src/vtkAlembicImporter.cxx
  )

if (WIN32)
   target_compile_definitions(vtkAlembic PUBLIC IOImport_EXPORTS)
endif()

target_link_libraries(vtkAlembic
  VTK::IOImport
  Alembic::Alembic
  )

include_directories(src)

add_executable(test_vtkAlembicImporter
  tests/test_vtkAlembicImporter_main.cxx
  )

target_link_libraries(test_vtkAlembicImporter
  vtkAlembic
  VTK::IOImport
  )

# OBJ tests
add_executable(interactive_vtkOBJImporter
  tests/interactive_vtkOBJImporter_main.cxx
  )

target_link_libraries(interactive_vtkOBJImporter
  PRIVATE ${VTK_LIBRARIES})

# vtk_module_autoinit is needed
vtk_module_autoinit(
  TARGETS interactive_vtkOBJImporter
  MODULES ${VTK_LIBRARIES}
  )

# Alembic tests

add_executable(interactive_vtkAlembicImporter
  tests/interactive_vtkAlembicImporter_main.cxx
  )

target_link_libraries(interactive_vtkAlembicImporter
  PRIVATE ${VTK_LIBRARIES}
  vtkAlembic)

# vtk_module_autoinit is needed
vtk_module_autoinit(
  TARGETS interactive_vtkAlembicImporter
  MODULES ${VTK_LIBRARIES}
  )
