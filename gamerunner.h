#ifndef GAMERUNNER_H
#define GAMERUNNER_H

#include <QObject>
#include "player.h"
#include "bullet.h"
#include "enemy.h"
#include "enemy_red.h"
#include "enemy_yellow.h"
#include "enemy_yellow_tri.h"
#include "enemy_pink.h"
#include "enemy_green.h"
#include "enemy_blue_1.h"
#include "enemy_blue_2.h"
#include "enemy_blue_3.h"
#include "enemy_blue_4.h"
#include "enemy_blue_5.h"
#include "enemy_2_green.h"
#include "enemy_2_red.h"
#include "enemy_2_yellow.h"
#include "enemy_2_pink.h"
#include "enemy_2_green_tri.h"
#include "enemy_2_blue_1.h"
#include "enemy_2_blue_2.h"
#include "enemy_2_blue_3.h"
#include "enemy_2_blue_4.h"
#include "enemy_2_blue_5.h"
#include "enemy_3_green.h"
#include "enemy_3_red.h"
#include "enemy_3_yellow.h"
#include "enemy_3_pink.h"
#include "enemy_3_pink_cross.h"
#include "enemy_3_blue_1.h"
#include "enemy_3_blue_2.h"
#include "enemy_3_blue_3.h"
#include "enemy_3_blue_4.h"
#include "enemy_3_blue_5.h"
#include "enemy_4_green.h"
#include "enemy_4_red.h"
#include "enemy_4_yellow.h"
#include "enemy_4_pink.h"
#include "enemy_4_blue_1.h"
#include "enemy_4_blue_2.h"
#include "enemy_4_blue_3.h"
#include "enemy_4_blue_4.h"
#include "enemy_4_blue_5.h"
#include "enemy_4_blue_6.h"
#include "environment_1.h"
#include "environment_2.h"
#include "teammate.h"
#include "game.h"

class MainWindow;

class GameRunner : public QObject
{
    Q_OBJECT
public:
    explicit GameRunner(MainWindow *mainwindow, QObject *parent = nullptr);
    bool endListShowPulse;
signals:
    void doTick();
public slots:
    Player* player;
    std::vector<Bullet*>& player_bullets;
    std::vector<Enemy*>& enemies;
    std::list<Bullet*>& enemy_bullets;
    Enemy* new_enemy, *new_boss;
    int &gamestate, &level;
    bool &dialogueProcessing;
};

#endif // GAMERUNNER_H
