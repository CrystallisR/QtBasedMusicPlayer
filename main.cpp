#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QFile themeFile( ":css/styles/normalTheme.css" );
    themeFile.open( QFile::ReadOnly );
    QString appStyleSheet( themeFile.readAll() );
    app.setStyleSheet(appStyleSheet);

    MainWindow w;
    w.show();
    return app.exec();
}
