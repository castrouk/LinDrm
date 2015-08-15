#ifndef LOGVIEW_H
#define LOGVIEW_H

// #include "ui_LogView.h"
#include "ui_logview.h"
#include <QWidget>
#include <QTime>
#include <QDebug>
#include <QVector>
#include <QFile>

class LogView : public QWidget , private Ui::Form
{
Q_OBJECT
public:
	LogView( QWidget * parent = 0, Qt::WindowFlags f = 0 );
	~LogView();
	void saveLogs();
	void openLogs();
	void Save();
	int contains(QString s);
	void insert(QString);

protected slots:
	void on_NewButton_clicked();
	void on_DeleteButton_clicked();
	void on_ListButton_clicked();
	void on_QuitButton_clicked();
	void on_InsertButton_clicked();

private:
	QVector<QStringList> saveVector;
};
#endif //LOGVIEW_H
