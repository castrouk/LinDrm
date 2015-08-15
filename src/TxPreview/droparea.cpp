#include "droparea.h"
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QUrl>
//
DropArea::DropArea(QWidget *parent)
		: QLabel(parent)
{
//	setMinimumSize(200, 200);
//	setFrameStyle(QFrame::Sunken | QFrame::StyledPanel);
//	setAlignment(Qt::AlignCenter);
	setAcceptDrops(true);
	setAutoFillBackground(true);
	clear();
	// TODO
}

void DropArea::dragEnterEvent(QDragEnterEvent *event)
{
//	setText(tr("<drop content>"));
//	setBackgroundRole(QPalette::Highlight);

	event->acceptProposedAction();
//	emit changed(event->mimeData());
}

void DropArea::dragMoveEvent(QDragMoveEvent *event)
{
	// qWarning("TODO: %s	%d", __FILE__, __LINE__);
	event->acceptProposedAction();
}

void DropArea::dropEvent(QDropEvent *event)
{
	const QMimeData *mimeData = event->mimeData();
	if (mimeData->hasImage())
	{
		setPixmap(qvariant_cast<QPixmap>(mimeData->imageData()));
	}
	else if (mimeData->hasHtml())
	{
		setText(mimeData->html());
		setTextFormat(Qt::RichText);
	}
//	else if (mimeData->hasText())
//	{
//			emit pic_changed(mimeData->text());
//	}
	else if (mimeData->hasUrls())
	{
		QList<QUrl> urlList = mimeData->urls();
		QString text;
		for (int i = 0; i < urlList.size() && i < 32; ++i)
		{
			QString url = urlList.at(i).path();
			text += url + QString("\n");
		}
		emit pic_changed(text.simplified());

//		setText(text);
	}
	else
	{
//		qWarning("TODO: %s	%d", __FILE__, __LINE__);
		setText(tr("Cannot display data"));
	}
//	qWarning("TODO: %s	%d", __FILE__, __LINE__);
}

void DropArea::dragLeaveEvent(QDragLeaveEvent *event)
{
	clear();
	event->accept();
}

void DropArea::clear()
{
//	setText(tr("<drop content>"));
//	setBackgroundRole(QPalette::Dark);

//	emit changed();
}



//
