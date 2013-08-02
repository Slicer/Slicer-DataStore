set(KIT qSlicer${MODULE_NAME}Module)

#-----------------------------------------------------------------------------
set(KIT_TEST_SRCS
  vtkSlicerDataStoreLogicTest.cxx
  qSlicerDataStoreModuleTest.cxx
  )

set(INPUT "${CMAKE_CURRENT_SOURCE_DIR}/../Data/")

#-----------------------------------------------------------------------------
slicerMacroConfigureModuleCxxTestDriver(
  NAME ${KIT}
  SOURCES ${KIT_TEST_SRCS}
  WITH_VTK_DEBUG_LEAKS_CHECK
  )

#-----------------------------------------------------------------------------
simple_test(vtkSlicerDataStoreLogicTest ${INPUT}EmptyScene.mrb)
simple_test(qSlicerDataStoreModuleTest)
