#include <QGuiApplication>
//#include <QQmlApplicationEngine>
#include <QQmlEngine>
#include <QQmlContext>
//#include <QQuickView>
#include <QDebug>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    //QQmlApplicationEngine engine;
    QQmlEngine engine;
    engine.load(QUrl::fromLocalFile("main.qml"));
/*
    QQuickView view;

    view.setSource(QUrl("./main.qml"));

    view.setTitle(QLatin1String("PmLog QML Plugin Test"));
    view.resize(100, 300);
    view.show();
*/
    return app.exec();
}
