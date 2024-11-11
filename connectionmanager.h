#ifndef CONNECTIONMANAGER_H
#define CONNECTIONMANAGER_H

#include <QModbusRtuSerialClient>
#include <QObject>
#include "settingsdialog.h"

class SettingsDialog;

class ConnectionManager : public QObject {
    Q_OBJECT
public:
    explicit ConnectionManager(QObject *parent = nullptr);
    bool connectDevice(SettingsDialog::Settings settings);
    void disconnectDevice();
    bool isConnected() const;
    QModbusRtuSerialClient* getDevice();

signals:
    void connectionFailed(const QString &errorMessage);
    void connectionStatusChanged(bool isConnected);

private:
    QModbusRtuSerialClient *m_modbusDevice;
    bool m_isConnected;
};

#endif // CONNECTIONMANAGER_H
