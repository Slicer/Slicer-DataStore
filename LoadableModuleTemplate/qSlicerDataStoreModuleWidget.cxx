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

// SlicerQt includes
#include "qSlicerDataStoreModuleWidget.h"
#include "qSlicerDataStoreModule.h"
#include "ui_qSlicerDataStoreModuleWidget.h"

//LoadableModule includes
#include "DataStoreGUI.h"

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
    this->DataStoreWindow = new DataStoreGUI();
    QObject::connect(d->SlicerDataStoreWidget, SIGNAL(DisplayButtonClicked()),
                     this->DataStoreWindow, SLOT(displayWindow()));
    QObject::connect(d->SlicerDataStoreWidget, SIGNAL(UrlModified(QString)),
                     this->DataStoreWindow, SLOT(loadDataStoreURLs(QString)));
    QObject::connect(this->DataStoreWindow, SIGNAL(ScheduleLoad(QString)),
                     this->Module, SLOT(LoadScene(QString)));
    QObject::connect(this->DataStoreWindow, SIGNAL(ScheduleSave(QString)),
                     this->Module, SLOT(SaveScene(QString)));
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
  this->DataStoreWindow = 0;
  
  this->Superclass::setup();
}

