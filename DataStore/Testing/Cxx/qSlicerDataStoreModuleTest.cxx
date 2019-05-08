/*==============================================================================

  Program: 3D Slicer

  Copyright (c) Kitware Inc.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  This file was originally developed by Luis Ibanez, Kitware Inc.
  and was partially funded by NIH grant 3P41RR013218-12S1

==============================================================================*/
// Slicer includes
#include "qSlicerCoreApplication.h"
#include <qSlicerAbstractModuleRepresentation.h>
#include <qSlicerApplication.h>

// Qt includes
#include <QDir>
#include <QFile>
#include <QTimer>
#include <QSettings>
#include <QTextStream>

// VTK Includes
#include "vtkMRMLScene.h"
#include "vtkNew.h"

// Modules includes
#include "vtkSlicerDataStoreLogic.h"
#include "qSlicerDataStoreModule.h"
#include "qDataStoreWidget.h"
#include "qSlicerDataStoreModuleWidget.h"

int qSlicerDataStoreModuleTest(int argc, char * argv [] )
{
  // Initialize Tests.
  qSlicerApplication app(argc, argv);
  qSlicerDataStoreModule module;
  module.initialize(0);
  qSlicerCoreApplication * coreApp = qSlicerCoreApplication::application();
  QString dataSetDir = 	QFileInfo(coreApp->userSettings()->fileName()).absoluteDir().path() + QString("/DataStore/");
  QString filename = dataSetDir + QString("qSlicerDataStoreModuleTest.mrb");
  QFile file( filename );
  if(file.exists(filename))
    {
    file.remove(filename);
    }

  // Load the WebView
  qDataStoreWidget* webWidget = new qDataStoreWidget();
  webWidget->loadDataStoreURLs(QString("http://slicer.kitware.com/midas3"));
  webWidget->show();

  QString downloadUrl("http://www.slicer.org/img/3DSlicerLogo-H-Color-218x144.png");

  // Test if we can cancel a download
  webWidget->download(downloadUrl, downloadUrl);
  webWidget->cancelDownload();

  // Test if we can and remove a dataset from the local cache.
  int numberDonwloadItems = webWidget->getDownloadedItems().split(";;").size();
  if(file.open(QIODevice::ReadWrite) )
    {
    QTextStream stream( &file );
    stream << "qSlicerDataStoreModuleTest" << endl;
    }
  file.close();

  QFileInfo fileInfo(filename);
  webWidget->addNewTreeItem(fileInfo);

  int numberNewListDonwloadItems = webWidget->getDownloadedItems().split(";;").size();
  if((numberDonwloadItems + 1) != numberNewListDonwloadItems)
    {
    std::cerr << "The method getDownloadedItems doesn't work."  << std::endl;
    return EXIT_FAILURE;
    }

  if(file.exists(filename))
    {
    file.remove(filename);
    }

  // Test the initialization of the default widgets.
  vtkNew<vtkSlicerDataStoreLogic> logic;
  vtkNew<vtkMRMLScene> scene;
  logic->SetMRMLScene(scene.GetPointer());
  module.setMRMLScene(scene.GetPointer());
  dynamic_cast<QWidget*>(module.widgetRepresentation())->show();
  dynamic_cast<qSlicerDataStoreModuleWidget*>(module.widgetRepresentation())->enter();
  dynamic_cast<qSlicerDataStoreModuleWidget*>(module.widgetRepresentation())->exit();
  QTimer::singleShot(500, &app, SLOT(quit()));

  return app.exec();
}

