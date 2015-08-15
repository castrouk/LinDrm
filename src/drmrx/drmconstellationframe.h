#ifndef DRMCONSTELLATIONFRAME_H
#define DRMCONSTELLATIONFRAME_H

#include <QFrame>

enum econstellation {MT,FAC,MSC};

namespace Ui {
class drmConstellationFrame;
}

class drmConstellationFrame : public QFrame
{
  Q_OBJECT
  
public:
  explicit drmConstellationFrame(QWidget *parent = 0);
  ~drmConstellationFrame();
  void setConstellation(econstellation constellation);
  void clearConstellation();
  void drawGrid(QPainter *);
private:
  Ui::drmConstellationFrame *ui;
 void paintEvent (QPaintEvent *);
 float ConstellationArray[2 * 2959];
 int lmsc;
 int m_constellation = 0;
 QString constType;
};

#endif // DRMCONSTELLATIONFRAME_H
