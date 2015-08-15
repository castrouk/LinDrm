#include "soundcard.h"
#include <QList>
#include <QAudioDeviceInfo>
#include <QSettings>
#include <QDebug>



Soundcard::Soundcard( QWidget * parent, Qt::WindowFlags f) : QDialog(parent,f)
{
	setupUi(this);
	SetNames();
}

//Soundcard::Soundcard(QStringList names,QStringList /* descriptions */)
//{
//	setupUi(this);
//	listCards();
//	for(int i = 0; i < names.size();i++)
//	{
//		qDebug() << __FILE__ << __LINE__ << names.at(i);
////		if(names.at(i).contains("input"))
//		{
//			comboIN->addItem(names.at(i));
//		}
////		else
////		if(names.at(i).contains("output"))
//		{
//			comboOUT->addItem(names.at(i));
//		}
//	}
//	SetNames();
//}



Soundcard::~Soundcard()
{
}

# ifdef USE_QTAUDIO

void Soundcard::listCards()
{
	CSoundIn *sndin = new CSoundIn;
	CSoundOut *sndout = new CSoundOut;
//	vector<string> names;
//	vector<string> descriptions;
	vector<deviceprop> devs;
//	sndin.Enumerate(names, descriptions);

	const int desiredsamplerates = 48000;
	sndin->Enumerate(devs, &desiredsamplerates);


		//	foreach (string s, names)
	foreach (deviceprop p, devs)
	{
		p.desc.clear();
		QString qs = p.name.c_str();
		QString qd = p.desc.c_str();

//		qDebug() << __FILE__ << __LINE__ << qs << qd;

//		if(qs.contains("input"))
//		{
			m_card_namesIN << qs;
//		}
	}
	comboIN->addItems(m_card_namesIN);

	sndout->Enumerate(devs, &desiredsamplerates);
	foreach (deviceprop p, devs)
	{
		p.desc.clear();

		QString qs = p.name.c_str();
		QString qd = p.desc.c_str();

		qDebug() << __FILE__ << __LINE__ << qs << qd;

//		if(qs.contains("output"))
//		{
			m_card_namesOUT << qs;
//		}
	}
	comboOUT->addItems(m_card_namesOUT);
//	delete sndin;
//	delete sndout;

}
# endif

# ifdef USE_PULSEAUDIO
void Soundcard::listCards()
{
	CSoundIn sndin;
	CSoundOut sndout;
	vector<string> names;
	vector<string> descriptions;
	sndin.Enumerate(names, descriptions);
	foreach (string s, names)
	{
		QString qs = s.c_str();
		if(qs.contains("input"))
		{
			m_card_namesIN << qs;
		}
	}
	sndout.Enumerate(names, descriptions);

	foreach (string s, names)
	{
		QString qs = s.c_str();
		if(qs.contains("output"))
		{
			m_card_namesOUT << qs;
		}
	}
	comboIN->addItems(m_card_namesIN);
	comboOUT->addItems(m_card_namesOUT);
}
# endif

# ifdef USE_PORTAUDIO
void Soundcard::listCards()
{
	CSoundIn sndin;
	vector<string> names;
	vector<string> descriptions;
	sndin.Enumerate(names, descriptions);
	foreach (string s, names)
	{
		QString qs = s.c_str();
//		if(qs.contains("input"))
		{
			m_card_namesIN << qs;
		}
//		else
		{
			m_card_namesOUT << qs;
		}
	}
	comboIN->addItems(m_card_namesIN);
	comboOUT->addItems(m_card_namesOUT);
}
# endif

void Soundcard::SetNames()
{
	qWarning("TODO: %s	%d",__FILE__,__LINE__);
	listCards();
	QSettings settings("G4EKQ", "LinDRM");

	settings.beginGroup("DrmRX");
	{
		comboIN->setCurrentIndex(settings.value("Soundindex_IN").toInt());
	}
	settings.endGroup();

	settings.beginGroup("DrmTX");
	{
		comboOUT->setCurrentIndex(settings.value("Soundindex_OUT").toInt());
	}
	settings.endGroup();

	m_deviceIN = comboIN->currentText();
	m_deviceOUT = comboOUT->currentText();
}


int Soundcard::on_buttonBox_accepted()
{
	m_deviceIN = comboIN->currentText();

	QSettings settings("G4EKQ", "LinDRM");

	settings.beginGroup("DrmRX");
	settings.setValue("Soundindex_IN", comboIN->currentIndex());
	settings.setValue("SoundINdev", comboIN->currentText());
	settings.endGroup();

	settings.beginGroup("DrmTX");
	settings.setValue("Soundindex_OUT", comboOUT->currentIndex());
	settings.setValue("SoundOUTdev", comboOUT->currentText());
	settings.endGroup();
//	SetNames();
	return true;
}

