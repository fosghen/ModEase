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
    QString registerPath;

    void customizeWindow();
    void initActions();
    void defineComPorts();
    void openFileDialog();

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
