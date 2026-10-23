#ifndef NORMWIDGET_H
#define NORMWIDGET_H

#include <QWidget>

class QSpinBox;

class NormWidget : public QWidget
{
    Q_OBJECT
public:
    explicit NormWidget(int minNorm, int maxNorm, QWidget *parent = nullptr);

    int minNorm() const;
    int maxNorm() const;

signals:
    void minNormChanged(int value);
    void maxNormChanged(int value);

private slots:
    void onMinSpinChanged(int value);
    void onMaxSpinChanged(int value);

private:
    QSpinBox *m_minSpin;
    QSpinBox *m_maxSpin;
};

#endif // NORMWIDGET_H
