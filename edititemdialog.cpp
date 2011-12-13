#include "edititemdialog.h"
#include "ui_edititemdialog.h"

EditItemDialog::EditItemDialog(DataType *d,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditItemDialog)
{
    ui->setupUi(this);

    item = d;
}

EditItemDialog::~EditItemDialog()
{
    delete ui;
}

void EditItemDialog::on_buttonBox_accepted()
{
    item->value = ui->lineEdit->displayText();
}
