
#ifndef __qSlicerDataStoreWebWidgetPrivate_p_h
#define __qSlicerDataStoreWebWidgetPrivate_p_h

// DataStore includes
class qSlicerDataStoreWebWidget;
#include "qDataStoreWidget.h"
#include "ui_qDataStoreWidget.h"

// Slicer includes
#include <qSlicerWebWidget_p.h>

// Qt includes
#include <QFile>
#include <QObject>
#include <QSignalMapper>
#if (QT_VERSION < QT_VERSION_CHECK(5, 6, 0))
class QWebChannel;
#else
#include <QWebChannel>
#endif

// --------------------------------------------------------------------------
class qSlicerDataStoreWidgetWebChannelProxy : public QObject
{
  Q_OBJECT
public:
  qSlicerDataStoreWidgetWebChannelProxy() : DataStoreWidget(0){}
  qDataStoreWidget* DataStoreWidget;
public slots:
  void download(const QString& url, const QString& thumbnail);
  void cancelDownload();
  QString getStreamStat();
private:
  Q_DISABLE_COPY(qSlicerDataStoreWidgetWebChannelProxy);
};

// --------------------------------------------------------------------------
class qSlicerDataStoreWebWidgetPrivate  : public qSlicerWebWidgetPrivate
{
  Q_DECLARE_PUBLIC(qSlicerDataStoreWebWidget);
protected:
  qSlicerDataStoreWebWidget* const q_ptr;

public:
  typedef qSlicerWebWidgetPrivate Superclass;
  qSlicerDataStoreWebWidgetPrivate(qSlicerDataStoreWebWidget& object) :
    Superclass(object), q_ptr(&object), DataStoreWidgetWebChannelProxy(new qSlicerDataStoreWidgetWebChannelProxy) {}
  virtual ~qSlicerDataStoreWebWidgetPrivate()
  {
    delete this->DataStoreWidgetWebChannelProxy;
  }

  virtual void updateWebChannelScript(QByteArray& webChannelScript);
  virtual void initializeWebChannel(QWebChannel* webChannel);

  void setFailurePage();

  qSlicerDataStoreWidgetWebChannelProxy* DataStoreWidgetWebChannelProxy;
};

// --------------------------------------------------------------------------
class qDataStoreWidgetPrivate : public Ui_qDataStoreWidget
{
  Q_DECLARE_PUBLIC(qDataStoreWidget)
protected:
  qDataStoreWidget* const q_ptr;

public:
  qDataStoreWidgetPrivate(qDataStoreWidget& object);

  void init();

  qSlicerDataStoreWebWidget* DownloadPage;
  qSlicerDataStoreWebWidget* UploadPage;

  QNetworkAccessManager networkDownloadManager;
  QNetworkAccessManager networkIconManager;
  QNetworkAccessManager networkUploadManager;
  QNetworkReply* CurrentReply;
  QTime StreamTime;
  QString StreamStat;
  QFile* StreamedFile;
  QString StreamId;
  bool DownloadCanceled;

  QSignalMapper LoadButtonMapper;
  QSignalMapper DeleteButtonMapper;

  QString DataSetDir;
};

#endif
