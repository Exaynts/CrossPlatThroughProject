#include "filecontrolwidget.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>

FileControlWidget::FileControlWidget(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    QHBoxLayout *btnLayout = new QHBoxLayout();
    QPushButton *saveBtn = new QPushButton("💾 Сохранить", this);
    QPushButton *loadBtn = new QPushButton("📂 Загрузить", this);

    btnLayout->addWidget(saveBtn);
    btnLayout->addWidget(loadBtn);
    mainLayout->addLayout(btnLayout);

    m_statusLabel = new QLabel("", this);
    m_statusLabel->setAlignment(Qt::AlignCenter);
    m_statusLabel->setStyleSheet("color: gray; font-size: 10px;");
    mainLayout->addWidget(m_statusLabel);

    connect(saveBtn, &QPushButton::clicked, this, &FileControlWidget::saveRequested);
    connect(loadBtn, &QPushButton::clicked, this, &FileControlWidget::loadRequested);
}

void FileControlWidget::setStatus(const QString &text)
{
    m_statusLabel->setStyleSheet("color: gray; font-size: 10px;");
    m_statusLabel->setText(text);
}

void FileControlWidget::setStatusError(const QString &text)
{
    m_statusLabel->setStyleSheet("color: red; font-size: 10px;");
    m_statusLabel->setText(text);
}
