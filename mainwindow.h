#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#define N_COLS 5

#include <QMainWindow>
#include <QDialog>
#include <QTableWidget>
#include <QJsonObject>

QT_BEGIN_NAMESPACE
namespace Ui {
class ModEase;
}

enum HoldingRegisterAddresses {
    TGet = 0,
    Current = 1,
    Voltage = 2,
    TSet = 0
};

class QModbusClient;
class SettingsDialog;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    QMap<QString, QTableWidget*> tableContainer;
    Ui::ModEase *ui;
    ~MainWindow();

private:
    void initActions();
    void defineComPorts();
    void updateTable();
    void setupTable(QStringList &registers_info);
    void readInitialize();
    void pasteJsonData();
    void readJsonData(const QString jsonDataPath);
    void configItem(QTableWidgetItem* item);
    QTableWidget* configTab(QString tab, QWidget* newTab, QJsonObject regObject);
    int configTable(QJsonObject regInfo, QTableWidget* table, int row);
    void toggleRegisterState(QTableWidget* table, int row);


private slots:
    void onConnectButtonClicked();
    void onGetDataButtonClicked();
    void onSetDataButtonClicked();
    void onReadReady();
    void onCellClicked(int row, int column);
    void clearTable();
    void onEnterClicked(QTableWidget* table);
    void multyBoxChanged();
    void onCellDoubleClicked(int row, int column);
    void customizeWindow();

private:
    QModbusClient *modbusDevice = nullptr;
    SettingsDialog *m_settingsDialog = nullptr;

    QJsonObject jsonData;
    QVector<int> registersAddress;
    QMap<int, int> registersValues;
    QMap<int, bool> registersWrite;
    QMap<int, int> registersDigits;
    QMap<int, float> registersMulty;
    QMap<int, int> registersAccess;
    QMap<int, QTableWidgetItem> registersTalbeItem;
    bool isConnected = false;
};



#endif // MAINWINDOW_H
