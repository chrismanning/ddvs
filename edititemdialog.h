#ifndef EDITITEMDIALOG_H
#define EDITITEMDIALOG_H

#include <QDialog>
#include <graphicsitems.h>

namespace Ui {
    class EditItemDialog;
}

using Graphics::GraphicsWidget;

class EditItemDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditItemDialog(int& d,QWidget *parent = 0);
    ~EditItemDialog();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::EditItemDialog *ui;
    int& d;
};

#endif // EDITITEMDIALOG_H
