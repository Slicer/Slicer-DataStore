cmake_minimum_required(VERSION 2.8.9)

project(slicerdatastore)

#-----------------------------------------------------------------------------
set(EXTENSION_HOMEPAGE "todo")
set(EXTENSION_CATEGORY "Utilities")
set(EXTENSION_CONTRIBUTORS "Charles Marion, Jean-Baptiste Berger and Jean-Christophe Fillion-Robin (Kitware)")
set(EXTENSION_DESCRIPTION "This module allows users to download and upload datasets.")
set(EXTENSION_ICONURL "todo")
set(EXTENSION_SCREENSHOTURLS "todo")

#-----------------------------------------------------------------------------
find_package(Slicer REQUIRED)
include(${Slicer_USE_FILE})

#-----------------------------------------------------------------------------
add_subdirectory(LoadableModuleDataStore)

#-----------------------------------------------------------------------------
include(${Slicer_EXTENSION_CPACK})
