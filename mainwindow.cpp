#include "mainwindow.h"
#include "ui_mainwindow.h"
#ifndef QT_NO_OPENGL
#include <QtOpenGL>
#endif

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    interpreter(new interpreter::Interpreter)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("Dynamic Data Structure Visualisation"));

    zoomer = new ZoomWidget(graphicsView);
    ui->statusBar->addWidget(zoomer);

    QSize minSize(600, 600);
    scene = new QGraphicsScene(0, 0, minSize.width(), minSize.height());
    graphicsView = new QGraphicsView(scene, this);
    graphicsView->setMinimumSize(minSize + QSize(10,10));

    //UI Layout
    mainSplitter = new QSplitter(Qt::Horizontal, this);
    mainSplitter->addWidget(graphicsView);
    mainSplitter->setStretchFactor(0,1);
    setCentralWidget(mainSplitter);

    //buttons
    interpretButton = new QPushButton(tr("Interpret"));
#ifdef QT_DEBUG
    printStackButton = new QPushButton(tr("Print Stack"));
    printCodeButton = new QPushButton(tr("Print Code"));
    printVarsButton = new QPushButton(tr("Print Globals"));
#endif
    rightSplitter = new QSplitter(Qt::Vertical, this);
    rightSplitter->setMinimumWidth(200);

    //text input
    interpreterInput = new QPlainTextEdit(tr("Enter code here"), this);
    rightSplitter->addWidget(interpreterInput);
    rightSplitter->addWidget(interpretButton);
#ifdef QT_DEBUG
    rightSplitter->addWidget(printStackButton);
    rightSplitter->addWidget(printCodeButton);
    rightSplitter->addWidget(printVarsButton);
#endif
    mainSplitter->addWidget(rightSplitter);

#ifndef QT_NO_OPENGL
    //ui->graphicsView->setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));
#endif

    this->setMinimumWidth(graphicsView->minimumWidth() + rightSplitter->minimumWidth() + 5);
    this->setMinimumHeight(graphicsView->minimumHeight() + ui->menuBar->height() + ui->mainToolBar->height()
                           + zoomer->height() + ui->statusBar->height() + 5);

    //interpreter = new interpreter::Interpreter;
    connect(interpretButton, SIGNAL(clicked()), this, SLOT(interpretButton_clicked()));
#ifdef QT_DEBUG
    connect(printStackButton, SIGNAL(clicked()), this, SLOT(printStackButton_clicked()));
    connect(printCodeButton, SIGNAL(clicked()), this, SLOT(printCodeButton_clicked()));
    connect(printVarsButton, SIGNAL(clicked()), this, SLOT(printVarsButton_clicked()));
#endif
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
    AddItemDialog *dialog = new AddItemDialog(scene, &items);
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

void MainWindow::interpretButton_clicked()
{
    QString tmp_str = interpreterInput->toPlainText();
    if(interpreter->parse(tmp_str)) {
        interpreterInput->clear();
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

#ifdef QT_DEBUG

void MainWindow::printStackButton_clicked()
{
    QString tmp;
    foreach(int const& val, interpreter->getStack()) {
        tmp += QString::number(val) + ",";
    }
    qDebug() << "stack: " << tmp;
}

void MainWindow::printCodeButton_clicked()
{
    QString tmp;
    foreach(int const& val, interpreter->getCode()) {
        tmp += QString::number(val) + ",";
    }
    qDebug() << "code: " << tmp;
}

void MainWindow::printVarsButton_clicked()
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
#endif
