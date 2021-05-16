#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QGridLayout>
#include <QScreen>
#include <QResizeEvent>
#include <QGuiApplication>

#include <math.h>

QT_BEGIN_NAMESPACE
namespace Ui { class Calculator; }
QT_END_NAMESPACE

class Calculator : public QWidget
{
    Q_OBJECT

public:
    Calculator(QWidget *parent = nullptr);
    ~Calculator();

protected:
    void resizeEvent(QResizeEvent *event);

private slots:
    void digitClicked();
    void unaryOperatorClicked();
    void additiveOperatorClicked();
    void multiplicativeOperatorClicked();
    void equalClicked();
    void pointClicked();
    void changeSignClicked();
    void backspaceClicked();
    void clear();
    void clearAll();
    void clearMemory();
    void readMemory();
    void setMemory();
    void addToMemory();

    void on_pushButton_exit_clicked();

private:
    Ui::Calculator *ui;
    void abortOperation();
    bool calculate(double rightOperand, const QString &pendingOperator);

    double sumInMemory;
    double sumSoFar;
    double factorSoFar;
    QString pendingAdditiveOperator;
    QString pendingMultiplicativeOperator;
    bool waitingForOperand;

    QLineEdit *display;

    enum { NumDigitButtons = 10 };
    QPushButton *digitButtons[NumDigitButtons];
};

#endif // CALCULATOR_H
