add_executable(SinkExample # MACOSX_BUNDLE
  SinkExample.cxx
  vtkSink.cxx
  vtkPODInt.cxx
)
 
target_link_libraries(SinkExample
  VTK::CommonCore
  VTK::CommonExecutionModel
  )


install(TARGETS SinkExample
  DESTINATION
  bin
  )

# install(IMPORTED_RUNTIME_ARTIFACTS SinkExample)
