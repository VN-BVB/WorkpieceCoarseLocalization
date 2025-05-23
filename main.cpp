#include <test.h>

#include <QApplication>

#include "WorkpieceCoarseLocalization.h"
int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    WorkpieceCoarseLocalization w;
    w.show();
    // Test test;
    // test.checkTensorRTEngine();
    return a.exec();
}

// clang-format off
// clang-format on
