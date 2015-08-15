#ifndef DROPAREA_H
#define DROPAREA_H

#include <qglobal.h>

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
	#include <QtWidgets>
#else
	#include <QtGui>
#endif


//
#include <QLabel>
#include <QMimeData>
//
class DropArea : public QLabel
{
Q_OBJECT
public:
	DropArea(QWidget *parent = 0);
 
 public slots:
     void clear();

 signals:
     void changed(const QMimeData *mimeData = 0);
     void pic_changed(const QString &str);

 protected:
     void dragEnterEvent(QDragEnterEvent *event);
     void dragMoveEvent(QDragMoveEvent *event);
     void dragLeaveEvent(QDragLeaveEvent *event);
     void dropEvent(QDropEvent *event);


 private:
     QLabel *label;
	
};
#endif
