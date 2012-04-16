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
    this->setWindowTitle(tr("Dynamic Data Structure Visualisation"));

    QSize minSize(600, 600);
    scene = new QGraphicsScene(0, 0, minSize.width(), minSize.height());
    graphicsView = new GraphicsView(this);
    graphicsView->setMinimumSize(minSize + QSize(10,10));
    graphicsView->setScene(scene);

    //UI Layout
    mainSplitter = new QSplitter(Qt::Horizontal, this);
    mainSplitter->addWidget(graphicsView);
    mainSplitter->setStretchFactor(0,1);
    setCentralWidget(mainSplitter);

    //buttons
    interpretButton = new QPushButton(tr("Interpret"));
#ifdef QT_DEBUG
    printStackButton = new QPushButton(tr("Print Stack"));
    printOffsetButton = new QPushButton(tr("Print Offset"));
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
    rightSplitter->addWidget(printOffsetButton);
    rightSplitter->addWidget(printVarsButton);
#endif
    mainSplitter->addWidget(rightSplitter);

#ifndef QT_NO_OPENGL
    //ui->graphicsView->setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));
#endif

    this->setMinimumWidth(graphicsView->minimumWidth() + rightSplitter->minimumWidth() + 5);
    this->setMinimumHeight(graphicsView->minimumHeight() + ui->menuBar->height()
                           + ui->mainToolBar->height() + ui->statusBar->height() + 5);

    //interpreter = new interpreter::Interpreter;
    connect(interpretButton, SIGNAL(clicked()), this, SLOT(interpretButton_clicked()));
#ifdef QT_DEBUG
    connect(printStackButton, SIGNAL(clicked()), this, SLOT(printStackButton_clicked()));
    connect(printOffsetButton, SIGNAL(clicked()), this, SLOT(printOffsetButton_clicked()));
    connect(printVarsButton, SIGNAL(clicked()), this, SLOT(printVarsButton_clicked()));
#endif
}

MainWindow::~MainWindow()
{
    delete scene;
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
    if(interpreter.parse(tmp_str)) {
        interpreterInput->clear();
        interpreter.execute();
        typedef std::map<std::string, boost::shared_ptr<interpreter::variable> > varstype;
        //varstype vars = interpreter.getGlobals();
//        foreach(varstype::value_type var, vars) {
//            if(!items.contains(QString(var.first.c_str()))) {
//                DataType* tmp = new DataType(QString(var.first.c_str()),QString::number(interpreter->getStack()[var.second]));
//                scene->addItem(tmp);
//                items.insert(QString(var.first.c_str()), tmp);
//            }
//        }
    }
}

#ifdef QT_DEBUG

void MainWindow::printStackButton_clicked()
{
    QString tmp;
    QDebug d(&tmp);
    qDebug().nospace() << "stack: ";
    foreach(interpreter::variable const& val, interpreter.getStack()) {
        std::cout << val << ',';
        //tmp += QString::number(val) + ",";
    }
    std::cout << std::endl;
}

void MainWindow::printOffsetButton_clicked()
{
    qDebug() << "offset: " << interpreter.getOffset();
}

void MainWindow::printVarsButton_clicked()
{
    QString tmp;
    QMap<std::string,int> map(interpreter.getGlobals());
    QMapIterator<std::string, int> i(map);
    while(i.hasNext()) {
        i.next();
        tmp += QString(i.key().c_str()) + ": " + QString::number(i.value()) + ", ";
    }
    qDebug() << tmp;
}
#endif

void MainWindow::on_actionZoom_In_triggered()
{
    graphicsView->zoomIn();
}

void MainWindow::on_actionZoom_Out_triggered()
{
    graphicsView->zoomOut();
}

void MainWindow::on_actionAbout_Qt_triggered()
{
    QMessageBox::aboutQt(this);
}
