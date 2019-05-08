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

// .NAME qSlicerDataStoreModule

#ifndef __qSlicerDataStoreModule_h
#define __qSlicerDataStoreModule_h

// SlicerQt includes
#include "qSlicerLoadableModule.h"

#include "qSlicerDataStoreModuleExport.h"

class qSlicerDataStoreModulePrivate;
class vtkSlicerDataStoreLogic;

/// \ingroup Slicer_QtModules_ExtensionTemplate
class Q_SLICER_QTMODULES_DATASTORE_EXPORT
qSlicerDataStoreModule
  : public qSlicerLoadableModule
{
  Q_OBJECT
#ifdef Slicer_HAVE_QT5
  Q_PLUGIN_METADATA(IID "org.slicer.modules.loadable.qSlicerLoadableModule/1.0");
#endif
  Q_INTERFACES(qSlicerLoadableModule);

public:

  typedef qSlicerLoadableModule Superclass;
  explicit qSlicerDataStoreModule(QObject *parent=nullptr);
  ~qSlicerDataStoreModule() override;

  qSlicerGetTitleMacro(QTMODULE_TITLE);

  QString helpText()const override;
  QString acknowledgementText()const override;
  QStringList contributors()const override;

  QIcon icon()const override;

  QStringList categories()const override;
  QStringList dependencies() const override;
  
public slots:
  void LoadScene(QString MRMLFilePath);
  void SaveScene(QString fileName);

protected:

  /// Initialize the module. Register the volumes reader/writer
  void setup() override;

  /// Create and return the widget representation associated to this module
  qSlicerAbstractModuleRepresentation * createWidgetRepresentation() override;

  /// Create and return the logic associated to this module
  vtkMRMLAbstractLogic* createLogic() override;

protected:
  QScopedPointer<qSlicerDataStoreModulePrivate> d_ptr;

private:
  vtkSlicerDataStoreLogic* Logic;
  
  Q_DECLARE_PRIVATE(qSlicerDataStoreModule);
  Q_DISABLE_COPY(qSlicerDataStoreModule);

};

#endif
