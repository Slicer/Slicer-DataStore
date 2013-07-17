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

  This file was originally developed by Jean-Christophe Fillion-Robin, Kitware Inc.
  and was partially funded by NIH grant 3P41RR013218-12S1

==============================================================================*/

// Widgets includes
#include "qSlicerDataStoreWidget.h"
#include "ui_qSlicerDataStoreWidget.h"

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
    }
}

//-----------------------------------------------------------------------------
qSlicerDataStoreWidget
::~qSlicerDataStoreWidget()
{
}
