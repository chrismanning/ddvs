#include "mainwindow.h"
#include "ui_mainwindow.h"
#ifndef QT_NO_OPENGL
#include <QtOpenGL>
#endif

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    zoomer = new ZoomWidget(ui->graphicsView);
    ui->statusBar->addWidget(zoomer);

    scene = new QGraphicsScene(0, 0, 600, 600);
    ui->graphicsView->setScene(scene);
    #ifndef QT_NO_OPENGL
    //ui->graphicsView->setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));
    #endif

    types << "int" << "int*";
}

MainWindow::~MainWindow()
{
    delete scene;
    delete zoomer;
    delete ui;
}

void MainWindow::on_actionAdd_Item_triggered()
{
    qDebug("Adding item...");
    AddItemDialog *dialog = new AddItemDialog(scene, &items, &types);
    dialog->setAttribute(Qt::WA_DeleteOnClose); //make it free its memory on close
    dialog->show();
}

void MainWindow::on_actionRemove_Item_triggered()
{
    QList<QGraphicsItem*> items = scene->selectedItems();
    foreach(QGraphicsItem *item,items) {
        scene->removeItem(item);
        this->items.removeAll((DataType*)item);
    }
    qDebug("Removing %d items...",items.size());
    qDeleteAll(items);
}
