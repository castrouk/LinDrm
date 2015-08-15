/**************************************************************************
*   Copyright (C) 2000-2012 by Johan Maes                                 *
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

#include "supportfunctions.h"
#include <QDateTime>
#include <QDebug>
#include <stdarg.h>
// #include "qsstvglobal.h"


QString start("");

bool getValue(int &val, QLineEdit* input)
{
	bool ok;
	QString s;
	s=input->text();
	val=s.toInt(&ok,0); // allow ayutomatic conversion from hex to decimal in the classic C++ way : 0x is hex other are decimal
	return ok;
}

bool getValue(double &val, QLineEdit* input)
{
	bool ok;
	QString s;
	s=input->text();
	val=s.toDouble(&ok);
	return ok;
}

bool getValue(int &val, QString input)
{
	bool ok;
	val=input.toInt(&ok);
	return ok;
}
bool getValue(double &val, QString input)
{
	bool ok;
	val=input.toDouble(&ok);
	return ok;
}

void getValue(bool &val, QCheckBox *input)
{
	val=input->isChecked();
}

void getValue(int &val, QSpinBox *input)
{
	val=input->value();
}

void getValue(uint &val, QSpinBox *input)
{
  val=input->value();
}

void getValue(QString &s, QLineEdit *input)
{
	s=input->text();
}

void getValue(QString &s, QPlainTextEdit *input)
{
  s=input->toPlainText();
}



void getValue(int &s, QComboBox *input)
{
	s=input->currentText().toInt();
}

void getIndex(int &s, QComboBox *input)
{
  s=input->currentIndex();
}

void getValue(QString &s, QComboBox *input)
{
	s=input->currentText();
}

void getValue(int &s, QButtonGroup *input)
{
	s=input->checkedId();
}

void getValue(bool &s, QRadioButton *input)
{
	s=input->isChecked();
}

void getValue(int &val, QSlider *input)
{
  val=input->value();
}

void setValue(int val, QLineEdit* output)
{
	output->setText(QString::number(val));
}
 
void setValue(double val, QLineEdit* output)
{
	output->setText(QString::number(val));
}
/**
	\brief sets double number in a QlineEdit
	\param val  the value to set
	\param output pointer to QLineEdit
	\param prec the required precision
*/
 
void setValue(double val, QLineEdit* output,int prec)
{
	output->setText(QString::number(val,'g',prec));
}

void setValue(bool val, QCheckBox *input)
{
	input->setChecked(val);
}

void setValue(int val, QSpinBox *input)
{
	input->setValue(val);
}

void setValue(uint val, QSpinBox *input)
{
  input->setValue(val);
}

void setValue(QString s, QLineEdit *input)
{
	input->setText(s);
}

void setValue(QString s, QPlainTextEdit *input)
{
  input->setPlainText(s);
}

void setValue(int s, QComboBox *input)
{
	int i;
	for(i=0;i<input->count();i++)
		{
			if(input->itemText(i).toInt()==s)
				{
					input->setCurrentIndex(i);
					return;
				}
		}
	input->setCurrentIndex(0);
}

void setIndex(int s, QComboBox *input)
{
  input->setCurrentIndex(s);
}

void setValue(QString s, QComboBox *input)
{
	int i;
	for(i=0;i<input->count();i++)
		{
			if(input->itemText(i)==s)
				{
					input->setCurrentIndex(i);
					return;
				}
		}
	input->setCurrentIndex(0);
}

void setValue(int s, QButtonGroup *input)
{
	input->button(s)->setChecked(TRUE);
}

void setValue(bool s, QRadioButton *input)
{
	input->setChecked(s);
}

void setValue(int val, QSlider *input)
{
  input->setValue(val);
}




bool browseGetFile(QLineEdit *le,QString deflt, const QString &filter)
{
    dirDialog d((QWidget *)le,"Browse",TRUE);
    d.show();
	QString s=d.openFileName(deflt,filter);
	if (s==QString::null) return FALSE;
	if (s.isEmpty()) return FALSE;
	le->setText(s);
	return TRUE;
}

bool browseSaveFile(QLineEdit *le,QString deflt,const QString &filter)
{
    dirDialog d((QWidget *)le,"Browse",TRUE);
    d.show();
	QString s=d.saveFileName(deflt,filter,"");
	if (s==QString::null) return FALSE;
	if (s.isEmpty()) return FALSE;
	le->setText(s);
	return TRUE;
}

bool browseDir(QLineEdit *le,QString deflt,const QString &filter)
{
    dirDialog d((QWidget *)le,"Browse",TRUE);
    d.show();
	QString s=d.openDirName(deflt,filter);
	if (s==QString::null) return FALSE;
	if (s.isEmpty()) return FALSE;
	le->setText(s);
	return TRUE;
}




dirDialog::dirDialog(QWidget * parent ,const char * name,bool modal) :  QFileDialog( parent, name)
{
	setViewMode( QFileDialog::List);
	setModal(modal);
}

dirDialog::~dirDialog()
{
}

QString dirDialog::openFileName(const QString &startWith, const QString &filter, bool single)
{
//	QString start=startWith;
//    setDirectory("/home/joma");
	start=startWith;
	if (start.isNull() )
  	{
			if(start.isEmpty()) start=QDir::currentPath();
		}
	if(single) 	setFileMode(ExistingFile);	
	else setFileMode(ExistingFiles);	
	
    setNameFilter(filter);
    setDirectory( start);

//    qDebug() << directory().absolutePath();
	if(exec()==QDialog::Accepted)
		{
			QFileInfo fi(selectedFiles().at(0));
			start=fi.absolutePath();
			return fi.absoluteFilePath();
		}
	return "";
}

/*!
		\fn dirDialog::openDirName(const QString &startWith, const QString &filter)
		\brief selection of a directory
		
		Opens a directory #define
		\param startWith directory to open (preselected)
		\param filter    types to select from (e.g. mydirs*)
		
		\return if canceled or no selection then return an empty string else return string containing absolute dirname
*/

QString dirDialog::openDirName(const QString &startWith, const QString &filter)
{
	if (startWith.isNull() )
  	{
			if(start.isEmpty()) start=QDir::currentPath();
		}
	setFileMode(DirectoryOnly);	
    setNameFilter(filter);
	setDirectory( start);
	if(exec())
		{
			return selectedFiles().at(0);
		}
	return "";
}

/*!
		\fn dirDialog::saveFileName(const QString &startWith, const QString &filter,QString extension)
		\brief Save a file to disk
		
		Saves a file to disk. A dialogbox is opened with \a startWith directory (or /dir/subdir/..../filename) preselected 
		\param startWith directory to open (can include filename to preselect)
		\param filter    file types to select from (e.g. *.txt *.doc)
		\param extension if extension is not empty or NULL, thenn this string will be appended to the filename. A dot will automatically be insterted (i.e specify "txt" not ".txt").
		\return if canceled or no selection then return an empty string else return string containing absolute filename.
*/

QString dirDialog::saveFileName(const QString &startWith, const QString &filter,QString extension)
{

	QString exten(extension);
	if (startWith.isNull() )
  	{
			if(start.isEmpty()) start=QDir::currentPath();
		}
	else
		{
			start=startWith;
		}
	setFileMode(AnyFile);	
    setNameFilter(filter);
	setDirectory( start);
	setAcceptMode(QFileDialog::AcceptSave);
	if(exec())
		{
			QFileInfo  fi(selectedFiles().at(0));
			if(!exten.isEmpty())
				{
					if(fi.suffix()=="")
						{
							fi.setFile(fi.absoluteFilePath()+"."+exten);
						}
				}
			return fi.absoluteFilePath();
		}
	return "";
}

void deleteFiles(QString dirPath,QString extension)
{
  int i;
  QDir dir(dirPath);
  QStringList filters;
  QFile fi;
  filters << extension;
  dir.setNameFilters(filters);
  QFileInfoList entries = dir.entryInfoList(filters,QDir::Files|QDir::NoSymLinks);
  for(i=0;i<entries.count();i++)
    {
      fi.setFileName(entries.at(i).absoluteFilePath());
      fi.remove();
    }
}

bool trash(QString filename,bool forceDelete)
{
  QString tmp;
  QFile orgFile(filename);
  QFileInfo modifiedFileInfo(filename);
  QFileInfo info(filename);
  QFile infoFile;
  QFile modifiedFile;
  QDir trDir;
  QDir infoDir;
  QDir filesDir;
  QString infoTxt;
  trDir.setPath(getenv("XDG_DATA_HOME"));
  if (trDir.path().isEmpty())  trDir.setPath(QDir::homePath()+"/.local/share/Trash");
  infoDir.setPath(trDir.path()+"/info");
  filesDir.setPath(trDir.path()+"/files");
  infoFile.setFileName(infoDir.path()+"/"+info.fileName()+".trashinfo");
  modifiedFile.setFileName(filesDir.path()+"/"+modifiedFileInfo.fileName());
  int counter=0;
  do
    {
      if(!modifiedFile.exists()) break;
      counter++;
      tmp=QString("%1/%2_%3.%4").arg(filesDir.path()).arg(modifiedFileInfo.completeBaseName()).arg(QString::number(counter)).arg(modifiedFileInfo.suffix());
      modifiedFile.setFileName(tmp);
      tmp=QString("%1/%2_%3.%4").arg(infoDir.path()).arg(modifiedFileInfo.completeBaseName()).arg(QString::number(counter)).arg(modifiedFileInfo.suffix());
      infoFile.setFileName(tmp+".trashinfo");
    }
  while(1);

  infoTxt=QString("[Trash Info]\nPath=%1\nDeletionDate=%2")
      .arg(filename).arg(QDateTime::currentDateTime().toString(Qt::ISODate));

  if((!trDir.exists()) || (!infoDir.exists()) || (!filesDir.exists()))
    {
      qDebug() << "Trash folder or one of its components does not exist";
      if(forceDelete) orgFile.remove();
      return FALSE;
    }

  if(!infoFile.open(QIODevice::WriteOnly))
  {
    qDebug() << QString("Trash folder: can't open %1 for writing").arg(infoFile.fileName());
    if(forceDelete) orgFile.remove();
    return FALSE;
  }
  infoFile.write(infoTxt.toLatin1().data());
  infoFile.close();
  QFile trashFile(info.absoluteFilePath());
  QString target;
  target=QString("%1").arg(modifiedFile.fileName());
  if(!trashFile.rename(filename,target))
  {
    qDebug() << QString("Trash folder: can't rename %1 to %2").arg(filename).arg(target);
    if(forceDelete) orgFile.remove();
    return FALSE;
  }
  return TRUE;
}

void arrayComplexDump(QString label,CVectorEx<_COMPLEX> data,unsigned int len,bool toAux)
{
  CVectorEx<_COMPLEX> vct=data;
  _COMPLEX c;
  unsigned int i,j;
//  unsigned int len;
//  len=vct.Size()/8;
  QString dumpStr,tmp;

  for( i=0;i<len;i+=16)
    {
      dumpStr=label+" ";
      for(j=0;(j<16)&&((i+j)<len);j++)
        {
          c=vct[i+j];
          tmp=QString::number(c.real())+","+QString::number(c.imag());
//          while(tmp.length()<2) tmp.prepend("0");
          dumpStr+=tmp+" ";
        }
      dumpStr.chop(1);
//      if(toAux)
//        {
//          logfile->addToAux(dumpStr);
//        }
//      else
//        {
//          addToLog(dumpStr,LOGALL);
//        }
    }
}


void arrayBinDump(QString label,CVector<_BINARY> data,unsigned int len,bool toAux)
{
  CVector<_BINARY> vct=data;
  unsigned int i,j;
//  unsigned int len;
//  len=vct.Size()/8;
  QString dumpStr,tmp;

  for( i=0;i<len;i+=16)
    {
      dumpStr=label+" ";
      for(j=0;(j<16)&&((i+j)<len);j++)
        {
          tmp=QString::number(vct.Separate(8),16).right(2);
          while(tmp.length()<2) tmp.prepend("0");
          dumpStr+=tmp+" ";
        }
      dumpStr.chop(1);
//      if(toAux)
//        {
//          logfile->addToAux(dumpStr);
//        }
//      else
//        {
//          addToLog(dumpStr,LOGALL);
//        }
    }
}


void arrayDump(QString label,short int *data, unsigned int len,bool toAux)
{
  unsigned int i,j;
  QString dumpStr,tmp;
  for( i=0;i<len;i+=16)
    {
      dumpStr=label+" ";
      for(j=0;(j<16)&&((i+j)<len);j++)
        {
          tmp=QString::number(data[i+j],16).right(4);
          while(tmp.length()<4) tmp.prepend("0");
          dumpStr+=tmp+" ";
        }
      dumpStr.chop(1);
//      if(toAux)
//        {
//          logfile->addToAux(dumpStr);
//        }
//      else
//        {
//          addToLog(dumpStr,LOGALL);
//        }
    }
}

void arrayDump(QString label,int *data, unsigned int len,bool toAux)
{
  unsigned int i,j;
  QString dumpStr,tmp;
  for( i=0;i<len;i+=16)
    {
      dumpStr=label+" ";
      for(j=0;(j<16)&&((i+j)<len);j++)
        {
          tmp=QString::number(data[i+j],16).right(4);
          while(tmp.length()<4) tmp.prepend("0");
          dumpStr+=tmp+" ";
        }
      dumpStr.chop(1);
//      if(toAux)
//        {
//          logfile->addToAux(dumpStr);
//        }
//      else
//        {
//          addToLog(dumpStr,LOGALL);
//        }
    }
}

void arrayDump(QString label, float *data, unsigned int len, bool toAux)
{
  unsigned int i,j;
  QString dumpStr,tmp;
  for( i=0;i<len;i+=16)
    {
      dumpStr=label+" ";
      for(j=0;(j<16)&&((i+j)<len);j++)
        {
          tmp=QString::number(data[i+j]).right(8);
          while(tmp.length()<8) tmp.prepend(" ");
          dumpStr+=tmp+" ";
        }
      dumpStr.chop(1);
//      if(toAux)
//        {
//          logfile->addToAux(dumpStr);
//        }
//      else
//        {
//          addToLog(dumpStr,LOGALL);
//        }
    }
}


void arrayDump(QString label, quint32* data, unsigned int len, bool toAux)
{
  unsigned int i,j;
  QString dumpStr,tmp;
  for( i=0;i<len;i+=16)
    {
      dumpStr=label+" ";
      for(j=0;(j<16)&&((i+j)<len);j++)
        {
          tmp=QString::number(data[i+j],16).right(8);
          while(tmp.length()<8) tmp.prepend("0");
          dumpStr+=tmp+" ";
        }
      dumpStr.chop(1);
//      if(toAux)
//        {
//          logfile->addToAux(dumpStr);
//        }
//      else
//        {
//          addToLog(dumpStr,LOGALL);
//        }
    }
}

timingAnalyser::timingAnalyser()
{

}

timingAnalyser::~timingAnalyser()
{
}
void timingAnalyser::start()
{
  tm.start();
}

unsigned long timingAnalyser::result()
{
  return tm.elapsed();
}



