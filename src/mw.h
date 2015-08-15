#ifndef MW_H
#define MW_H

#include "ui_Mw.h"
#include <QMainWindow>
#include <sndfile.hh>
#include "DrmReceiver.h"
#include "waterfallwidget.h"
#include "specwidget.h"
#include "runtx.h"
#include "soundcard.h"
#include "TxPreview/droparea.h"
#include <QPixmap>
#include <QProgressDialog>
#include "txprogress.h"
#include <QPainter>
#include "logs/logview.h"

//class DropArea;

class Mw : public QMainWindow , private Ui::MainWindow
{
	Q_OBJECT
public:
	Mw( QWidget * parent = 0, Qt::WindowFlags f = 0 );
	~Mw();

	//    void setCall(QString call){CallSign->setText(call);}


public slots:
	void update();
	void on_tabWidget_currentChanged(int);
	void on_actionSound_Card_triggered();
	void on_actionDrm_Settings_triggered();
	void on_actionSession_Log_triggered();
	void TextColour(QPushButton *, QColor );
	void on_Record_toggled(bool);
	void on_FromFile_toggled(bool);
	void on_TxPic_clicked();
	void on_pic_changed(const QString& s);
	void onVolumeChanged(int);
	void onSpecChanged(int);
	void resizeEvent(QResizeEvent * /* event */);
	void fromPlayList();
	QString getImag(){return imgFname;}
	void on_Bsr_clicked();
	void on_Fix_clicked();

private:
	QString RecFname;
	vector<_REAL>		vecrData;
	float specagc;
	float specbufarr[300];
	float specarr[512];
	RunTx *TX;
	QPixmap img;
	QString imgFname;
	int nblocks;
	double TxTime;
	bool DispWf;
//	TableEditor *editor;

};
#endif //MW_H
