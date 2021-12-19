add_executable(SourceExample # MACOSX_BUNDLE
  SourceExample.cxx
  vtkSource.cxx
  vtkPODInt.cxx
)
 
target_link_libraries(SourceExample
  VTK::CommonCore
  VTK::CommonExecutionModel
  )


install(TARGETS SourceExample
  DESTINATION
  bin
  )

install(IMPORTED_RUNTIME_ARTIFACTS SourceExample)

