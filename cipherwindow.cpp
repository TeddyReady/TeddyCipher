#include "cipherwindow.h"
#include "ui_cipherwindow.h"

CipherWindow::CipherWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::CipherWindow)
{
    ui->setupUi(this);
    ui->sbShift->setMaximum(32);
    ui->sbShift->setMinimum(1);
    ui->sbShift->setValue(4);
    ui->sbPhaseShift->setMaximum(8);
    ui->sbPhaseShift->setMinimum(1);
    ui->sbPhaseShift->setValue(3);
    ui->sbMesh->setMaximum(8);
    ui->sbMesh->setMinimum(1);
    ui->sbMesh->setValue(4);

    QFile config(":/data/alphabet.conf");
    if (!config.open(QFile::ReadOnly)) qDebug() << "Cannot open config file!";
    else {
        QTextStream out(&config);
        while (!out.atEnd())
        {
            QString str = out.readLine();
            ui->cbBaseSym->addItem(str.split("=")[0].right(1));
            curTable.append(str.split("=")[0].right(1));
            alphabet.insert(str.split("=")[0].right(1), str.split("=")[1]);
        }
    }

    connect(ui->pbEncrypt, &QPushButton::clicked, this, &CipherWindow::encrypt);
}

CipherWindow::~CipherWindow()
{
    delete ui;
}

void CipherWindow::encrypt()
{
    QString message = ui->teMessage->toPlainText();
    QString cipherText;
    QString baseSymbol = ui->cbBaseSym->currentText();
    for (int i = 0; i < message.size(); ++i)
    {

        if (alphabet.contains(QString(message[i])))
        {
            //QString symbol = alphabet.value(QString(message[i]));
            int cols = 0, rows = 0;

            qDebug() << table(baseSymbol, bin(alphabet.value(baseSymbol)));

            baseSymbol = QString(message[i]);

        } else cipherText.append(QString(message[i]));

    }
    ui->teMessage->append("\n###    Your Cipher Text below  ###:");
    ui->teMessage->append(cipherText);
}

bool CipherWindow::norm(QString binNum)
{
    int cnt_1 = 0, cnt_0 = 0;
    for (int i = 0; i < binNum.size(); ++i)
    {
        if (QString(binNum[i]).toInt() == 0) ++cnt_0;
        else ++cnt_1;
    }
    return cnt_1 >= cnt_0 ? true : false;
}

QString CipherWindow::rotate(QString str, int shift)
{
    QString result;
    for (int i = 0; i < str.size(); ++i)
        result.append(str[(i + shift) % str.size()]);
    return result;
}

QString CipherWindow::table(QString base, QString binCode)
{
    QString table = rotate(curTable, curTable.indexOf(base));
    QString result;
    for (int i = 0; i < table.size();)
    {
        for (int j = 0; j < binCode.size(); ++j) {
            if (QString(binCode[j]).toInt() == norm(binCode)) {
                result.append(table[i]);
                ++i;
                if (i == table.size()) break;
            }
            else result.append("_");
        }
    }
    return result;
}
