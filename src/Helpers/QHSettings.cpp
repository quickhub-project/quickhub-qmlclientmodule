#include "QHSettings.h"
#include <QtDebug>
#include <QDir>
#include <QTimer>

Q_GLOBAL_STATIC(QHSettings, qhSettings);

QHSettings::QHSettings(QObject *parent)
    : QSettings{parent}
{
    std::function<void(void)> *testSettingsReady = new std::function<void(void)>();
    *testSettingsReady = [=](){
        if (this->status() == QSettings::NoError) {
            delete testSettingsReady;
            setReady(true);
            qDebug()<<"QSettings is ready.";
        } else {
            QTimer::singleShot(10, *testSettingsReady);
        }
    };
    (*testSettingsReady)();
}

QHSettings *QHSettings::instance()
{
    return qhSettings;
}

bool QHSettings::ready()
{
    return _ready;
}

void QHSettings::setReady(bool newReady)
{
    if (_ready == newReady)
        return;

    _ready = newReady;
    emit readyChanged();
}

