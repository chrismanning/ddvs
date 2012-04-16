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
#include <graphicsview.h>
#include <additemdialog.h>
#include <edititemdialog.h>
#include <zoomwidget.h>
#include <datatype.h>
#include <interpreter.h>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = 0);
    ~MainWindow();

private slots:
    void on_actionAdd_Item_triggered();
    void on_actionRemove_Item_triggered();
    void on_actionEdit_Item_triggered();

    void interpretButton_clicked();
#ifdef QT_DEBUG
    //debugging buttons
    void printStackButton_clicked();
    void printOffsetButton_clicked();
    void printVarsButton_clicked();
#endif
    void on_actionZoom_In_triggered();
    void on_actionZoom_Out_triggered();

private:
    QHash<QString,DataType*> items;
    Ui::MainWindow* ui;
    QGraphicsScene* scene;
    GraphicsView* graphicsView;
    ZoomWidget* zoomer;
    QPlainTextEdit* interpreterInput;
    QPushButton* interpretButton;
#ifdef QT_DEBUG
    QPushButton* printStackButton;
    QPushButton* printOffsetButton;
    QPushButton* printVarsButton;
#endif
    QSplitter* mainSplitter;
    QSplitter* rightSplitter;
    interpreter::Interpreter interpreter;
};

#endif // MAINWINDOW_H
