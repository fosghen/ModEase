#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#define N_COLS 5

#include <QMainWindow>
#include <QDialog>
#include <QTableWidget>
#include <QJsonObject>

// QT_BEGIN_NAMESPACE
// namespace Ui {
// class ModEase;
// }

// class QModbusClient;
// class SettingsDialog;
// class ConnectionManager;
// class RegisterManager;
// class TableManager;

// QT_END_NAMESPACE

// class MainWindow : public QMainWindow
// {
//     Q_OBJECT

// public:
//     MainWindow(QWidget *parent = nullptr);
//     QMap<QString, QTableWidget*> tableContainer;
//     Ui::ModEase *ui;
//     ~MainWindow();

// private:
//     void initActions();
//     void defineComPorts();
//     void updateTable();
//     void setupTable(QStringList &registers_info);
//     void readInitialize();
//     void pasteJsonData();
//     void readJsonData(const QString jsonDataPath);
//     void configItem(QTableWidgetItem* item);
//     QTableWidget* configTab(QString tab, QWidget* newTab, QJsonObject regObject);
//     int configTable(QJsonObject regInfo, QTableWidget* table, int row);
//     void toggleRegisterState(QTableWidget* table, int row);


// private slots:
//     void onConnectButtonClicked();
//     void onGetDataButtonClicked();
//     void onSetDataButtonClicked();
//     void onReadReady();
//     void onCellClicked(int row, int column);
//     void clearTable();
//     void onEnterClicked(QTableWidget* table);
//     void multyBoxChanged();
//     void onCellDoubleClicked(int row, int column);
//     void customizeWindow();

// private:
//     QModbusClient *modbusDevice = nullptr;
//     SettingsDialog *m_settingsDialog = nullptr;

//     QJsonObject jsonData;
//     QVector<int> registersAddress;
//     QMap<int, int> registersValues;
//     QMap<int, bool> registersWrite;
//     QMap<int, int> registersDigits;
//     QMap<int, float> registersMulty;
//     QMap<int, int> registersAccess;
//     QMap<int, QTableWidgetItem> registersTalbeItem;
//     bool isConnected = false;
// };

QT_BEGIN_NAMESPACE
namespace Ui {
class ModEase;
}

class ConnectionManager;
class RegisterManager;
class SettingsDialog;
class TableManager;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);


    ~MainWindow();

private:
    Ui::ModEase *ui;
    SettingsDialog *m_settingsDialog = nullptr;
    ConnectionManager *m_connectionManager;
    RegisterManager *m_registerManager;
    TableManager *m_tableManager;

    void customizeWindow();
    void initActions();
    void defineComPorts();

private slots:
    void updateUIOnConnectionStatus(bool isConnected);
    void displayConnectionError(const QString &errorMessage);
    void handleConnectButton();
    void multyBoxChanged();
    void clearStatusBar();
    void sendData();
    void getData();
    void finishGetData();
};



#endif // MAINWINDOW_H
