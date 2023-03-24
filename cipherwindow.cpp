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
    connect(ui->pbDecrypt, &QPushButton::clicked, this, &CipherWindow::decrypt);
    connect(ui->pbClear, &QPushButton::clicked, [&](){ui->teMessage->clear();});

    qDebug() << bin("154");
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
            QString curBinCode = bin(getCode(
                table(baseSymbol, bin(alphabet.value(baseSymbol))),
                    QString(message[i]), ui->sbShift->value(),
                                         ui->sbPhaseShift->value()));
            cipherText.append(curBinCode);
            baseSymbol = QString(message[i]);
        } else cipherText.append(QString(message[i]));

    }
    ui->teMessage->clear();
    ui->teMessage->append(setMesh(cipherText, ui->sbMesh->value()));
}

void CipherWindow::decrypt()
{
    QString shiferText = removeMesh(ui->teMessage->toPlainText(), ui->sbMesh->value());
    QString baseSymbol = ui->cbBaseSym->currentText();
    QString sourceText;
    for (int i = 0; i < shiferText.size(); i+=8) {
            QString curSymbol = shiferText.mid(i, 8);
            baseSymbol = getSourceCode(table(baseSymbol, bin(alphabet.value(baseSymbol))), curSymbol,
                            ui->sbShift->value(), ui->sbPhaseShift->value());
            sourceText += baseSymbol;
    }
    ui->teMessage->clear();
    ui->teMessage->append(sourceText);
}

QString CipherWindow::bin(QString str) {
    QString bin = str.setNum(str.toInt(), 2);
    int binSize = 8 - bin.size();
    if (bin.size() != 8) {
        for (int i = 0; i < binSize; ++i)
        {
            bin = "0" + bin;
        }
    } return bin;
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

QString CipherWindow::getCode(QString table, QString symbol, int shift, int phase)
{
    QString code;
    int curRow = 0;
    for (int col = 0; col < alphabet.size() * 2;) {
        if (QString(table[col]) == symbol) {
            code = QString::number(col % 8) + QString::number(curRow);
            break;
        } else {
            ++col;
            if (col % 8 == 0) {
                ++curRow;
            }
        }
    } if (code == "") return QString("ERROR_ENCRYPT");
    //shift
    int curCol = QString(code[0]).toInt() + QString(code[1]).toInt() * 8;
    curRow = QString(code[1]).toInt();
    while (shift != 0)
    {
        ++curCol;
        if (QString(table[curCol % table.size()]) != "_")
            --shift;
        if (curCol % 8 == 0) ++curRow;
    }
    code = QString::number(curCol % 8) + QString::number(curRow % 8);
    //phaseShift
    curCol = QString(code[0]).toInt() + QString(code[1]).toInt() * 8;
    curRow = QString(code[1]).toInt();
    if (phase % 2 == 0)
    {
        curCol += (phase / 2) % 8;
        curRow += (phase / 2) % 8;
    } else {
        curCol += phase % 8;
        curRow += phase % 8;
    }
    code = QString::number(curCol % 8) + QString::number(curRow % 8);
    return code;
}

QString CipherWindow::setMesh(QString shiferText, int meshValue)
{
    int meshUp, meshCounter = 0;
    if (meshValue % 2 == 1) meshUp = 1;
    else meshUp = 0;

    QString result;
    for (int i = 0; i < shiferText.size(); ++i)
    {
        result.append(shiferText[i]);
        ++meshCounter;
        if (meshCounter == meshValue) {
            result.append(QString::number(meshUp));
            meshUp = (meshUp + 1) % 2;
            meshCounter = 0;
        }
    }
    return result;
}

QString CipherWindow::removeMesh(QString shiferText, int meshValue)
{
    int meshCounter = 0;
    QString result;
    for (int i = 0; i < shiferText.size(); ++i)
    {
        result.append(shiferText[i]);
        ++meshCounter;
        if (meshCounter == meshValue) {
            ++i;
            meshCounter = 0;
        }
    }
    return result;
}

QString CipherWindow::getSourceCode
(QString table, QString shiferText,int shift, int phase)
{
    QString code = fromBinToDec(shiferText);
    if (code.size() < 2) code = "0" + code;

    int realPhase;
    if (phase % 2 == 0) realPhase = phase / 2;
    else realPhase = phase;


    int col = (QString(code[0]).toInt() - realPhase + 8) % 8;
    int row = (QString(code[1]).toInt() - realPhase + 8) % 8;
    int indexCol = row * 8 + col;
    shift = 32 - shift;
    qDebug() << table;
    while (shift != 0)
    {
        ++indexCol;
        qDebug() << QString(table[indexCol % table.size()]) << shift << indexCol << row;
        if (QString(table[indexCol % table.size()]) != "_") {
            --shift;
            if (indexCol == table.size()) {
                indexCol = 0; row = 0;
            }
        }
        if (indexCol % 8 == 0 && indexCol != 0) ++row;
    }

    col = indexCol % 8;
    row = row % 8;

    return QString(table[col + row*8]);
}

QString CipherWindow::fromBinToDec(QString bin)
{
    int num = 0;
    for (int i = 0; i < bin.size(); ++i) {
        num += pow(2, i) * QString(bin[bin.size() - i - 1]).toInt();
    }
    return QString::number(num);
}
