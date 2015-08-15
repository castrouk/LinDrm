#ifndef CONSTELLATIONWIDGET_H
#define CONSTELLATIONWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QDebug>

class ConstellationWidget : public QWidget
{
	Q_OBJECT
public:
	explicit ConstellationWidget(QWidget *parent = 0);
	~ConstellationWidget();

signals:

public slots:

protected:

	void paintEvent(QPaintEvent *);
	void resizeEvent(QResizeEvent *);
	QPixmap cMsc;
	QPixmap cFac;


};

#endif // CONSTELLATIONWIDGET_H
