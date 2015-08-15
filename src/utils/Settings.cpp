#include "Settings.h"
// #include "soundcard.h"

Settings::Settings()
{
    ReadRX();
}

Settings::~Settings()
{

}

void Settings::ReadRX()
{
    QSettings settings("G4EKQ", "LinDRM");
    settings.beginGroup("DrmRX");
    comboIN->setCurrentIndex(settings.value("Soundindex").toInt());
    settings.endGroup();
}

void Settings::WriteRX()
{
    QSettings settings("G4EKQ", "LinDRM");
    settings.beginGroup("DrmRX");
    settings.setValue("SoundCard", comboIN->currentText());
    settings.setValue("Soundindex", comboIN->currentIndex ());
    settings.endGroup();
}
