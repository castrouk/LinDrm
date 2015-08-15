#include "constellationwidget.h"
#include <QResizeEvent>
#include <QPainter>
#include <QPaintEvent>

ConstellationWidget::ConstellationWidget(QWidget *parent) : QWidget(parent)
{
	QPen pe;
	cMsc = QPixmap(280,280);
	cFac = QPixmap(280,280);
	cMsc.fill(Qt::darkBlue);
	cFac.fill(Qt::darkBlue);
	QFont font;
	font.setPointSize(60);
//	font.setPixelSize(60);
	font.setBold(true);
	font.setWeight(75);

	QPainter p;
	p.begin(&cMsc);
	p.setFont(font);
	pe.setColor(Qt::black);
	p.drawText(35,(cMsc.width() + 60 )/ 2,"MSC");
	pe.setColor(Qt::white);
//	pe.setColor(QColor::fromRgb(0xef, 0xeb, 0xe7));
	pe.setWidth(1);
	p.setPen(pe);
	p.drawRect(1,1,cMsc.width()-1,cMsc.width()-1); // 0,281
//	drawText(const QPoint & position, const QString & text)
	p.end();

	p.begin(&cFac);
	p.setFont(font);
	p.drawText(35,(cMsc.width() + 60 )/ 2,"FAC");
//	pe.setColor(Qt::white);
//	pe.setColor(QColor::fromRgb(0xef, 0xeb, 0xe7));
	pe.setWidth(1);
	p.setPen(pe);
	p.drawRect(1,1,cFac.width()-1,cFac.width()-1);
	p.end();
	update();

}

ConstellationWidget::~ConstellationWidget()
{

}

void ConstellationWidget::paintEvent(QPaintEvent *)
{
	qDebug() << __FILE__ << __LINE__;
	QPainter painter(this);
	painter.drawPixmap(0, 0, cMsc);
	painter.drawPixmap(0, width(), cFac);
}

void ConstellationWidget::resizeEvent(QResizeEvent *e)
{

	qDebug() << __FILE__ << __LINE__ << cMsc.width() << e->size().width();

}

