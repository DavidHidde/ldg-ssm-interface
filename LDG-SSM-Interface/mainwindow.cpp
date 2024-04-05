#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "widgets/ldg_tree_view_button.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    LDGTreeViewButton *button = new LDGTreeViewButton();
    ui->scrollArea->setWidget(button);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_scrollArea_customContextMenuRequested(const QPoint &pos)
{

}

