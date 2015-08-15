#include "fftdisplay.h"
// #include "qsstvglobal.h"
#include "math.h"
#include <QPainter>

#define FFTAVERAGING 0.1
#define VALAVG 0.02
#define FFTHIGH 3000
#define FFTLOW  200
#define FFTSPAN (FFTHIGH-FFTLOW)
#define FFTMAX 75.
#define RANGE 30.


fftDisplay::fftDisplay(QWidget *parent) : QLabel(parent)
{
  blockIndex=0;
  arMag=NULL;
  arMagAvg=NULL;
//  ready=FALSE;
  windowWidth=0;
  windowHeight=0;
  im=NULL;
  fftArray=NULL;
//  windowWidth=294;windowHeight=112;
//  im=new QImage( windowWidth,windowHeight,QImage::Format_RGB32);
  setSize(120,180);

  out=NULL;
  dataBuffer=NULL;
  showWaterfall=FALSE;
  fftMax=FFTMAX;
  range=RANGE;
}

void fftDisplay::setSize(int w,int h)
{
  windowWidth=w;
  windowHeight=h;
  if(im)
    {
      *im=im->scaled(QSize(windowWidth, windowHeight));
     }
  else
    {
      im=new QImage( windowWidth,windowHeight,QImage::Format_RGB32);
      im->fill(Qt::black);
    }
  if(fftArray) delete fftArray;
  fftArray=new QPolygon(windowWidth);

  if(arMag) delete []arMag;
  arMag=new double[windowWidth];
  if(arMagAvg) delete []arMagAvg;
  arMagAvg=new double[windowWidth];
  for(int i=0;i<windowWidth;i++) arMag[i]=0.;
  for(int i=0;i<windowWidth;i++) arMagAvg[i]=0.;
}

fftDisplay::~fftDisplay()
{
//  ready=FALSE;
  delete im;
  if(fftArray) delete fftArray;
  fftw_destroy_plan(plan);
  if(out) fftw_free(out);
  if(dataBuffer) fftw_free(dataBuffer);
  if (arMag) delete [] arMag;
  if(arMagAvg) delete []arMagAvg;
}

void fftDisplay::init(int size,int nblocks,int isamplingrate)
{
  int i;
  windowSize=size;
  fftLength=windowSize*nblocks;
  blocks=nblocks;
  createHamming();
  samplingrate=isamplingrate;
  //prepare fft
  out = (double *)fftw_malloc(fftLength * sizeof(double));
  dataBuffer  = (double *)fftw_malloc(fftLength * sizeof(double));
  arMag=new double[windowSize];
  for(i=0;i<windowWidth;i++) arMag[i]=0.;
  arMagAvg=new double[windowSize];
  for(i=0;i<windowWidth;i++) arMagAvg[i]=0.;
  fftArray=new QPolygon(windowWidth);
   // create the fftw plan
  plan = fftw_plan_r2r_1d(fftLength, dataBuffer, out, FFTW_R2HC, FFTW_ESTIMATE);
  update();
  QLabel::update();

}

void fftDisplay::createHamming()
{
  int i;
  hammingBuffer= new double[fftLength];
  for(i=0;i<fftLength;i++)
    {
      hammingBuffer[i]=0.54-(0.46*cos(2*M_PI*((double)i/((double)(fftLength-1)))));
    }

}

void fftDisplay::realFFT(short int *data)
{
  int i,j;
  for(i=0,j=windowSize*blockIndex;i<windowSize;i++,j++)
  {
    dataBuffer[j]=(double)data[i];
  }
  doFFT();
}

void fftDisplay::realFFT(float *data)
{
  int i,j;
  double max=0;
  for(i=0,j=windowSize*blockIndex;i<windowSize;i++,j++)
  {
    dataBuffer[j]=(double)data[i];
    if(dataBuffer[j]>max) max=dataBuffer[j];
  }
  doFFT();
}

void fftDisplay::realFFT(double *data)
{
  int i,j;
  for(i=0,j=windowSize*blockIndex;i<windowSize;i++,j++)
  {
    dataBuffer[j]=data[i];

  }
  doFFT();
}

void fftDisplay::doFFT()
{
  int i;
  double val;
  double re,imag,tmp,step;
  double maxTmp=0;
  double magTmp;
  QColor c;
  blockIndex++;
  if(blockIndex<blocks) return;
  blockIndex=0;
//   apply hamming
//  if(!ready) return;
  for(i=0;i<fftLength;i++)
    {

      dataBuffer[i]*=hammingBuffer[i];
      if(dataBuffer[i]>maxTmp) maxTmp=dataBuffer[i];
    }

  fftw_execute(plan);
  step=(double)samplingrate/(double)fftLength;  //freq step per bin
  arMag[0]=0;
  arMagAvg[0]=0;
  for(i=0;i<windowWidth;i++)
    {
      int idx=rint((FFTLOW+(i*FFTSPAN)/windowWidth)/step);
      re=out[idx]/fftLength;
      imag=out[fftLength-idx]/fftLength;
      tmp=10*log10(2*(re*re+imag*imag))-55;
      arMag[i]=tmp;
      if(arMagAvg[i]<tmp) arMagAvg[i]=tmp;
      else arMagAvg[i]=arMagAvg[i]*(1-VALAVG)+VALAVG*tmp;
    }
  maxMagnitude=fftMax;

  for(i=0;i<windowWidth;i++)
    {
      magTmp=arMagAvg[i];
      if(magTmp<(maxMagnitude-range)) magTmp=maxMagnitude-range;
      fftArray->setPoint(i,i,((windowHeight)*(maxMagnitude-magTmp))/range);
    }
  memmove(im->scanLine(1),im->scanLine(0),(windowWidth*(windowHeight-2))*sizeof(uint));
  uint *ptr=(uint *)im->scanLine(0);
  for(i=0;i<windowWidth;i++)
    {
      //              arMag[i]=fftMin;
      if(arMag[i]>fftMax) arMag[i]=fftMax;
      if(arMag[i]<fftMax-range) arMag[i]=fftMax-range;
      val=((arMag[i]-(fftMax-range))/range); // value in range 0-1
      if(val<0) val=0;
      if(val>1) val=1;
      //              tmp=rint(180*((double)i)/((double)FFTSPAN));
      //              c.setHsv(240-val*240,255,255);
      c.setHsv(240-val*60,255,val*255);
      //      c.setRgb(val*255,val*255,val*255);
      ptr[i]=c.rgb();
      //              ptr[i]=(uint) val*256+255;
    }
  update();
}


void fftDisplay::paintEvent(QPaintEvent *p)
{


  QPen pn;

//  if(!ready) return;
  QPainter painter(this);
  //  painter.setWindow(0,0,windowWidth,windowHeight);
  if((windowWidth!=width())||(windowHeight!=height()))
    {
      setSize(width(),height());
      return;
    }
  //  painter.setRenderHint(QPainter::Antialiasing);
  if(!showWaterfall)
    {
      pn.setColor(Qt::red);
      pn.setWidth(1);
      painter.setPen(pn);
      painter.drawLine((((1200-FFTLOW)*windowWidth)/FFTSPAN),0,(((1200-FFTLOW)*windowWidth)/FFTSPAN),windowHeight);
      painter.drawLine((((1500-FFTLOW)*windowWidth)/FFTSPAN),0,(((1500-FFTLOW)*windowWidth)/FFTSPAN),windowHeight);
      painter.drawLine((((2300-FFTLOW)*windowWidth)/FFTSPAN),0,(((2300-FFTLOW)*windowWidth)/FFTSPAN),windowHeight);
      pn.setColor(Qt::green);
      painter.setPen(pn);
      painter.drawPolyline(*fftArray);

    }
  else
    {
      painter.drawImage(0,0,*im);
    }

  QLabel::paintEvent(p);
}



void fftDisplay::mousePressEvent( QMouseEvent *e )
{
  if (e->button() == Qt::LeftButton)
    {
      showWaterfall=!showWaterfall;
    }
}

