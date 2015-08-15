#ifndef BSRFIX_H
#define BSRFIX_H

#include <QByteArray>
#include <QString>

#include "runtx.h"

class bsrFix
{
public:
	bsrFix();
	~bsrFix();
	bool prepareFIX(QByteArray bsrByteArray);
//	txSession *getSessionPtr(uint transportID);
	bool getSessionPtr(uint transportID);
};

#endif // BSRFIX_H
