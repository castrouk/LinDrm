#ifndef DRMSTATUSFRAME_H
#define DRMSTATUSFRAME_H

#include <QFrame>
#include <QPixmap>
#define DRM
#include "externs.h"

class demodulator;

namespace Ui {
class drmStatusFrame;
}

class drmStatusFrame : public QFrame
{
  Q_OBJECT
  
public:
  explicit drmStatusFrame(QWidget *parent = 0);
  ~drmStatusFrame();
  void init();
  void setStatus();

private:
  Ui::drmStatusFrame *ui;
  void paintEvent (QPaintEvent *);
  QString mode;
  float bandwidth;
  QString interleave;
  QString protection;
  int qam;
  QString call;
  QPixmap *greenPXM;
  QPixmap *redPXM;
  QPixmap *yellowPXM;
  int currentSegment;
  int transportID;
  int totalSeg;
  int rxSeg;
};

QString modeToString(uint mode);


#endif // DRMSTATUSFRAME_H
