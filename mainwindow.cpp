#include "mainwindow.h"
#include "connectionmanager.h"
#include "registermanager.h"
#include "tablemanager.h"
#include "./ui_mainwindow.h"
#include "settingsdialog.h"

#include <QFile>
#include <QSerialPortInfo>
#include <QModbusDevice>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ModEase)
 {
    m_connectionManager = new ConnectionManager(this);
    m_registerManager = new RegisterManager(this);
    m_tableManager = new TableManager(this);
    m_settingsDialog = new SettingsDialog(this);
    ui->setupUi(this);

    customizeWindow();
    initActions();
    defineComPorts();
    openFileDialog();
    m_tableManager->readInitialize(ui->tabWidget, ui->multyBox, registerPath);
    m_registerManager->registersInit(m_tableManager);
}

void MainWindow::customizeWindow() {
    this->setWindowTitle("ModEase");
    this->setWindowIcon(QIcon(":/icons/main_icon.png"));

    QFile file(":/style.qss");
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream stream(&file);
        qApp->setStyleSheet(stream.readAll());
     }
}

void MainWindow::initActions() {
    connect(m_connectionManager, &ConnectionManager::connectionStatusChanged, this, &MainWindow::updateUIOnConnectionStatus);
    connect(m_connectionManager, &ConnectionManager::connectionFailed, this, &MainWindow::displayConnectionError);

    connect(ui->connectButton, &QPushButton::clicked, this, &MainWindow::handleConnectButton);
    connect(ui->actionSettings, &QAction::triggered, m_settingsDialog, &QDialog::show);
    connect(ui->multyBox, &QCheckBox::checkStateChanged, this, &MainWindow::multyBoxChanged);
    connect(ui->setDataButton, &QPushButton::clicked, this, &MainWindow::sendData);
    connect(ui->getDataButton, &QPushButton::clicked, this, &MainWindow::getData);

    connect(m_registerManager, &RegisterManager::clearStatusBar, this, &MainWindow::clearStatusBar);
    connect(m_registerManager, &RegisterManager::writeError, this, &MainWindow::displayConnectionError);
    connect(m_registerManager, &RegisterManager::isFinished, this, &MainWindow::finishGetData);
}


void MainWindow::openFileDialog() {
    registerPath = QFileDialog::getOpenFileName(this, tr("Выберите файл"), "", tr("All Files (*);;Text Files (*.txt)"));
}

void MainWindow::sendData()
{
    m_registerManager->writeRegisters(m_tableManager, ui->slaveEdit->value(), m_connectionManager->getDevice());
}

void MainWindow::finishGetData()
{
    m_tableManager->updateTable();
    ui->getDataButton->setEnabled(true);
}

void MainWindow::getData()
{
    ui->getDataButton->setEnabled(false);
    m_registerManager->readRegisters(ui->slaveEdit->value(), m_connectionManager->getDevice());
}

void MainWindow::clearStatusBar()
{
    statusBar()->clearMessage();
}

void MainWindow::defineComPorts()
{
    QList<QSerialPortInfo> serialPorts = QSerialPortInfo::availablePorts();

    for (const QSerialPortInfo &portInfo : serialPorts) {
        ui->portComboBox->addItem(portInfo.portName());
    }
}

void MainWindow::updateUIOnConnectionStatus(bool isConnected) {
    if (isConnected) {
        ui->connectButton->setText("Отключиться");
        ui->getDataButton->setEnabled(true);
        ui->setDataButton->setEnabled(true);
    } else {
        ui->connectButton->setText("Подключиться");
        ui->getDataButton->setEnabled(false);
        ui->setDataButton->setEnabled(false);
    }
}

void MainWindow::displayConnectionError(const QString &errorMessage) {
    statusBar()->showMessage(errorMessage, 5000);
}

void MainWindow::handleConnectButton() {
    auto settings = m_settingsDialog->settings();
    settings.portName = ui->portComboBox->currentText();
    if (m_connectionManager->connectDevice(settings)) {
        ui->connectButton->setText("Отключиться");
    } else {
        ui->connectButton->setText("Подключиться");
    }
}

void MainWindow::multyBoxChanged()
{
    if (!ui->multyBox->isChecked()){
        m_tableManager->clearTable();
    }
}

MainWindow::~MainWindow() {
    delete m_connectionManager;
    delete m_registerManager;
    delete m_tableManager;
}





// MainWindow::~MainWindow()
// {
//     delete ui;
// }
