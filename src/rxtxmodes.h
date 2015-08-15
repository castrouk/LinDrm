#ifndef RXTXMODES_H
#define RXTXMODES_H

#include "ui_RxTxModes.h"
#include <QWidget>
#include <QTimer>

class RxTxModes : public QWidget , private Ui::RxTxModes_W
{
Q_OBJECT
public:
	RxTxModes( QWidget * parent = 0, Qt::WindowFlags f = 0 );
	~RxTxModes();
protected slots:
	void on_tabWidget_currentChanged(int);
	void on_drmTxModeComboBox_activated(const QString&);
	void on_drmTxBandwidthComboBox_activated(const QString&);
	void on_drmTxQAMComboBox_activated(const QString&);
	void on_drmTxInterleaveComboBox_activated(const QString&);
	void on_drmTxProtectionComboBox_activated(const QString&);
	void on_drmTxInstancesSpinBox_valueChanged(int);
	void on_drmTxReedSolomonComboBox_activated(const QString&);
	void update();
	void setStatus();

private:
	int freqOff;
	QString mode;
	float bandwidth;
	QString interleave;
	QString protection;
	int qam;
	QString call;
	QPixmap *greenPXM;
	QPixmap *redPXM;
	QPixmap *yellowPXM;
	int currentSegment;
	int transportID;
	int totalSeg;
	int rxSeg;
};
#endif //RXTXMODES_H
