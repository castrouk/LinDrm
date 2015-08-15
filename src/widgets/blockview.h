#ifndef BLOCKVIEW_H
#define BLOCKVIEW_H

#include <QFrame>

namespace Ui {
  class blockView;
  }

class blockView : public QFrame
{
  Q_OBJECT
  
public:
  explicit blockView(QWidget *parent = 0);
  ~blockView();
  void setColorFail(QColor color);
  void setColorOK(QColor color);
  void setMaxBlocks(int mb) {maxBlocks=mb;}
  void setBlocks(QList<short unsigned int> blkList);

protected:

    void paintEvent(QPaintEvent *);
  
private:
  Ui::blockView *ui;
  QColor colFail;
  QColor colOK;
  QList<short unsigned int> blockList;
  short int maxBlocks;
};

#endif // BLOCKVIEW_H
