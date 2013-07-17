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

#ifndef __qSlicerDataStoreWidget_h
#define __qSlicerDataStoreWidget_h

// Qt includes
#include <QWidget>

// Button Widgets includes
#include "qSlicerLoadableModuleTemplateModuleWidgetsExport.h"

class qSlicerDataStoreWidgetPrivate;

/// \ingroup Slicer_QtModules_LoadableModuleTemplate
class Q_SLICER_MODULE_LOADABLEMODULETEMPLATE_WIDGETS_EXPORT qSlicerDataStoreWidget
  : public QWidget
{
  Q_OBJECT
public:
  typedef QWidget Superclass;
  qSlicerDataStoreWidget(QWidget *parent=0);
  virtual ~qSlicerDataStoreWidget();

protected slots:
  void onDisplayButtonClicked();
  void onUrlModified();

signals:
  void DisplayButtonClicked();
  void UrlModified(QString);
  

protected:
  QScopedPointer<qSlicerDataStoreWidgetPrivate> d_ptr;

private:
  QString PreviousUrl;
  
  Q_DECLARE_PRIVATE(qSlicerDataStoreWidget);
  Q_DISABLE_COPY(qSlicerDataStoreWidget);
};

#endif
