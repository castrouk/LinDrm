#ifndef QPFNAME_H
#define QPFNAME_H
//
// Parse QString file names:
//
// 1: Return Directory PATH with out file name
// 2: Return file name with out Directory PATH
// 3: Return file name with out EXT
// 4: Return EXT name with out file name
// 5: Strip of leading "file://","http://" etc. from URL
// EXAMPLE Qpfn::getFname_Ext(str) - getFname + Ext
//
#include <QString>

//
class Qpfn
{

public:
	Qpfn()
	{};

// 1: Return Directory PATH with out file name
	static QString getDir(QString &s)
	{
		return s.left(s.lastIndexOf('/'));
	}
// 2: Return file name.ext, with out Directory PATH
	static QString getFname_Ext(QString &s)
	{
		return s.section('/', -1);
	}
// 3: Return path & file name with out EXT
	static QString stripExt(QString &s)
	{
		return s.left(s.lastIndexOf('.'));
	//	return s;
	}
// 4: Return file name - path and ext	
	static QString getFname(QString &str)
	{
		QString s = getFname_Ext(str);
		s = s.left(s.lastIndexOf('.'));
		return s;
	}
// 5: Return EXT name with out file name
	static QString getExt(QString &s)
	{
		return s.section('.', -1);
	}
// 6: Strip of leading "file://","http://" etc. from URL
	static QString stripUrl(QString &str)
	{
		QString s = str.section("//", -1);
		return s;
	}

};
#endif // QPFNAME_H

