#include "calculator.h"
#include "ui_calculator.h"

#include <QDebug>

Calculator::Calculator(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Calculator), sumInMemory(0.0), sumSoFar(0.0)
    , factorSoFar(0.0), waitingForOperand(true)
{
    ui->setupUi(this);

    digitButtons[0] = ui->digitButton_0;
    digitButtons[1] = ui->digitButton_1;
    digitButtons[2] = ui->digitButton_2;
    digitButtons[3] = ui->digitButton_3;
    digitButtons[4] = ui->digitButton_4;
    digitButtons[5] = ui->digitButton_5;
    digitButtons[6] = ui->digitButton_6;
    digitButtons[7] = ui->digitButton_7;
    digitButtons[8] = ui->digitButton_8;
    digitButtons[9] = ui->digitButton_9;

    for (int i = 0; i < NumDigitButtons; ++i)
        connect(digitButtons[i], SIGNAL(clicked()), this, SLOT(digitClicked()));

    connect(ui->pointButton, SIGNAL(clicked()), this, SLOT(pointClicked()));
    connect(ui->changeSignButton, SIGNAL(clicked()), this, SLOT(changeSignClicked()));

    connect(ui->backspaceButton, SIGNAL(clicked()), this, SLOT(backspaceClicked()));
    connect(ui->clearButton, SIGNAL(clicked()), this, SLOT(clear()));
    connect(ui->clearAllButton, SIGNAL(clicked()), this, SLOT(clearAll()));

    connect(ui->clearMemoryButton, SIGNAL(clicked()), this, SLOT(clearMemory()));
    connect(ui->readMemoryButton, SIGNAL(clicked()), this, SLOT(readMemory()));
    connect(ui->setMemoryButton, SIGNAL(clicked()), this, SLOT(setMemory()));
    connect(ui->addToMemoryButton, SIGNAL(clicked()), this, SLOT(addToMemory()));

    connect(ui->divisionButton, SIGNAL(clicked()), this, SLOT(multiplicativeOperatorClicked()));
    connect(ui->timesButton, SIGNAL(clicked()), this, SLOT(multiplicativeOperatorClicked()));
    connect(ui->minusButton, SIGNAL(clicked()), this, SLOT(additiveOperatorClicked()));
    connect(ui->plusButton, SIGNAL(clicked()), this, SLOT(additiveOperatorClicked()));

    connect(ui->squareRootButton, SIGNAL(clicked()), this, SLOT(unaryOperatorClicked()));
    connect(ui->powerButton, SIGNAL(clicked()), this, SLOT(unaryOperatorClicked()));
    connect(ui->reciprocalButton, SIGNAL(clicked()), this, SLOT(unaryOperatorClicked()));
    connect(ui->equalButton, SIGNAL(clicked()), this, SLOT(equalClicked()));

    ui->lineEdit->setText(QString("0"));

//    QFont font = ui->lineEdit->font();
//    font.setPointSize(font.pointSize() + 8);
//    ui->lineEdit->setFont(font);

    setWindowTitle(tr("Calculator"));
}

Calculator::~Calculator()
{
    delete ui;
}

void Calculator::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);

    qDebug() << endl << "Window Resize:" << this->size();

    // 屏幕长宽
    QList<QScreen*> scr = QGuiApplication::screens();
    QRect rect = scr.at(0)->geometry();
    quint32 desktop_wid = rect.width();
    quint32 desktop_hei = rect.height();

    // 根据不同屏幕分辨率, 调节字体大小
    QString style_str;
    quint32 font_size = 10;
    if (desktop_wid >= 1280 && desktop_hei >= 720) {
        qDebug() << "1280*720";
        font_size = 46;
        style_str = QString("QWidget { font: %1pt \"WenQuanYi Micro Hei Mono\"; }").arg(font_size);
        ui->lineEdit->setStyleSheet(QString("font: 100pt \"WenQuanYi Micro Hei Mono\";"));
    } else if (desktop_wid >= 800 && desktop_hei >= 600) {
        qDebug() << "800*600";
        font_size = 12;
        style_str = QString("QWidget { font: 10pt \"WenQuanYi Micro Hei Mono\"; }").arg(font_size);
        ui->lineEdit->setStyleSheet(QString("font: 16pt \"WenQuanYi Micro Hei Mono\";"));
    }

    this->setStyleSheet(style_str);
}

void Calculator::digitClicked()
{
    QPushButton *clickedButton = qobject_cast<QPushButton *>(sender());
    int digitValue = clickedButton->text().toInt();
    if (ui->lineEdit->text() == "0" && digitValue == 0.0)
        return;

    if (waitingForOperand) {
        ui->lineEdit->clear();
        waitingForOperand = false;
    }
    ui->lineEdit->setText(ui->lineEdit->text() + QString::number(digitValue));
}

void Calculator::unaryOperatorClicked()
{
    QPushButton *clickedButton = qobject_cast<QPushButton *>(sender());
    QString clickedOperator = clickedButton->text();
    double operand = ui->lineEdit->text().toDouble();
    double result = 0.0;

    if (clickedOperator == tr("Sqrt")) {
        if (operand < 0.0) {
            abortOperation();
            return;
        }
        result = std::sqrt(operand);
    } else if (clickedOperator == tr("x\302\262")) {
        result = std::pow(operand, 2.0);
    } else if (clickedOperator == tr("1/x")) {
        if (operand == 0.0) {
            abortOperation();
            return;
        }
        result = 1.0 / operand;
    }
    ui->lineEdit->setText(QString::number(result));
    waitingForOperand = true;
}

void Calculator::additiveOperatorClicked()
{
    QPushButton *clickedButton = qobject_cast<QPushButton *>(sender());
    if (!clickedButton)
      return;
    QString clickedOperator = clickedButton->text();
    double operand = ui->lineEdit->text().toDouble();

    if (!pendingMultiplicativeOperator.isEmpty()) {
        if (!calculate(operand, pendingMultiplicativeOperator)) {
            abortOperation();
            return;
        }
        ui->lineEdit->setText(QString::number(factorSoFar));
        operand = factorSoFar;
        factorSoFar = 0.0;
        pendingMultiplicativeOperator.clear();
    }

    if (!pendingAdditiveOperator.isEmpty()) {
        if (!calculate(operand, pendingAdditiveOperator)) {
            abortOperation();
            return;
        }
        ui->lineEdit->setText(QString::number(sumSoFar));
    } else {
        sumSoFar = operand;
    }

    pendingAdditiveOperator = clickedOperator;
    waitingForOperand = true;
}

void Calculator::multiplicativeOperatorClicked()
{
    QPushButton *clickedButton = qobject_cast<QPushButton *>(sender());
    if (!clickedButton)
      return;
    QString clickedOperator = clickedButton->text();
    double operand = ui->lineEdit->text().toDouble();

    if (!pendingMultiplicativeOperator.isEmpty()) {
        if (!calculate(operand, pendingMultiplicativeOperator)) {
            abortOperation();
            return;
        }
        ui->lineEdit->setText(QString::number(factorSoFar));
    } else {
        factorSoFar = operand;
    }

    pendingMultiplicativeOperator = clickedOperator;
    waitingForOperand = true;
}

void Calculator::equalClicked()
{
    double operand = ui->lineEdit->text().toDouble();

    if (!pendingMultiplicativeOperator.isEmpty()) {
        if (!calculate(operand, pendingMultiplicativeOperator)) {
            abortOperation();
            return;
        }
        operand = factorSoFar;
        factorSoFar = 0.0;
        pendingMultiplicativeOperator.clear();
    }
    if (!pendingAdditiveOperator.isEmpty()) {
        if (!calculate(operand, pendingAdditiveOperator)) {
            abortOperation();
            return;
        }
        pendingAdditiveOperator.clear();
    } else {
        sumSoFar = operand;
    }

    ui->lineEdit->setText(QString::number(sumSoFar));
    sumSoFar = 0.0;
    waitingForOperand = true;
}

void Calculator::pointClicked()
{
    if (waitingForOperand)
        ui->lineEdit->setText("0");
    if (!ui->lineEdit->text().contains('.'))
        ui->lineEdit->setText(ui->lineEdit->text() + tr("."));
    waitingForOperand = false;
}

void Calculator::changeSignClicked()
{
    QString text = ui->lineEdit->text();
    double value = text.toDouble();

    if (value > 0.0) {
        text.prepend(tr("-"));
    } else if (value < 0.0) {
        text.remove(0, 1);
    }
    ui->lineEdit->setText(text);
}

void Calculator::backspaceClicked()
{
    if (waitingForOperand)
        return;

    QString text = ui->lineEdit->text();
    text.chop(1);
    if (text.isEmpty()) {
        text = "0";
        waitingForOperand = true;
    }
    ui->lineEdit->setText(text);
}

void Calculator::clear()
{
    if (waitingForOperand)
        return;

    ui->lineEdit->setText("0");
    waitingForOperand = true;
}

void Calculator::clearAll()
{
    sumSoFar = 0.0;
    factorSoFar = 0.0;
    pendingAdditiveOperator.clear();
    pendingMultiplicativeOperator.clear();
    ui->lineEdit->setText("0");
    waitingForOperand = true;
}

void Calculator::clearMemory()
{
    sumInMemory = 0.0;
}

void Calculator::readMemory()
{
    ui->lineEdit->setText(QString::number(sumInMemory));
    waitingForOperand = true;
}

void Calculator::setMemory()
{
    equalClicked();
    sumInMemory = ui->lineEdit->text().toDouble();
}

void Calculator::addToMemory()
{
    equalClicked();
    sumInMemory += ui->lineEdit->text().toDouble();
}

void Calculator::abortOperation()
{
    clearAll();
    ui->lineEdit->setText(tr("####"));
}

bool Calculator::calculate(double rightOperand, const QString &pendingOperator)
{
    if (pendingOperator == tr("+")) {
        sumSoFar += rightOperand;
    } else if (pendingOperator == tr("-")) {
        sumSoFar -= rightOperand;
    } else if (pendingOperator == tr("\303\227")) {
        factorSoFar *= rightOperand;
    } else if (pendingOperator == tr("\303\267")) {
        if (rightOperand == 0.0)
            return false;
        factorSoFar /= rightOperand;
    }
    return true;
}

void Calculator::on_pushButton_exit_clicked()
{
    close();
}
