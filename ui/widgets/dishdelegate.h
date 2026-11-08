#ifndef DISHDELEGATE_H
#define DISHDELEGATE_H

#include <QStyledItemDelegate>

class QLineEdit;
class QDoubleSpinBox;

class DishDelegate : public QStyledItemDelegate
{
    Q_OBJECT // Не обязателен здесь, но можно оставить для будущего
public:
    explicit DishDelegate(QObject *parent = nullptr);

    // Создание редактора (два поля: текст и число)
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    // Заполнение редактора данными из модели
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;

    // Сохранение данных из редактора обратно в модель
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;

    // Позиционирование редактора поверх ячейки
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    // Кастомная отрисовка (название слева, калории справа)
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

#endif // DISHDELEGATE_H
