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


//Loadable module includes
#include "qDataStoreWidget.h"

//GUI includes
#include "ui_qDataStoreWidget.h"
#include "ui_DataStoreButtonBox.h"

//Qt Includes
#include <QDebug>
#include <QDesktopServices>
#include <QDir>
#include <QFileInfo>
#include <QString>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QSettings>
#include <QTreeWidget>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QUrlQuery>
#endif
#include <QUuid>
#if (QT_VERSION < QT_VERSION_CHECK(5, 6, 0))
#include <QWebFrame>
#include <QWebView>
#else
#include <QWebEngineSettings>
#include <QWebEngineView>
#endif

//Midas includes
#include <qMidasAPI.h>

//Slicer includes
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

qDataStoreWidget::qDataStoreWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::qDataStoreWidget)
{
  this->CurrentReply = 0;
  this->StreamedFile = 0;
  qSlicerCoreApplication * coreApp = qSlicerCoreApplication::application();
  this->DataSetDir = 	QFileInfo(coreApp->userSettings()->fileName()).absoluteDir().path() + QString("/DataStore/");
  
  QObject::connect(&this->LoadButtonMapper, SIGNAL(mapped(QString)),
                   this, SLOT(loadDataset(QString)));
  
  QObject::connect(&this->DeleteButtonMapper, SIGNAL(mapped(QString)),
                   this, SLOT(deleteDataset(QString)));
  
  ui->setupUi(this);

  this->DownloadPage = new qSlicerWebWidget();
  this->UploadPage = new qSlicerWebWidget();

  this->DownloadPage->webView()->setUrl(
        QUrl("http://10.33.0.107/Midas/Midas3/slicerdatastore"));
  ui->verticalLayout_4->insertWidget(0, this->DownloadPage);

  this->UploadPage->webView()->setUrl(
        QUrl("http://10.33.0.107/Midas/Midas3/slicerdatastore/user/login"));
  ui->verticalLayout->insertWidget(0, this->UploadPage);

  
  ui->tabWidget->setCurrentIndex(1);
#if (QT_VERSION < QT_VERSION_CHECK(5, 6, 0))
  this->downloadFrame = this->DownloadPage->webView()->page()->mainFrame();
  this->uploadFrame = this->UploadPage->webView()->page()->mainFrame();
#endif

  //Configure "local dataset" tab
  ui->treeWidget->setColumnCount(qDataStoreWidget::ColumnCount);
  ui->treeWidget->setColumnWidth(qDataStoreWidget::NameColumn, 500);
  ui->treeWidget->setHeaderHidden(true);
  ui->treeWidget->setRootIsDecorated(false);
  ui->treeWidget->setIconSize(QSize(64, 64));
  ui->treeWidget->setAllColumnsShowFocus(true);
  ui->treeWidget->setAlternatingRowColors(true);
  ui->treeWidget->setSelectionMode(QAbstractItemView::NoSelection);
  ui->treeWidget->hide();
  ui->noDatasetMessage->show();
  QDir dataPath(this->DataSetDir);
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
    dataPath.mkdir(this->DataSetDir);
    }
  
#if (QT_VERSION < QT_VERSION_CHECK(5, 6, 0))
  QWebSettings::globalSettings();
  this->DownloadPage->webView()->settings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);
  this->UploadPage->webView()->settings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);
  
  QObject::connect(this->DownloadPage->webView(), SIGNAL(loadStarted()),
                  this, SLOT(onLoadStarted()));
  
  QObject::connect(this->UploadPage->webView(), SIGNAL(loadStarted()),
                  this, SLOT(onLoadStarted()));

  QObject::connect(this->DownloadPage->webView(), SIGNAL(loadFinished(bool)),
                  this, SLOT(onLoadFinished(bool)));
  
  QObject::connect(this->UploadPage->webView(), SIGNAL(loadFinished(bool)),
                  this, SLOT(onLoadFinished(bool)));

  QObject::connect(this->DownloadPage->webView(), SIGNAL(loadProgress(int)),
                  ui->DownloadProgressBar, SLOT(setValue(int)));
  
  QObject::connect(this->UploadPage->webView(), SIGNAL(loadProgress(int)),
                  ui->UploadProgressBar, SLOT(setValue(int)));
  
  this->DownloadPage->webView()->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
  this->UploadPage->webView()->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
  
  QObject::connect(this->downloadFrame, SIGNAL(javaScriptWindowObjectCleared()),
                  this, SLOT(initJavascript()));
  
  QObject::connect(this->uploadFrame, SIGNAL(javaScriptWindowObjectCleared()),
                  this, SLOT(initJavascript()));
  
  QObject::connect(this->DownloadPage->webView()->page(), SIGNAL(linkClicked(QUrl)),
                  this, SLOT(onLinkClicked(QUrl)));
  
  QObject::connect(this->UploadPage->webView()->page(), SIGNAL(linkClicked(QUrl)),
                  this, SLOT(onLinkClicked(QUrl)));
#else
  QWebEngineSettings::globalSettings();

  qDebug() << "qDataStoreWidget::qDataStoreWidget - "
              "configuration of widget connections not implemented with Qt5";
#endif
  QObject::connect(&this->networkDownloadManager, SIGNAL(finished(QNetworkReply*)),
                  this, SLOT(downloaded(QNetworkReply*)));
  
  QObject::connect(&this->networkIconManager, SIGNAL(finished(QNetworkReply*)),
                  this, SLOT(iconDownloaded(QNetworkReply*)));
  
  QObject::connect(&this->networkUploadManager, SIGNAL(finished(QNetworkReply*)),
                  this, SLOT(uploaded(QNetworkReply*)));
    
}

// --------------------------------------------------------------------------
void qDataStoreWidget::addNewTreeItem(QFileInfo fileName)
{
  ui->treeWidget->show();
  ui->noDatasetMessage->hide();
  QTreeWidgetItem* item = new QTreeWidgetItem();
  ui->treeWidget->addTopLevelItem(item);
  
  item->setText(qDataStoreWidget::NameColumn, fileName.fileName());
  item->setIcon(qDataStoreWidget::IconColumn,
                QIcon(this->DataSetDir + fileName.baseName() + ".jpeg"));

  DataStoreButtonBox* buttonBox = new DataStoreButtonBox();
  ui->treeWidget->setItemWidget(item, qDataStoreWidget::ButtonsColumn, buttonBox);
  this->LoadButtonMapper.setMapping(buttonBox->LoadButton, fileName.fileName());
  QObject::connect(buttonBox->LoadButton, SIGNAL(clicked()), &this->LoadButtonMapper, SLOT(map()));
  this->DeleteButtonMapper.setMapping(buttonBox->DeleteButton, fileName.fileName());
  QObject::connect(buttonBox->DeleteButton, SIGNAL(clicked()), &this->DeleteButtonMapper, SLOT(map()));
}

// --------------------------------------------------------------------------
void qDataStoreWidget::deleteTreeItem(QString fileName)
{
  QList<QTreeWidgetItem*> items = ui->treeWidget->findItems(fileName, Qt::MatchExactly, qDataStoreWidget::NameColumn);
  QTreeWidgetItem* item = items.at(0);
  ui->treeWidget->setCurrentItem(item);
  delete ui->treeWidget->takeTopLevelItem(ui->treeWidget->currentIndex().row());
}

// --------------------------------------------------------------------------
void qDataStoreWidget::onLoadStarted()
{
#if (QT_VERSION < QT_VERSION_CHECK(5, 6, 0))
  QWebView* webView = dynamic_cast<QWebView*>(sender());
#else
  QWebEngineView* webView = dynamic_cast<QWebEngineView*>(sender());
#endif
  if(webView == this->DownloadPage->webView())
    {
    ui->DownloadProgressBar->setFormat("%p%");
    ui->DownloadProgressBar->setVisible(true);
    }
  else if(webView == this->UploadPage->webView())
    {
    ui->UploadProgressBar->setFormat("%p%");
    ui->UploadProgressBar->setVisible(true);
    }
}

// --------------------------------------------------------------------------
void qDataStoreWidget::onLoadFinished(bool ok)
{
#if (QT_VERSION < QT_VERSION_CHECK(5, 6, 0))
  QWebView* webView = dynamic_cast<QWebView*>(sender());
#else
  QWebEngineView* webView = dynamic_cast<QWebEngineView*>(sender());
#endif
  if(webView == this->DownloadPage->webView())
    {
    ui->DownloadProgressBar->reset();
    ui->DownloadProgressBar->setVisible(false);
    }
  else if(webView == this->UploadPage->webView())
    {
    ui->UploadProgressBar->reset();
    ui->UploadProgressBar->setVisible(false);
    }
  if(!ok)
    {
    this->setFailurePage(webView);
    }
}

// --------------------------------------------------------------------------
#if (QT_VERSION < QT_VERSION_CHECK(5, 6, 0))
void qDataStoreWidget::setFailurePage(QWebView* webView)
#else
void qDataStoreWidget::setFailurePage(QWebEngineView* webView)
#endif
{
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

  webView->setHtml(html.arg(webView->url().toString()));
}

// --------------------------------------------------------------------------
void qDataStoreWidget::onLinkClicked(const QUrl& url)
{
#if (QT_VERSION < QT_VERSION_CHECK(5, 6, 0))
  QWebPage* webPage = dynamic_cast<QWebPage*>(sender());
  QWebView* webView;
  if(webPage == this->DownloadPage->webView()->page())
    {
    webView = this->DownloadPage->webView();
    }
  else
    {
    webView = this->UploadPage->webView();
    }
  QUrl serverUrl = webView->url();
  if(url.host() == serverUrl.host())
    {
    webView->load(url);
    }
  else
    {
    if(!QDesktopServices::openUrl(url))
      {
      qWarning() << "Failed to open url:" << url;
      }
    }
#else
  qDebug() << "qDataStoreWidget::onLinkClicked" << url
           << " - not implemented with Qt5";
#endif
}

#if (QT_VERSION < QT_VERSION_CHECK(5, 6, 0))
// --------------------------------------------------------------------------
void qDataStoreWidget::setDocumentWebkitHidden(QWebFrame* webFrame, bool value)
{
  this->evalJS(webFrame, QString("document.webkitHidden = %1").arg(value ? "true" : "false"));
}

//---------------------------------------------------------------------------
QString qDataStoreWidget::evalJS(QWebFrame* webFrame, const QString &js)
{
  return webFrame->evaluateJavaScript(js).toString();
}
#endif

//---------------------------------------------------------------------------
void qDataStoreWidget::loadDataset(QString fileName)
{
  emit ScheduleLoad(this->DataSetDir + fileName);
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
  QFileInfo fileInfo(this->DataSetDir + fileName);
  QFile::remove(this->DataSetDir + fileName); //DataSet
  QFile::remove(this->DataSetDir + fileInfo.baseName() + ".jpeg"); //Icon
  this->deleteTreeItem(fileName);
}

//---------------------------------------------------------------------------
void qDataStoreWidget::loadDataStoreURLs(QString url)
{
  if(url.at(url.size()-1) != '/')
    {
    url += "/";
    }
  url += "slicerdatastore/";
  this->DownloadPage->webView()->setUrl(QUrl(url));
  this->UploadPage->webView()->setUrl(QUrl(url+"user/login"));
}

//---------------------------------------------------------------------------
void qDataStoreWidget::download(const QString &url, const QString& thumbnail)
{
  this->DownloadCanceled = false;
  QUrl qUrl = QUrl(url);
  QUrl iconUrl = QUrl(thumbnail);
  
  if(!QDir(this->DataSetDir).exists())
    {
    QDir().mkdir(this->DataSetDir);
    }
#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
  QString fileName = qUrl.queryItemValue("name") + ".mrb";
#else
  QString fileName = QUrlQuery(qUrl).queryItemValue("name") + ".mrb";
#endif
  QFile* file = new QFile(this->DataSetDir + fileName);
  if(file->exists())
    {    
    //The file was already downloaded so we just load it
    this->loadDataset(fileName);
    this->StreamStat="-1";
    this->hide();
    delete file;
    return;
    }
  
  this->CurrentReply = this->networkDownloadManager.post(QNetworkRequest(qUrl), QByteArray());
  if(this->CurrentReply->error() != QNetworkReply::NoError)
    {
    qWarning() << "Network error. Unable to download file:" << this->CurrentReply->error();
    }
  QObject::connect(this->CurrentReply, SIGNAL(downloadProgress(qint64,qint64)),
          this, SLOT(onStreamProgress(qint64,qint64)));
  this->networkIconManager.get(QNetworkRequest(iconUrl));
  this->StreamTime.start();
  this->StreamStat="0;;Speed: 0 B/s";
  
  if(this->StreamedFile)
    {
    delete this->StreamedFile;
    this->StreamedFile = 0;
    }
  this->StreamedFile = file;
}

//---------------------------------------------------------------------------
void qDataStoreWidget::upload(const QString& url)
{
  QString completeUrl = url;
  QString name = QUuid::createUuid().toString();
  this->saveDataset(QDir::tempPath() + "/" + name + ".mrb");
  if(this->StreamedFile)
    {
    delete this->StreamedFile;
    this->StreamedFile = 0;
    }
  this->StreamedFile = new QFile(QDir::tempPath() + "/" + name + ".mrb");
  if(this->StreamedFile->open(QIODevice::ReadOnly))
    {
    completeUrl += QString::number(this->StreamedFile->size());
    QUrl qUrl = QUrl(completeUrl);
      
    QNetworkRequest request(qUrl);
    this->CurrentReply = this->networkUploadManager.put(request, this->StreamedFile);    
    if(this->CurrentReply->error() != QNetworkReply::NoError)
      {
      qWarning() << "Network error. Unable to upload file:" << this->CurrentReply->error();
      }
    QObject::connect(this->CurrentReply, SIGNAL(uploadProgress(qint64,qint64)),
            this, SLOT(onStreamProgress(qint64,qint64)));
    
    this->StreamTime.start();
    this->StreamStat="0;;Speed: 0 B/s";
    }
  else
    {
    this->StreamStat="-1";
    }
}

//---------------------------------------------------------------------------
void qDataStoreWidget::onStreamProgress(qint64 bytes, qint64 bytesTotal)
{
  double speed = bytes * 1000.0 / this->StreamTime.elapsed();
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
  
  this->StreamStat = QString::number(100.0*(double)bytes/(double)bytesTotal, 'f', 1)
                       + ";;Speed: " + QString::number(speed, 'f', 2) + unit;
}

//---------------------------------------------------------------------------
QString qDataStoreWidget::getStreamStat()
{
//   std::cout << "Dl progress " << this->StreamStat.toStdString() << std::endl;
  return this->StreamStat;
}

//---------------------------------------------------------------------------
QString qDataStoreWidget::getDownloadedItems()
{
  QString items;
  for(int i = 0; i < ui->treeWidget->topLevelItemCount(); i++)
    {
    QTreeWidgetItem* item = ui->treeWidget->topLevelItem(i);
    QString Id = item->text(qDataStoreWidget::NameColumn).section('_', 0, 0);
    items += Id + ";;";
    }
  return items;
}

//---------------------------------------------------------------------------
void qDataStoreWidget::downloaded(QNetworkReply* reply)
{
  if(reply->error() != QNetworkReply::NoError)
    { 
    qWarning() << "Network error. Unable to download file:" << reply->error();
    }
  if(!this->DownloadCanceled)
    {
    QByteArray data = reply->readAll();
    
    if(!this->StreamedFile->open(QIODevice::WriteOnly))
      {
      delete this->StreamedFile;
      this->StreamedFile = 0;
      return;
      }
    this->StreamedFile->write(data);
    this->StreamedFile->close();
    
    QFileInfo fileInfo(this->StreamedFile->fileName());
    this->addNewTreeItem(fileInfo);
    this->loadDataset(fileInfo.fileName());
    
    delete this->StreamedFile;
    this->StreamedFile = 0;
    
    this->hide();
    }
  
  this->StreamStat = "-1"; //For the webpage to know dl is finished
  reply->deleteLater();
  this->CurrentReply = 0;
}

//---------------------------------------------------------------------------
void qDataStoreWidget::uploaded(QNetworkReply* reply)
{  
  this->StreamedFile->remove();
  delete this->StreamedFile;
  this->StreamedFile = 0;
  this->StreamStat = "-1"; //For the webpage to know dl is finished
  reply->deleteLater();
}

//---------------------------------------------------------------------------
void qDataStoreWidget::iconDownloaded(QNetworkReply* reply)
{
  QByteArray data = reply->readAll();
  QFileInfo fileInfo(this->StreamedFile->fileName());
  QFile file(this->DataSetDir + fileInfo.baseName() + ".jpeg");
  file.open(QIODevice::WriteOnly);
  file.write(data);
  file.close();
}

//---------------------------------------------------------------------------
//Abort network reply and call downloaded slot
void qDataStoreWidget::cancelDownload()
{
//   std::cout << "Cancel DL" << std::endl;
  if(this->CurrentReply)
    {
    this->DownloadCanceled = true;
    this->CurrentReply->abort(); //Send finished signal
    }
}

// --------------------------------------------------------------------------
void qDataStoreWidget::displayWindow()
{
  this->show();
  this->raise();
}
// --------------------------------------------------------------------------
void qDataStoreWidget::initJavascript()
{
#if (QT_VERSION < QT_VERSION_CHECK(5, 6, 0))
  QWebFrame* webFrame = dynamic_cast<QWebFrame*>(sender());
  bool isVisible = false;
  if(webFrame == this->downloadFrame)
    {
    isVisible = this->DownloadPage->isVisible();
    }
  else if(webFrame == this->uploadFrame)
    {
    isVisible = this->UploadPage->isVisible();
    }
  this->setDocumentWebkitHidden(webFrame, !isVisible);
  webFrame->addToJavaScriptWindowObject("DataStoreGUI", this);
#else
  qDebug() << "qDataStoreWidget::initJavascript - not implemented with Qt5";
#endif
}

// --------------------------------------------------------------------------
qDataStoreWidget::~qDataStoreWidget()
{
  delete ui;
}
