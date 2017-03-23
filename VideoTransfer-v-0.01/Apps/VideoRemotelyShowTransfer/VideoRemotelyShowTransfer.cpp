#include <QApplication>
#include <QTextCodec>
#include "vacMainInterface.h"
//#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" )

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
    QTextCodec *codec = QTextCodec::codecForName("System");
    QTextCodec::setCodecForLocale(codec);
	QTextCodec::setCodecForTr(codec);
	QTextCodec::setCodecForCStrings(codec);
	vacMainInterface MainWin;
	MainWin.show();
	return app.exec();
}