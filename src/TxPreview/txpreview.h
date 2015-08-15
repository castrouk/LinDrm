#ifndef TXPREVIEW_H
#define TXPREVIEW_H

#include <QObject>
#include "droparea.h"

class TxPreview : public DropArea
{
	Q_OBJECT
public:
	TxPreview();
	~TxPreview();
};

#endif // TXPREVIEW_H
