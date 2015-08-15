#ifndef CAMERACONTROL_H
#define CAMERACONTROL_H

#include <QDialog>


namespace Ui {
    class cameraControl;
}
class videoCapture;


class cameraControl : public QDialog
{
    Q_OBJECT

public:
    explicit cameraControl(QWidget *parent = 0);
    ~cameraControl();
    void readSettings();
    void writeSettings();
    int exec();
    QImage *getImage();
  private slots:
    void slotBrightnessSliderChanged();
    void slotHueSliderChanged();
    void slotSaturationSliderChanged();
    void slotContrastSliderChanged();
    void slotSharpnessSliderChanged();

private:
    Ui::cameraControl *ui;
    void getParams();
    void setParams();
    bool cameraActive;

    int brightness;
    int hue;
    int saturation;
    int contrast;
    int sharpness;
    videoCapture *camera;
    void timerEvent(QTimerEvent *event);
};

#endif // CAMERACONTROL_H
