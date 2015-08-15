#ifndef SOUNDCARD_H
#define SOUNDCARD_H

#include "ui_SoundCard.h"
#include <QDialog>
#include <QString>
#include "sound/sound.h"

class Soundcard : public QDialog , private Ui::Dialog
{
		Q_OBJECT
public:
		Soundcard( QWidget * parent = 0, Qt::WindowFlags f = 0 );
//		Soundcard(QStringList, QStringList);
		~Soundcard();
		void SetNames();
		void SetDevNames(QStringList names,QStringList descriptions);
		QString GetSoundINdev(){ return m_deviceIN;}
		QString GetSoundOUTdev(){ return m_deviceOUT;}
		void listCards();
		void SetSoundIn(CSoundIn *s)
		{
			SoundIn = s;
		}

protected slots:
		int on_buttonBox_accepted();
		int on_buttonBox_rejected(){return false;}

private:
		QString m_deviceIN;
		QString m_deviceOUT;
		QStringList m_card_namesIN;
		QStringList m_card_namesOUT;
		CSoundIn *SoundIn;
};
#endif //SOUNDCARD_H
