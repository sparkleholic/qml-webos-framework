#include "ServiceRequest.h"
#include <QApplication>
#include <QLabel>
#include <QDesktopWidget>
#include <QWindow>
#include <qpa/qplatformnativeinterface.h>
  
int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    QRect rec = QApplication::desktop()->screenGeometry();
  
    QLabel *label = new QLabel("Hello, Native Qt Application!!");
    label->setFixedSize(rec.width(), rec.height());
    label->setStyleSheet("background-color:yellow;");
    label->setAlignment(Qt::AlignCenter);
  
    QFont font = label->font();
    font.setPointSize(50);
    label->setFont(font);
    label->show();
  
    ServiceRequest s_request("com.example.app.nativeqt");
    s_request.registerNativeApp();
  
    QWidget *widget = label->window();
    QWindow *window = widget->windowHandle();
  
    QApplication::platformNativeInterface()->setWindowProperty(window->handle(), QStringLiteral("appId"), QStringLiteral("com.example.app.nativeqt"));
  
    return app.exec();
}
