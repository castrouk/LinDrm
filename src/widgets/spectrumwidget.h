#ifndef SPECTRUMWIDGET_H
#define SPECTRUMWIDGET_H

#include <QFrame>

namespace Ui {
  class spectrumWidget;
  }

class spectrumWidget : public QFrame
{
  Q_OBJECT
  
public:
  spectrumWidget(QWidget *parent = 0);
  ~spectrumWidget();
  void init(int size,int slices,int isamplingrate);
  void realFFT(short int *iBuffer);
  void realFFT(float *iBuffer);
  void realFFT(double *iBuffer);
  void readSettings();
  void writeSettings();
  void displayWaterfall(bool wf);
private slots:
  void slotMaxDbChanged(int mb);
  void slotRangeChanged(int rg);
  void getParams();
  void setParams();
  
private:
  Ui::spectrumWidget *ui;
  int maxdb;
  int range;
};

#endif // SPECTRUMWIDGET_H
