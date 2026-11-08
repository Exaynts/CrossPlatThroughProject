#include "dishdelegate.h"
#include "dishmodel.h"
#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QHBoxLayout>
#include <QPainter>
#include <QApplication>
#include <QStyle>

DishDelegate::DishDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

QWidget *DishDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &/*option*/, const QModelIndex &/*index*/) const
{
    // Создаем контейнер для двух полей ввода
    QWidget *editor = new QWidget(parent);
    QHBoxLayout *layout = new QHBoxLayout(editor);
    layout->setContentsMargins(0, 0, 0, 0);

    QLineEdit *nameEdit = new QLineEdit(editor);
    nameEdit->setPlaceholderText("Название");

    QDoubleSpinBox *caloriesSpin = new QDoubleSpinBox(editor);
    caloriesSpin->setRange(0, 5000);
    caloriesSpin->setSuffix(" ккал");

    layout->addWidget(nameEdit, 1); // Название тянется
    layout->addWidget(caloriesSpin); // Калории фиксированы

    return editor;
}

void DishDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    // Достаем данные из модели по нашим кастомным ролям
    QString name = index.data(DishModel::NameRole).toString();
    double calories = index.data(DishModel::CaloriesRole).toDouble();

    // Находим поля внутри нашего виджета-редактора и заполняем их
    QLineEdit *nameEdit = editor->findChild<QLineEdit*>();
    QDoubleSpinBox *caloriesSpin = editor->findChild<QDoubleSpinBox*>();

    if (nameEdit) nameEdit->setText(name);
    if (caloriesSpin) caloriesSpin->setValue(calories);
}

void DishDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    // Забираем данные из полей
    QLineEdit *nameEdit = editor->findChild<QLineEdit*>();
    QDoubleSpinBox *caloriesSpin = editor->findChild<QDoubleSpinBox*>();

    if (nameEdit && caloriesSpin) {
        // Записываем обратно в модель (используя наши роли)
        model->setData(index, nameEdit->text(), DishModel::NameRole);
        model->setData(index, caloriesSpin->value(), DishModel::CaloriesRole);
    }
}

void DishDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &/*index*/) const
{
    editor->setGeometry(option.rect);
}

void DishDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    // 1. Рисуем фон (выделение, если выбрано)
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);

    // Рисуем стандартный фон через стиль приложения
    QApplication::style()->drawPrimitive(QStyle::PE_PanelItemViewItem, &opt, painter);

    // 2. Извлекаем данные
    QString name = index.data(DishModel::NameRole).toString();
    double calories = index.data(DishModel::CaloriesRole).toDouble();
    QString caloriesText = QString::number(calories) + " ккал";

    // 3. Сохраняем состояние painter (обязательно!)
    painter->save();

    // Настройка шрифта
    QFont font = painter->font();
    font.setBold(true);
    painter->setFont(font);

    // Цвет текста (белый на синем фоне выделения, черный в обычном состоянии)
    if (option.state & QStyle::State_Selected) {
        painter->setPen(option.palette.color(QPalette::HighlightedText));
    } else {
        painter->setPen(option.palette.color(QPalette::Text));
    }

    // 4. Рисуем название (слева)
    QRect textRect = option.rect.adjusted(10, 0, -100, 0); // Оставляем место справа
    painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, name);

    // 5. Рисуем калории (справа)
    // Сбрасываем жирность шрифта для калорий
    font.setBold(false);
    painter->setFont(font);

    QRect calRect = option.rect.adjusted(0, 0, -10, 0); // Отступ справа
    painter->drawText(calRect, Qt::AlignRight | Qt::AlignVCenter, caloriesText);

    // 6. Восстанавливаем состояние painter (обязательно!)
    painter->restore();
}
