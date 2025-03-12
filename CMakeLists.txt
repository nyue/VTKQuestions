cmake_minimum_required(VERSION 3.12)

project(VTKQuestions)

option(ENABLE_ALEMBIC "Enable Alembic" OFF)

find_package(VTK
  COMPONENTS
  CommonColor
  CommonCore
  CommonDataModel
  FiltersSources
  InteractionStyle
  RenderingContextOpenGL2
  RenderingCore
  RenderingFreeType
  RenderingGL2PSOpenGL2
  RenderingOpenGL2
  IOImport
  CONFIG REQUIRED)

add_subdirectory(Algorithm)
add_subdirectory(Importer)

