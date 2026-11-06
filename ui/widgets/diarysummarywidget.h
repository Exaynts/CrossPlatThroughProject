#ifndef DIARYSUMMARYWIDGET_H
#define DIARYSUMMARYWIDGET_H

#include <QWidget>
#include "nutritiondiary.h"

class QLabel;

class DiarySummaryWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DiarySummaryWidget(QWidget *parent = nullptr);

public slots:
    void updateTotal(double total);
    void updateStatus(NutritionDiary::NormStatus status, double total, double minNorm, double maxNorm);

private:
    QLabel *m_totalLabel;
    QLabel *m_statusLabel;
};

#endif // DIARYSUMMARYWIDGET_H
