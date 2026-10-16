#include "calculator.h"
#include <QDebug>
#include <QtMath>

Calculator::Calculator(QObject *parent)
    : QObject(parent)
    , m_result(0.0)
    , m_hasError(false)
{
    qDebug() << "[CREATE] Calculator";
}

void Calculator::setResult(double value)
{
    m_result = value;
    m_hasError = false;
    m_errorMessage.clear();
    emit resultReady(m_result);
}

void Calculator::add(double a, double b)
{
    qDebug() << "[CALC] add(" << a << ", " << b << ")";
    setResult(a + b);
}

void Calculator::subtract(double a, double b)
{
    qDebug() << "[CALC] subtract(" << a << ", " << b << ")";
    setResult(a - b);
}

void Calculator::multiply(double a, double b)
{
    qDebug() << "[CALC] multiply(" << a << ", " << b << ")";
    setResult(a * b);
}

void Calculator::divide(double a, double b)
{
    qDebug() << "[CALC] divide(" << a << ", " << b << ")";
    if (qFuzzyIsNull(b)) {
        m_hasError = true;
        m_errorMessage = "Деление на ноль невозможно!";
        emit errorOccurred(m_errorMessage);
        return;
    }
    setResult(a / b);
}

void Calculator::reset()
{
    qDebug() << "[CALC] reset()";
    m_result = 0.0;
    m_hasError = false;
    m_errorMessage.clear();
    emit resultReady(m_result);
}
