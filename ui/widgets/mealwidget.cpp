#include "mealwidget.h"
#include "meal.h"
#include "dishmodel.h"
#include "dishdelegate.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QListView>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>
#include <QItemSelectionModel>

MealWidget::MealWidget(Meal *meal, QWidget *parent)
    : QGroupBox(meal->name(), parent)
    , m_meal(meal)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // 1. Строка ввода нового блюда
    QHBoxLayout *inputLayout = new QHBoxLayout();
    m_nameEdit = new QLineEdit(this);
    m_nameEdit->setPlaceholderText("Название блюда");

    m_caloriesSpin = new QDoubleSpinBox(this);
    m_caloriesSpin->setRange(0, 5000);
    m_caloriesSpin->setSuffix(" ккал");

    m_addBtn = new QPushButton("Добавить", this);

    inputLayout->addWidget(m_nameEdit, 1); // Поле имени растягивается
    inputLayout->addWidget(m_caloriesSpin);
    inputLayout->addWidget(m_addBtn);
    mainLayout->addLayout(inputLayout);

    // 2. Список блюд (Model-View)
    m_view = new QListView(this);
    m_view->setModel(m_meal->model()); // Привязываем модель
    m_view->setItemDelegate(new DishDelegate(m_view)); // Вешаем делегат
    m_view->setSelectionMode(QAbstractItemView::SingleSelection);
    m_view->setEditTriggers(QAbstractItemView::DoubleClicked); // Редактирование по двойному клику
    mainLayout->addWidget(m_view);

    // 3. Панель кнопок управления (Удалить, Очистить)
    QHBoxLayout *btnLayout = new QHBoxLayout();
    m_removeBtn = new QPushButton("Удалить блюдо", this);
    m_clearBtn = new QPushButton("Очистить", this);

    m_removeBtn->setEnabled(false); // Изначально ничего не выделено

    btnLayout->addWidget(m_removeBtn);
    btnLayout->addWidget(m_clearBtn);
    btnLayout->addStretch(); // Прижимаем влево
    mainLayout->addLayout(btnLayout);

    // 4. Итог по приему пищи
    m_totalLabel = new QLabel("Итого: 0 ккал", this);
    mainLayout->addWidget(m_totalLabel);

    // 5. Кнопка удаления всего приема пищи
    QPushButton *deleteMealBtn = new QPushButton("Удалить приём пищи", this);
    deleteMealBtn->setStyleSheet("color: darkred;");
    mainLayout->addWidget(deleteMealBtn);

    // --- Связки ---
    connect(m_addBtn, &QPushButton::clicked, this, &MealWidget::onAddDish);
    connect(m_removeBtn, &QPushButton::clicked, this, &MealWidget::onRemoveDish);
    connect(m_clearBtn, &QPushButton::clicked, this, &MealWidget::onClearDishes);
    connect(deleteMealBtn, &QPushButton::clicked, this, &MealWidget::onDeleteMealClicked);

    // Обновление состояния кнопки "Удалить" при изменении выделения
    connect(m_view->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &MealWidget::onSelectionChanged);

    // Обновление итога при изменении данных в модели
    connect(m_meal, &Meal::totalChanged, this, &MealWidget::updateTotal);

    // Начальное обновление
    updateTotal(m_meal->totalCalories());
}

void MealWidget::onAddDish()
{
    QString name = m_nameEdit->text().trimmed();
    double calories = m_caloriesSpin->value();

    if (name.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите название блюда!");
        return;
    }
    if (calories <= 0) {
        QMessageBox::warning(this, "Ошибка", "Калорийность должна быть больше 0!");
        return;
    }

    // Добавляем через модель (она сама испустит сигналы для обновления view)
    m_meal->addDish(name, calories);

    m_nameEdit->clear();
    m_caloriesSpin->setValue(0);
    m_nameEdit->setFocus();
}

void MealWidget::onRemoveDish()
{
    QModelIndex current = m_view->currentIndex();
    if (!current.isValid()) return;

    // Удаляем строку из модели
    m_meal->removeDish(current.row());
}

void MealWidget::onClearDishes()
{
    if (m_meal->model()->rowCount() == 0) return;

    auto answer = QMessageBox::question(this, "Очистка",
                                        QString("Удалить все блюда из приёма пищи \"%1\"?").arg(m_meal->name()),
                                        QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

    if (answer == QMessageBox::Yes) {
        m_meal->clear();
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

void MealWidget::onSelectionChanged()
{
    // Кнопка "Удалить блюдо" активна только если что-то выделено
    bool hasSelection = m_view->currentIndex().isValid();
    m_removeBtn->setEnabled(hasSelection);
}

void MealWidget::updateTotal(double total)
{
    m_totalLabel->setText(QString("Итого: %1 ккал").arg(total));
}
