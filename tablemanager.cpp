#include "tablemanager.h"
#include <QJsonDocument>
#include <QFile>
#include <QVBoxLayout>
#include <QShortcut>
#include <QCheckBox>

int prev_reg = 0;

TableManager::TableManager(QObject *parent)
    : QObject{parent}
{}

void TableManager::readInitialize(QTabWidget *tabWidget_, QCheckBox *multyBox, QString registerPath)
{
    m_tabWidget = tabWidget_;
    m_multyBox = multyBox;

    readJsonData(registerPath);
    pasteJsonData();
}

void TableManager::readJsonData(const QString jsonDataPath)
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

void TableManager::toggleRegisterState(QTableWidget* table, int row)
{
    int register_ = table->item(row, 3)->text().toInt();

    if (!m_multyBox->isChecked()){
        if (registers[register_].isWrite) {
            clearTable();
            return;
        }
        clearTable();
    }

    for (int col = 0; col < table->columnCount(); ++col) {
        QTableWidgetItem *item = table->item(row, col);
        if (item) {
            if (!registers[register_].isWrite){
                item->setBackground(QBrush(QColor(255, 255, 0, 128)));
            } else {
                item->setBackground(QBrush(QColor(255, 255, 255)));
            }
        }
    }

    registers[register_].isWrite = !registers[register_].isWrite;
}

void TableManager::onCellClicked(int row, int column)
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

void TableManager::onEnterClicked(QTableWidget* table)
{
    int row = table->currentRow();
    if (row >= 0) {
        toggleRegisterState(table, row);
    }
}

void TableManager::onCellDoubleClicked(int row, int column)
{
    QTableWidget *table = qobject_cast<QTableWidget*>(sender());
    if (table) {
        toggleRegisterState(table, row);
    }
}

QTableWidget* TableManager::configTab(QString tab, QWidget* newTab, QJsonObject regObject)
{
    QTableWidget *newTable = new QTableWidget(regObject.size(), 5, newTab);
    QVBoxLayout *layout = new QVBoxLayout(newTab);
    newTable->setHorizontalHeaderLabels({"Ключевое название", "Значение", "Ед. измерения", "Адрес", "Описание"});
    newTable->setColumnWidth(4, 200);
    newTable->setColumnWidth(0, 150);
    newTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    newTable->setSelectionMode(QAbstractItemView::SingleSelection);
    layout->addWidget(newTable);
    newTable->setStyleSheet("QTableWidget::item:selected { background-color: rgba(169, 169, 169, 128); }");
    newTab->setLayout(layout);
    m_tabWidget->addTab(newTab, tab);

    QShortcut *shortcut = new QShortcut(QKeySequence(Qt::Key_Return), newTable);

    connect(newTable, &QTableWidget::cellClicked, this, &TableManager::onCellClicked);
    connect(newTable, &QTableWidget::cellDoubleClicked, this, &TableManager::onCellDoubleClicked);
    // Чтобы подключить шорткат для Enter надо создать лямбду функцию
    connect(shortcut, &QShortcut::activated, this, [this, newTable]() {onEnterClicked(newTable);});
    connect(m_tabWidget, &QTabWidget::currentChanged, this, &TableManager::clearTable);

    return newTable;
}

int TableManager::configTable(QJsonObject regInfo, QTableWidget* table, int row)
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

void TableManager::configItem(QTableWidgetItem* item)
{
    QFont *font = new QFont();
    font->setBold(true);

    item->setFont(*font);
    item->setFlags(item->flags() & ~Qt::ItemIsEditable);
}

void TableManager::pasteJsonData()
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
            struct Register register_;
            QJsonObject regInfo = regObject.value(QString::number(reg)).toObject();
            int address = configTable(regInfo, newTable, row);
            float multy = regInfo.value("multy").toDouble();
            int digits = regInfo.value("digits").toInt();
            int access = regInfo.value("access").toInt();

            register_.Address = address;
            register_.Value = 0;
            register_.Access = access;
            register_.Digits = digits;
            register_.Multy = multy;
            register_.isWrite = false;

            registers[address] = register_;
            regAddress.append(address);

            row++;
        }
    }
}

void TableManager::clearTable()
{
    int pageCount = m_tabWidget->count();

    for (int i = 0; i < pageCount; i++){
        QWidget *tab = m_tabWidget->widget(i);
        QVBoxLayout *layout = qobject_cast<QVBoxLayout*>(tab->layout());
        QTableWidget *tableWidget = qobject_cast<QTableWidget*>(layout->itemAt(0)->widget());
        for (int row = 0; row < tableWidget->rowCount(); row++){
            for (int colomn = 0; colomn < tableWidget->columnCount(); colomn++){
                QTableWidgetItem *item = tableWidget->item(row, colomn);
                if (item) item->setBackground(QBrush(QColor(255, 255, 255)));
            }
            QTableWidgetItem *item = tableWidget->item(row, 3);
            int address = item->text().toInt();
            registers[address].isWrite = false;
        }
    }
}

void TableManager::updateTable()
{
    for (int i = 0; i < regAddress.size(); i++){
        float value = registers[regAddress[i]].Value;
        value *= registers[regAddress[i]].Multy;

        registersTalbeItem[regAddress[i]].setText(QString::number(value, 'f', registers[regAddress[i]].Digits));
   }
}

