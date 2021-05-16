#include "FileUploader.h"
#include <QHttpMultiPart>
#include <QNetworkAccessManager>
#include "../Core/CloudModel.h"

#include <QFile>
#include <QUrlQuery>

FileUploader::FileUploader(QObject *parent) : QObject(parent)
{
    _nam = new QNetworkAccessManager(this);
    _host = QUrl(CloudModel::instance()->getServer()).host();
    connect(CloudModel::instance(), &CloudModel::onServerUrlChanged, this, &FileUploader::hostNameChanged);
}

void FileUploader::uploadImage(QString filename)
{
    uploadFile(filename, "images", imageID());
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

void FileUploader::uploadFile(QString filename, QString endpoint, QString address)
{
    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);


    QHttpPart textPart;
    textPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"upload\""));
    textPart.setBody("finish");


    QHttpPart imagePart;
    imagePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("file"));
    imagePart.setHeader(QNetworkRequest::ContentDispositionHeader,
                        QVariant("form-data; name=\"file1\"; filename=\""+QUrl::fromLocalFile(filename).fileName()+"\""));
    QFile *file = new QFile(filename.remove("file://"));

    if(!file->open(QIODevice::ReadOnly))
    {
      qDebug()<<filename;
      qDebug()<<"could not open file";
      qDebug()<<file->errorString();
    }

    imagePart.setBodyDevice(file);
    file->setParent(multiPart); // we cannot delete the file now, so delete it with the multiPart

    multiPart->append(textPart);
    multiPart->append(imagePart);

    QUrlQuery query;
    query.addQueryItem("token",_token);

    QUrl url;
    url.setHost(_host);
    url.setScheme("https");
    url.setQuery(query);
    url.setPath("/qhapi/"+endpoint+"/"+address);

    QNetworkRequest request(url);
      qDebug()<<url;

    QNetworkReply *reply = _nam->post(request, multiPart);
    qDebug()<<reply->errorString();
    connect(reply, &QNetworkReply::finished, this, &FileUploader::requestFinished);
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(requestError(QNetworkReply::NetworkError)));
    multiPart->setParent(reply); // delete the multiPart with the reply
}

void FileUploader::requestFinished()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if(!reply)
        return;

    qDebug()<<reply->errorString();

    delete reply;
    reply = nullptr;
}

void FileUploader::requestError(QNetworkReply::NetworkError code)
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if(!reply)
        return;

    qDebug()<<Q_FUNC_INFO<<" "<<reply->errorString();

    delete reply;
    reply = nullptr;
}

void FileUploader::hostNameChanged()
{
    _host = QUrl(CloudModel::instance()->getServer()).host();
}
