#include "picoaprstrackerconfig.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    PicoAPRSTrackerConfig w;
    w.show();
    return a.exec();
}
