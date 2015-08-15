#ifndef FFTDISPLAY_H
#define FFTDISPLAY_H
#include <QtGui>
#include <QLabel>
#include "qsstvdefs.h"
#include "fftw3.h"
#include <qpolygon.h>
#include <QImage>


class fftDisplay : public QLabel
{
  Q_OBJECT
  
public:
  explicit fftDisplay(QWidget *parent = 0);
  ~fftDisplay();

  void init(int size,int slices,int isamplingrate);
  void realFFT(short int *iBuffer);
  void realFFT(float *iBuffer);
  void realFFT(double *iBuffer);
  void setMaxDb(int mb){fftMax=mb;}
  void setRange(int rg) {range=rg;}
  void displayWaterfall(bool wf) {showWaterfall=wf;}

private:
  void createHamming();
  void doFFT();
  void paintEvent(QPaintEvent *p);
  void mousePressEvent( QMouseEvent *e );
  double *hammingBuffer;
  double *dataBuffer;
  int windowSize;
  int fftLength;
  int samplingrate;
  unsigned int blocks;
  unsigned int blockIndex;
  fftw_plan plan;
  double *arMag;
  double *arMagAvg;
  double *out;
  double maxMagnitude;
  QPolygon *fftArray;
//  bool ready;
  bool showWaterfall;
  double fftMax;
  double fftMin;
  double range;
  QImage *im;
  int windowWidth;
  int windowHeight;
  void setSize(int w,int h);
  QMutex mutex;
};

#endif // FFTDISPLAY_H
