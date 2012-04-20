#include "edititemdialog.h"
#include "ui_edititemdialog.h"

EditItemDialog::EditItemDialog(int& d,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditItemDialog),
    d(d)
{
    ui->setupUi(this);
}

EditItemDialog::~EditItemDialog()
{
    delete ui;
}

void EditItemDialog::on_buttonBox_accepted()
{
    d = ui->lineEdit->text().toInt();
}
