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


// DataStore includes
#include "qDataStoreWidget.h"
#include "qDataStoreWidget_p.h"
#include "ui_DataStoreButtonBox.h"

// Qt Includes
#include <QDebug>
#include <QDesktopServices>
#include <QDir>
#include <QFileInfo>
#include <QString>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QSettings>
#include <QTime>
#include <QTreeWidget>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QUrlQuery>
#endif
#include <QUuid>
#if (QT_VERSION < QT_VERSION_CHECK(5, 6, 0))
#include <QWebFrame>
#include <QWebPage>
#include <QWebView>
#else
#include <QWebChannel>
#include <QWebEngineSettings>
#include <QWebEngineView>
#endif

// qMidasAPI includes
#include <qMidasAPI.h>

// Slicer includes
#include <qSlicerCoreApplication.h>
#include <qSlicerWebWidget.h>

// --------------------------------------------------------------------------
class DataStoreButtonBox : public QWidget, public Ui_DataStoreButtonBox
{
public:
  typedef QWidget Superclass;
  DataStoreButtonBox(QWidget* parent = 0) : Superclass(parent)
    {
    this->setupUi(this);
    }
};

// --------------------------------------------------------------------------
// qSlicerDataStoreWidgetWebChannelProxy

// --------------------------------------------------------------------------
void qSlicerDataStoreWidgetWebChannelProxy::download(const QString& url, const QString& thumbnail)
{
  if (!this->DataStoreWidget)
    {
    qWarning() << Q_FUNC_INFO << "DataStoreWidget is null";
    return;
    }
  this->DataStoreWidget->download(url, thumbnail);
}

// --------------------------------------------------------------------------
void qSlicerDataStoreWidgetWebChannelProxy::cancelDownload()
{
  if (!this->DataStoreWidget)
    {
    qWarning() << Q_FUNC_INFO << "DataStoreWidget is null";
    return;
    }
  this->DataStoreWidget->cancelDownload();
}

// --------------------------------------------------------------------------
QString qSlicerDataStoreWidgetWebChannelProxy::getStreamStat()
{
  if (!this->DataStoreWidget)
    {
    qWarning() << Q_FUNC_INFO << "DataStoreWidget is null";
    return QString();
    }
  return this->DataStoreWidget->getStreamStat();
}

// --------------------------------------------------------------------------
// qSlicerDataStoreWebWidget

// --------------------------------------------------------------------------
qSlicerDataStoreWebWidget::qSlicerDataStoreWebWidget(QWidget* _parent) :
    Superclass(new qSlicerDataStoreWebWidgetPrivate(*this), _parent)
{
  Q_D(qSlicerDataStoreWebWidget);
  d->init();
#if (QT_VERSION < QT_VERSION_CHECK(5, 6, 0))
  QWebSettings::globalSettings();
  this->webView()->settings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);
  this->webView()->page()->setLinkDelegationPolicy(QWebPage::DelegateExternalLinks);
#endif
}

// --------------------------------------------------------------------------
qDataStoreWidget* qSlicerDataStoreWebWidget::dataStoreWidget()
{
  Q_D(qSlicerDataStoreWebWidget);
  return d->DataStoreWidgetWebChannelProxy->DataStoreWidget;
}

// --------------------------------------------------------------------------
void qSlicerDataStoreWebWidget::setDataStoreWidget(qDataStoreWidget* widget)
{
  Q_D(qSlicerDataStoreWebWidget);
  d->DataStoreWidgetWebChannelProxy->DataStoreWidget = widget;
}

// --------------------------------------------------------------------------
void qSlicerDataStoreWebWidget::initJavascript()
{
#if (QT_VERSION < QT_VERSION_CHECK(5, 6, 0))
  this->webView()->page()->mainFrame()->addToJavaScriptWindowObject("DataStoreGUI", this->dataStoreWidget());
#endif
}

// --------------------------------------------------------------------------
void qSlicerDataStoreWebWidget::onLoadFinished(bool ok)
{
  Q_D(qSlicerDataStoreWebWidget);
  this->Superclass::onLoadFinished(ok);
  if(!ok && d->NavigationRequestAccepted)
    {
    d->setFailurePage();
    }
}

// --------------------------------------------------------------------------
// qSlicerDataStoreWebWidgetPrivate

// --------------------------------------------------------------------------
void qSlicerDataStoreWebWidgetPrivate::initializeWebChannelTransport(QByteArray& webChannelScript)
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
  this->Superclass::initializeWebChannelTransport(webChannelScript);
  webChannelScript.append(" window.DataStoreGUI = channel.objects.DataStoreGUI;\n");
#else
  Q_UNUSED(webChannelScript);
#endif
}

// --------------------------------------------------------------------------
void qSlicerDataStoreWebWidgetPrivate::initializeWebChannel(QWebChannel* webChannel)
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
  Q_Q(qSlicerDataStoreWebWidget);
  this->Superclass::initializeWebChannel(webChannel);
  webChannel->registerObject("DataStoreGUI", this->DataStoreWidgetWebChannelProxy);
#else
  Q_UNUSED(webChannel);
#endif
}

// --------------------------------------------------------------------------
void qSlicerDataStoreWebWidgetPrivate::setFailurePage()
{
  Q_Q(qSlicerDataStoreWebWidget);
  QString html =
      "<style type='text/css'>"
      "  div.viewWrapperSlicer{"
      "    font-family:'Lucida Grande','Lucida Sans Unicode',helvetica,arial,Verdana,sans-serif;"
      "    font-size:13px;margin-left:8px;color:#777777;"
      "  }"
      "  div.extensionsHeader,div.extensionsBody{margin-right:10px;}"
      "  div.extensionsHeader{height:45px;border-bottom:1px solid #d0d0d0;}"
      "  div.extensionsTitle{float:left;font-size:24px;font-weight:bolder;margin-top:10px;}"
      "  div.extensionsBodyLeftColumn{float:left;width:230px;border-right:1px solid #d0d0d0;min-height:450px;}"
      "  div.extensionsBodyRightColumn{margin-left:230px;}"
      "</style>"
      "<div class='viewWrapperSlicer'>"
      "  <div class='extensionsHeader'>"
      "    <div class='extensionsTitle'>Slicer Data Store</div>"
      "  </div>"
      "  <div class='extensionsBody'>"
      "    <p>Failed to load data store page using the following URL:<br>%1</p>"
      "  </div>"
      "</div>";

  q->webView()->setHtml(html.arg(q->webView()->url().toString()));
}

// --------------------------------------------------------------------------
// qDataStoreWidgetPrivate

// --------------------------------------------------------------------------
qDataStoreWidgetPrivate::qDataStoreWidgetPrivate(qDataStoreWidget& object)
  :q_ptr(&object)
{
}

// --------------------------------------------------------------------------
void qDataStoreWidgetPrivate::init()
{
  Q_Q(qDataStoreWidget);

  this->setupUi(q);

  this->CurrentReply = 0;
  this->StreamedFile = 0;

  qSlicerCoreApplication * coreApp = qSlicerCoreApplication::application();
  this->DataSetDir = 	QFileInfo(coreApp->userSettings()->fileName()).absoluteDir().path() + QString("/DataStore/");

  this->DownloadPage = new qSlicerDataStoreWebWidget();
  this->DownloadPage->setHandleExternalUrlWithDesktopService(true);
  this->DownloadPage->setDataStoreWidget(q);
  this->verticalLayout_4->insertWidget(0, this->DownloadPage);

  this->UploadPage = new qSlicerDataStoreWebWidget();
  this->UploadPage->setHandleExternalUrlWithDesktopService(true);
  this->UploadPage->setDataStoreWidget(q);
  this->verticalLayout->insertWidget(0, this->UploadPage);

  this->tabWidget->setCurrentIndex(1);

  // Configure "local dataset" tab
  this->treeWidget->setColumnCount(qDataStoreWidget::ColumnCount);
  this->treeWidget->setColumnWidth(qDataStoreWidget::NameColumn, 500);
  this->treeWidget->setHeaderHidden(true);
  this->treeWidget->setRootIsDecorated(false);
  this->treeWidget->setIconSize(QSize(64, 64));
  this->treeWidget->setAllColumnsShowFocus(true);
  this->treeWidget->setAlternatingRowColors(true);
  this->treeWidget->setSelectionMode(QAbstractItemView::NoSelection);
  this->treeWidget->hide();
  this->noDatasetMessage->show();
}

// --------------------------------------------------------------------------
// qDataStoreWidget

// --------------------------------------------------------------------------
qDataStoreWidget::qDataStoreWidget(QWidget *parent)
  :
    QWidget(parent)
  , d_ptr(new qDataStoreWidgetPrivate(*this))
{
  Q_D(qDataStoreWidget);
  d->init();

  QObject::connect(&d->LoadButtonMapper, SIGNAL(mapped(QString)),
                   this, SLOT(loadDataset(QString)));

  QObject::connect(&d->DeleteButtonMapper, SIGNAL(mapped(QString)),
                   this, SLOT(deleteDataset(QString)));

  QDir dataPath(d->DataSetDir);
  if(dataPath.exists())
    {
    dataPath.setFilter(QDir::Files);
    QFileInfoList dataList = dataPath.entryInfoList();
    for(int i=0; i < dataList.size(); i++)
      {
      QFileInfo fileInfo = dataList.at(i);
      if(fileInfo.suffix() == "mrb")
        {
        this->addNewTreeItem(fileInfo);
        }
      }
    }
  else
    {
    dataPath.mkdir(d->DataSetDir);
    }

  QObject::connect(&d->networkDownloadManager, SIGNAL(finished(QNetworkReply*)),
                  this, SLOT(downloaded(QNetworkReply*)));
  
  QObject::connect(&d->networkIconManager, SIGNAL(finished(QNetworkReply*)),
                  this, SLOT(iconDownloaded(QNetworkReply*)));
  
  QObject::connect(&d->networkUploadManager, SIGNAL(finished(QNetworkReply*)),
                  this, SLOT(uploaded(QNetworkReply*)));
}

// --------------------------------------------------------------------------
void qDataStoreWidget::addNewTreeItem(QFileInfo fileName)
{
  Q_D(qDataStoreWidget);

  d->treeWidget->show();
  d->noDatasetMessage->hide();
  QTreeWidgetItem* item = new QTreeWidgetItem();
  d->treeWidget->addTopLevelItem(item);
  
  item->setText(qDataStoreWidget::NameColumn, fileName.fileName());
  item->setIcon(qDataStoreWidget::IconColumn,
                QIcon(d->DataSetDir + fileName.baseName() + ".jpeg"));

  DataStoreButtonBox* buttonBox = new DataStoreButtonBox();
  d->treeWidget->setItemWidget(item, qDataStoreWidget::ButtonsColumn, buttonBox);
  d->LoadButtonMapper.setMapping(buttonBox->LoadButton, fileName.fileName());
  QObject::connect(buttonBox->LoadButton, SIGNAL(clicked()), &d->LoadButtonMapper, SLOT(map()));
  d->DeleteButtonMapper.setMapping(buttonBox->DeleteButton, fileName.fileName());
  QObject::connect(buttonBox->DeleteButton, SIGNAL(clicked()), &d->DeleteButtonMapper, SLOT(map()));
}

// --------------------------------------------------------------------------
void qDataStoreWidget::deleteTreeItem(QString fileName)
{
  Q_D(qDataStoreWidget);

  QList<QTreeWidgetItem*> items = d->treeWidget->findItems(fileName, Qt::MatchExactly, qDataStoreWidget::NameColumn);
  QTreeWidgetItem* item = items.at(0);
  d->treeWidget->setCurrentItem(item);
  delete d->treeWidget->takeTopLevelItem(d->treeWidget->currentIndex().row());
}

//---------------------------------------------------------------------------
void qDataStoreWidget::loadDataset(QString fileName)
{
  Q_D(qDataStoreWidget);
  emit ScheduleLoad(d->DataSetDir + fileName);
  this->hide();
}

//---------------------------------------------------------------------------
void qDataStoreWidget::saveDataset(QString fileName)
{
  emit ScheduleSave(fileName);
}

//---------------------------------------------------------------------------
void qDataStoreWidget::deleteDataset(QString fileName)
{
  Q_D(qDataStoreWidget);
  QFileInfo fileInfo(d->DataSetDir + fileName);
  QFile::remove(d->DataSetDir + fileName); //DataSet
  QFile::remove(d->DataSetDir + fileInfo.baseName() + ".jpeg"); //Icon
  this->deleteTreeItem(fileName);
}

//---------------------------------------------------------------------------
void qDataStoreWidget::loadDataStoreURLs(QString url)
{
  Q_D(qDataStoreWidget);
  if(url.at(url.size()-1) != '/')
    {
    url += "/";
    }
  url += "slicerdatastore/";
  d->DownloadPage->webView()->setUrl(QUrl(url));
  d->DownloadPage->setInternalHosts(QStringList() << QUrl(url).host());
  d->UploadPage->webView()->setUrl(QUrl(url+"user/login"));
  d->UploadPage->setInternalHosts(QStringList() << QUrl(url).host());
}

//---------------------------------------------------------------------------
void qDataStoreWidget::download(const QString &url, const QString& thumbnail)
{
  Q_D(qDataStoreWidget);
  d->DownloadCanceled = false;
  QUrl qUrl = QUrl(url);
  QUrl iconUrl = QUrl(thumbnail);
  
  if(!QDir(d->DataSetDir).exists())
    {
    QDir().mkdir(d->DataSetDir);
    }
#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
  QString fileName = qUrl.queryItemValue("name") + ".mrb";
#else
  QString fileName = QUrlQuery(qUrl).queryItemValue("name") + ".mrb";
#endif
  QFile* file = new QFile(d->DataSetDir + fileName);
  if(file->exists())
    {    
    //The file was already downloaded so we just load it
    this->loadDataset(fileName);
    d->StreamStat="-1";
    this->hide();
    delete file;
    return;
    }
  
  d->CurrentReply = d->networkDownloadManager.post(QNetworkRequest(qUrl), QByteArray());
  if(d->CurrentReply->error() != QNetworkReply::NoError)
    {
    qWarning() << "Network error. Unable to download file:" << d->CurrentReply->error();
    }
  QObject::connect(d->CurrentReply, SIGNAL(downloadProgress(qint64,qint64)),
          this, SLOT(onStreamProgress(qint64,qint64)));
  d->networkIconManager.get(QNetworkRequest(iconUrl));
  d->StreamTime.start();
  d->StreamStat="0;;Speed: 0 B/s";
  
  if(d->StreamedFile)
    {
    delete d->StreamedFile;
    d->StreamedFile = 0;
    }
  d->StreamedFile = file;
}

//---------------------------------------------------------------------------
void qDataStoreWidget::upload(const QString& url)
{
  Q_D(qDataStoreWidget);
  QString completeUrl = url;
  QString name = QUuid::createUuid().toString();
  this->saveDataset(QDir::tempPath() + "/" + name + ".mrb");
  if(d->StreamedFile)
    {
    delete d->StreamedFile;
    d->StreamedFile = 0;
    }
  d->StreamedFile = new QFile(QDir::tempPath() + "/" + name + ".mrb");
  if(d->StreamedFile->open(QIODevice::ReadOnly))
    {
    completeUrl += QString::number(d->StreamedFile->size());
    QUrl qUrl = QUrl(completeUrl);
      
    QNetworkRequest request(qUrl);
    d->CurrentReply = d->networkUploadManager.put(request, d->StreamedFile);
    if(d->CurrentReply->error() != QNetworkReply::NoError)
      {
      qWarning() << "Network error. Unable to upload file:" << d->CurrentReply->error();
      }
    QObject::connect(d->CurrentReply, SIGNAL(uploadProgress(qint64,qint64)),
            this, SLOT(onStreamProgress(qint64,qint64)));
    
    d->StreamTime.start();
    d->StreamStat="0;;Speed: 0 B/s";
    }
  else
    {
    d->StreamStat="-1";
    }
}

//---------------------------------------------------------------------------
void qDataStoreWidget::onStreamProgress(qint64 bytes, qint64 bytesTotal)
{
  Q_D(qDataStoreWidget);
  double speed = bytes * 1000.0 / d->StreamTime.elapsed();
  QString unit;
  if (speed < 1024)
    {
    unit = " B/s";
    }
  else if (speed < 1024*1024) {
    speed /= 1024;
    unit = " kB/s";
    }
  else
    {
    speed /= 1024*1024;
    unit = " MB/s";
    }
  
  d->StreamStat = QString::number(100.0*(double)bytes/(double)bytesTotal, 'f', 1)
                       + ";;Speed: " + QString::number(speed, 'f', 2) + unit;
}

//---------------------------------------------------------------------------
QString qDataStoreWidget::getStreamStat()
{
  Q_D(qDataStoreWidget);
//   std::cout << "Dl progress " << d->StreamStat.toStdString() << std::endl;
  return d->StreamStat;
}

//---------------------------------------------------------------------------
QString qDataStoreWidget::getDownloadedItems()
{
  Q_D(qDataStoreWidget);

  QString items;
  for(int i = 0; i < d->treeWidget->topLevelItemCount(); i++)
    {
    QTreeWidgetItem* item = d->treeWidget->topLevelItem(i);
    QString Id = item->text(qDataStoreWidget::NameColumn).section('_', 0, 0);
    items += Id + ";;";
    }
  return items;
}

//---------------------------------------------------------------------------
void qDataStoreWidget::downloaded(QNetworkReply* reply)
{
  Q_D(qDataStoreWidget);
  if(reply->error() != QNetworkReply::NoError)
    { 
    qWarning() << "Network error. Unable to download file:" << reply->error();
    }
  if(!d->DownloadCanceled)
    {
    QByteArray data = reply->readAll();
    
    if(!d->StreamedFile->open(QIODevice::WriteOnly))
      {
      delete d->StreamedFile;
      d->StreamedFile = 0;
      return;
      }
    d->StreamedFile->write(data);
    d->StreamedFile->close();
    
    QFileInfo fileInfo(d->StreamedFile->fileName());
    this->addNewTreeItem(fileInfo);
    this->loadDataset(fileInfo.fileName());
    
    delete d->StreamedFile;
    d->StreamedFile = 0;
    
    this->hide();
    }
  
  d->StreamStat = "-1"; //For the webpage to know dl is finished
  reply->deleteLater();
  d->CurrentReply = 0;
}

//---------------------------------------------------------------------------
void qDataStoreWidget::uploaded(QNetworkReply* reply)
{
  Q_D(qDataStoreWidget);
  d->StreamedFile->remove();
  delete d->StreamedFile;
  d->StreamedFile = 0;
  d->StreamStat = "-1"; //For the webpage to know dl is finished
  reply->deleteLater();
}

//---------------------------------------------------------------------------
void qDataStoreWidget::iconDownloaded(QNetworkReply* reply)
{
  Q_D(qDataStoreWidget);
  QByteArray data = reply->readAll();
  QFileInfo fileInfo(d->StreamedFile->fileName());
  QFile file(d->DataSetDir + fileInfo.baseName() + ".jpeg");
  file.open(QIODevice::WriteOnly);
  file.write(data);
  file.close();
}

//---------------------------------------------------------------------------
//Abort network reply and call downloaded slot
void qDataStoreWidget::cancelDownload()
{
  Q_D(qDataStoreWidget);
//   std::cout << "Cancel DL" << std::endl;
  if(d->CurrentReply)
    {
    d->DownloadCanceled = true;
    d->CurrentReply->abort(); //Send finished signal
    }
}

// --------------------------------------------------------------------------
void qDataStoreWidget::displayWindow()
{
  this->show();
  this->raise();
}

// --------------------------------------------------------------------------
qDataStoreWidget::~qDataStoreWidget()
{
}
