#include "cameracontrol.h"
#include "qsstvglobal.h"
#include "videocapt/videocapture.h"
#include "ui_cameracontrol.h"
#include "utils/supportfunctions.h"
#include <QSettings>
#include <QPainter>

cameraControl::cameraControl(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::cameraControl)
{
    ui->setupUi(this);
    camera=new videoCapture;
    readSettings();

 }

cameraControl::~cameraControl()
{
    camera->close();
    delete camera;
    writeSettings();
    delete ui;
}

void cameraControl::readSettings()
{
  QSettings qSettings;
  qSettings.beginGroup("Capture");
  brightness=qSettings.value("brightness",128).toInt();
  hue=qSettings.value("hue",0).toInt();
  saturation=qSettings.value("saturation",128).toInt();
  contrast=qSettings.value("contrast",128).toInt();
  sharpness=qSettings.value("sharpness",128).toInt();
  qSettings.endGroup();
  setParams();
  cameraActive=camera->open();
  slotBrightnessSliderChanged();
  slotHueSliderChanged();
  slotSaturationSliderChanged();
  slotContrastSliderChanged();
  slotSharpnessSliderChanged();
  connect(ui->brightnessSlider,SIGNAL(valueChanged(int)),SLOT(slotBrightnessSliderChanged()));
  connect(ui->hueSlider,SIGNAL(valueChanged(int)),SLOT(slotHueSliderChanged()));
  connect(ui->saturationSlider,SIGNAL(valueChanged(int)),SLOT(slotSaturationSliderChanged()));
  connect(ui->contrastSlider,SIGNAL(valueChanged(int)),SLOT(slotContrastSliderChanged()));
  connect(ui->sharpnessSlider,SIGNAL(valueChanged(int)),SLOT(slotSharpnessSliderChanged()));
}

void cameraControl::writeSettings()
{
  getParams();
  QSettings qSettings;
  qSettings.beginGroup("Capture");
  qSettings.setValue( "brightness",brightness);
  qSettings.setValue( "hue",hue);
  qSettings.setValue( "saturation",saturation);
  qSettings.setValue( "contrast",contrast);
  qSettings.setValue( "sharpness",sharpness);
  qSettings.endGroup();
}


void cameraControl::getParams()
{
  getValue(brightness,ui->brightnessSlider);
  getValue(hue,ui->hueSlider);
  getValue(saturation,ui->saturationSlider);
  getValue(contrast,ui->contrastSlider);
  getValue(sharpness,ui->sharpnessSlider);
}

void cameraControl::setParams()
{
  setValue(brightness,ui->brightnessSlider);
  setValue(hue,ui->hueSlider);
  setValue(saturation,ui->saturationSlider);
  setValue(contrast,ui->contrastSlider);
  setValue(sharpness,ui->sharpnessSlider);
}

int cameraControl::exec()
{
  if(!cameraActive) return FALSE;
  int timerID;
  int result;
  addToLog("cameracontrol exec",LOGCAM);
  camera->startSnapshots();
  timerID=startTimer(50);
  result=QDialog::exec();
  killTimer(timerID);
  camera->abortCapture();
  camera->stopStreaming();
  if(result==QDialog::Accepted) return TRUE;
  return FALSE;
}


void cameraControl::timerEvent(QTimerEvent *)
 {
     if(camera->getFrame())
      {
        ui->imageFrame->openImage(*camera->getImage());
      }
 }

QImage *cameraControl::getImage()
{
  return camera->getImage();
}

void cameraControl::slotBrightnessSliderChanged()
{
  getParams();
  camera->changeCtrl(videoCapture::Brightness,brightness);
}

void cameraControl::slotHueSliderChanged()
{
  getParams();
  camera->changeCtrl(videoCapture::Hue,hue);
}

void cameraControl::slotSaturationSliderChanged()
{
  getParams();
  camera->changeCtrl(videoCapture::Saturation,saturation);
}

void cameraControl::slotContrastSliderChanged()
{
  getParams();
  camera->changeCtrl(videoCapture::Contrast,contrast);
}

void cameraControl::slotSharpnessSliderChanged()
{
  getParams();
  camera->changeCtrl(videoCapture::Sharpness,sharpness);
}



