#include "diarysummarywidget.h"
#include <QVBoxLayout>
#include <QLabel>

DiarySummaryWidget::DiarySummaryWidget(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 10, 0, 10);

    // Панель итого
    m_totalLabel = new QLabel("Итого за день: 0 ккал", this);
    m_totalLabel->setAlignment(Qt::AlignCenter);
    QFont tf = m_totalLabel->font();
    tf.setBold(true);
    tf.setPointSize(12);
    m_totalLabel->setFont(tf);
    layout->addWidget(m_totalLabel);

    // Панель статуса (недобор/перебор)
    m_statusLabel = new QLabel("", this);
    m_statusLabel->setAlignment(Qt::AlignCenter);
    m_statusLabel->setWordWrap(true);
    layout->addWidget(m_statusLabel);
}

void DiarySummaryWidget::updateTotal(double total)
{
    m_totalLabel->setText(QString("Итого за день: %1 ккал").arg(total));
}

void DiarySummaryWidget::updateStatus(NutritionDiary::NormStatus status, double total, double minNorm, double maxNorm)
{
    switch (status) {
    case NutritionDiary::BelowNorm:
        m_statusLabel->setText(QString("⚠ Недобор: %1 ккал").arg(minNorm - total, 0, 'f', 0));
        m_statusLabel->setStyleSheet("color: orange; font-weight: bold;");
        break;
    case NutritionDiary::InRange:
        m_statusLabel->setText("✓ В пределах нормы");
        m_statusLabel->setStyleSheet("color: green; font-weight: bold;");
        break;
    case NutritionDiary::AboveNorm:
        m_statusLabel->setText(QString("⚠ Перебор: %1 ккал").arg(total - maxNorm, 0, 'f', 0));
        m_statusLabel->setStyleSheet("color: red; font-weight: bold;");
        break;
    }
}
