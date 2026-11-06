#include "normwidget.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QSpinBox>


NormWidget::NormWidget(int minNorm, int maxNorm, QWidget *parent)
    : QWidget(parent)
{
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0); // Убираем лишние отступы

    layout->addWidget(new QLabel("Норма от:"));

    m_minSpin = new QSpinBox(this);
    m_minSpin->setRange(0, 10000);
    m_minSpin->setValue(minNorm);
    m_minSpin->setSuffix(" ккал");
    layout->addWidget(m_minSpin);

    layout->addWidget(new QLabel("до:"));

    m_maxSpin = new QSpinBox(this);
    m_maxSpin->setRange(0, 10000);
    m_maxSpin->setValue(maxNorm);
    m_maxSpin->setSuffix(" ккал");
    layout->addWidget(m_maxSpin);

    layout->addStretch();

    connect(m_minSpin, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &NormWidget::onMinSpinChanged);
    connect(m_maxSpin, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &NormWidget::onMaxSpinChanged);
}

int NormWidget::minNorm() const { return m_minSpin->value(); }
int NormWidget::maxNorm() const { return m_maxSpin->value(); }

void NormWidget::onMinSpinChanged(int value)
{
    // Если нижняя граница стала выше верхней, двигаем верхнюю
    if (value > m_maxSpin->value()) {
        m_maxSpin->setValue(value);
    }
    emit minNormChanged(value);
}

void NormWidget::onMaxSpinChanged(int value)
{
    // Если верхняя граница стала ниже нижней, двигаем нижнюю
    if (value < m_minSpin->value()) {
        m_minSpin->setValue(value);
    }
    emit maxNormChanged(value);
}

void NormWidget::setDiary(NutritionDiary *newDiary)
{
    if (m_diary == newDiary) return;
    m_diary = newDiary;

    if (!m_diary) return;

    // Блокируем сигналы, чтобы при программном изменении значений
    // они не улетали обратно в модель и не вызывали зацикливание
    const QSignalBlocker blockerMin(m_minSpin);
    const QSignalBlocker blockerMax(m_maxSpin);

    m_minSpin->setValue(static_cast<int>(m_diary->minNorm()));
    m_maxSpin->setValue(static_cast<int>(m_diary->maxNorm()));
}
