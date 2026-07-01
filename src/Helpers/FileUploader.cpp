#include "FileUploader.h"
#include <QHttpMultiPart>
#include <QNetworkAccessManager>
#include "../Core/CloudModel.h"
#include "../Core/ConnectionManager.h"
#include <QFile>
#include <QUrlQuery>
#include <QMimeDatabase>

FileUploader::FileUploader(QObject *parent) : QObject(parent)
{
    _nam = new QNetworkAccessManager(this);
    _host = QUrl(ConnectionManager::instance()->getServer()).host();
    connect(ConnectionManager::instance(), &ConnectionManager::onServerUrlChanged, this, &FileUploader::hostNameChanged);
}

void FileUploader::uploadImage(QString filename, QString serverFilename)
{
    uploadFile(filename, "images", imageID(), serverFilename);
}

void FileUploader::sendFileToDevice(QString filename, QString deviceID)
{
    uploadFile(filename, "files", "device/"+deviceID);
}

QString FileUploader::token() const
{
    return _token;
}

void FileUploader::setToken(const QString &token)
{
    _token = token;
    Q_EMIT tokenChanged();
}

QString FileUploader::imageID() const
{
    return _imageID;
}

void FileUploader::setImageID(const QString &imageID)
{
    _imageID = imageID;
    Q_EMIT imageIDChanged();
}

void FileUploader::uploadFile(QString filename, QString endpoint, QString address, QString serverFilename)
{
    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    QString localPath = filename;
    localPath.remove("file://");
    QString displayName = serverFilename.isEmpty()
                              ? QUrl::fromLocalFile(localPath).fileName()
                              : serverFilename;

    QMimeDatabase mimeDb;
    QMimeType mimeType = mimeDb.mimeTypeForFile(localPath);

    QHttpPart filePart;
    filePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant(mimeType.name()));
    filePart.setHeader(QNetworkRequest::ContentDispositionHeader,
                        QVariant("form-data; name=\"file\"; filename=\"" + displayName + "\""));
    QFile *file = new QFile(localPath);

    if(!file->open(QIODevice::ReadOnly))
    {
      qDebug()<<localPath;
      qDebug()<<"could not open file";
      qDebug()<<file->errorString();
    }

    filePart.setBodyDevice(file);
    file->setParent(multiPart);

    multiPart->append(filePart);

    QUrlQuery query;
    query.addQueryItem("token",_token);

    QUrl url;
    url.setHost(_host);
    url.setScheme("http");
    url.setQuery(query);
    url.setPort(8080);
    url.setPath("/" + endpoint + "/" + address);

    QNetworkRequest request(url);
    qDebug()<<url;

    QNetworkReply *reply = _nam->post(request, multiPart);
    qDebug()<<reply->errorString();
    connect(reply, &QNetworkReply::finished, this, &FileUploader::requestFinished);
    connect(reply, &QNetworkReply::errorOccurred, this, &FileUploader::requestError);
    multiPart->setParent(reply);
}

void FileUploader::requestFinished()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if(!reply)
        return;

    bool success = reply->error() == QNetworkReply::NoError;
    Q_EMIT uploadFinished(success, reply->errorString());

    reply->deleteLater();
}

void FileUploader::requestError(QNetworkReply::NetworkError code)
{
    Q_UNUSED(code)
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if(!reply)
        return;

    Q_EMIT uploadFinished(false, reply->errorString());
    reply->deleteLater();
}

void FileUploader::hostNameChanged()
{
    _host = QUrl(ConnectionManager::instance()->getServer()).host();
}
