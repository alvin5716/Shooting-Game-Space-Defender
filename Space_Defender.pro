#-------------------------------------------------
#
# Project created by QtCreator 2018-05-16T22:13:37
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Space_Defender
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    character.cpp \
    player.cpp \
    bullet.cpp \
    enemy.cpp \
    enemy_red.cpp \
    enemy_yellow.cpp \
    enemy_pink.cpp \
    enemy_blue.cpp \
    enemy_blue_1.cpp \
    enemy_blue_2.cpp \
    bullet_rotate.cpp \
    enemy_blue_3.cpp \
    enemy_blue_4.cpp \
    enemy_blue_5.cpp \
    flash.cpp \
    effect.cpp \
    shield.cpp \
    bullet_nether.cpp \
    bullet_terminal.cpp \
    laser.cpp \
    enemy_2_green.cpp \
    enemy_2_red.cpp \
    enemy_2_yellow.cpp \
    enemy_2_pink.cpp \
    enemy_2_blue.cpp \
    enemy_2_blue_1.cpp \
    enemy_2_blue_2.cpp \
    enemy_2_blue_3.cpp \
    enemy_2_blue_4.cpp \
    enemy_2_blue_5.cpp \
    effect_shaking.cpp \
    bullet_sin.cpp \
    enemy_3.cpp \
    enemy_3_green.cpp \
    enemy_3_red.cpp \
    enemy_3_yellow.cpp \
    bullet_time.cpp \
    enemy_3_pink.cpp \
    enemy_3_blue.cpp \
    bullet_wall.cpp

HEADERS += \
        mainwindow.h \
    character.h \
    player.h \
    bullet.h \
    enemy.h \
    enemy_red.h \
    enemy_yellow.h \
    enemy_yellow_tri.h \
    enemy_pink.h \
    enemy_blue.h \
    enemy_blue_1.h \
    enemy_blue_2.h \
    bullet_rotate.h \
    enemy_blue_3.h \
    enemy_blue_4.h \
    enemy_blue_5.h \
    flash.h \
    effect.h \
    shield.h \
    bullet_nether.h \
    bullet_terminal.h \
    laser.h \
    game.h \
    enemy_2_green.h \
    enemy_2_red.h \
    enemy_2_yellow.h \
    enemy_2_pink.h \
    enemy_2_green_tri.h \
    enemy_2_blue.h \
    enemy_2_blue_1.h \
    enemy_2_blue_2.h \
    enemy_2_blue_3.h \
    enemy_2_blue_4.h \
    enemy_2_blue_5.h \
    effect_shaking.h \
    bullet_sin.h \
    enemy_3.h \
    enemy_3_green.h \
    enemy_3_red.h \
    enemy_3_yellow.h \
    bullet_time.h \
    enemy_3_pink.h \
    enemy_3_pink_cross.h \
    enemy_3_blue.h \
    bullet_wall.h

FORMS += \
        mainwindow.ui

RESOURCES += \
    res.qrc

DISTFILES +=
