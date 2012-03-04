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
#include <QSplitter>
#include <QVBoxLayout>
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
    //debugging buttons
#ifdef QT_DEBUG
    void on_printStackButton_clicked();
    void on_printCodeButton_clicked();
    void on_printVarsButton_clicked();
#endif

private:
    QHash<QString,DataType*> items;
    Ui::MainWindow* ui;
    QGraphicsScene* scene;
    QGraphicsView* graphicsView;
    ZoomWidget* zoomer;
    QPlainTextEdit* interpreterInput;
    QPushButton* interpretButton;
#ifdef QT_DEBUG
    QPushButton* printStackButton;
    QPushButton* printCodeButton;
    QPushButton* printVarsButton;
#endif
    QSplitter* mainSplitter;
    QSplitter* rightSplitter;
    interpreter::Interpreter* interpreter;
};

#endif // MAINWINDOW_H
