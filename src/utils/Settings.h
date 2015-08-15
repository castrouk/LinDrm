#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>


class Settings : public QSettings
{
public:
    Settings();
    ~Settings();
    void ReadRX();
    void WriteRX();
private:

};



#endif // SETTINGS_H
