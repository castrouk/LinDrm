#include "drmsettings.h"
#include "ui_drmsettings.h"
#include "utils/supportfunctions.h"
#include <QDebug>
#include <QSettings>

DrmSettings::DrmSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DrmSettings)
{
    ui->setupUi(this);
    readSettings();
    setParams();
}


void DrmSettings::readSettings()
{
    QSettings settings("G4EKQ", "LinDRM");
    settings.beginGroup("DrmTxSettings");

    drmParams.bandwith=settings.value("drmBandWith",0).toInt();
    drmParams.interleaver=settings.value("drmInterLeaver",0).toInt();
    drmParams.protection=settings.value("drmProtection",0).toInt();
    drmParams.qam=settings.value("drmQAM",0).toInt();
    drmParams.robMode=settings.value("drmRobMode",0).toInt();
    drmParams.readSolomon=settings.value("drmReedSolomon",0).toInt();
    qDebug() << __FILE__ << __LINE__ << "TO DO drmParams.callsign=myCallsign";
    drmParams.callsign = "G4EKQ";
    settings.endGroup();
//    setParams();

}

void DrmSettings::writeSettings()
{
    QSettings settings("G4EKQ", "LinDRM");
    settings.beginGroup("DrmTxSettings");
    settings.setValue("drmBandWith",drmParams.bandwith);
    settings.setValue("drmInterLeaver",drmParams.interleaver);
    settings.setValue("drmProtection",drmParams.protection);
    settings.setValue("drmQAM",drmParams.qam);
    settings.setValue("drmRobMode",drmParams.robMode);
    settings.setValue("drmReedSolomon",drmParams.readSolomon);
//    qSettings.setValue("quality",quality);
    settings.endGroup();
}

void DrmSettings::GetParams()
{
    getIndex(drmParams.bandwith,ui->drmTxBandwidthComboBox);
    getIndex(drmParams.bandwith,ui->drmTxBandwidthComboBox);
    getIndex(drmParams.interleaver,ui->drmTxInterleaveComboBox);
    getIndex(drmParams.protection,ui->drmTxProtectionComboBox);
    getIndex(drmParams.qam,ui->drmTxQAMComboBox);
    getIndex(drmParams.robMode,ui->drmTxModeComboBox);
    getIndex(drmParams.readSolomon,ui->drmTxReedSolomonComboBox);
}

void DrmSettings::setParams()
{
    setIndex(drmParams.bandwith,ui->drmTxBandwidthComboBox);
    setIndex(drmParams.interleaver,ui->drmTxInterleaveComboBox);
    setIndex(drmParams.protection,ui->drmTxProtectionComboBox);
    setIndex(drmParams.qam,ui->drmTxQAMComboBox);
    setIndex(drmParams.robMode,ui->drmTxModeComboBox);
    setIndex(drmParams.readSolomon,ui->drmTxReedSolomonComboBox);
//    setValue(quality,ui->qualitySpinBox);
}

DrmSettings::~DrmSettings()
{
    delete ui;
}
