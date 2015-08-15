######################################################################
# Automatically generated by qmake (2.01a) Sun Mar 16 14:34:21 2014
######################################################################

MOC_DIR = build/moc
OBJECTS_DIR = build/obj
UI_DIR = build/ui

QT += multimedia
QT += widgets
QT += core
# CONFIG += c++11
CONFIG += C++14

# CONFIG += address_sanitizer
# run ./LinDrm  2>&1 | asan_symbolize.py | c++filt
address_sanitizer {
	QMAKE_CXXFLAGS += -fsanitize=address -fno-omit-frame-pointer
	QMAKE_CFLAGS += -fsanitize=address -fno-omit-frame-pointer
	QMAKE_LFLAGS += -fsanitize=address
}

# CONFIG += qwt
# CONFIG += pulseaudio
#CONFIG += portaudio
CONFIG += qtaudio
# CONFIG += alsa
# QT += sql


LIBS += \
	-lm \
	-lstdc++ \
	-lfftw3 \
	-lfftw3f \
	-lhamlib \
	-lsndfile




DEFINES += QT_CORE_LIB HAVE_STDINT_H


qtaudio {
	 DEFINES += USE_QTAUDIO
	 HEADERS += src/sound/qtaudio.h
	 SOURCES += src/sound/qtaudio.cpp
	 message("with Qt audio")
}

#alsa {
#	 DEFINES += USE_ALSA
#	 HEADERS += src/linux/soundsrc.h \
#					src/linux/soundin.h \
#					src/linux/soundout.h
#	 SOURCES += src/linux/alsa.cpp
#				 #  src/linux/soundsrc.cpp
#	 message("with alsa")
#}


#portaudio {
#	 DEFINES += USE_PORTAUDIO
#	 HEADERS += src/sound/pa_ringbuffer.h \
#					src/sound/drm_portaudio.h
#	 SOURCES += src/sound/drm_portaudio.cpp \
#					src/sound/pa_ringbuffer.c
#	 LIBS += -lportaudio
#	 unix:PKGCONFIG += portaudio-2.0
#	 message("with portaudio")
#}


#pulseaudio {
#	 DEFINES += USE_PULSEAUDIO
#	 HEADERS += src/sound/drm_pulseaudio.h
#	 SOURCES += src/sound/drm_pulseaudio.cpp
#	 LIBS += -lpulse
#	 unix:PKGCONFIG += libpulse
#	 message("with pulseaudio")
#}




TEMPLATE = app
# TARGET = 
DEPENDPATH += . src src/sound
INCLUDEPATH += . src src/sound

# Input
HEADERS +=	\
	src/logs/logview.h \
	src/TxPreview/droparea.h \
	src/version.h \
	src/utils/rs.h \
	src/utils/vector.h \
	src/utils/reedsolomoncoder.h \
	src/common/GVector.h \
	src/util/Modul.h \
	src/common/maths-utils/FFT.h \
	src/common/maths-utils/Filters.h \
	src/common/maths-utils/Functions.h \
	src/mw.h \
	src/waterfallwidget.h \
	src/specwidget.h \
	src/configparams.h\
	src/soundcard.h \
	src/sound/sound.h \
	src/sound/selectioninterface.h \
	src/sound/soundinterface.h \
	src/sound/resampler.h \
	src/sound/resamplefilter.h \
	src/qsstvdefs.h \
	src/drmrx/demodulator.h \
	src/drmrx/sourcedecoder.h \
	src/drmrx/channeldecode.h \
	src/drmrx/drm.h \
	src/drmrx/mkmap.h \
	src/drmrx/msd_hard.h \
	src/drmrx/msd_hard_sdc.h \
	src/drmrx/nrutil.h \
	src/drmrx/viterbi_decode.h \
	src/drmrx/drmdatainput.h \
	src/drmrx/drmconstellationframe.h \
	src/dsp/downsamplefilter.h \
	src/widgets/blockview.h \
	src/DrmReceiver.h \
# TX
	src/runtx.h \
	src/drmtx/drmtransmitter.h \
	src/drmsettings.h \
	src/DrmTransmitter.h \
	src/Parameter.h \
	src/GlobalDefinitions.h \
	src/ofdmcellmapping/OFDMCellMapping.h \
	src/ofdmcellmapping/CellMappingTable.h \
	src/tables/TableAMSS.h \
	src/tables/TableCarMap.h \
	src/tables/TableCarrier.h \
	src/tables/TableDRMGlobal.h \
	src/tables/TableFAC.h \
	src/tables/TableMLC.h \
	src/tables/TableQAMMapping.h \
	src/mlc/BitInterleaver.h \
	src/mlc/ChannelCode.h \
	src/mlc/ConvEncoder.h \
	src/mlc/EnergyDispersal.h \
	src/mlc/MLC.h \
	src/mlc/QAMMapping.h \
	src/interleaver/BlockInterleaver.h \
	src/interleaver/SymbolInterleaver.h \
	src/OFDM.h \
	src/DRMSignalIO.h \
	src/sourcedecoders/AudioSourceDecoder.h \
	src/datadecoding/DABMOT.h \
	src/datadecoding/DataDecoder.h \
	src/datadecoding/MOTSlideShow.h \
	src/FAC/FAC.h \
	src/DataIO.h \
	src/SDC/SDC.h \
	src/util/Utilities.h \
	src/util/CRC.h \
	src/utils/supportfunctions.h \
	src/matlib/Matlib.h \
	src/matlib/MatlibSigProToolbox.h \
	src/matlib/MatlibStdToolbox.h \
	src/rxstatus.h \
	src/txprogress.h \
	src/drmtx/bsrform.h \
	 src/objnotify.h \
    src/qpfn.h \
    src/drmrx/bsrfix.h \
    src/mytab.h \
    src/constellationwidget.h

FORMS += \
	 src/Mw.ui \
    src/SoundCard.ui \
    src/widgets/blockview.ui \
	 src/drmsettings.ui \
	src/RxStatus.ui \
	src/txProgress.ui \
	src/drmtx/bsrform.ui \
	src/drmrx/drmconstellationframe.ui \
	src/logs/logview.ui


SOURCES += 	\
	src/logs/logview.cpp \
	src/TxPreview/droparea.cpp \
	src/utils/rs.cpp \
	src/utils/reedsolomoncoder.cpp \
	src/common/GVector.cpp \
	src/common/maths-utils/FFT.cpp \
	src/common/maths-utils/Functions.cpp \
	src/main.cpp \
	src/mw.cpp \
  src/waterfallwidget.cpp \
  src/specwidget.cpp \
	src/configparams.cpp \
	src/soundcard.cpp \
	src/sound/resampler.cpp \
	src/sound/resamplefilter.cpp \
	src/drmrx/demodulator.cpp \
	src/drmrx/sourcedecoder.cpp \
	src/drmrx/channeldecode.cpp \
	src/drmrx/crc16_bytewise.cpp \
	src/drmrx/drm.cpp \
	src/drmrx/bits2bytes.cpp \
	src/drmrx/getmode.cpp \
	src/drmrx/getofdm.cpp \
	src/drmrx/getfoffsint.cpp \
	src/drmrx/getsymbolidx.cpp \
	src/drmrx/getofdmsync.cpp \
	src/drmrx/mkfacmap.cpp \
	src/drmrx/mkmscmap.cpp \
	src/drmrx/msdhardfac.cpp \
	src/drmrx/msdhardmsc.cpp \
	src/drmrx/nrutil.cpp \
	src/drmrx/lubksb.cpp \
	src/drmrx/ludcmp.cpp \
	src/drmrx/psdcmean.cpp \
	src/drmrx/psdmean.cpp \
	src/drmrx/crc8_c.cpp \
	src/drmrx/filter0.cpp \
	src/drmrx/filter1.cpp \
	src/drmrx/filter1c.cpp \
	src/drmrx/deinterleaver.cpp \
	src/drmrx/viterbi_decode.cpp \
	src/drmrx/newfft.cpp \
	src/drmrx/drmdatainput.cpp \
	src/drmrx/drmconstellationframe.cpp \
	src/dsp/filter.cpp \
	src/dsp/downsamplefilter.cpp \
	src/dsp/filterparam.cpp \
	src/widgets/blockview.cpp \
	src/DrmReceiver.cpp \
# TX
	src/runtx.cpp \
	src/drmtx/drmtransmitter.cpp \
	src/drmsettings.cpp \
	src/DrmTransmitter.cpp \
	src/Parameter.cpp \
	src/ofdmcellmapping/OFDMCellMapping.cpp \
	src/ofdmcellmapping/CellMappingTable.cpp \
	src/tables/TableFAC.cpp \
	src/mlc/BitInterleaver.cpp \
	src/mlc/ChannelCode.cpp \
	src/mlc/ConvEncoder.cpp \
	src/mlc/EnergyDispersal.cpp \
	src/mlc/MLC.cpp \
	src/mlc/QAMMapping.cpp \
	src/interleaver/BlockInterleaver.cpp \
	src/interleaver/SymbolInterleaver.cpp \
	src/OFDM.cpp \
	src/DRMSignalIO.cpp \
	src/sourcedecoders/AudioSourceDecoder.cpp \
	src/datadecoding/DABMOT.cpp \
	src/datadecoding/DataDecoder.cpp \
	src/datadecoding/MOTSlideShow.cpp \
	src/FAC/FAC.cpp \
	src/DataIO.cpp \
	src/SDC/SDCTransmit.cpp \
	src/util/Utilities.cpp \
	src/util/CRC.cpp \
	src/utils/supportfunctions.cpp \
	src/matlib/MatlibSigProToolbox.cpp \
	src/matlib/MatlibStdToolbox.cpp \
	src/rxstatus.cpp \
	src/txprogress.cpp \
	src/drmtx/bsrform.cpp \
	 src/objnotify.cpp \
    src/drmrx/bsrfix.cpp \
    src/mytab.cpp \
    src/constellationwidget.cpp





