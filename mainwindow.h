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
#include <QTabWidget>
#include <QTreeWidget>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QGraphicsGridLayout>
#include <graphicsview.h>
#include <additemdialog.h>
#include <edititemdialog.h>
#include <datatype.h>
#include <interpreter.h>
#include <graphicsitems.h>
#include <tuple>

namespace Ui {
    class MainWindow;
}

using interpreter::cstruct;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = 0);
    ~MainWindow();
    void updateAll();

private slots:
    void on_actionAdd_Item_triggered();
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
    void on_actionAbout_Qt_triggered();
    void on_actionRefresh_Visualisation_triggered();

public slots:
    void structDefined(cstruct const& s);
signals:
    void newStructDefinition(cstruct const& s);

private:
    QMessageBox msgbox;
    std::string const findByValue(const int value);
    void updateStackTable();
    void updateVariableTree();
    void updateVisualisation();
    std::map<std::string, cstruct> const& structs;
    QTabWidget* tabWidget;
    QTreeWidget* structTreeWidget;
    QTreeWidget* variableTreeWidget;
    QTableWidget* stackTableWidget;
    QHash<QString, Graphics::GraphicsWidget*> items;
    Ui::MainWindow* ui;
    QGraphicsScene* scene;
    QGraphicsGridLayout* layout;
    GraphicsView* graphicsView;
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

    int row, column;
};

#endif // MAINWINDOW_H
