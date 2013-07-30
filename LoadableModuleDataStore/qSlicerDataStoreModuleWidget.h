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

// .NAME qSlicerDataStoreModuleWidget

#ifndef __qSlicerDataStoreModuleWidget_h
#define __qSlicerDataStoreModuleWidget_h

// SlicerQt includes
#include "qSlicerAbstractModuleWidget.h"

#include "qSlicerDataStoreModuleExport.h"

class qSlicerDataStoreModuleWidgetPrivate;
class qSlicerDataStoreModule;
class vtkMRMLNode;
class DataStoreGUI;

/// \ingroup Slicer_QtModules_ExtensionTemplate
class Q_SLICER_QTMODULES_DATASTORE_EXPORT qSlicerDataStoreModuleWidget :
  public qSlicerAbstractModuleWidget
{
  Q_OBJECT

public:

  typedef qSlicerAbstractModuleWidget Superclass;
  qSlicerDataStoreModuleWidget(QWidget *parent=0);
  virtual ~qSlicerDataStoreModuleWidget();
  virtual void enter();

public slots:


protected:
  QScopedPointer<qSlicerDataStoreModuleWidgetPrivate> d_ptr;
  
  virtual void setup();

private:
  Q_DECLARE_PRIVATE(qSlicerDataStoreModuleWidget);
  Q_DISABLE_COPY(qSlicerDataStoreModuleWidget);
  
  DataStoreGUI* DataStoreWindow;
  qSlicerDataStoreModule* Module;
};

#endif
