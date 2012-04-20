#ifndef EDITITEMDIALOG_H
#define EDITITEMDIALOG_H

#include <QDialog>
#include <mainwindow.h>
#include <interpreter.h>

namespace Ui {
    class EditItemDialog;
}

using interpreter::Interpreter;

class EditItemDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditItemDialog(std::string const& name, Interpreter& inter,QWidget *parent = 0);
    ~EditItemDialog();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::EditItemDialog *ui;
    Interpreter& inter;
    std::string const& name;
};

#endif // EDITITEMDIALOG_H
