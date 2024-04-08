#include "ldg_ssm_interface.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    LDGSSMInterface w;
    w.show();
    return a.exec();
}
