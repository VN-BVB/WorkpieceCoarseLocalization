#include <QApplication>

#include "WorkpieceCoarseLocalization.h"
int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    WorkpieceCoarseLocalization w;
    w.show();
    return a.exec();
}

// clang-format off
// clang-format on
