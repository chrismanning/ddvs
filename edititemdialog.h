#ifndef EDITITEMDIALOG_H
#define EDITITEMDIALOG_H

#include <QDialog>
#include <datatype.h>

namespace Ui {
    class EditItemDialog;
}

class EditItemDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditItemDialog(DataType *d,QWidget *parent = 0);
    ~EditItemDialog();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::EditItemDialog *ui;
    DataType *item;
};

#endif // EDITITEMDIALOG_H
