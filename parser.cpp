#include "parser.h"
#include "mainwindow.h"
#include "QJsonObject"
#include "QDebug"
#include "QJsonDocument"

RegisterParser::RegisterParser() {}

void RegisterParser::ReadData(const QString filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open file";
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull() || !doc.isObject()) {
        qDebug() << "Failed to parse JSON";
    }

    root = doc.object();
}


void RegisterParser::PasteData(Ui::MainWindow *ui)
{
    ui->tabWidget;
}
