#include "mainwindow.h"

#include <QApplication>
#include <openssl/opensslv.h>



int main(int argc, char *argv[])
{
    qDebug() << "OpenSSL version:" << OPENSSL_VERSION_TEXT;
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();

}
