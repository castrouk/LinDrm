#ifndef WATERFALLWIDGET_H
#define WATERFALLWIDGET_H

#include <QWidget>
#include <QPixmap>
#include <vector>
#include <drmtx/common/GlobalDefinitions.h>

class QPaintEvent;
class QResizeEvent;

class WaterfallWidget : public QWidget
{
	Q_OBJECT
public:
	explicit WaterfallWidget(QWidget *parent = 0);

public slots:
	void updatePlot(const std::vector<_REAL>& vec);
	void paintSpec(const std::vector<_REAL>& vec);
	void clear();

protected:
	QPixmap Canvas;
	QPixmap Spec;
	QImage  image;
//	QVector specVec;
	void    paintEvent(QPaintEvent *);
	void    resizeEvent(QResizeEvent *);
	bool    resizeGlitch;

};

#endif // WATERFALLWIDGET_H
