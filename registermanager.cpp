#include "registermanager.h"

#include "tablemanager.h"
#include <QThread>

int current_register;

RegisterManager::RegisterManager(QObject *parent)
    : QObject{parent}
{}

void RegisterManager::registersInit(TableManager* m_tableManager)
{
    registers = &m_tableManager->registers;
    numRegisterRead = &m_tableManager->numRegisterRead;
    startRegAddress = &m_tableManager->startRegAddress;
    regAddress = &m_tableManager->regAddress;
}

void RegisterManager::writeRegisters(TableManager* m_tableManager, int slaveAddress, QModbusRtuSerialClient *modbusDevice)
{
    if (!modbusDevice)
        return;
    emit clearStatusBar();

    for (int i = 0; i < regAddress->size(); i++){
        if (!(*registers)[(*regAddress)[i]].isWrite) continue;
        float settedValue = m_tableManager->registersTalbeItem[(*regAddress)[i]].text().toDouble();
        settedValue = settedValue / (*registers)[(*regAddress)[i]].Multy + 0.5;
        (*registers)[(*regAddress)[i]].Value = static_cast<int>(settedValue);

        QModbusDataUnit writeUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters,
                                                    (*regAddress)[i],
                                                    1);
        writeUnit.setValue(0, (*registers)[(*regAddress)[i]].Value);

        if (auto *reply = modbusDevice->sendWriteRequest(writeUnit, slaveAddress)) {
            if (!reply->isFinished()) {
                connect(reply, &QModbusReply::finished, this, [this, reply]() {
                    const auto error = reply->error();
                    if (error == QModbusDevice::ProtocolError) {
                        emit writeError(tr("Ошибка записи ответа: %1 (Исключение Modbus: 0x%2)").
                                        arg(reply->errorString()).arg(reply->rawResult().exceptionCode(), -1, 16));

                    } else if (error != QModbusDevice::NoError) {
                        emit writeError(tr("Ошибка записи ответа: %1 (код: 0x%2)").
                                        arg(reply->errorString()).arg(error, -1, 16));
                    }
                    reply->deleteLater();
                });
            } else {
                reply->deleteLater();
            }
        }
    }
    m_tableManager->clearTable();
}

void RegisterManager::readRegisters(int slaveAddress, QModbusRtuSerialClient *modbusDevice)
{
    if (!modbusDevice)
        return;
    emit clearStatusBar();
    current_register = 0;
    int a = 0;
    for (int i = 0; i < (*regAddress).size(); i++){

        if (!(*registers)[(*regAddress)[i]].isRead) continue;
        qDebug() << (*registers)[(*regAddress)[i]].Address;
        QModbusDataUnit readRequest = QModbusDataUnit(QModbusDataUnit::HoldingRegisters,
                                                      (*regAddress)[i],
                                                      1);
        if (auto *reply = modbusDevice->sendReadRequest(readRequest, slaveAddress)) {
            qDebug() << slaveAddress << "slaveAddress";
            if (!reply->isFinished()){
                connect(reply, &QModbusReply::finished, this, &RegisterManager::onReadReady);
                a++;
            }
            else
                delete reply;
        } else {
            emit writeError(tr("Ошибка чтения: %1").arg(modbusDevice->errorString()));
        }
    }
}

void RegisterManager::onReadReady()
{
    auto reply = qobject_cast<QModbusReply *>(sender());
    if (!reply)
        return;

    if (reply->error() == QModbusDevice::NoError) {
        const QModbusDataUnit unit = reply->result();
        qDebug() <<current_register << "Reply";
        // if ((*registers)[(*regAddress)[current_register]].Access == 1) current_register ++;
        (*registers)[(*regAddress)[*startRegAddress + current_register]].Value = unit.value(0);
        current_register++;

    } else if (reply->error() == QModbusDevice::ProtocolError) {
        emit writeError(tr("Ошибка чтения ответа: %1 (Исключение Modbus: 0x%2)").
                                 arg(reply->errorString()).
                                 arg(reply->rawResult().exceptionCode(), -1, 16));
    } else {
        emit writeError(tr("Ошибка чтения ответа: %1 (код: 0x%2)").
                                 arg(reply->errorString()).
                                 arg(reply->error(), -1, 16));
    }

    reply->deleteLater();

    qDebug() << *startRegAddress << "," << *numRegisterRead;
    if (current_register == *numRegisterRead){
        emit isFinished();
    }
    QThread::msleep(10);
}


