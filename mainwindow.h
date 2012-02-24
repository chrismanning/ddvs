#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsItem>
#include <QPlainTextEdit>
#include <QSlider>
#include <QLabel>
#include <QHash>
#include <additemdialog.h>
#include <edititemdialog.h>
#include <zoomwidget.h>
#include <datatype.h>
#include <boost/spirit/include/qi.hpp>
#include <interpreter.h>

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
    void on_actionEdit_Item_triggered();

    void on_interpretButton_clicked();

    void on_printStackButton_clicked();

    void on_printCodeButton_clicked();

    void on_printVarsButton_clicked();

private:
    QHash<QString,DataType*> items;
    QStringList types;
    Ui::MainWindow *ui;
    QGraphicsScene *scene;
    ZoomWidget *zoomer;
    QSlider *zoomSlider;
    QLabel *zoomLabel;
    interpreter::Interpreter *interpreter;
};

#endif // MAINWINDOW_H
