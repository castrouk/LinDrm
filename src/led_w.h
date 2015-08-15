#ifndef LED_W_H
#define LED_W_H

#include <Qt>
#include "ui_led_w.h"
#include <QWidget>
#include <QColor>
#include <QTimer>


class Led_W : public QWidget , private Ui::Led_W
{
  Q_OBJECT
public:
  Led_W( QWidget * parent = 0, Qt::WindowFlags f = 0 );
  ~Led_W();
  void SetLed(QLabel *Led, QColor );

protected slots:
  void update();

private:

};
#endif //LED_W_H
