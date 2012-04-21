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
    structTreeWidget->setHeaderLabels(QStringList() << "Type" << "Name");//
    //variables
    variableTreeWidget = new QTreeWidget(this);
    variableTreeWidget->setColumnCount(3);
    variableTreeWidget->setHeaderLabels(QStringList() << "Name" << "Address" << "Width");//
    //stack
    stackTableWidget = new QTableWidget(DDVS_STACK_SIZE, 2, this);
    stackTableWidget->setHorizontalHeaderLabels(QStringList() << "Value" << "Type");//
    stackTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    //set row height + labels
    QStringList vertLabels;
    for(int i = 0; i < stackTableWidget->rowCount(); i++) {
        stackTableWidget->verticalHeader()->resizeSection(i, stackTableWidget->rowHeight(i)-15);
        vertLabels << QString::number(i);
    }
    stackTableWidget->setVerticalHeaderLabels(vertLabels);
    stackTableWidget->verticalHeader()->setUpdatesEnabled(true);
    //set initial column widths
    stackTableWidget->setColumnWidth(0, 45);
    variableTreeWidget->setColumnWidth(0,60);
    variableTreeWidget->setColumnWidth(1,50);
    variableTreeWidget->setColumnWidth(2,30);
    structTreeWidget->setColumnWidth(0, structTreeWidget->width()/2);
    structTreeWidget->setColumnWidth(1, structTreeWidget->width()/2);
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

void MainWindow::updateAll() {
    updateStackTable();
    updateVariableTree();
    updateVisualisation();
}

void MainWindow::on_actionAdd_Item_triggered()
{
    qDebug("Adding item...");
    AddItemDialog *dialog = new AddItemDialog(interpreter, structs, this);
    dialog->setAttribute(Qt::WA_DeleteOnClose); //make it free its memory on close
    dialog->show();
}

void MainWindow::on_actionEdit_Item_triggered()
{
    if(scene->selectedItems().size() == 0) return;
    qDebug("Editing item...");
    Graphics::GraphicsWidget* item = dynamic_cast<Graphics::GraphicsWidget*>(scene->selectedItems().first());
    QString ts = QString::fromStdString(item->type_str);
    if(!ts.contains('*') && ts.contains("struct")) {
        msgbox.setText("Cannot edit struct value");
        msgbox.setStandardButtons(QMessageBox::Close);
        msgbox.setDefaultButton(QMessageBox::Close);
        msgbox.exec();
        return;
    }
    EditItemDialog *dialog = new EditItemDialog(dynamic_cast<Graphics::Variable*>(item)->name, interpreter, this);
    dialog->setAttribute(Qt::WA_DeleteOnClose); //make it free its memory on close
    dialog->show();
}

void MainWindow::interpretButton_clicked()
{
    QString tmp_str = interpreterInput->toPlainText();
    if(interpreter.parse(tmp_str)) {
        interpreterInput->clear();
        interpreter.execute();

        updateStackTable();
        updateVariableTree();
        updateVisualisation();
    }
}

std::string const MainWindow::findByValue(const int value)
{
    for(auto const& var : interpreter.getGlobals()) {
        if(var.second == value) {
            return var.first;
        }
    }
    return "";
}

void MainWindow::updateVisualisation()
{
    auto const& stack = interpreter.getStack();
    for(auto i=stack.begin(); i<interpreter.getStackPos(); ) {
        ast::Type t = i->type;
        auto std_name = findByValue(i-stack.begin());
        if(std_name.size() <= 0) {
            std::stringstream ss;
            ss << t.type_str << i-stack.begin();
            std_name = ss.str();
            qDebug() << "name:" << std_name.c_str();
        }
        auto name = QString::fromStdString(std_name);
        if(!items.contains(name)) {
            Graphics::GraphicsWidget* item;
            if(t.pointer) {
                auto link = new QGraphicsLineItem;
                scene->addItem(link);
                item = new Graphics::Pointer(std_name, i->var, t.type_str + "*",
                                             interpreter.getGlobals(), items, link);
            }
            else if(t == "struct") {
                auto const& structs = interpreter.getStructs();
                auto j = structs.find(t.type_str.substr(7));
                if(j != structs.end()) {
                    interpreter::cstruct cs = j->second;
                    std::list<Graphics::member_container> tmp_mems;
                    for(auto const& mem : cs.member_specs) {
                        tmp_mems.push_back(
                                    Graphics::member_container(mem.first,
                                                               mem.second.type_str,
                                                               std::cref((i+cs.members[mem.first])->var)
                                                               ));
                    }
                    item = new Graphics::Struct(std_name, t.type_str, tmp_mems);
                }
                else
                    return;
            }
            else {
                item = new Graphics::Variable(std_name, i->var, t.type_str);
            }
            items.insert(name, item);
            qDebug() << items;
            if(!(layout->count() % 6)) {
                column = 0;
                row++;
            }
            layout->addItem(item, row, column);
            column++;
        }
        i += t.width;
    }
//    for(auto const& var : interpreter.getGlobals()) {
//    }
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
        QTreeWidgetItem* item = new QTreeWidgetItem(QStringList()
            << QString::fromStdString(var.first)
            << QString::number(var.second)
            << QString::number(stack[var.second].type.width));//
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
    QTreeWidgetItem* item = new QTreeWidgetItem(QStringList() << QString::fromStdString(s.name));
    item->setToolTip(0, QString::fromStdString(s.name));
    for(auto const& mem : s.member_specs) {
        QString p = "";
        if(mem.second.pointer) {
            p += "*";
        }
        QStringList cols(QStringList() << (QString::fromStdString(mem.second.type_str) + p) << QString::fromStdString(mem.first));
        auto child = new QTreeWidgetItem(cols);
        child->setToolTip(0,cols.at(0));
        child->setToolTip(1,cols.at(1));

        item->addChild(child);
    }
    structTreeWidget->insertTopLevelItem(structTreeWidget->topLevelItemCount(), item);
}

void MainWindow::on_actionRefresh_Visualisation_triggered()
{
    updateStackTable();
    updateVariableTree();
    updateVisualisation();
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this, "Information", "Dynamic Data Structure Visualisation\n"
                       "Copyright \x00A9 2012 Christian Manning");
}
