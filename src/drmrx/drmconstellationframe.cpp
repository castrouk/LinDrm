#include "drmconstellationframe.h"
#include "ui_drmconstellationframe.h"
#include <QPainter>
#include <QDebug>
#include "drm.h"
#include "math.h"
#include "qsstvdefs.h"


extern float MSC_cells_sequence[2 * 2959];
extern int lMSC;
extern bool MSCAvailable;

extern int lFAC;
extern float FAC_cells_sequence[200];
extern bool FACAvailable;

//#define CSTRANGE 1.5
//#define CSTSPAN 3.
#define CSTRANGE 1.4
#define CSTSPAN 2.8

drmConstellationFrame::drmConstellationFrame(QWidget *parent) :
  QFrame(parent),
  ui(new Ui::drmConstellationFrame)
{
  ui->setupUi(this);
  m_constellation = 0;
  lmsc=0;
}

drmConstellationFrame::~drmConstellationFrame()
{
  delete ui;
}


void drmConstellationFrame::drawGrid(QPainter *p)
{
	if(m_constellation == MT)
		return;

float TOP = 0;
float BOTOM = height();
float LEFT = 0;
float RIGHT = width();
	// setWindow(int x, int y, int width, int height)
	p->setWindow( TOP + 13.5, LEFT + 13.5, RIGHT - 27.0 , BOTOM - 27.0 );


	extern int msc_mode_new;
	int a = msc_mode_new;
	float inc = width() / 9.0f;

	float h = 0 - (inc / 2.0f);
	float v = height() + (inc / 2.0f );

//	qDebug() << __FILE__ << __LINE__ << inc << width() << height();

	p->setPen (QPen (Qt::black));
	p->drawText(35,((height()/2) + 30 ),constType);

	p->setPen (QPen (Qt::green, 1));
	switch(m_constellation)
	{
	case FAC:
		p->drawLine(width()/2, 0, width()/2, height()); // vert
		p->drawLine(0,height()/2, width(), height()/2); // horz
		break;
	case MSC:

		for ( int i = 0; i < 9; i++)
		{
			h += inc;
			v -= inc;
			if(!(i & a))
			{
//				qDebug() << __FILE__ << __LINE__ << i;
				p->drawLine(h, TOP, h, BOTOM);
				p->drawLine(LEFT, v, RIGHT, v);
			}
		}
		break;
	default:
		break;
	}
}

void drmConstellationFrame::paintEvent (QPaintEvent *e)
{
    int i,posx,posy;
    QPainter qpainter (this);
	 drawGrid(&qpainter);
	 qpainter.setPen (QPen (Qt::yellow, 1));
	 for(i=0;i<lmsc/2;i++)
    {
        posx=rint(((ConstellationArray[2*i]+CSTRANGE)/CSTSPAN)*(float)contentsRect().width());
        posy=contentsRect().height()-rint(((ConstellationArray[2*i+1]+CSTRANGE)/CSTSPAN)*(float)contentsRect().height());
		  qpainter.drawEllipse(posx,posy,1,1);
    }
  QFrame::paintEvent(e);
 }

void drmConstellationFrame::setConstellation(econstellation constellation)
{
	m_constellation = constellation;
//	drawGrid(constellation);
  if (MSCAvailable && constellation==MSC)
    {
	  constType = "MSC";
      lmsc=lMSC;
//		drawGrid(constellation);
		for (int i=0; i < lMSC; i++)
        {
          ConstellationArray[i]=MSC_cells_sequence[i];
        }
      update();
      MSCAvailable=FALSE;
    }
  if (FACAvailable  && constellation==FAC)
    {
	  constType = "FAC";
//	  drawGrid(MSC);
		lmsc=lFAC;
      for (int i=0; i < lmsc; i++)
        {
          ConstellationArray[i]=FAC_cells_sequence[i];
        }
      update();
      FACAvailable=FALSE;
    }
}

void drmConstellationFrame::clearConstellation()
{
  lmsc=0;
  update();
}
