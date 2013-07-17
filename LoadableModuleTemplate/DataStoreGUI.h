#ifndef DATASTOREGUI_H
#define DATASTOREGUI_H

#include <QWidget>
#include <QUrl>
#include <QTime>
#include <iostream>
#include <QNetworkRequest>
#include <QWebFrame>
#include <QFile>
#include <QSignalMapper>
#include <QFileInfo>
#include <QNetworkReply>

class QWebView;

namespace Ui {
class DataStoreGUI;
}

class DataStoreGUI : public QWidget
{
    Q_OBJECT
    
public:  
  explicit DataStoreGUI(QWidget *parent = 0);
  ~DataStoreGUI();
  void setFailurePage(QWebView* webView);
  
  enum ColumnsIds
  {
  IconColumn = 0,
  NameColumn,
  ButtonsColumn,
  ColumnCount
  };
  
  void addNewTreeItem(QFileInfo fileName);
  void deleteTreeItem(QString fileName);
  void cancelDownload();
    
public slots:
  void downloaded(QNetworkReply* reply);
  void uploaded(QNetworkReply* reply);
  void iconDownloaded(QNetworkReply* reply);
  void onStreamProgress(qint64 bytes, qint64 bytesTotal);
  void loadDataset(QString fileName);
  void deleteDataset(QString fileName);
  void loadURL(QString);
  void onNetworkError(QNetworkReply::NetworkError);
  
  void download(const QString& url, const QString& thumbnail);
  void upload(const QString& url);
  QString getStreamStat();
  
signals:
  void ScheduleLoad(QString);
  void ScheduleSave(QString);
    
protected slots:
  void onLoadStarted();
  void onLoadFinished(bool ok);
  void onLinkClicked(const QUrl& url);
  void initJavascript();
  void displayWindow();
    
private:
    Ui::DataStoreGUI *ui;
    QWebFrame* downloadFrame;
    QWebFrame* uploadFrame;
    QNetworkAccessManager networkDownloadManager;
    QNetworkAccessManager networkIconManager;
    QNetworkAccessManager networkUploadManager;
    QNetworkReply* CurrentReply;
    QTime StreamTime;
    QString StreamStat;
    QFile* StreamedFile;
    bool DownloadCanceled;
    QSignalMapper LoadButtonMapper;
    QSignalMapper DeleteButtonMapper;
    
    QString DataSetDir;
    
    void saveDataset(QString fileName);
    void setDocumentWebkitHidden(QWebFrame* webFrame, bool value);
    QString evalJS(QWebFrame* webFrame, const QString &js);
};

#endif // DATASTOREGUI_H
