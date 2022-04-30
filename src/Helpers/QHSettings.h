#ifndef QHSETTINGS_H
#define QHSETTINGS_H

#include <QSettings>
#include <QObject>



class QHSettings : public QSettings
{
    Q_OBJECT
    Q_PROPERTY(bool ready READ ready NOTIFY readyChanged)

public:
    explicit QHSettings(QObject *parent = nullptr);
    static QHSettings *instance();
    bool ready();

private:
    void setReady(bool newReady);
    bool _ready = false;

signals:
    void readyChanged();
};

#endif // QHSETTINGS_H
