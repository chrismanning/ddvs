#include "additemdialog.h"
#include "ui_additemdialog.h"

AddItemDialog::AddItemDialog(Interpreter& inter, std::map<std::string, cstruct> const& structs, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddItemDialog),
    structs(structs),
    inter(inter)
{
    ui->setupUi(this);

    layout()->setSizeConstraint(QLayout::SetFixedSize);

    ui->userTypeGroupBox->hide();
    ui->simpleTypeGroupBox->hide();
    ui->pointerGroupBox->hide();
    //ui->buttonBox->setEnabled(false);
}

AddItemDialog::~AddItemDialog()
{
    delete ui;
    qDebug("Closing dialog...");
}

void AddItemDialog::on_buttonBox_accepted()
{
    //type_name = ui->typeComboBox->currentText();
//    GraphicsWidget *item;
    if(ui->typeComboBox->currentIndex() == 1) {
        str = "int ";
        str += ui->nameText->text();
        if(ui->valueText->text().length()) {
            str += " = " + ui->valueText->text();
        }
        str += ";";
    }
    else if(ui->typeComboBox->currentIndex() == 2) {
        str = "struct " + ui->structTypeComboBox->currentText();
        str += " " + ui->structNameText->text() + ";\n";
        for(int i=0; i < ui->structMembersWidget->rowCount(); i++) {
            str += ui->structNameText->text() + ".";
            str += ui->structMembersWidget->item(i,0)->text();
            str += " = " + ui->structMembersWidget->item(i,1)->text() + ";\n";
        }
    }
    else if(ui->typeComboBox->currentIndex() == 3) {
        str = ui->pointerTypeText->text() + " * ";
        str += ui->pointerNameText->text();
        if(ui->pointerValueText->text().length()) {
            str += " = " + ui->pointerValueText->text();
        }
        str += ";";
    }
    else {
        return;
    }
    if(inter.parse(str)) {
        inter.execute();
        dynamic_cast<MainWindow*>(parent())->updateAll();
        this->close();
    }
//    else
//        item = new Variable(typeName, ui->nameText->displayText(), ui->valueText->displayText());
//    items->insert(item->name, item);
//    scene->addItem(item);
}

void AddItemDialog::on_typeComboBox_activated(int index)
{
    ui->userTypeGroupBox->hide();
    ui->simpleTypeGroupBox->hide();
    ui->pointerGroupBox->hide();

    switch(index)
    {
        case 0://none
            break;
        case 1:
            ui->simpleTypeGroupBox->setVisible(true);
            break;
        case 2:{
            int i = 0;
            for(auto const& sn : structs) {
                ui->structTypeComboBox->insertItem(i, QString::fromStdString(sn.first));
                ++i;
            }
            ui->userTypeGroupBox->setVisible(true);
        }
            break;
        case 3:
            ui->pointerGroupBox->setVisible(true);
            break;
        default:
            break;
    }
}

void AddItemDialog::on_structTypeComboBox_activated(const QString &arg1)
{
    std::string str = arg1.toStdString();
    cstruct const* cs;
    auto i = structs.find(str);
    if(i != structs.end()) {
        cs = &(i->second);
    }
    QStringList labels;
    for(auto member : cs->member_specs) {
        const int j = ui->structMembersWidget->rowCount();
        ui->structMembersWidget->insertRow(ui->structMembersWidget->rowCount());
        QTableWidgetItem* name = new QTableWidgetItem(QString::fromStdString(member.first));
        name->setFlags(name->flags() &= ~Qt::ItemIsEditable);
        ui->structMembersWidget->setItem(j, 0, name);
        QTableWidgetItem* value;
        value = new QTableWidgetItem(QString::number(0));
        ui->structMembersWidget->setItem(j, 1, value);

        labels << QString::fromStdString(member.second.type_str);
    }
    ui->structMembersWidget->setVerticalHeaderLabels(labels);
}
