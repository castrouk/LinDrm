#include "specwidget.h"
#include <QPainter>
#include <QPaintEvent>
#include <QResizeEvent>
#include <../matlib/MatlibStdToolbox.h>
#include <algorithm>
#include <QDebug>

/* Init some constants */
static const int maxHue = 359; /* Range of "Hue" is 0-359 */
static const int maxSat = 255; /* Range of saturation is 0-255 */

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

SpecWidget::SpecWidget(QWidget *parent) :
	QWidget(parent), resizeGlitch(false)
{
	setAttribute(Qt::WA_OpaquePaintEvent, true);
	setAttribute(Qt::WA_TransparentForMouseEvents, true);
	setCursor(Qt::CrossCursor);
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}


void SpecWidget::paintEvent(QPaintEvent *)
{
//	qDebug() << __FILE__ << __LINE__;
	QPainter painter(this);
	painter.drawPixmap(0, 0, Canvas);
}

void SpecWidget::resizeEvent(QResizeEvent *e)
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
	QPoint origin(0,0);
	QSize top(Canvas.width(), tmp.height());
	p.drawPixmap(QRect(origin, top), tmp);
}

void SpecWidget::clear()
{
	// first time ever - initialise to black
	Canvas.fill(QColor::fromRgb(0, 0, 0));
}

void SpecWidget::updatePlot(const vector<_REAL>& vec, _REAL min, _REAL max)
{
	int w = image.width();
	if (w == 0)
		return;
	//				qDebug() << "vec.data:" << vec[250];

	//				qDebug() << "vec.size():" << vec.size();

	QPainter painter;
	if(!painter.begin(&Canvas)) // David had problems on Linux without this
		return;

	/* Scroll Canvas */
	Canvas.scroll(0, 1, Canvas.rect());

	// cast from u_char (8bits wide) to QRgb (probably 24bits wide)
	QRgb* scanLine = ((QRgb*)image.scanLine(0));

	// scaling factor between data and image
	_REAL ratio = _REAL(vec.size())/ (_REAL(w) * 2.0);
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
	painter.drawImage(0, 0, image);
	painter.end();
	update();
}


void SpecWidget::PutSpec(vector<_REAL> vecrData)
{
	int w = image.width();
	if (w == 0)
		return;
	Canvas.fill(QColor::fromRgb(0, 0, 128));
	QPainter painter;
	if(!painter.begin(&Canvas)) // David had problems on Linux without this
		return;
	QPen p;
	p.setColor(Qt::white);
	painter.setPen(p);

	if (vecrData.size() >= 300)
	{
//		qDebug() << __FILE__ << __LINE__ << vecrData.size();
		int i;
		for (i = 0;i < 250;i++)
			specbufarr[i] = (3.0 * specbufarr[i] + vecrData[2*i] + vecrData[2*i+1]) * 0.2;
		for (i = 0;i < 250;i++)
		{

//			painter.drawLine(i,specbufarr[i],this->width(),this->height());
			specarr[i] = specbufarr[i] / 50.0;
//			specarr[i] = (int)(specbufarr[i] * 40.0);
			// specarr[i] = -(specbufarr[i] * 40.0 * specagc);
		}
	}
	//	painter.drawPolyline();
	float psize = 250.0;
//	float vs = 2.0;
	float ssize = psize / 1.8;
//	painter.setWindow( 0, -100, ssize, 500 );
	painter.setWindow( -5, -150, ssize, 250);
	painter.setPen( Qt::yellow  );
	int i = 1;
	while (i < ssize)
	{
//		painter.drawLine(i - 1, specarr[i-1] * vs, i, specarr[i] * vs);
		painter.drawLine(i - 1, -specarr[i-1], i, -specarr[i]);
//		qDebug() << __FILE__ << __LINE__  << specarr[i];
		i++;
	}


//	painter.drawLine(0,0,this->width(),this->height());
//	qDebug() << __FILE__ << __LINE__ << this->width() << this->height();
	painter.end();
	update();

	//	Gdisp->setb(SPEC, specarr, 250);
}


