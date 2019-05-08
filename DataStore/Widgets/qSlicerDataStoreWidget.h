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

// .NAME qSlicerDataStoreWidget - Ui component located on the left hand of the application


#ifndef __qSlicerDataStoreWidget_h
#define __qSlicerDataStoreWidget_h

// Qt includes
#include <QWidget>

// Button Widgets includes
#include "qSlicerDataStoreModuleWidgetsExport.h"

class qSlicerDataStoreWidgetPrivate;

/// \ingroup Slicer_QtModules_LoadableModuleTemplate
class Q_SLICER_MODULE_DATASTORE_WIDGETS_EXPORT qSlicerDataStoreWidget
  : public QWidget
{
  Q_OBJECT
public:
  typedef QWidget Superclass;
  explicit qSlicerDataStoreWidget(QWidget *parent=nullptr);
  ~qSlicerDataStoreWidget() override;

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
