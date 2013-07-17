/*==============================================================================

  Program: 3D Slicer

  Portions (c) Copyright Brigham and Women's Hospital (BWH) All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

==============================================================================*/

// Qt includes
#include <QtPlugin>

// SlicerDataStore Logic includes
#include <vtkSlicerDataStoreLogic.h>

// SlicerDataStore includes
#include "qSlicerDataStoreModule.h"
#include "qSlicerDataStoreModuleWidget.h"
#include "DataStoreGUI.h"
#include <qSlicerSceneBundleIO.h>

//-----------------------------------------------------------------------------
Q_EXPORT_PLUGIN2(qSlicerDataStoreModule, qSlicerDataStoreModule);

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerDataStoreModulePrivate
{
public:
  qSlicerDataStoreModulePrivate();
};

//-----------------------------------------------------------------------------
// qSlicerDataStoreModulePrivate methods

//-----------------------------------------------------------------------------
qSlicerDataStoreModulePrivate
::qSlicerDataStoreModulePrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerDataStoreModule methods

//-----------------------------------------------------------------------------
qSlicerDataStoreModule
::qSlicerDataStoreModule(QObject* _parent)
  : Superclass(_parent)
  , d_ptr(new qSlicerDataStoreModulePrivate)
{
}

//-----------------------------------------------------------------------------
qSlicerDataStoreModule::~qSlicerDataStoreModule()
{
}

//-----------------------------------------------------------------------------
QString qSlicerDataStoreModule::helpText()const
{
  return "This is a loadable module bundled in an extension";
}

//-----------------------------------------------------------------------------
QString qSlicerDataStoreModule::acknowledgementText()const
{
  return "This work was was partially funded by NIH grant 3P41RR013218-12S1";
}

//-----------------------------------------------------------------------------
QStringList qSlicerDataStoreModule::contributors()const
{
  QStringList moduleContributors;
  moduleContributors << QString("Jean-Baptiste Berger (Kitware)");
  return moduleContributors;
}

//-----------------------------------------------------------------------------
QIcon qSlicerDataStoreModule::icon()const
{
  return QIcon(":/Icons/LoadableModuleTemplate.png");
}

//-----------------------------------------------------------------------------
QStringList qSlicerDataStoreModule::categories() const
{
  return QStringList() << "Examples";
}

//-----------------------------------------------------------------------------
QStringList qSlicerDataStoreModule::dependencies() const
{
  return QStringList();
}

//-----------------------------------------------------------------------------
void qSlicerDataStoreModule::setup()
{
  this->Superclass::setup();
  this->Logic = vtkSlicerDataStoreLogic::SafeDownCast(this->logic());
}

//-----------------------------------------------------------------------------
void qSlicerDataStoreModule::LoadScene(QString MRMLFilePath)
{
  this->Logic->LoadMRMLScene(MRMLFilePath);
}

//-----------------------------------------------------------------------------
void qSlicerDataStoreModule::SaveScene(QString fileName)
{
  this->Logic->SaveMRMLScene(fileName);
}

//-----------------------------------------------------------------------------
qSlicerAbstractModuleRepresentation * qSlicerDataStoreModule
::createWidgetRepresentation()
{
  return new qSlicerDataStoreModuleWidget;
}

//-----------------------------------------------------------------------------
vtkMRMLAbstractLogic* qSlicerDataStoreModule::createLogic()
{
  return vtkSlicerDataStoreLogic::New();
}
