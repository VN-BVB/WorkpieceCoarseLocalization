#include "WorkpieceCoarseLocalization.h"
#include <QApplication>
#include <test.h>
int main(int argc, char *argv[])
{

    QApplication a(argc, argv);

    WorkpieceCoarseLocalization w;
    w.show();
    // Test test;
    // test.checkTensorRTEngine();
    return a.exec();
}

