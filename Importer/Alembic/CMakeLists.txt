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
