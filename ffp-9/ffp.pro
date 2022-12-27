#-------------------------------------------------
#
# Project created by QtCreator 2020-05-12T08:23:13
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets multimedia

TARGET = ffp
TEMPLATE = app


SOURCES += main.cpp\
    MediaResolver.cpp \
        mainwindow.cpp \
    audioplayer.cpp \
    audioresampler.cpp \
    mediadecoder.cpp \
    mediademux.cpp \
    videowidget.cpp \
    r2d.cpp \
    ffwidget.cpp \
    audioplaythread.cpp

HEADERS  += mainwindow.h \
    MediaResolver.h \
    audioplayer.h \
    audioresampler.h \
    mediadecoder.h \
    mediademux.h \
    videowidget.h \
    r2d.h \
    ffwidget.h \
    audioplaythread.h

INCLUDEPATH += $$PWD/../../ffmpeg-4.2.1/include

#LIBS     += $$PWD/../../ffmpeg-4.2.1/bin/win32/avcodec-58.dll \
#            $$PWD/../../ffmpeg-4.2.1/bin/win32/avformat-58.dll \
#            $$PWD/../../ffmpeg-4.2.1/bin/win32/avutil-56.dll \
#            $$PWD/../../ffmpeg-4.2.1/bin/win32/swscale-5.dll \
#            $$PWD/../../ffmpeg-4.2.1/bin/win32/swresample-3.dll

#LIBS     += $$PWD/../../ffmpeg-4.2.1/bin/win64/avcodec-58.dll \
#            $$PWD/../../ffmpeg-4.2.1/bin/win64/avformat-58.dll \
#            $$PWD/../../ffmpeg-4.2.1/bin/win64/avutil-56.dll \
#            $$PWD/../../ffmpeg-4.2.1/bin/win64/swscale-5.dll \
#            $$PWD/../../ffmpeg-4.2.1/bin/win64/swresample-3.dll

#LIBS     += E:\\work\\ffmpeg-4.2.1\\lib\\win64\\avcodec.lib \
#            E:\\work\\ffmpeg-4.2.1\\lib\\win64\\avformat.lib \
#            E:\\work\\ffmpeg-4.2.1\\lib\\win64\\avutil.lib \
#            E:\\work\\ffmpeg-4.2.1\\lib\\win64\\swscale.lib \
#            E:\\work\\ffmpeg-4.2.1\\lib\\win64\\swresample.lib

#LIBS     += D:/work/ffmpeg-4.2.1/lib/win64/avcodec.lib \
#            D:/work/ffmpeg-4.2.1/lib/win64/avformat.lib \
#            D:/work/ffmpeg-4.2.1/lib/win64/avutil.lib \
#            D:/work/ffmpeg-4.2.1/lib/win64/swscale.lib \
#            D:/work/ffmpeg-4.2.1/lib/win64/swresample.lib

#LIBS     += D:/work/ffmpeg-4.2.1/lib/win64/libavcodec.dll.a \
#            D:/work/ffmpeg-4.2.1/lib/win64/libavformat.dll \
#            D:/work/ffmpeg-4.2.1/lib/win64/libavutil.dll.a \
#            D:/work/ffmpeg-4.2.1/lib/win64/libswscale.dll.a \
#            D:/work/ffmpeg-4.2.1/lib/win64/libswresample.dll.a

LIBS     += D:/work/ffmpeg-4.2.1/bin/win64/avcodec-58.dll \
            D:/work/ffmpeg-4.2.1/bin/win64/avformat-58.dll \
            D:/work/ffmpeg-4.2.1/bin/win64/avutil-56.dll \
            D:/work/ffmpeg-4.2.1/bin/win64/swscale-5.dll \
            D:/work/ffmpeg-4.2.1/bin/win64/swresample-3.dll

FORMS    += mainwindow.ui

RESOURCES += \
    file.qrc
