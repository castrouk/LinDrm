#include "spectrumwidget.h"
//#include "qsstvglobal.h"
#include "ui_spectrumwidget.h"
#include "utils/supportfunctions.h"
#include "utils/logging.h"

spectrumWidget::spectrumWidget(QWidget *parent) :
  QFrame(parent),
  ui(new Ui::spectrumWidget)
{
		qDebug() << "^^^^^^^^^^^^^^^ xxxx ^^^^^^^^^^^^^^^";
		ui->setupUi(this);
  readSettings();
  connect(ui->maxDbSpinbox,SIGNAL(valueChanged(int)),SLOT(slotMaxDbChanged(int)));
  connect(ui->rangeSpinBox,SIGNAL(valueChanged(int)),SLOT(slotRangeChanged(int)));
}

spectrumWidget::~spectrumWidget()
{
  writeSettings();
  delete ui;
}

void spectrumWidget::init(int size,int slices,int isamplingrate)
{
//		addToLog(QString("Size: %1, Slices %2, Samplingrate %3").arg(size).arg(slices).arg(isamplingrate),LOGFFT);
		ui->fftFrame->init(size,slices,isamplingrate);
//  ui->fftFrame->setSize(ui->fftFrame->width(),ui->fftFrame->height());
}
void spectrumWidget::realFFT(short int *iBuffer)
{
		qDebug() << "^^^^^^^^^^^^^^^ HERE 1 ^^^^^^^^^^^^^^^";
		ui->fftFrame->realFFT(iBuffer);
}
void spectrumWidget::realFFT(float *iBuffer)
{
		qDebug() << "^^^^^^^^^^^^^^^ HERE 2 ^^^^^^^^^^^^^^^";
		ui->fftFrame->realFFT(iBuffer);
}

void spectrumWidget::realFFT(double *iBuffer)
{
		qDebug() << "^^^^^^^^^^^^^^^ HERE 3 ^^^^^^^^^^^^^^^";
		ui->fftFrame->realFFT(iBuffer);
}




void spectrumWidget::slotMaxDbChanged(int mb)
{
  ui->fftFrame->setMaxDb(mb);
  maxdb=mb;
}
void spectrumWidget::slotRangeChanged(int rg)
{
  ui->fftFrame->setRange(rg);
  range=rg;
}

void spectrumWidget::readSettings()
{
  QSettings qSettings;
  qSettings.beginGroup("SPECTRUM");
  maxdb=qSettings.value("maxdb",0).toInt();
  slotMaxDbChanged(maxdb);
  range=qSettings.value("range",35).toInt();
  slotRangeChanged(range);
  qSettings.endGroup();
  setParams();
}


void spectrumWidget::writeSettings()
{
  QSettings qSettings;
  getParams();
  qSettings.beginGroup("SPECTRUM");
  qSettings.setValue( "maxdb",maxdb);
  qSettings.setValue( "range",range);
  qSettings.endGroup();
}

void spectrumWidget::getParams()
{
//  getValue(maxdb,ui->maxDbSpinbox);
//  getValue(range,ui->rangeSpinBox);
}

void spectrumWidget::setParams()
{
//  setValue(maxdb,ui->maxDbSpinbox);
//  setValue(range,ui->rangeSpinBox);
//  slotMaxDbChanged(maxdb);
//  slotRangeChanged(range);
}

void spectrumWidget::displayWaterfall(bool wf)
{
		ui->fftFrame->displayWaterfall(wf);
}
