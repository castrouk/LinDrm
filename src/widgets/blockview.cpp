#include "blockview.h"
#include "ui_blockview.h"
#include <QPainter>

blockView::blockView(QWidget *parent) :
  QFrame(parent),
  ui(new Ui::blockView)
{
  maxBlocks=40;
  colFail=QColor(Qt::red);
  colOK=QColor(Qt::green);
  ui->setupUi(this);
}

blockView::~blockView()
{
  delete ui;
}


void blockView::paintEvent(QPaintEvent *)
{
  int i;
  QRectF rct;
  QPainter painter(this);
  painter.setPen(QPen(colFail, 1, Qt::SolidLine));
  painter.setBrush(QBrush(colFail));
  painter.setRenderHint(QPainter::Antialiasing);
  rct=QRectF(0,0,maxBlocks-1,30);
  painter.setWindow(0, 0, maxBlocks-1,30);
  painter.drawRect(rct);
  painter.setPen(QPen(colOK, 1, Qt::SolidLine));
  for(i=0;i<blockList.count();i++)
    {
      painter.drawLine(blockList.at(i),0,blockList.at(i),29);
    }
}

void blockView::setColorFail(QColor color)
{
  colFail = color;
  update();
}

void blockView::setColorOK(QColor color)
{
  colOK = color;
  update();
}

 void blockView::setBlocks(QList<unsigned short> blkList)
 {
   blockList=blkList;
	update();
 }
