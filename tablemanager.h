#ifndef TABLEMANAGER_H
#define TABLEMANAGER_H

#include <QObject>
#include <QJsonObject>
#include <QTableWidget>
#include <QMap>
#include <QCheckBox>


class TableManager : public QObject {
    Q_OBJECT

public:
    explicit TableManager(QObject *parent = nullptr);

    void readInitialize(QTabWidget* tabWidget_, QCheckBox *multyBox, QString registerPath);
    void chooseAllRegInTab(int index);
    void clearTable();
    void updateTable();

    struct Register
    {
        int Address;
        int Value;
        int Access;
        int Digits;
        float Multy;
        bool isWrite;
        bool isRead;
    };

    QMap<int, Register> registers;
    int numRegisterRead;
    int startRegAddress;
    QMap<int, QTableWidgetItem> registersTalbeItem;
    QVector<int> regAddress;

private slots:
    void onCellClicked(int row, int column);
    void onEnterClicked(QTableWidget* table);
    void onCellDoubleClicked(int row, int column);

private:
    void readJsonData(const QString jsonDataPath);
    void pasteJsonData();
    QTableWidget* configTab(QString tab, QWidget* newTab, QJsonObject regObject);
    int configTable(QJsonObject regInfo, QTableWidget* table, int row);
    void configItem(QTableWidgetItem* item);
    void toggleRegisterState(QTableWidget* table, int row);

    QTableWidget *m_tableWidget;
    QJsonObject jsonData;
    QTabWidget *m_tabWidget;
    QCheckBox *m_multyBox;
};

#endif // TABLEMANAGER_H
