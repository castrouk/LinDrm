#include "waterfallwidget.h"
#include <QPainter>
#include <QPaintEvent>
#include <QResizeEvent>
#include <../matlib/MatlibStdToolbox.h>
#include <algorithm>
#include <QDebug>
// #include <QTimer>

/* Init some constants */
static const int maxHue = 359; /* Range of "Hue" is 0-359 */
static const int maxSat = 255; /* Range of saturation is 0-255 */

//			qDebug() << __FILE__ << __LINE__;

static QColor fromReal(_REAL val)
{
	//	val /= 7.0;
	val /= 20.0;
	//		val -= 50.0;
	//		val = drand48();
	//				qDebug() << val;
	int hue = maxHue - 60 - int(Round(val * maxHue));

	/* Prevent out-of-range */
	if(hue>maxHue) hue = maxHue;
	if (hue < 0) hue = 0;

	/* Also change saturation to get dark colors when low level */
	int sat = int((1 - double(hue) / maxHue) * maxSat);
	/* Prevent out-of-range */
	if (sat > maxSat) sat = maxSat;
	if (sat < 0) sat = 0;

	return QColor::fromHsv(hue, sat, sat);
}

WaterfallWidget::WaterfallWidget(QWidget *parent) :
	QWidget(parent), resizeGlitch(false)
{
	setAttribute(Qt::WA_OpaquePaintEvent, true);
	setAttribute(Qt::WA_TransparentForMouseEvents, true);
	setCursor(Qt::CrossCursor);
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	Spec = QPixmap(width(), 100);
}


void WaterfallWidget::paintEvent(QPaintEvent *)
{
	QPainter painter(this);
	painter.drawPixmap(0, 0, Canvas);
}

void WaterfallWidget::resizeEvent(QResizeEvent *e)
{
	// prevent resize glitch when the widget is created,
	// the fist resizeEvent() is bogus
	if(!resizeGlitch)
	{
		resizeGlitch = true;
		return;
	}

	QPixmap tmp = Canvas;
	Canvas = QPixmap(e->size());
	image = QImage(e->size().width(), 1, QImage::Format_RGB32);

	// first time ever - initialise to black
	if(tmp.size().width()==0)
	{
		tmp = QPixmap(e->size());
		// always use a black background
		tmp.fill(QColor::fromRgb(0, 0, 0));
	}
	// vertical stretch - fill below old with black
	if(Canvas.height()>tmp.height())
		Canvas.fill(QColor::fromRgb(0, 0, 0));

	// copy old data, scaling to fit horizontally but making space below
	// TODO - the width scaling is slightly off.
	QPainter p(&Canvas);
//	p.drawPixmap(0,0, Spec);

	QPoint origin(0,0);
	QSize top(Canvas.width(), tmp.height());
	p.drawPixmap(QRect(origin, top), tmp);

}

void WaterfallWidget::clear()
{
	// first time ever - initialise to black
	Canvas.fill(QColor::fromRgb(0, 0, 0));
}

vector<_REAL>	specbufarr;


void WaterfallWidget::paintSpec(const vector<_REAL>& vec)
{
	int w = image.width();
	if (w == 0)
		return;
//	qDebug() << __FILE__ << __LINE__ << w;
	extern int specLen;
	specLen = w * 2;
	QPainter p;
	Spec.fill(QColor(0,0,90));
	p.begin(&Spec);

			p.setRenderHints(QPainter::SmoothPixmapTransform | QPainter::Antialiasing,true);
//			p.setRenderHint(QPainter::Antialiasing,true);
	QPen pen;
	pen.setWidth(2);
	// Pilot markers
	//	m1 = 62;
	//	m2 = 126;
	//	m3 = 158;
//	int	m1 = 62;
//	int	m2 = 126;
//	int	m3 = 158;

	float	m1 = 62.5f;
	float	m2 = 126.5f;
	float	m3 = 158.5f;

	pen.setColor(QColor(0x00, 0x80 , 0xFF));
	pen.setWidth(1);
	p.setPen(pen);
	p.drawLine(m1, 0, m1, 100);
	p.drawLine(m2, 0, m2, 100);
	p.drawLine(m3, 0, m3, 100);
	// End Pilot markers

	pen.setColor(Qt::yellow);
	p.setPen(pen);
	//to do

	if(specbufarr.size() != vec.size())
		specbufarr.resize(vec.size());

	for (uint i = 0; i < vec.size() / 2; i++)
	{
		// Smooth display
		specbufarr.at(i) = (3.0 * specbufarr.at(i) + vec[2*i] + vec[2*i+1]) * 0.2;
	}


//	qDebug() << __FILE__ << __LINE__ << vec.size() << specbufarr.size(); //vec.size() = 512 was 300
	for(uint i = 1; i < specbufarr.size(); i++)
	{
		extern float specV;
		float v1 = specbufarr.at(i-1) / specV;
		float v2 = specbufarr.at(i) / specV;
		float z = i * 2;
		QPointF p1(z-1, -v1 + 95.0f); // 95 = 5 pixels up from bottom
		QPointF p2(z, -v2 + 95.0f);
		p.drawLine(p1,p2);
	}
//	QLine line(0 = left,width(),100);
//	p.drawLine(0,0,width(),height());
	p.end();

}

void WaterfallWidget::updatePlot(const vector<_REAL>& vec)
{
#define MIN_VAL_INP_SPEC_Y_AXIS_DB ((double) -120.0)
#define MAX_VAL_INP_SPEC_Y_AXIS_DB ((double) 0.0)

	_REAL min = MIN_VAL_INP_SPEC_Y_AXIS_DB;
	_REAL max = MAX_VAL_INP_SPEC_Y_AXIS_DB;
	int w = image.width();
	if (w == 0)
		return;
	paintSpec(vec);
	QPainter painter;
	if(!painter.begin(&Canvas)) // David had problems on Linux without this
		return;

	/* Scroll Canvas */
	Canvas.scroll(0, 1, Canvas.rect());

	// cast from u_char (8bits wide) to QRgb (probably 24bits wide)
	QRgb* scanLine = ((QRgb*)image.scanLine(0));

	// scaling factor between data and image
	_REAL ratio = _REAL(vec.size()) / (_REAL(w) * 2.0);
	if(ratio<1.0)
	{
		ratio = 1.0; // TODO handle stretching
	}
	vector<_REAL>::const_iterator vi = vec.begin();
	size_t l=0; // index of left edge of bin
	for(int i=0; i < w; i++)
	{
		// calculate right edge of current bin - not all bins are same width for all possible ratios
		size_t r = size_t((i+1)*ratio); // index of right edge (ie left edge of following bin
		vector<_REAL>::const_iterator biggest = max_element(vi+l, vi+r);
		// reduce dB value by 6 dB to avoid FSD and normalise
		_REAL norm = ((*biggest) - 6.0 - min) / (max - min);
		// Translate normalised value to a color and generate pixel
		scanLine[i] = fromReal(norm).rgb();
		l = r;
		//								qDebug() << "zz" << *biggest;
	}
	painter.drawPixmap(0,0, Spec);
	painter.drawImage(0, 100, image);
	painter.end();
	update();
}
