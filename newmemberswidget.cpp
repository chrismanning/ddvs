#include "newmemberswidget.h"
#include "ui_newmemberswidget.h"

NewMembersWidget::NewMembersWidget(QWidget *parent, QStringList * types) :
    QWidget(parent),
    ui(new Ui::NewMembersWidget)
{
    ui->setupUi(this);

    this->types = types;
}

NewMembersWidget::~NewMembersWidget()
{
    delete ui;
}

void NewMembersWidget::setTypes(QStringList * types)
{
    this->types = types;
}

void NewMembersWidget::on_addMemberButton_clicked()
{
    ui->tableWidget->insertRow(ui->tableWidget->rowCount());
    if(types != 0)
    {
        QComboBox *typeBox = new QComboBox;
        typeBox->addItems(*types);
        ui->tableWidget->setCellWidget(ui->tableWidget->rowCount()-1,0,typeBox);
    }
}

void NewMembersWidget::on_removeMemberButton_clicked()
{
    //foreach(QTableWidgetItem * item)
}

void NewMembersWidget::on_clearMembersButton_clicked()
{
    if(ui->tableWidget->rowCount() > 0)
    {
        qDebug("rows: %d",ui->tableWidget->rowCount());
        for(int i=ui->tableWidget->rowCount()-1; i>=0; i--)
        {
            ui->tableWidget->removeRow(i);
        }
    }
}
