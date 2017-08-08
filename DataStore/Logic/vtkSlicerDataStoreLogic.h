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

// .NAME vtkSlicerDataStoreLogic - slicer logic class for scene manipulation
// .SECTION Description
// This class manages the logic associated with reading and writing a scene


#ifndef __vtkSlicerDataStoreLogic_h
#define __vtkSlicerDataStoreLogic_h

// Slicer includes
#include "vtkSlicerModuleLogic.h"

// MRML includes

// STD includes
#include <cstdlib>

#include "vtkSlicerDataStoreModuleLogicExport.h"
#include <QString>

// CTK includes
#include <ctkPimpl.h>
class ctkPythonConsole;
class ctkErrorLogWidget;


/// \ingroup Slicer_QtModules_ExtensionTemplate
class VTK_SLICER_DATASTORE_MODULE_LOGIC_EXPORT vtkSlicerDataStoreLogic :
  public vtkSlicerModuleLogic
{
public:

  static vtkSlicerDataStoreLogic *New();
  vtkTypeMacro(vtkSlicerDataStoreLogic, vtkSlicerModuleLogic);
  void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;
  
  // Load a MRML File.
  void LoadMRMLScene(QString mrmlFilePath);
  
  // Save the current Scene
  void SaveMRMLScene(QString fileName);

protected:
  vtkSlicerDataStoreLogic();
  virtual ~vtkSlicerDataStoreLogic();

  virtual void SetMRMLSceneInternal(vtkMRMLScene* newScene) VTK_OVERRIDE;
  
  /// Register MRML Node classes to Scene. Gets called automatically when the MRMLScene is attached to this logic class.
  virtual void RegisterNodes() VTK_OVERRIDE;
  virtual void UpdateFromMRMLScene() VTK_OVERRIDE;
  virtual void OnMRMLSceneNodeAdded(vtkMRMLNode* node) VTK_OVERRIDE;
  virtual void OnMRMLSceneNodeRemoved(vtkMRMLNode* node) VTK_OVERRIDE;
private:

  vtkSlicerDataStoreLogic(const vtkSlicerDataStoreLogic&); // Not implemented
  void operator=(const vtkSlicerDataStoreLogic&);               // Not implemented
};

#endif
