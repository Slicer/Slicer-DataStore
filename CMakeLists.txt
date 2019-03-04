cmake_minimum_required(VERSION 3.13.0)

project("DataStore")

#-----------------------------------------------------------------------------
set(EXTENSION_HOMEPAGE "http://www.slicer.org/slicerWiki/index.php/Documentation/Nightly/Modules/DataStore")
set(EXTENSION_CATEGORY "Informatics")
set(EXTENSION_CONTRIBUTORS "Charles Marion, Jean-Baptiste Berger and Jean-Christophe Fillion-Robin (Kitware)")
set(EXTENSION_DESCRIPTION "This module allows users to download and upload datasets from a common database.")
set(EXTENSION_ICONURL "http://wiki.slicer.org/slicerWiki/images/a/a9/DataStoreIcon_128.png")
set(EXTENSION_SCREENSHOTURLS "http://www.slicer.org/slicerWiki/images/thumb/d/da/DownloadFile_2.jpg/800px-DownloadFile_2.jpg http://www.slicer.org/slicerWiki/images/thumb/8/85/DownloadFile_3.jpg/800px-DownloadFile_3.jpg http://www.slicer.org/slicerWiki/images/thumb/5/54/UploadFile_2.jpg/800px-UploadFile_2.jpg")

#-----------------------------------------------------------------------------
find_package(Slicer REQUIRED)
include(${Slicer_USE_FILE})

#-----------------------------------------------------------------------------
find_package(qRestAPI REQUIRED)
include(${qRestAPI_USE_FILE})

#-----------------------------------------------------------------------------
add_subdirectory(DataStore)

#-----------------------------------------------------------------------------
include(${Slicer_EXTENSION_CPACK})
