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
#include <QDebug>
#include <QSettings>
#include <QDir>

// SlicerQt includes
#include "qSlicerDataStoreModuleWidget.h"
#include "qSlicerDataStoreModule.h"
#include "ui_qSlicerDataStoreModuleWidget.h"
#include <qSlicerCoreApplication.h>

//Module includes
#include "qDataStoreWidget.h"

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerDataStoreModuleWidgetPrivate: public Ui_qSlicerDataStoreModuleWidget
{
public:
  qSlicerDataStoreModuleWidgetPrivate();
};

//-----------------------------------------------------------------------------
// qSlicerDataStoreModuleWidgetPrivate methods

//-----------------------------------------------------------------------------
qSlicerDataStoreModuleWidgetPrivate::qSlicerDataStoreModuleWidgetPrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerDataStoreModuleWidget methods

//-----------------------------------------------------------------------------
qSlicerDataStoreModuleWidget::qSlicerDataStoreModuleWidget(QWidget* _parent)
  : Superclass( _parent )
  , d_ptr( new qSlicerDataStoreModuleWidgetPrivate )
{
}

//-----------------------------------------------------------------------------
qSlicerDataStoreModuleWidget::~qSlicerDataStoreModuleWidget()
{
}

//-----------------------------------------------------------------------------
void qSlicerDataStoreModuleWidget::enter()
{
  Q_D(qSlicerDataStoreModuleWidget);
  
  if(!this->DataStoreWindow)
    {
    this->DataStoreWindow = new qDataStoreWidget();
    QObject::connect(d->SlicerDataStoreWidget, SIGNAL(DisplayButtonClicked()),
                     this->DataStoreWindow, SLOT(displayWindow()));
    QObject::connect(d->SlicerDataStoreWidget, SIGNAL(UrlModified(QString)),
                     this->DataStoreWindow, SLOT(loadDataStoreURLs(QString)));
    QObject::connect(this->DataStoreWindow, SIGNAL(ScheduleLoad(QString)),
                     this->Module, SLOT(LoadScene(QString)));
    QObject::connect(this->DataStoreWindow, SIGNAL(ScheduleSave(QString)),
                     this->Module, SLOT(SaveScene(QString)));
    
    qSlicerCoreApplication * coreApp = qSlicerCoreApplication::application();
    QString dirPath = 	QFileInfo(coreApp->userSettings()->fileName()).absoluteDir().path() + QString("/DataStore/");
    QSettings settings (dirPath + "settings.ini",
                    QSettings::IniFormat);
  
    QString url = settings.value("datastore/serverUrl").toString();
    if(url.isEmpty())
      {
      url = QString("http://slicer.kitware.com/midas3");
      }
    this->DataStoreWindow->loadDataStoreURLs(url);
    }
  this->DataStoreWindow->show();
  
  this->Superclass::enter();
}

//-----------------------------------------------------------------------------
void qSlicerDataStoreModuleWidget::setup()
{
  Q_D(qSlicerDataStoreModuleWidget);
  d->setupUi(this);
  
  this->Module = dynamic_cast<qSlicerDataStoreModule*>(this->module());
  this->DataStoreWindow = nullptr;
  
  this->Superclass::setup();
}

