#include "mealwidget.h"
#include "meal.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLineEdit>
#include <QSpinBox>
#include <QListWidget>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>

MealWidget::MealWidget(Meal *meal, QWidget *parent)
    : QGroupBox(meal->name(), parent)
    , m_meal(meal)
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    // Форма ввода блюда
    QFormLayout *form = new QFormLayout();
    m_nameEdit = new QLineEdit(this);
    m_nameEdit->setPlaceholderText("Название блюда");
    m_caloriesSpin = new QSpinBox(this);
    m_caloriesSpin->setRange(0, 5000);
    m_caloriesSpin->setSuffix(" ккал");
    form->addRow("Блюдо:", m_nameEdit.data());
    form->addRow("Калории:", m_caloriesSpin.data());
    layout->addLayout(form);

    // Кнопки работы с блюдами
    QHBoxLayout *btnLayout = new QHBoxLayout();
    QPushButton *addBtn = new QPushButton("Добавить", this);
    QPushButton *delDishBtn = new QPushButton("Удалить блюдо", this);
    btnLayout->addWidget(addBtn);
    btnLayout->addWidget(delDishBtn);
    layout->addLayout(btnLayout);

    // Список блюд
    m_list = new QListWidget(this);
    layout->addWidget(m_list.data());

    // Итог по приёму пищи
    m_totalLabel = new QLabel("Итого: 0 ккал", this);
    layout->addWidget(m_totalLabel.data());

    // Кнопка удаления всего приёма пищи
    QPushButton *delMealBtn = new QPushButton("Удалить приём пищи", this);
    delMealBtn->setStyleSheet("color: darkred;");
    layout->addWidget(delMealBtn);

    // Связки
    connect(addBtn, &QPushButton::clicked, this, &MealWidget::onAddDish);
    connect(delDishBtn, &QPushButton::clicked, this, &MealWidget::onDeleteDish);
    connect(delMealBtn, &QPushButton::clicked, this, &MealWidget::onDeleteMealClicked);

    connect(m_meal, &Meal::dishAdded, this, [this](const QString &, double) { refreshList(); });
    connect(m_meal, &Meal::dishRemoved, this, [this](int) { refreshList(); });
    connect(m_meal, &Meal::cleared, this, [this]() { refreshList(); });
    connect(m_meal, &Meal::totalChanged, this, &MealWidget::updateTotal);

    // Двойной клик по блюду — быстрое удаление
    connect(m_list.data(), &QListWidget::itemDoubleClicked, this, &MealWidget::onDeleteDish);

    refreshList();
}

void MealWidget::onAddDish()
{
    QString name = m_nameEdit->text().trimmed();
    int cal = m_caloriesSpin->value();
    if (name.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите название блюда!");
        return;
    }
    if (cal <= 0) {
        QMessageBox::warning(this, "Ошибка", "Калорийность должна быть больше 0!");
        return;
    }
    m_meal->addDish(name, cal);
    m_nameEdit->clear();
    m_caloriesSpin->setValue(0);
    m_nameEdit->setFocus();
}

void MealWidget::onDeleteDish()
{
    int row = m_list->currentRow();
    if (row < 0) {
        QMessageBox::information(this, "Удаление",
                                 "Выберите блюдо в списке для удаления.");
        return;
    }
    QString dishName = m_meal->dishes().at(row).first;
    auto answer = QMessageBox::question(this, "Удаление блюда",
                                        QString("Удалить блюдо \"%1\"?").arg(dishName),
                                        QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (answer == QMessageBox::Yes) {
        m_meal->removeDish(row);
    }
}

void MealWidget::onDeleteMealClicked()
{
    auto answer = QMessageBox::question(this, "Удаление приёма пищи",
                                        QString("Удалить приём пищи \"%1\" и все его блюда?").arg(m_meal->name()),
                                        QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (answer == QMessageBox::Yes) {
        emit deleteMealRequested(this);
    }
}

void MealWidget::refreshList()
{
    m_list->clear();
    for (const auto &dish : m_meal->dishes()) {
        m_list->addItem(QString("%1 — %2 ккал").arg(dish.first).arg(dish.second));
    }
}

void MealWidget::updateTotal(double total)
{
    m_totalLabel->setText(QString("Итого: %1 ккал").arg(total));
}
