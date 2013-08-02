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
#include <QDesktopServices>
#include <QString>
#include <QDebug>
#include <QNetworkRequest>
#include <QWebFrame>
#include <QNetworkReply>
#include <QFileInfo>
#include <QDir>
#include <QSettings>
#include <QTreeWidget>
#include <QHttpMultiPart>
#include <QDebug>
#include <QUuid>

//Midas includes
#include <qMidasAPI.h>

//Slicer includes
#include <qSlicerCoreApplication.h>

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
  
  ui->tabWidget->setCurrentIndex(1);
  this->downloadFrame = ui->DownloadPage->page()->mainFrame();
  this->uploadFrame = ui->UploadPage->page()->mainFrame();
  
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
  
  QWebSettings::globalSettings();
  ui->DownloadPage->settings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);
  ui->UploadPage->settings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);
  
  QObject::connect(ui->DownloadPage, SIGNAL(loadStarted()),
                  this, SLOT(onLoadStarted()));
  
  QObject::connect(ui->UploadPage, SIGNAL(loadStarted()),
                  this, SLOT(onLoadStarted()));

  QObject::connect(ui->DownloadPage, SIGNAL(loadFinished(bool)),
                  this, SLOT(onLoadFinished(bool)));
  
  QObject::connect(ui->UploadPage, SIGNAL(loadFinished(bool)),
                  this, SLOT(onLoadFinished(bool)));

  QObject::connect(ui->DownloadPage, SIGNAL(loadProgress(int)),
                  ui->DownloadProgressBar, SLOT(setValue(int)));
  
  QObject::connect(ui->UploadPage, SIGNAL(loadProgress(int)),
                  ui->UploadProgressBar, SLOT(setValue(int)));
  
  ui->DownloadPage->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
  ui->UploadPage->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
  
  QObject::connect(this->downloadFrame, SIGNAL(javaScriptWindowObjectCleared()),
                  this, SLOT(initJavascript()));
  
  QObject::connect(this->uploadFrame, SIGNAL(javaScriptWindowObjectCleared()),
                  this, SLOT(initJavascript()));
  
  QObject::connect(ui->DownloadPage->page(), SIGNAL(linkClicked(QUrl)),
                  this, SLOT(onLinkClicked(QUrl)));
  
  QObject::connect(ui->UploadPage->page(), SIGNAL(linkClicked(QUrl)),
                  this, SLOT(onLinkClicked(QUrl)));
   
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
  QWebView* webView = dynamic_cast<QWebView*>(sender());
  if(webView == ui->DownloadPage)
    {
    ui->DownloadProgressBar->setFormat("%p%");
    ui->DownloadProgressBar->setVisible(true);
    }
  else if(webView == ui->UploadPage)
    {
    ui->UploadProgressBar->setFormat("%p%");
    ui->UploadProgressBar->setVisible(true);
    }
}

// --------------------------------------------------------------------------
void qDataStoreWidget::onLoadFinished(bool ok)
{
  //   std::cout << "Load Finished" << std::endl;
  QWebView* webView = dynamic_cast<QWebView*>(sender());
  if(webView == ui->DownloadPage)
    {
    ui->DownloadProgressBar->reset();
    ui->DownloadProgressBar->setVisible(false);
    }
  else if(webView == ui->UploadPage)
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
void qDataStoreWidget::setFailurePage(QWebView* webView)
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
  QWebPage* webPage = dynamic_cast<QWebPage*>(sender());
  QWebView* webView;
  if(webPage == ui->DownloadPage->page())
    {
    webView = ui->DownloadPage;
    }
  else
    {
    webView = ui->UploadPage;
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
}

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
  ui->DownloadPage->setUrl(QUrl(url));
  ui->UploadPage->setUrl(QUrl(url+"user/login"));
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
  QString fileName = qUrl.queryItemValue("name") + ".mrb";
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
  
  this->CurrentReply = this->networkDownloadManager.get(QNetworkRequest(qUrl));
  QObject::connect(this->CurrentReply, SIGNAL(downloadProgress(qint64,qint64)),
          this, SLOT(onStreamProgress(qint64,qint64)));
  this->networkIconManager.get(QNetworkRequest(iconUrl));
  this->StreamTime.start();
  this->StreamStat="0;;0bytes/sec";
  
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
  
    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
    QHttpPart filePart;
    filePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/octet-stream"));//stream?
    filePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"file\"; filename=\""+ qUrl.queryItemValue("filename") + "\""));
    filePart.setBodyDevice(this->StreamedFile);
    this->StreamedFile->setParent(multiPart); // we cannot delete the file now, so delete it with the multiPart
    multiPart->append(filePart);
    
    QNetworkRequest request(qUrl);
    this->CurrentReply = this->networkUploadManager.post(request, multiPart);
    multiPart->setParent(this->CurrentReply); // delete the multiPart with the reply
    
    QObject::connect(this->CurrentReply, SIGNAL(uploadProgress(qint64,qint64)),
            this, SLOT(onStreamProgress(qint64,qint64)));
    
    this->StreamTime.start();
    this->StreamStat="0;;Download Speed: 0 B/s";
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
                       + ";;Download Speed: " + QString::number(speed, 'f', 2) + unit;
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
//   std::cout << "DL End " << std::endl;
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
  QWebFrame* webFrame = dynamic_cast<QWebFrame*>(sender());
  bool isVisible = false;
  if(webFrame == this->downloadFrame)
    {
    isVisible = ui->DownloadPage->isVisible();
    }
  else if(webFrame == this->uploadFrame)
    {
    isVisible = ui->UploadPage->isVisible();
    }
  this->setDocumentWebkitHidden(webFrame, !isVisible);
  webFrame->addToJavaScriptWindowObject("DataStoreGUI", this);
}

// --------------------------------------------------------------------------
qDataStoreWidget::~qDataStoreWidget()
{
  delete ui;
}
