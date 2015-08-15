#ifndef SPECWIDGET_H
#define SPECWIDGET_H

#include <QWidget>
#include <QPixmap>
#include <vector>
#include <drmtx/common/GlobalDefinitions.h>

class QPaintEvent;
class QResizeEvent;

class SpecWidget : public QWidget
{
	Q_OBJECT
public:
	explicit SpecWidget(QWidget *parent = 0);

public slots:
	void updatePlot(const std::vector<_REAL>& vec, _REAL min, _REAL max);
//	void paint_spec();
	void PutSpec(vector<_REAL>);
	void clear();

protected:
	QPixmap Canvas;
	QImage  image;
	void    paintEvent(QPaintEvent *);
	void    resizeEvent(QResizeEvent *);
	bool    resizeGlitch;
	vector<_REAL>		vecrData;
	float specbufarr[300];
	float specarr[530];

};

#endif // WATERFALLWIDGET_H
