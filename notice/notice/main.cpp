#include "dialog.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Dialog w;
    //w.setAttribute(Qt::WA_TranslucentBackground);
    //w.setWindowOpacity(0.4);
    w.setFilePath(QString(argv[1]));
    w.show();

    return a.exec();
}
