#include "mainwindow.h"
#include "settingsdialog.h"
#include "./ui_mainwindow.h"
#include "algorithm"

#include <QModbusRtuSerialClient>
#include <QtCore>
#include <QtGui>
#include <QtWidgets>
#include <QtSerialPort>

int current_register = 0;
int prev_reg = 0;
int count_click = 0;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ModEase)
{
    m_settingsDialog = new SettingsDialog(this);
    ui->setupUi(this);
    initActions();
    defineComPorts();
    readInitialize();
    customizeWindow();
}

void MainWindow::customizeWindow()
{
    this->setWindowTitle("ModEase");
    this->setWindowIcon(QIcon(":/icons/main_icon.png"));

    QFile file(":/style.qss");
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream stream(&file);
        qApp->setStyleSheet(stream.readAll());
    }
}

void MainWindow::initActions()
{
    connect(ui->connectButton, &QPushButton::clicked, this, &MainWindow::onConnectButtonClicked);
    connect(ui->setDataButton, &QPushButton::clicked, this, &MainWindow::onSetDataButtonClicked);
    connect(ui->getDataButton, &QPushButton::clicked, this, &MainWindow::onGetDataButtonClicked);

    connect(ui->actionSettings, &QAction::triggered, m_settingsDialog, &QDialog::show);
    connect(ui->multyBox, &QCheckBox::checkStateChanged, this, &MainWindow::multyBoxChanged);
}

void MainWindow::multyBoxChanged()
{
    if (!ui->multyBox->isChecked()){
        clearTable();
    }
}

void MainWindow::onConnectButtonClicked()
{
    if (isConnected){
        modbusDevice->disconnectDevice();
        delete modbusDevice;
        modbusDevice = nullptr;

        ui->connectButton->setText("Подключиться");
        ui->getDataButton->setEnabled(false);
        ui->setDataButton->setEnabled(false);

        defineComPorts();

        isConnected = !isConnected;
    } else{
        modbusDevice = new QModbusRtuSerialClient(this);

        const auto settings = m_settingsDialog->settings();

        modbusDevice->setConnectionParameter(QModbusDevice::SerialPortNameParameter,
                                             ui->portComboBox->currentText());
        modbusDevice->setConnectionParameter(QModbusDevice::SerialParityParameter,
                                             settings.parity);
        modbusDevice->setConnectionParameter(QModbusDevice::SerialBaudRateParameter,
                                             settings.baud);
        modbusDevice->setConnectionParameter(QModbusDevice::SerialDataBitsParameter,
                                             settings.dataBits);
        modbusDevice->setConnectionParameter(QModbusDevice::SerialStopBitsParameter,
                                             settings.stopBits);

        modbusDevice->setTimeout(1000);
        modbusDevice->setNumberOfRetries(3);
        if (!modbusDevice->connectDevice()) {
            statusBar()->showMessage(tr("Подключение не выполнено: %1").arg(modbusDevice->errorString()), 5000);
        } else {
            ui->connectButton->setText("Отключиться");
            ui->getDataButton->setEnabled(true);
            ui->setDataButton->setEnabled(true);
            isConnected = !isConnected;
        }
    }
}

void MainWindow::defineComPorts()
{
    QList<QSerialPortInfo> serialPorts = QSerialPortInfo::availablePorts();

    for (const QSerialPortInfo &portInfo : serialPorts) {
        ui->portComboBox->addItem(portInfo.portName());
    }
}

void MainWindow::toggleRegisterState(QTableWidget* table, int row)
{
    int register_ = table->item(row, 3)->text().toInt();

    if (!ui->multyBox->isChecked()){
        if (registersWrite[register_] == 1) {
            clearTable();
            return;
        }
        clearTable();
    }

    for (int col = 0; col < table->columnCount(); ++col) {
        QTableWidgetItem *item = table->item(row, col);
        if (item) {
            if (!registersWrite[register_]){
                item->setBackground(QBrush(QColor(255, 255, 0, 128)));
            } else {
                item->setBackground(QBrush(QColor(255, 255, 255)));
            }
        }
    }

    registersWrite[register_] = !registersWrite[register_];
}

void MainWindow::onCellClicked(int row, int column)
{
    QTableWidget *table = qobject_cast<QTableWidget*>(sender());
    if (!table) return;

    int register_ = table->item(row, 3)->text().toInt();
    if (register_ != prev_reg) {
        prev_reg = register_;
        return;
    }

    toggleRegisterState(table, row);
}

void MainWindow::onEnterClicked(QTableWidget* table)
{
    int row = table->currentRow();
    if (row >= 0) {
        toggleRegisterState(table, row);
    }
}

void MainWindow::onCellDoubleClicked(int row, int column)
{
    QTableWidget *table = qobject_cast<QTableWidget*>(sender());
    if (table) {
        toggleRegisterState(table, row);
    }
}

void MainWindow::onSetDataButtonClicked()
{
    if (!modbusDevice)
        return;
    statusBar()->clearMessage();

    for (int i = 0; i < registersAddress.size(); i++){
        if (!registersWrite[registersAddress[i]]) continue;
        float settedValue = registersTalbeItem[registersAddress[i]].text().toDouble();
        settedValue = settedValue / registersMulty[registersAddress[i]] + 0.5;
        registersValues[registersAddress[i]] = static_cast<int>(settedValue);

        QModbusDataUnit writeUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters,
                                                    registersAddress[i],
                                                    1);
        writeUnit.setValue(0, registersValues[registersAddress[i]]);

        if (auto *reply = modbusDevice->sendWriteRequest(writeUnit, ui->slaveEdit->value())) {
            if (!reply->isFinished()) {
                connect(reply, &QModbusReply::finished, this, [this, reply]() {
                    const auto error = reply->error();
                    if (error == QModbusDevice::ProtocolError) {
                        statusBar()->showMessage(tr("Ошибка записи ответа: %1 (Исключение Modbus: 0x%2)")
                                                     .arg(reply->errorString()).arg(reply->rawResult().exceptionCode(), -1, 16),
                                                     5000);
                    } else if (error != QModbusDevice::NoError) {
                        statusBar()->showMessage(tr("Ошибка записи ответа: %1 (код: 0x%2)").
                                                 arg(reply->errorString()).arg(error, -1, 16), 5000);
                    }
                    reply->deleteLater();
                });
            } else {
                reply->deleteLater();
            }
        }
    }
    clearTable();
}

void MainWindow::onGetDataButtonClicked()
{
    ui->getDataButton->setEnabled(false);
    if (!modbusDevice)
        return;
    statusBar()->clearMessage();
    current_register = 0;

    for (int i = 0; i < registersAddress.size(); i++){
        if (registersAccess[registersAddress[i]] == 1) continue; // Не тестировалось
        QModbusDataUnit readRequest = QModbusDataUnit(QModbusDataUnit::HoldingRegisters,
                                                      registersAddress[i],
                                                      1);
        if (auto *reply = modbusDevice->sendReadRequest(readRequest, ui->slaveEdit->value())) {
            if (!reply->isFinished())
                connect(reply, &QModbusReply::finished, this, &MainWindow::onReadReady);
            else
                delete reply;
        } else {
            statusBar()->showMessage(tr("Ошибка чтения: %1").arg(modbusDevice->errorString()), 5000);
        }
    }
}

void MainWindow::onReadReady()
{
    auto reply = qobject_cast<QModbusReply *>(sender());
    if (!reply)
        return;

    if (reply->error() == QModbusDevice::NoError) {
        const QModbusDataUnit unit = reply->result();
        registersValues[registersAddress[current_register]] = unit.value(0);
        current_register++;
        if (registersAccess[registersAddress[current_register]] == 1) current_register ++;
    } else if (reply->error() == QModbusDevice::ProtocolError) {
        statusBar()->showMessage(tr("Ошибка чтения ответа: %1 (Исключение Modbus: 0x%2)").
                                 arg(reply->errorString()).
                                 arg(reply->rawResult().exceptionCode(), -1, 16), 5000);
    } else {
        statusBar()->showMessage(tr("Ошибка чтения ответа: %1 (код: 0x%2)").
                                 arg(reply->errorString()).
                                 arg(reply->error(), -1, 16), 5000);
    }

    reply->deleteLater();

    if (current_register == registersAddress.size()){
        updateTable();
        ui->getDataButton->setEnabled(true);
    }
}

void MainWindow::updateTable()
{
    for (int i = 0; i < registersAddress.size(); i++){
        float value = registersValues[registersAddress[i]];
        value *= registersMulty[registersAddress[i]];

        registersTalbeItem[registersAddress[i]].setText(QString::number(value, 'f', registersDigits[registersAddress[i]]));
    }
}

void MainWindow::readInitialize()
{
    QString filePath = "registers.json";

    readJsonData(filePath);
    pasteJsonData();
}

void MainWindow::readJsonData(const QString jsonDataPath)
{
    QFile file(jsonDataPath);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open file";
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull() || !doc.isObject()) {
        throw std::runtime_error("Failed to parse JSON");
    }

    jsonData = doc.object();
}

void MainWindow::configItem(QTableWidgetItem* item)
{
    QFont *font = new QFont();
    font->setBold(true);

    item->setFont(*font);
    item->setFlags(item->flags() & ~Qt::ItemIsEditable);
}

QTableWidget* MainWindow::configTab(QString tab, QWidget* newTab, QJsonObject regObject)
{
    QTableWidget *newTable = new QTableWidget(regObject.size(), N_COLS, newTab);
    QVBoxLayout *layout = new QVBoxLayout(newTab);
    newTable->setHorizontalHeaderLabels({"Ключевое название", "Значение", "Ед. измерения", "Адрес", "Описание"});
    newTable->setColumnWidth(4, 200);
    newTable->setColumnWidth(0, 150);
    newTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    newTable->setSelectionMode(QAbstractItemView::SingleSelection);
    layout->addWidget(newTable);
    newTable->setStyleSheet("QTableWidget::item:selected { background-color: rgba(169, 169, 169, 128); }");
    newTab->setLayout(layout);
    ui->tabWidget->addTab(newTab, tab);

    QShortcut *shortcut = new QShortcut(QKeySequence(Qt::Key_Return), newTable);

    connect(newTable, &QTableWidget::cellClicked, this, &MainWindow::onCellClicked);
    connect(newTable, &QTableWidget::cellDoubleClicked, this, &MainWindow::onCellDoubleClicked);
    // Чтобы подключить шорткат для Enter надо создать лямбду функцию
    connect(shortcut, &QShortcut::activated, this, [this, newTable]() {onEnterClicked(newTable);});
    connect(ui->tabWidget, &QTabWidget::currentChanged, this, &MainWindow::clearTable);

    return newTable;
}

int MainWindow::configTable(QJsonObject regInfo, QTableWidget* table, int row)
{
    QTableWidgetItem *meaning = new QTableWidgetItem(regInfo.value("meaning").toString());
    configItem(meaning);

    QTableWidgetItem *units = new QTableWidgetItem(regInfo.value("units").toString());
    configItem(units);

    QTableWidgetItem *description = new QTableWidgetItem(regInfo.value("description").toString());
    configItem(description);

    int address_ = regInfo.value("address").toInt();
    QTableWidgetItem *address = new QTableWidgetItem(QString::number(address_));
    configItem(address);

    int access = regInfo.value("access").toInt();
    QTableWidgetItem *value = new QTableWidgetItem(regInfo.value("").toString());
    if (access == 0) configItem(value);

    registersTalbeItem[address_] = *value;
    table->setItem(row, 0, meaning);
    table->setItem(row, 1, &registersTalbeItem[address_]);
    table->setItem(row, 2, units);
    table->setItem(row, 3, address);
    table->setItem(row, 4, description);

    return address_;
}

void MainWindow::pasteJsonData()
{
    for (const QString& tab : jsonData.keys())
    {
        QWidget *newTab = new QWidget();
        QJsonObject regObject = jsonData.value(tab).toObject();
        QTableWidget *newTable = configTab(tab, newTab, regObject);
        QVector<int> keys;
        for (const QString& reg : regObject.keys()){
            keys.append(reg.toInt());
        }

        std::sort(keys.begin(), keys.end());

        int row = 0;
        for (int reg : keys)
        {
            QJsonObject regInfo = regObject.value(QString::number(reg)).toObject();
            int address = configTable(regInfo, newTable, row);
            float multy = regInfo.value("multy").toDouble();
            int digits = regInfo.value("digits").toInt();
            int access = regInfo.value("access").toInt();

            registersAddress.append(address);
            registersValues.insert(address, 0);
            registersWrite.insert(address, false);
            registersDigits.insert(address, digits);
            registersMulty.insert(address, multy);
            registersAccess.insert(address, access);

            row++;
        }
    }
}

void MainWindow::clearTable()
{
    int pageCount = ui->tabWidget->count();

    for (int i = 0; i < pageCount; i++){
        QWidget *tab = ui->tabWidget->widget(i);
        QVBoxLayout *layout = qobject_cast<QVBoxLayout*>(tab->layout());
        QTableWidget *tableWidget = qobject_cast<QTableWidget*>(layout->itemAt(0)->widget());
        for (int row = 0; row < tableWidget->rowCount(); row++){
            for (int colomn = 0; colomn < tableWidget->columnCount(); colomn++){
                QTableWidgetItem *item = tableWidget->item(row, colomn);
                if (item) item->setBackground(QBrush(QColor(255, 255, 255)));
            }
            QTableWidgetItem *item = tableWidget->item(row, 3);
            int address = item->text().toInt();
            registersWrite[address] = 0;
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
