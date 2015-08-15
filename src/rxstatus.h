#ifndef RXSTATUS_H
#define RXSTATUS_H

#include "widgets/blockview.h"

#include "ui_RxStatus.h"
#include <QWidget>
#include <QColor>

class RxStatus : public QWidget , private Ui::RxStatus
{
Q_OBJECT
public:
	RxStatus( QWidget * parent = 0, Qt::WindowFlags f = 0 );
	~RxStatus();
  void SetLed(QLabel *Led, QColor );
  int chkList(QStringList &);
  void update();

// protected slots:
//	void update();

private:
	int freqOff;
	QString mode;
	float bandwidth;
	QString interleave;
	QString protection;
	int qam;
	QString call;
	int currentSegment;
	int transportID;
	int totalSeg;
	int rxSeg;
	QStringList calllist;
};

QString modeToString(uint mode);


#endif //RXSTATUS_H
