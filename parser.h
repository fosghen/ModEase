#ifndef PARSER_H
#define PARSER_H

#include <QMainWindow>
#include <QFile>
#include <QJsonObject>
#include "mainwindow.h"

struct RegisterData {
    QString meaning;
    QString units;
    int address;
    int multy = 1;
    int digits = 0;
};

class RegisterParser {
public:
    RegisterParser();
    void ReadData(const QString filePath);
    void PasteData(Ui::MainWindow *ui);
    ~RegisterParser();

private:
    QString m_filePath;
    QJsonObject root;
};

#endif // PARSER_H
