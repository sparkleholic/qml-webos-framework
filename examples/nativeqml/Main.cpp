#include "ServiceRequest.h"
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QTimer>
 
int main(int argc, char* argv[])
{
    ServiceRequest s_request("com.example.app.nativeqt");
    s_request.registerNativeApp();
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    QTimer timer;
    QObject::connect(&timer, &QTimer::timeout, []() {
        qDebug("com.example.app.nativeqt: 5000ms timer");
    });
    timer.start(5000);
    return app.exec();
}
