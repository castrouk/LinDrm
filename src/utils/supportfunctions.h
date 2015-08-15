#ifndef SUPPORTFUNCTIONS_H
#define SUPPORTFUNCTIONS_H

#include <QString>
#include <QLineEdit>
#include <QCheckBox>
#include <QComboBox>
#include <QButtonGroup>
#include <QSpinBox>
#include <QFileDialog>
#include <QRadioButton>
#include <QSlider>
#include <QPlainTextEdit>
#include <QTime>
#include "vector.h"

/** \file */

#define OK TRUE
#define NOK FALSE
/** get int value from a QLinedit */ 
bool getValue(int &val, QLineEdit *input);
/** get double value from a QLinedit */ 
bool getValue(double &val, QLineEdit *input);
/** get int value from a QString */ 
bool getValue(int &val, QString input);
bool getValue(double &val, QString input);
void getValue(bool &val, QCheckBox *input);
void getValue(int &val, QSpinBox *input);
void getValue(uint &val, QSpinBox *input);
void getValue(QString &s, QLineEdit *input);
void getValue(QString &s, QPlainTextEdit *input);

void getValue(int &s, QComboBox *input);
void getIndex(int &s, QComboBox *input);
void getValue(QString &s, QComboBox *input);
void getValue(int &s, QButtonGroup *input);
void getValue(bool &val, QRadioButton *input);
void getValue(int &val, QSlider *input);

void setValue(int val, QLineEdit* output);
void setValue(double val, QLineEdit* output);
void setValue(double val, QLineEdit* output,int prec);
void setValue(bool val, QCheckBox *input);
void setValue(int val, QSpinBox *input);
void setValue(uint val, QSpinBox *input);
void setValue(QString s, QLineEdit *input);
void setValue(QString s, QPlainTextEdit *input);
void setValue(int s, QComboBox *input);
void setIndex(int s, QComboBox *input);
void setValue(QString s, QComboBox *input);
void setValue(int s, QButtonGroup *input);
void setValue(bool val, QRadioButton *input);
void setValue(int val, QSlider *input);



bool browseGetFile(QLineEdit *le,QString deflt,const QString &filter="*");
bool browseSaveFile(QLineEdit *le,QString deflt,const QString &filter="*");
bool browseDir(QLineEdit *le,QString deflt,const QString &filter="*");
void deleteFiles(QString dirPath,QString extension);

bool trash(QString filename,bool forceDelete);
void arrayDump(QString label, short int *data, unsigned int len, bool toAux);
void arrayDump(QString label, int *data, unsigned int len, bool toAux);
void arrayDump(QString label,float *data, unsigned int len,bool toAux);
void arrayDump(QString label, quint32 *data, unsigned int len, bool toAux);
void arrayBinDump(QString label, CVector<_BINARY> data, unsigned int len, bool toAux);
void arrayComplexDump(QString label,CVectorEx<_COMPLEX> data,unsigned int len,bool toAux);


class dirDialog : public QFileDialog
{
public:
	
    dirDialog(QWidget *parent, const char * name, bool modal);
  ~dirDialog();
  QString openFileName(const QString & startWith, const QString &filter="*",bool single=TRUE);
  QString openDirName(const QString & startWith, const QString &filter="*");
	QString saveFileName(const QString & startWith, const QString &filter,QString extension);
};

class timingAnalyser
{
public:
  timingAnalyser();
  ~timingAnalyser();
  void start();
  unsigned long result();
private:
  QTime tm;
};



#endif

