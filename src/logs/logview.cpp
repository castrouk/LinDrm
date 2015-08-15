#include "logview.h"
#include <QFlags>

LogView::LogView( QWidget * parent, Qt::WindowFlags f) : QWidget(parent,f)
{
	setupUi(this);
	this->setWindowTitle("Session Logs");
	//		buttonBox->addButton(quitButton, QDialogButtonBox::RejectRole)
	//	QuitButton->setWindowRole(QDialogButtonBox::RejectRole);
	tableWidget->setColumnCount(4);
	QStringList horzHeaders;
	horzHeaders << "Call" << "Name" << "Comment" << "Time";
	tableWidget->setHorizontalHeaderLabels( horzHeaders );
		tableWidget->setColumnWidth(0,80);
		tableWidget->setColumnWidth(1,90);
		tableWidget->setColumnWidth(2,120);
		tableWidget->setColumnWidth(3,65);
	openLogs();
}

LogView::~LogView()
{
//	qDebug() << __FILE__ << __LINE__ << "************";
}

void LogView::on_NewButton_clicked()
{
	qDebug() << __FILE__ << __LINE__ << tableWidget->rowCount();
	int row = tableWidget->rowCount();
	tableWidget->setRowCount(row + 1);

	QTableWidgetItem *c = new QTableWidgetItem("");
	tableWidget->setItem(row, 0,c->clone());
	tableWidget->setItem(row, 1,c->clone());
	tableWidget->setItem(row, 2,c->clone());
	tableWidget->setItem(row, 3,c->clone());
	tableWidget->item(row, 3)->setText(QTime::currentTime().toString());
}
void LogView::on_InsertButton_clicked()
{
	// While testing
	return;
	insert("G4EKQ");
}

void LogView::insert(QString call)
{
//	qDebug() << __FILE__ << __LINE__ << call;
	//	QString call("G3OTY");
	//	QStringList list;
	//	list << call << "" << "" << "";
	int Vindex = contains(call);
	if(Vindex == -1)
	{
//		qDebug() << __FILE__ << __LINE__;
		int row = tableWidget->rowCount();
		// add new MT row
		tableWidget->setRowCount(row + 1);
		QTableWidgetItem *c = new QTableWidgetItem("");
		tableWidget->setItem(row, 0,c->clone());
		tableWidget->setItem(row, 1,c->clone());
		tableWidget->setItem(row, 2,c->clone());
		tableWidget->setItem(row, 3,c->clone());
		// add call & time
		tableWidget->item(row, 0)->setText(call);
		tableWidget->item(row, 3)->setText(QTime::currentTime().toString());
		Save();
	}
	else
	{
//		qDebug() << __FILE__ << __LINE__;
		QList<QTableWidgetItem *> Ql;
		Ql << tableWidget->findItems(call,Qt::MatchExactly);
//		qDebug() << __FILE__ << __LINE__ << Ql.isEmpty();
		if(!Ql.isEmpty())
		{
//			qDebug() << __FILE__ << __LINE__;
			int row = tableWidget->row(Ql.at(0));
			QStringList list;
			list << tableWidget->item(row, 0)->text();
			list << tableWidget->item(row, 1)->text();
			list << tableWidget->item(row, 2)->text();
			list << QTime::currentTime().toString();
			tableWidget->removeRow(row);
			row = tableWidget->rowCount();
			// add new MT row
			tableWidget->setRowCount(row + 1);
			QTableWidgetItem *c = new QTableWidgetItem("");
			tableWidget->setItem(row, 0,c->clone());
			tableWidget->setItem(row, 1,c->clone());
			tableWidget->setItem(row, 2,c->clone());
			tableWidget->setItem(row, 3,c->clone());

			tableWidget->item(row, 0)->setText(list.at(0));
			tableWidget->item(row, 1)->setText(list.at(1));
			tableWidget->item(row, 2)->setText(list.at(2));
			tableWidget->item(row, 3)->setText(list.at(3));
			Save();
		}
		else
		{
//			qDebug() << __FILE__ << __LINE__;
			// add new MT row
			int row = tableWidget->rowCount();
			tableWidget->setRowCount(row + 1);
			QTableWidgetItem *c = new QTableWidgetItem("");
			tableWidget->setItem(row, 0,c->clone());
			tableWidget->setItem(row, 1,c->clone());
			tableWidget->setItem(row, 2,c->clone());
			tableWidget->setItem(row, 3,c->clone());
			;
//			qDebug() << __FILE__ << __LINE__;
			tableWidget->item(row, 0)->setText(saveVector.at(Vindex).at(0));
			tableWidget->item(row, 1)->setText(saveVector.at(Vindex).at(1));
			tableWidget->item(row, 2)->setText(saveVector.at(Vindex).at(2));
			tableWidget->item(row, 3)->setText(QTime::currentTime().toString());
//			Save();

		}
	}
	return;


	//	qDebug() << __FILE__ << __LINE__ << list.at(0);
	//	int v = contains(list.at(0));
	//	if(v < 0)
	//	{
	//		saveVector << list;
	//		Save();
	//	}
}


void LogView::on_DeleteButton_clicked()
{
	qDebug() << __FILE__ << __LINE__ << tableWidget->currentRow();
	tableWidget->removeRow(tableWidget->currentRow());
}

int LogView::contains(QString s)
{
	for (int i = 0; i < saveVector.size(); ++i)
	{
		if (saveVector.at(i).contains(s))
			return i;
	}
	return -1;
}

void LogView::openLogs()
{
	QString logFname(getenv("HOME"));
	logFname += "/.config/G4EKQ/LinDrm/session";
	qDebug() << __FILE__ << __LINE__ << logFname;
	QFile f(logFname);
	if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
		return;
	QTextStream in(&f);
	QString line = in.readLine();
	while (!line.isNull())
	{
		QStringList list;
		list = line.split(',');
		saveVector << list;
		line = in.readLine();
	}
}

void LogView::saveLogs()
{
	Save();
	QString logFname(getenv("HOME"));
	logFname += "/.config/G4EKQ/LinDrm/session";
	qDebug() << __FILE__ << __LINE__ << logFname;
	QFile f(logFname);
	if (!f.open(QIODevice::WriteOnly | QIODevice::Text))
		return;
	QTextStream out(&f);
	//	out << "The magic number is: " << 49 << "\n";
	for(int i = 0; i < saveVector.size(); i++)
	{
		QStringList list = saveVector.at(i);
		//		qDebug() << __FILE__ << __LINE__ << list;
		out << list.at(0) + ',';
		out << list.at(1) + ',';
		out << list.at(2) + '\n';
	}
	out.flush();
	f.close();
}

void LogView::Save()
{
//	qDebug() << __FILE__ << __LINE__;
	for (int i = 0; i < tableWidget->rowCount(); ++i)
	{
		QStringList list;
		list << tableWidget->item(i, 0)->text();
		list << tableWidget->item(i, 1)->text();
		list << tableWidget->item(i, 2)->text();
		if(!list.at(0).isEmpty())
		{
			int v = contains(list.at(0));
			if(v < 0)
				saveVector << list;
			else
			{
				saveVector.replace(v,list);
			}
		}
	}
}

void LogView::on_ListButton_clicked()
{
	Save();
}


//void LogView::Read()
//{
//	qDebug() << __FILE__ << __LINE__ <<;
//	for (int i = 0; i < saveVector.size(); ++i)
//	{
//		QDebug() << "Found Alfonso at position " << i;
////	    if (saveVector.at(i) == "Alfonso")
////	        QDebug() << "Found Alfonso at position " << i;
//	}
//}

void LogView::on_QuitButton_clicked()
{
	qDebug() << __FILE__ << __LINE__ << "****** void LogView::on_QuitButton_clicked() ******";
	close();
}
