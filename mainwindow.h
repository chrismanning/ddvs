#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsItem>
#include <QSlider>
#include <QLabel>
#include <additemdialog.h>
#include <zoomwidget.h>
#include <datatype.h>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionAdd_Item_triggered();
    void on_actionRemove_Item_triggered();

private:
    QList<DataType*> items;
    QStringList types;
    Ui::MainWindow *ui;
    QGraphicsScene *scene;
    ZoomWidget *zoomer;
    QSlider *zoomSlider;
    QLabel *zoomLabel;
};

#endif // MAINWINDOW_H
