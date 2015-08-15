#ifndef DRMSETTINGS_H
#define DRMSETTINGS_H

#include <QDialog>
#include "drmtx/drmtransmitter.h"
#include "utils/reedsolomoncoder.h"
#include "runtx.h"
//#include "externs.h"


namespace Ui {
class DrmSettings;
}

class DrmSettings : public QDialog
{
    Q_OBJECT

public:
    explicit DrmSettings(QWidget *parent = 0);
    ~DrmSettings();
    void GetParams();
    void setParams();
    void readSettings();
    void writeSettings();
    drmTxParams GetDrmParams() { return drmParams; }

    private:
    Ui::DrmSettings *ui;
    drmTxParams drmParams;

};

#endif // DRMSETTINGS_H
