#ifndef ADDITEMDIALOG_H
#define ADDITEMDIALOG_H

#include <QDialog>
#include <QGraphicsScene>
#include <mainwindow.h>
#include <datatype.h>

namespace Ui {
    class AddItemDialog;
}

class AddItemDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddItemDialog(QGraphicsScene *scene, QList<DataType*> *items, QStringList *types, QWidget *parent = 0);
    ~AddItemDialog();

private slots:
    void on_buttonBox_accepted();

    void on_typeComboBox_activated(int index);

private:
    Ui::AddItemDialog *ui;
    QGraphicsScene *scene;
    QList<DataType*> *items;
    QStringList *types;
};

#endif // ADDITEMDIALOG_H
