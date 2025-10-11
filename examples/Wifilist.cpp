#include <QGuiApplication>
#include <QQmlApplicationEngine>

auto main(int argc, char *argv[]) -> int {
  QGuiApplication app(argc, argv);

  QQmlApplicationEngine engine;
  engine.addImportPath("qrc:/esterVtech.com/imports");
  engine.loadFromModule("ExamplesWifilist", "Wifilist");

  return QGuiApplication::exec();
}
