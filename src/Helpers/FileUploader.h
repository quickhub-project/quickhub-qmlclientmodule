#ifndef IMAGEUPLOADER_H
#define IMAGEUPLOADER_H

#include <QObject>
#include <QNetworkReply>
#include <QNetworkAccessManager>

class FileUploader : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString token READ token WRITE setToken NOTIFY tokenChanged)
    Q_PROPERTY(QString resource READ imageID WRITE setImageID NOTIFY imageIDChanged)


public:
    explicit FileUploader(QObject *parent = nullptr);
    Q_INVOKABLE void uploadImage(QString filename);
    Q_INVOKABLE void sendFileToDevice(QString filename, QString deviceID);

    QString token() const;
    void setToken(const QString &token);

    QString imageID() const;
    void setImageID(const QString &imageID);

private:
    void uploadFile(QString filename, QString endpoint, QString address);
    QString _token;
    QString _imageID;
    QString _host;
    QNetworkAccessManager* _nam;

signals:
    void tokenChanged();
    void imageIDChanged();

private slots:
    void requestFinished();
    void requestError(QNetworkReply::NetworkError code);
    void hostNameChanged();
};

#endif // IMAGEUPLOADER_H
