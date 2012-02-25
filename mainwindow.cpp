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
    this->setWindowTitle("Dynamic Data Structure Visualisation");

    zoomer = new ZoomWidget(ui->graphicsView);
    ui->statusBar->addWidget(zoomer);

    scene = new QGraphicsScene(0, 0, 600, 600);
    ui->graphicsView->setScene(scene);
    #ifndef QT_NO_OPENGL
    //ui->graphicsView->setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));
    #endif

    interpreter = new interpreter::Interpreter;

    types << "int" << "*int";
}

MainWindow::~MainWindow()
{
    //delete scene;
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
        this->items.remove(((DataType*)item)->name);
    }
    qDebug("Removing %d items...",items.size());
    qDeleteAll(items);
}

void MainWindow::on_actionEdit_Item_triggered()
{
    if(scene->selectedItems().size() == 0) return;
    qDebug("Editing item...");
    EditItemDialog *dialog = new EditItemDialog((DataType*) scene->selectedItems().first());
    dialog->setAttribute(Qt::WA_DeleteOnClose); //make it free its memory on close
    dialog->show();
}

void MainWindow::on_interpretButton_clicked()
{
    QString tmp_str = ui->interpreterInput->toPlainText();
    if(interpreter->parse(tmp_str)) {
        ui->interpreterInput->clear();
        interpreter->execute();
        typedef std::map<std::string, int> varstype;
        varstype vars = interpreter->getGlobals();
        foreach(varstype::value_type var, vars) {
            if(!items.contains(QString(var.first.c_str()))) {
                DataType* tmp = new DataType(QString(var.first.c_str()),QString::number(interpreter->getStack()[var.second]));
                scene->addItem(tmp);
                items.insert(QString(var.first.c_str()), tmp);
            }
        }
    }
}

void MainWindow::on_printStackButton_clicked()
{
    QString tmp;
    foreach(int const& val, interpreter->getStack()) {
        tmp += QString::number(val) + ",";
    }
    qDebug() << "stack: " << tmp;
}

void MainWindow::on_printCodeButton_clicked()
{
    QString tmp;
    foreach(int const& val, interpreter->getCode()) {
        tmp += QString::number(val) + ",";
    }
    qDebug() << "code: " << tmp;
}

void MainWindow::on_printVarsButton_clicked()
{
    QString tmp;
    QMap<std::string,int> map(interpreter->getGlobals());
    QMapIterator<std::string, int> i(map);
    while(i.hasNext()) {
        i.next();
        tmp += QString(i.key().c_str()) + ": " + QString::number(i.value()) + ", ";
    }
    qDebug() << tmp;
}
