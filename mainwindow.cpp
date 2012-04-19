#include "mainwindow.h"
#include "ui_mainwindow.h"
#ifndef QT_NO_OPENGL
#include <QtOpenGL>
#endif

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    structs(interpreter.getStructs()),
    row(0), column(0)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("Dynamic Data Structure Visualisation"));

    QSize minSize(600, 600);
    scene = new QGraphicsScene(0, 0, minSize.width(), minSize.height());
    graphicsView = new GraphicsView(this);
    graphicsView->setMinimumSize(minSize + QSize(10,10));
    graphicsView->setScene(scene);

    layout = new QGraphicsGridLayout;
    QGraphicsWidget* form = new QGraphicsWidget;
    form->setLayout(layout);
    scene->addItem(form);
    layout->setHorizontalSpacing(10);
    layout->setVerticalSpacing(10);

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

    tabWidget = new QTabWidget(this);
    //structs
    structTreeWidget = new QTreeWidget(this);
    structTreeWidget->setColumnCount(2);
    structTreeWidget->setHeaderLabels({"Type", "Name"});//
    //variables
    variableTreeWidget = new QTreeWidget(this);
    variableTreeWidget->setColumnCount(3);
    variableTreeWidget->setHeaderLabels({"Name", "Address", "Width"});//
    //stack
    stackTableWidget = new QTableWidget(DDVS_STACK_SIZE, 2, this);
    stackTableWidget->setHorizontalHeaderLabels({"Value", "Type"});//
    stackTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    //set row height + labels
    QStringList vertLabels;
    for(int i = 0; i < stackTableWidget->rowCount(); i++) {
        stackTableWidget->verticalHeader()->resizeSection(i, stackTableWidget->rowHeight(i)-15);
        vertLabels << QString::number(i);
    }
    stackTableWidget->setVerticalHeaderLabels(vertLabels);
    stackTableWidget->verticalHeader()->setUpdatesEnabled(true);
    //set column widths
    stackTableWidget->setColumnWidth(0, 50);
    //tabs
    tabWidget->addTab(variableTreeWidget, "Variables");
    tabWidget->addTab(structTreeWidget, "Structs");
    tabWidget->addTab(stackTableWidget, "Stack");
    rightSplitter->addWidget(tabWidget);
    stackTableWidget->setColumnWidth(1, stackTableWidget->width()-7);

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
    //structs stuff
    connect(&interpreter, SIGNAL(newStructDefinition(cstruct)), this, SLOT(structDefined(cstruct)));
}

MainWindow::~MainWindow()
{
    delete scene;
    delete ui;
}

void MainWindow::on_actionAdd_Item_triggered()
{
    qDebug("Adding item...");
//    AddItemDialog *dialog = new AddItemDialog(scene, &items);
//    dialog->setAttribute(Qt::WA_DeleteOnClose); //make it free its memory on close
//    dialog->show();
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
        updateStackTable();
        updateVariableTree();
        updateVisualisation();
    }
}

void MainWindow::updateVisualisation()
{
    auto const& stack = interpreter.getStack();
    for(auto const& var : interpreter.getGlobals()) {
        ast::Type t = stack[var.second].type;
        auto name = QString::fromStdString(var.first);
        if(!items.contains(QString::fromStdString(var.first))) {
            QGraphicsWidget* item;
            if(t.pointer) {
                auto link = new QGraphicsLineItem;
                scene->addItem(link);
                item = new Graphics::Pointer(var.first, stack[var.second].var, t.type_str + "*",
                                             interpreter.getGlobals(), items, link);
            }
            else if(t == "struct") {
                //item = new Graphics::Struct(var.first, stack[var.second].var, t.type_str);
            }
            else {
                item = new Graphics::Variable(var.first, stack[var.second].var, t.type_str);
            }
            items.insert(name, item);
            if(!(layout->count() % 4)) {
                column = 0;
                row++;
            }
            layout->addItem(item, row, column);
            column++;
        }
    }
    scene->update(scene->sceneRect());
}

void MainWindow::updateStackTable()
{
    int i = 0;
    //populate stack table widget
    for(auto const& var : makeRange(interpreter.getStack().begin(), interpreter.getStackPos())) {
        QString p = "";
        if(var.type.pointer) {
            p += "*";
        }

        auto t1 = new QTableWidgetItem(QString::number(var.var));
        t1->setToolTip(t1->text());
        stackTableWidget->setItem(i, 0, t1);

        auto t2 = new QTableWidgetItem(QString::fromStdString(var.type.type_str)+p);
        t2->setToolTip(t2->text());
        stackTableWidget->setItem(i, 1, t2);
        ++i;
    }
}

void MainWindow::updateVariableTree()
{
    int i = 0;
    auto const& stack = interpreter.getStack();
    //populate variable tree
    variableTreeWidget->clear();
    for(auto const& var : interpreter.getGlobals()) {
        QTreeWidgetItem* item = new QTreeWidgetItem(
            {QString::fromStdString(var.first)
            ,QString::number(var.second)
            ,QString::number(stack[var.second].type.width)});//
        item->setToolTip(0,item->text(0));
        item->setToolTip(1,item->text(1));
        item->setToolTip(2,item->text(2));
        variableTreeWidget->insertTopLevelItem(i, item);
        ++i;
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

void MainWindow::structDefined(cstruct const& s)
{
    qDebug() << "Adding struct to tree";
    QTreeWidgetItem* item = new QTreeWidgetItem({QString::fromStdString(s.name)});
    item->setToolTip(0, QString::fromStdString(s.name));
    for(auto const& mem : s.member_specs) {
        QString p = "";
        if(mem.second.pointer) {
            p += "*";
        }
        QStringList cols({QString::fromStdString(mem.second.type_str) + p, QString::fromStdString(mem.first)});
        auto child = new QTreeWidgetItem(cols);
        child->setToolTip(0,cols.at(0));
        child->setToolTip(1,cols.at(1));

        item->addChild(child);
    }
    structTreeWidget->insertTopLevelItem(structTreeWidget->topLevelItemCount(), item);
}
