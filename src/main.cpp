#include  <QApplication>
// #include "sql/connection.h"

#include  "mw.h"

int main(int argc, char *argv[])
{
	QApplication app(argc,argv);
//	if (!createConnection())
//		 return 1;

	Mw widget;
	widget.show();
	return app.exec();
}
