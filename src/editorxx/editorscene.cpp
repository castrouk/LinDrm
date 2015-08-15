/***************************************************************************
 *   Copyright (C) 2000-2008 by Johan Maes                                 *
 *   on4qz@telenet.be                                                      *
 *   http://users.telenet.be/on4qz                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
// #include <QtGui>
//#include <QtWidgets>

#include <qglobal.h>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
	#include <QtWidgets>
#else
	#include <QtGui>
#endif



#include "editorscene.h"
#include "gradientdialog.h"
// #include "qsstvglobal.h"
#include "ui_textform.h"
// #include "gallerywidget.h"
#include "utils/qjp2io.h"

editorScene::editorScene(QGraphicsView *parent)  : QGraphicsScene(parent)
{
	contextMenu=new QMenu();
	arrange = new QMenu( "Arrange");
	arrange->setTearOffEnabled(TRUE);
	arrange->addAction("Forward",this,SLOT(slotSendForward()));
	arrange->addAction("Backward",this,SLOT(slotSendBackward()));
	arrange->addAction("Bring to front",this,SLOT(slotBringToFront()));
	arrange->addAction("Send to back",this,SLOT(slotSendToBack()));
	contextMenu->addMenu(arrange);
	contextMenu->addSeparator();
	contextMenu->addAction("Change Text",this,SLOT(slotChangeText()));
//	contextMenu->addAction("Copy",this,SLOT(slotCopy()));
//	contextMenu->addAction("Paste",this,SLOT(slotPaste()));
	contextMenu->addSeparator();
	contextMenu->addAction("Delete",this,SLOT(slotDeleteItem()));
	contextMenu->addAction("Expand",this,SLOT(slotExpand()));
	contextMenu->addAction("Lock",this,SLOT(slotLock()));
	contextMenu->addAction("Unlock",this,SLOT(slotUnlock()));
	zMax=0;
	pasted=FALSE;
	copyItem=NULL;
	mode=MOVE;
	imageType=NONE;
  localImage=NULL;
  rotate=0;
  vShear=0;
  hShear=0;
  border=QRectF(0,0,0,0);
  borderItemPtr=NULL;
  penWidth=1;
}

editorScene::~editorScene()
{
  if(localImage!=NULL) delete localImage;
	if((!pasted) &&(copyItem!=NULL)) delete copyItem;
  delete arrange;
  delete contextMenu;
}


bool editorScene::load(QFile &f)
{
		qWarning("TODO: %s	%d",__FILE__,__LINE__);
//  bool borderSet=FALSE;
//	QImage im;
//	itemBase *item;
//	quint32 magic;
//	QString version;
//	quint16 streamVersion;
//	int type;
//	if(!f.open(QIODevice::ReadOnly)) return FALSE;
//	QDataStream str(&f);
//  str >> magic;
	
//  if (magic != MAGICNUMBER)
//		{
//			//try to load an image
//			f.reset();
//			if(im.load(&f,0))
//				{
//          addToLog("image loaded",LOGEDIT);
//					imageType=FLATIMAGE;
//					setImage(&im);
//          border=QRect(0,0,im.width(),im.height());
//          borderSet=TRUE;
//					f.close();
//					return TRUE;
//				}
//			else
//				{
//          addToLog("image failed to load",LOGEDIT);
//					f.close();
//					return FALSE;
//				}
//		}
//	imageType=TEMPLATE;
//	str >> version;  // at this moment we do not use the version
//	str >> streamVersion;
//	str.setVersion(streamVersion);
//	while (!str.atEnd())
//		{
//			str >> type;
//			switch (type)
//				{
//					case itemBase::RECTANGLE:
//						item=new itemRectangle(contextMenu);
//					break;
//					case itemBase::ELLIPSE:
//						item=new itemEllipse(contextMenu);
//					break;
//					case itemBase::LINE:
//						item=new itemLine(contextMenu);
//					break;
//					case itemBase::TEXT:
//						item=new itemText(contextMenu);
//					break;
//					case itemBase::IMAGE:
//						item=new itemImage(contextMenu);
//					break;
//          case itemBase::REPLAY:
//            item=new itemReplayImage(contextMenu);
//          break;
//        case itemBase::SBORDER:
//            borderSet=TRUE;
//            item=new itemImage(contextMenu);
//            item->load(str);
//            border=item->rect();
//            delete item;
//            continue;
//        break;
//					default:
//            addToLog("Error in datastream",LOGEDIT);
//						f.close();
//						return FALSE;
//					break;
//				}
//			item->load(str);
//      addItem(item);

////      item->setTransform();
//      //itemSetup(item);
//		}
//  //border=sceneRect();
//  optimizeDepth();
//  if(!borderSet) border=QRectF(0,0,320,256);
//  addToLog(QString("border position %1,%2 size: %3 x %4 border set=%5")
//               .arg(border.topLeft().x()).arg(border.topLeft().y())
//               .arg(border.width()).arg(border.height()).arg(borderSet),LOGEDIT);
//  f.close();
//  setSceneRect(border);
//	return TRUE;
}

QImage *editorScene::renderImage(int w,int h)
{
  clearSelection();
  if (localImage!=NULL) delete localImage;
  //border=sceneRect();
  if(w==0)
    {
      localImage=new QImage(border.width(),border.height(),QImage::Format_ARGB32);
    }
  else
    {
      localImage=new QImage(w,h,QImage::Format_ARGB32);
    }
//  addToLog(QString("editorScene: pre-render size: %1 x %2").arg(localImage->size().width()).arg(localImage->size().height()),LOGEDIT);
  QPainter painter(localImage);
  painter.setRenderHint(QPainter::Antialiasing);
  //setSceneRect(0,0,localImage->width(),localImage->height());
  localImage->fill(0);
  render(&painter);
//  addToLog(QString("editor: post-render size: %1 x %2").arg(localImage->size().width()).arg(localImage->size().height()),LOGEDIT);
  return localImage;
}

void editorScene::flattenImage(int w,int h)
{
  if (localImage!=NULL) delete localImage;
  setSceneRect(border);
//  border=sceneRect();
  localImage=new QImage(w,h,QImage::Format_ARGB32);
  convertText();
  convertReplayImage();
  QPainter painter(localImage);
  painter.setRenderHint(QPainter::Antialiasing);
//  setSceneRect(0,0,localImage->width(),localImage->height());
  localImage->fill(0);
  render(&painter);
}


void editorScene::convertReplayImage()
{
		qWarning("TODO: %s	%d",__FILE__,__LINE__);
//    QString fn;
//    QImage im;
////  qDebug() << "last rx image"<<galleryWidgetPtr->getLastRxImage();
//    fn=galleryWidgetPtr->getLastRxImage();
//    if(fn.right(4).toUpper()==".JP2")
//    {
//        im=readJP2Image(fn);
//    }
//    else
//    {
//        im.load(fn);
//    }
//   if(im.isNull()) return;
//  //itemBase *it;
//  foreach(QGraphicsItem *t,items())
//    {
//      //it=qgraphicsitem_cast<itemBase *>(t);
//      if(t->type()==itemBase::REPLAY)
//        {
//          itemReplayImage *itt=qgraphicsitem_cast<itemReplayImage *>(t);
//          itt->setImage(im);
//        }
//    }
}

void editorScene::convertText()
{
  //itemBase *it;
  foreach(QGraphicsItem *t,items())
    {
    //  it=qgraphicsitem_cast<itemBase *>(t);
      if(t->type()==itemBase::TEXT)
        {
          itemText *itt=qgraphicsitem_cast<itemText *>(t);
          itt->setText(textConversion(itt->text()));
        }
    }
}

QString editorScene::textConversion(QString str)
{
  int i,j;
  QChar c;
  convertedText.clear();
  bool special=FALSE;
  for (i=0;i<str.length();i++)
  {
    if (special)
      {
        special=FALSE;
        c=str.at(i);
        if(c=='%')
        {
          convertedText.append('%');
          continue;
        }
        for (j=0;j<convertList.count();j++)
          {
            if(c==convertList.at(j).tag)
            {
              convertedText.append(convertList.at(j).replacement);
            }
          }
      }
    else
      {
        if(str.at(i)!='%') convertedText.append(str.at(i));
        else special=TRUE;
      }
  }
//  addToLog("converted text: "+convertedText,LOGEDIT);
  return convertedText;
}



bool editorScene::save(QFile &f,bool templ)
{
		qWarning("TODO: %s	%d",__FILE__,__LINE__);
//	QImage im(border.width(),border.height(),QImage::Format_ARGB32);
//	setSceneRect(border);
////  addToLog(QString("editorscene:save %1 x %2").arg(sceneRect().width()).arg(sceneRect().height()),LOGEDIT);
//	if(!templ)
//		{
//			QPainter painter(&im);
//      painter.setRenderHint(QPainter::Antialiasing);
//      render(&painter);
//			im.save(&f,"PNG");
//			return TRUE;
//		}
//	if(!f.open(QIODevice::WriteOnly)) return FALSE;
//	QDataStream str(&f);
//	str.setVersion(QDataStream::Qt_4_4);
//	 // Header with a "magic number" and a version
// 	str << (quint32) MAGICNUMBER;
//  str <<  CONFIGVERSION;
//  str << (quint16) QDataStream::Qt_4_4;
//	itemBase *it;
//	foreach(QGraphicsItem *t,items())
//		{
//			it=qgraphicsitem_cast<itemBase *>(t);
//      if(t->type()>itemBase::BASE)
//				{
//					it->save(str);
//				}
//		}
//	f.close();
	return TRUE;
}

void editorScene::setMode(eMode m)
{
    mode = m;
		if(mode==INSERT) clearSelection () ;
}

void editorScene::setItemType(itemBase::egraphType tp)
{
    itemType = tp;
}

void editorScene::apply(changeFlags cf)
{
	QPen p;
	itemBase *it;
	if(selectedItems().isEmpty()) return; // nothing to do 
	foreach(QGraphicsItem *t,selectedItems())
		{
				it=qgraphicsitem_cast<itemBase *>(t);
      if(cf & DFILLCOLOR)
				{
				 	it->setBrush(fillColor);
				}
      if(cf & DLINECOLOR)
				{
					p=it->pen();
					p.setColor(lineColor);
				 it->setPen(p);
				}
      if(cf & DPEN)
				{
					p=it->pen();
					p.setWidth(penWidth);
					it->setPen(p);
				}
      if(cf & DGRADIENT)
				{
					gradientDialog gd;
					sgradientParam tmp;
					tmp=gd.param();
					it->setGradient(tmp);
					it->update();
				}
      if(cf & DTRANSFORM)
				{
					it->setTransform(rotate,hShear,vShear);
				}
			if(t->type()==itemBase::TEXT)
				{
					itemText *itt=qgraphicsitem_cast<itemText *>(t);
          if(cf & DFONT) itt->setFont(font);
          if(cf & DTEXT) itt->setText(text);
				}
			it->update();
		}
}

void editorScene::clearAll()
{
	foreach(QGraphicsItem *t,items())
		{
      if((t->type()>itemBase::BASE) && (t->type()!=itemBase::SBORDER))
				{
					removeItem(t);
					delete t;
				}
		}
}


void editorScene::itemSetup(itemBase *item)
{
	QPen p;
	gradientDialog gd;
	sgradientParam tmp;
	tmp=gd.param();
	item->setGradient(tmp);
	item->setTransform(rotate,hShear,vShear);
	p=item->pen();
	p.setColor(lineColor);
	p.setWidth(penWidth);
	item->setPen(p);
	item->setBrush(fillColor);
	item->setZValue(zMax);
	zMax+=1;
	addItem(item);
}

void editorScene::setImage(QImage *im)
{
	itemBase *item;
	item=new itemImage(contextMenu);
	item->setImage(*im);
	item->setRect(0,0,im->width(),im->height());
	itemSetup(item);
	item->setPos(QPointF(0,0));
	item->setSelected(TRUE);
	emit changeSize(im->width(),im->height());
}

void editorScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
	itemBase *item;
	QImage im;
  if (mouseEvent->button() == Qt::LeftButton)
		{
			switch(mode)
				{
					case INSERT:
						switch(itemType)
							{
								case itemBase::RECTANGLE:
									item=new itemRectangle(contextMenu);
									itemSetup(item);
									item->setPos(mouseEvent->scenePos());
								break;
								case itemBase::LINE:
									item=new itemLine(contextMenu);
									itemSetup(item);
									item->setPos(mouseEvent->scenePos());
								break;
								case itemBase::ELLIPSE:
									item=new itemEllipse(contextMenu);
									itemSetup(item);
									item->setPos(mouseEvent->scenePos());
								break;
								case itemBase::TEXT:
									if (!text.isEmpty())
										{
											item=new itemText(contextMenu);
											item->setFont(font);
											item->setText(text);
											itemSetup(item);
											item->setPos(mouseEvent->scenePos());
										}
								break;
								case itemBase::IMAGE:
									if(im.load(fl))
										{
											item=new itemImage(contextMenu);
											item->setImage(im);	
											itemSetup(item);
											item->setPos(mouseEvent->scenePos());
										}
								break;
								case itemBase::REPLAY:
									item=new itemReplayImage(contextMenu);
									itemSetup(item);
									item->setPos(mouseEvent->scenePos());
								break;
                case itemBase::SBORDER:
								case itemBase::BASE:
								break;
							}
					break;
					case MOVE:
						if(!selectedItems().isEmpty())
							{
								item=qgraphicsitem_cast<itemBase *>(selectedItems().first());
							}
					break;
          case PICK:
          break;

				}
			}
		QGraphicsScene::mousePressEvent(mouseEvent);
}

void editorScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
 QGraphicsScene::mouseMoveEvent(mouseEvent);

}

void editorScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
	itemBase *item;
	if(mode==MOVE)
		{
			if(!selectedItems().isEmpty())
				{
					item=qgraphicsitem_cast<itemBase *>(selectedItems().first());
					emit itemSelected(item);
				}
		}
	else if(mode==PICK)
		{
			emit colorSelected(mouseEvent->scenePos());
			((QGraphicsView *)parent())->setCursor(Qt::ArrowCursor);
			
		}
	mode=MOVE;
	QGraphicsScene::mouseReleaseEvent(mouseEvent);
}


void editorScene::slotCopy()
{
	itemBase *item;
	if((!pasted) &&(copyItem!=NULL)) delete copyItem;
  if(selectedItems().isEmpty()) return; // nothing to do
	item=qgraphicsitem_cast<itemBase *>(selectedItems().first());
	makeCopy(item);
}

void editorScene::makeCopy(itemBase *it)
{
	itemBase *item=it;
	itemBase::egraphType type=(itemBase::egraphType)item->type();
	switch(type)
		{
			case itemBase::RECTANGLE:
				copyItem=new itemRectangle(item->getParam().menu);
			break;
			case itemBase::LINE:
				copyItem=new itemLine(item->getParam().menu);
			break;
			case itemBase::ELLIPSE:
				copyItem=new itemEllipse(item->getParam().menu);
			break;
			case itemBase::TEXT:
				copyItem=new itemText(item->getParam().menu);
			break;
			case itemBase::IMAGE:
				copyItem=new itemImage(item->getParam().menu);
			break;
    case itemBase::REPLAY:
      copyItem=new itemReplayImage(item->getParam().menu);
    break;
			default:
				return;
		}
	copyItem->setParam(item->getParam());
	copyItem->setPos(item->pos()+QPointF(10,10));
	pasted=FALSE;
}

void editorScene::slotPaste()
{
		clearSelection();
		copyItem->setZValue(zMax+1);
		zMax+=1;
		addItem(copyItem);
		pasted=TRUE,
		makeCopy(copyItem);
		clearSelection();
}

void editorScene::slotExpand()
{
	itemBase *it;
  if(selectedItems().isEmpty()) return; // nothing to do
	foreach(QGraphicsItem *t,selectedItems())
		{
			it=qgraphicsitem_cast<itemBase *>(t);
      if(it->type()!=itemBase::TEXT)
				{
					it->setRect(border);
					it->setPos(0,0);
				}
		}
}

void editorScene::slotChangeText()
{
  if(selectedItems().isEmpty()) return; // nothing to do
	itemText *item=qgraphicsitem_cast<itemText *>(selectedItems().first());
  if(!item)
    {
      return;
    }
	if(item->type()!=itemBase::TEXT) return;
	QDialog d(0);
	Ui::textForm t;
	t.setupUi(&d);
	t.lineEdit->setText(item->text());
	if(d.exec()==QDialog::Accepted)
		{
			item->setText(t.lineEdit->text());
		}
}


void editorScene::slotDeleteItem()
{
  if(selectedItems().isEmpty()) return; // nothing to do
	foreach(QGraphicsItem *t,selectedItems())
		{
			removeItem(t);
			delete t;
		}
}


void editorScene::slotLock()
{
	itemBase *it;
  if(selectedItems().isEmpty()) return; // nothing to do
	foreach(QGraphicsItem *t,selectedItems())
		{
			it=qgraphicsitem_cast<itemBase *>(t);
			it->setLocked(TRUE);
		}
}

void editorScene::slotUnlock()
{
	
	 itemBase *it;
   if(selectedItems().isEmpty()) return; // nothing to do
	foreach(QGraphicsItem *t,selectedItems())
		{
			it=qgraphicsitem_cast<itemBase *>(t);
			it->setLocked(FALSE);
		}
}


void editorScene::slotBringToFront()
{
  if(selectedItems().isEmpty()) return; // nothing to do
	foreach(QGraphicsItem *t,selectedItems())
		{
			zMax+=1;
			t->setZValue(zMax);
    }
	optimizeDepth();
}

void editorScene::slotSendToBack()
{
  if(selectedItems().isEmpty()) return; // nothing to do
	foreach(QGraphicsItem *t,selectedItems())
		{
			t->setZValue(0.5);
		}
	optimizeDepth();
}


void editorScene::slotSendBackward()
{
  if(selectedItems().isEmpty()) return; // nothing to do
	foreach(QGraphicsItem *t,selectedItems())
		{
			t->setZValue(t->zValue()-1.5);
		}
	optimizeDepth();
}

void editorScene::slotSendForward()
{
  if(selectedItems().isEmpty()) return; // nothing to do
	foreach(QGraphicsItem *t,selectedItems())
		{
			t->setZValue(t->zValue()+1.5);
		}
	optimizeDepth();
}

void editorScene::optimizeDepth()
{
  itemBase *it;
	zMax=items().count();
	qreal i=0;
	foreach(QGraphicsItem *t,items(itemsBoundingRect ()))
		{
      it=qgraphicsitem_cast<itemBase *>(t);
      if(it->type()==itemBase::SBORDER)
       {
          it->setZValue(0.1);
        }
      else if(it->type()>itemBase::BASE)
				{
          it->setZValue(zMax-i);
					i+=1;
				}
//      addToLog(QString("optimize_1 type=%1 pos=%2,%3 resctPos=%4,%5").arg(it->getTypeStr()).arg(t->pos().x()).arg(t->pos().y()).arg(it->rect().x()).arg(it->rect().y()),LOGEDIT);
//      addToLog(QString("Boundingrect t=%1 %2,%3 %4,%5").arg(it->getTypeStr()).arg(it->boundingRect().x()).arg(it->boundingRect().y()).arg(it->boundingRect().width()).arg(it->boundingRect().height()),LOGEDIT);
    }
  //optimize position

}

void editorScene::addBorder(int w,int h)
{
  if (borderItemPtr==NULL)
    {
      borderItemPtr=new itemBorder(contextMenu);
    }
  itemSetup(borderItemPtr);
  borderItemPtr->setPos(0,0);
  borderItemPtr->setRect(0,0,w,h);
  border=QRectF(0,0,w,h);
  slotSendToBack();
}

void editorScene::overlay(QImage *ima)
{
  clearSelection();
//  addToLog(QString("overlay: before number of items: %1").arg(items().count()),LOGEDIT);
//  itemBase *item;
//  item=new itemImage(contextMenu);
//  item->setImage(*ima);
//  itemSetup(item);
//  item->setPos(0,0);
//  item->setRect(0,0,ima->width(),ima->height());
//  item->setSelected(TRUE);
//  slotSendToBack();
//  clearSelection();
//  addToLog(QString("overlay: after number of items: %1").arg(items().count()),LOGEDIT);
 // flattenImage(ima->width(),ima->height());
  setSceneRect(border);
//  border=sceneRect();
  if (localImage!=NULL) delete localImage;
  localImage=new QImage(ima->copy());
//  localImage=ima;
  convertText();
  convertReplayImage();
  QPainter painter(localImage);
  painter.setRenderHint(QPainter::Antialiasing);
//  setSceneRect(0,0,localImage->width(),localImage->height());
//  localImage->fill(0);
  render(&painter,QRectF(),QRectF(),Qt::IgnoreAspectRatio);
}






