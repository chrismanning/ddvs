#ifndef NEWMEMBERSWIDGET_H
#define NEWMEMBERSWIDGET_H

#include <QWidget>
#include <QComboBox>
#include <QLayout>
#include <QStringList>
#include <additemdialog.h>

namespace Ui {
    class NewMembersWidget;
}

class NewMembersWidget : public QWidget
{
    Q_OBJECT

public:
    explicit NewMembersWidget(QWidget *parent = 0, QStringList * types = 0);
    ~NewMembersWidget();
    void setTypes(QStringList * types);

private slots:
    void on_addMemberButton_clicked();

    void on_removeMemberButton_clicked();

    void on_clearMembersButton_clicked();

private:
    Ui::NewMembersWidget *ui;
    QStringList *types;
};

#endif // NEWMEMBERSWIDGET_H
