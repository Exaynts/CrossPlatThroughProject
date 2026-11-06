#ifndef FILECONTROLWIDGET_H
#define FILECONTROLWIDGET_H

#include <QWidget>

class QLabel;

class FileControlWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FileControlWidget(QWidget *parent = nullptr);

public slots:
    void setStatus(const QString &text);
    void setStatusError(const QString &text);

signals:
    void saveRequested();
    void loadRequested();

private:
    QLabel *m_statusLabel;
};

#endif // FILECONTROLWIDGET_H
