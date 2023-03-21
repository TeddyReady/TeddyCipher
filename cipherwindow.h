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
private:
    QString bin(QString str) { return str.setNum(str.toInt(), 2);}
    bool norm(QString binNum);
    QString rotate(QString str, int shift);
    QString table(QString base, QString binCode);

    QMap<QString, QString> alphabet;
    QString curTable;
    Ui::CipherWindow *ui;
};
#endif // CIPHERWINDOW_H
