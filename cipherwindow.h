#ifndef CIPHERWINDOW_H
#define CIPHERWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QMap>
#include <QPushButton>
#include <QTextEdit>
#include <cmath>

QT_BEGIN_NAMESPACE
namespace Ui { class CipherWindow; }
QT_END_NAMESPACE

class CipherWindow : public QMainWindow
{
    Q_OBJECT

public:
    CipherWindow(QWidget *parent = nullptr);
    ~CipherWindow();

private slots:
    void encrypt();
    void decrypt();
private:
    QString bin(QString str);
    bool norm(QString binNum);
    QString rotate(QString str, int shift);
    QString table(QString base, QString binCode);
    QString getCode(QString table, QString symbol, int shift, int phase);
    QString setMesh(QString shiferText, int meshValue);

    QString removeMesh(QString shiferText, int meshValue);
    QString getSourceCode(QString table, QString shiferText, int shift, int phase);
    QString fromBinToDec(QString bin);

    QMap<QString, QString> alphabet;
    QString curTable;
    Ui::CipherWindow *ui;
};
#endif // CIPHERWINDOW_H
