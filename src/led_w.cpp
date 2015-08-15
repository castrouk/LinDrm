#include "led_w.h"

QColor Io_Led(Qt::red);
QColor Freq_Led(Qt::red);
QColor Time_Led(Qt::red);
QColor Frame_Led(Qt::red);
QColor FAC_Led(Qt::red);
QColor MSC_Led(Qt::red);


Led_W::Led_W( QWidget * parent, Qt::WindowFlags f) : QWidget(parent,f)
{
	setupUi(this);
	QTimer *timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(update()));
	timer->start(100);
}

Led_W::~Led_W()
{
}

void Led_W::SetLed(QLabel *Led, QColor c)
{
	QPalette p;
	p.setColor(QPalette::Background, c);
	Led->setPalette(p);
}

void Led_W::update()
{
	SetLed(Led_Io, Io_Led);
	SetLed(Led_Freq, Freq_Led);
	SetLed(Led_Time, Time_Led);
	SetLed(Led_Frame, Frame_Led);
	SetLed(Led_FAC, FAC_Led);
	SetLed(Led_MSC, MSC_Led);
}

