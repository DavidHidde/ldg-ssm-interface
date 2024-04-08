#include "ldg_ssm_interface.h"
#include "./ui_ldg_ssm_interface.h"
#include "util/build_tree.h"

/**
 * @brief LDGSSMInterface::LDGSSMInterface
 * @param parent
 */
LDGSSMInterface::LDGSSMInterface(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::LDGSSMInterface)
{
    ui->setupUi(this);
    setWindowTitle("LDG-SSM");

    scroll_area = new PannableScrollArea();
    root_ldg_button = buildTree(80, 128, this);

    setCentralWidget(scroll_area);
    scroll_area->setWidget(root_ldg_button);
    initializeMenus();
}

/**
 * @brief LDGSSMInterface::~LDGSSMInterface
 */
LDGSSMInterface::~LDGSSMInterface()
{
    delete ui;
}

/**
 * @brief LDGSSMInterface::keyPressEvent
 * @param event
 */
void LDGSSMInterface::keyPressEvent(QKeyEvent *event)
{
    auto key_pressed = event->key();
    if (key_pressed >= Qt::Key_0 && key_pressed <= Qt::Key_9) {
        emit selectionChanged(key_pressed - Qt::Key_0);
    }
}

/**
 * @brief LDGSSMInterface::LDGSSMInterface Initialize the menus of the window.
 */
void LDGSSMInterface::initializeMenus()
{
    // Initialize the file menu shortcuts.
    file_menu = ui->menuFile;

    // Initialize the view menu shortcuts.
    view_menu = ui->menuView;
    view_menu->addAction("Reset view", Qt::Key_R, scroll_area, &PannableScrollArea::fitWindow);
    view_menu->addAction("Zoom in", QKeySequence::ZoomIn, scroll_area, &PannableScrollArea::zoomIn);
    view_menu->addAction("Zoom out", QKeySequence::ZoomOut, scroll_area, &PannableScrollArea::zoomOut);
}
