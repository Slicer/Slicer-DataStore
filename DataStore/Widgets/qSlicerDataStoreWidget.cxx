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

// Widgets includes
#include "qSlicerDataStoreWidget.h"
#include "ui_qSlicerDataStoreWidget.h"
#include <QSettings>
#include <QDir>
#include <QDebug>

// Slicer includes
#include <qSlicerCoreApplication.h>

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_LoadableModuleTemplate
class qSlicerDataStoreWidgetPrivate
  : public Ui_qSlicerDataStoreWidget
{
  Q_DECLARE_PUBLIC(qSlicerDataStoreWidget);
protected:
  qSlicerDataStoreWidget* const q_ptr;

public:
  qSlicerDataStoreWidgetPrivate(
    qSlicerDataStoreWidget& object);
  virtual ~qSlicerDataStoreWidgetPrivate() {}
  virtual void setupUi(qSlicerDataStoreWidget*);
};

// --------------------------------------------------------------------------
qSlicerDataStoreWidgetPrivate
::qSlicerDataStoreWidgetPrivate(
  qSlicerDataStoreWidget& object)
  : q_ptr(&object)
{
}

// --------------------------------------------------------------------------
void qSlicerDataStoreWidgetPrivate
::setupUi(qSlicerDataStoreWidget* widget)
{
  this->Ui_qSlicerDataStoreWidget::setupUi(widget);
}

//-----------------------------------------------------------------------------
// qSlicerDataStoreWidget methods

//-----------------------------------------------------------------------------
qSlicerDataStoreWidget
::qSlicerDataStoreWidget(QWidget* parentWidget)
  : Superclass( parentWidget )
  , d_ptr( new qSlicerDataStoreWidgetPrivate(*this) )
{
  Q_D(qSlicerDataStoreWidget);
  d->setupUi(this);
  
  qSlicerCoreApplication * coreApp = qSlicerCoreApplication::application();
  QString dirPath  = 	QFileInfo(coreApp->userSettings()->fileName()).absoluteDir().path() + QString("/DataStore/");
  QSettings settings (dirPath + "settings.ini", QSettings::IniFormat);
  
  QString url = settings.value("datastore/serverUrl").toString();
  if(!url.isEmpty())
    {
    d->DataStoreUrl->setText(url);
    }
  else
    {
    d->DataStoreUrl->setText(QString("http://slicer.kitware.com/midas3"));
    }

  this->PreviousUrl = d->DataStoreUrl->text();
  
  QObject::connect(d->DisplayButton, SIGNAL(clicked()),
                     this, SLOT(onDisplayButtonClicked()));
  QObject::connect(d->DataStoreUrl, SIGNAL(editingFinished()),
                     this, SLOT(onUrlModified()));
}

//-----------------------------------------------------------------------------
void qSlicerDataStoreWidget::onDisplayButtonClicked()
{
  emit DisplayButtonClicked();
}

//-----------------------------------------------------------------------------
void qSlicerDataStoreWidget::onUrlModified()
{
  Q_D(qSlicerDataStoreWidget);
  if(d->DataStoreUrl->text() != this->PreviousUrl)
    {     
    emit UrlModified(d->DataStoreUrl->text());
    this->PreviousUrl = d->DataStoreUrl->text();
    qSlicerCoreApplication * coreApp = qSlicerCoreApplication::application();
    QString dirPath  = 	QFileInfo(coreApp->userSettings()->fileName()).absoluteDir().path() + QString("/DataStore/");
    QSettings settings (dirPath + "settings.ini", QSettings::IniFormat);
    settings.setValue("datastore/serverUrl", this->PreviousUrl);
    }
}

//-----------------------------------------------------------------------------
qSlicerDataStoreWidget
::~qSlicerDataStoreWidget()
{
}
