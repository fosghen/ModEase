#ifndef REGISTERMANAGER_H
#define REGISTERMANAGER_H

#include "tablemanager.h"

#include <QObject>
#include <QModbusRtuSerialClient>

class TableManager;
class RegisterManager : public QObject {
    Q_OBJECT
public:
    explicit RegisterManager(QObject *parent = nullptr);
    void registersInit(TableManager* m_tableManager);
    void readRegisters(int slaveAddress, QModbusRtuSerialClient *modbusDevice);
    void writeRegisters(TableManager* m_tableManager, int slaveAddress, QModbusRtuSerialClient *modbusDevice);
    void onReadReady();

signals:
    void clearStatusBar();
    void isFinished();
    void writeError(const QString &errorMessage);

private:
    QModbusRtuSerialClient *modbusDevice;
    QMap<int, TableManager::Register>* registers;
    QVector<int>* regAddress;

};

#endif // REGISTERMANAGER_H
