#include "edititemdialog.h"
#include "ui_edititemdialog.h"

EditItemDialog::EditItemDialog(std::string const& name, Interpreter& inter,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditItemDialog),
    inter(inter),
    name(name)
{
    ui->setupUi(this);
}

EditItemDialog::~EditItemDialog()
{
    delete ui;
}

void EditItemDialog::on_buttonBox_accepted()
{
    QString str = QString::fromStdString(name) + " = " + ui->lineEdit->text() + ";";
    if(inter.parse(str)) {
        inter.execute();
        dynamic_cast<MainWindow*>(parent())->updateAll();
        this->close();
    }
    else {
        return;
    }
}
