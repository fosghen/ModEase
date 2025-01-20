#include "connectionmanager.h"
#include "settingsdialog.h"

ConnectionManager::ConnectionManager(QObject *parent) :
    QObject(parent), m_modbusDevice (nullptr), m_isConnected(false) {}

bool ConnectionManager::connectDevice(SettingsDialog::Settings settings) {
    if (m_isConnected){
        disconnectDevice();
        emit connectionStatusChanged(m_isConnected);

        return m_isConnected;
    }

    m_modbusDevice  = new QModbusRtuSerialClient(this);

        m_modbusDevice ->setConnectionParameter(QModbusDevice::SerialPortNameParameter,
                                             settings.portName);
        m_modbusDevice ->setConnectionParameter(QModbusDevice::SerialParityParameter,
                                             settings.parity);
        m_modbusDevice ->setConnectionParameter(QModbusDevice::SerialBaudRateParameter,
                                             settings.baud);
        m_modbusDevice ->setConnectionParameter(QModbusDevice::SerialDataBitsParameter,
                                             settings.dataBits);
        m_modbusDevice ->setConnectionParameter(QModbusDevice::SerialStopBitsParameter,
                                             settings.stopBits);

        m_modbusDevice ->setTimeout(500);
        m_modbusDevice ->setNumberOfRetries(5);
        if (!m_modbusDevice ->connectDevice()) {
            emit connectionFailed(tr("Подключение не выполнено: %1").arg(m_modbusDevice ->errorString()));
            return false;
        }

    m_isConnected = true;
    emit connectionStatusChanged(m_isConnected);
    return m_isConnected;
}

void ConnectionManager::disconnectDevice() {
    if (m_isConnected && m_modbusDevice ) {
        m_modbusDevice ->disconnectDevice();
        delete m_modbusDevice ;
        m_modbusDevice  = nullptr;
        m_isConnected = false;
    }
}

QModbusRtuSerialClient* ConnectionManager::getDevice()
{
    return m_modbusDevice;
}

bool ConnectionManager::isConnected() const {
    return m_isConnected;
}
