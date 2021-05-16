#include "calculator.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Calculator w;

#if defined(PLATFORM_X86)
    w.show();
#elif defined(PLATFORM_ARM)
    w.showFullScreen();
#endif

    return a.exec();
}
