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

// LoadableModuleTemplate Logic includes
#include "vtkSlicerDataStoreLogic.h"

// MRML includes

// VTK includes
#include <vtkNew.h>
#include <qSlicerSceneBundleIO.h>
#include <../Loadable/Data/qSlicerSceneWriter.h>

// STD includes
#include <cassert>

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkSlicerDataStoreLogic);

//----------------------------------------------------------------------------
vtkSlicerDataStoreLogic::vtkSlicerDataStoreLogic()
{
}

//----------------------------------------------------------------------------
vtkSlicerDataStoreLogic::~vtkSlicerDataStoreLogic()
{
}

//----------------------------------------------------------------------------
void vtkSlicerDataStoreLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
void vtkSlicerDataStoreLogic::LoadMRMLScene(QString mrmlFilePath)
{
  if(mrmlFilePath != "")
    {
    qSlicerSceneBundleIO* sceneLoader = new qSlicerSceneBundleIO();
    qSlicerIO::IOProperties parameters;
    parameters["fileName"] = mrmlFilePath;
    parameters["clear"] = true;
    sceneLoader->setMRMLScene(this->GetMRMLScene());
    sceneLoader->load(parameters);
    }
  else
    {
    std::cout << "Error Loading Scene : no filename" << std::endl;
    }
}

//----------------------------------------------------------------------------
void vtkSlicerDataStoreLogic::SaveMRMLScene(QString fileName)
{
  if(fileName != "")
    {
    qSlicerSceneWriter* sceneWriter = new qSlicerSceneWriter();
    qSlicerIO::IOProperties parameters;
    parameters["fileName"] = fileName;
    sceneWriter->setMRMLScene(this->GetMRMLScene());
    sceneWriter->write(parameters);
    }
  else
    {
    std::cout << "Error Saving Scene : no filename" << std::endl;
    }
}

//---------------------------------------------------------------------------
void vtkSlicerDataStoreLogic::SetMRMLSceneInternal(vtkMRMLScene * newScene)
{
  vtkNew<vtkIntArray> events;
  events->InsertNextValue(vtkMRMLScene::NodeAddedEvent);
  events->InsertNextValue(vtkMRMLScene::NodeRemovedEvent);
  events->InsertNextValue(vtkMRMLScene::EndBatchProcessEvent);
  this->SetAndObserveMRMLSceneEventsInternal(newScene, events.GetPointer());
}

//-----------------------------------------------------------------------------
void vtkSlicerDataStoreLogic::RegisterNodes()
{
  assert(this->GetMRMLScene() != 0);
}

//---------------------------------------------------------------------------
void vtkSlicerDataStoreLogic::UpdateFromMRMLScene()
{
  assert(this->GetMRMLScene() != 0);
}

//---------------------------------------------------------------------------
void vtkSlicerDataStoreLogic
::OnMRMLSceneNodeAdded(vtkMRMLNode* vtkNotUsed(node))
{
}

//---------------------------------------------------------------------------
void vtkSlicerDataStoreLogic
::OnMRMLSceneNodeRemoved(vtkMRMLNode* vtkNotUsed(node))
{
}

