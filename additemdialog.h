#ifndef ADDITEMDIALOG_H
#define ADDITEMDIALOG_H

#include <QDialog>
#include <QGraphicsScene>
#include <QHash>
#include <mainwindow.h>
#include <graphicsitems.h>

using Graphics::GraphicsWidget;
using interpreter::cstruct;
using interpreter::Interpreter;

namespace Ui {
    class AddItemDialog;
}

class AddItemDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddItemDialog(Interpreter& inter
                           , std::map<std::string, cstruct> const& structs
                           , QWidget *parent);
    ~AddItemDialog();

private slots:
    void on_buttonBox_accepted();
    void on_typeComboBox_activated(int index);
    void on_structTypeComboBox_activated(const QString &arg1);

private:
    std::map<std::string, cstruct> const& structs;
    QString str;
    Ui::AddItemDialog * ui;
    Interpreter& inter;
};

#endif // ADDITEMDIALOG_H
