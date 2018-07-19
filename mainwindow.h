#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QTimer>
#include "player.h"
#include "bullet.h"
#include "enemy.h"
#include "enemy_red.h"
#include "enemy_yellow.h"
#include "enemy_yellow_tri.h"
#include "enemy_red_line.h"
#include "enemy_pink.h"
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
#include <QKeyEvent>
#include <vector>
#include "flash.h"
#include "effect.h"
#include "shield.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    void keyPressEvent(QKeyEvent *e);
    void keyReleaseEvent(QKeyEvent *e);
    void newEffectInit(Effect* new_effect);
    void newEnemyInit(Enemy* new_enemy);
    void newBossInit(Enemy* new_boss);
    void tickFreeze();
    bool tickCheck(unsigned int tick);
    bool tickCheck(unsigned int tick_init, unsigned int interval, unsigned int times);
    void pauseAndResume();
    void backToMenu();
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QGraphicsScene *scene;
    QTimer *timer;
    QTimer *freezeTimer;
    unsigned int tick, gametime, boss_tick;
    bool ticking;
    bool left, right, up, down, use_skill;
    Player *player;
    QGraphicsPixmapItem *background;
    std::vector<Bullet*> player_bullets;
    std::vector<Enemy*> enemies;
    std::vector<Bullet*> enemy_bullets;
    Enemy* new_enemy, *new_boss;
    Effect* new_effect;
    Flash* flash;
    int skill_times;
    Shield *dot;
    int secret;
    QPixmap *oriImg2, *cutImg, *oriImg;
    QGraphicsOpacityEffect* bossSkillOpacityEff;
    QPropertyAnimation* bossSkillFadeinAni, *bossSkillMoveInAni;
    int gamestate, level;

private slots:
    void levelSelect();
    void start();
    void start1();
    void start2();
    void doTick();
    void tickContinueCheck();
    void pointAdd(int);
    void bossCorpse(int,int);
    void healthColorChange(QString);

signals:
    void doMove();
    void doImgMove();
    void killEffects();
};

#endif // MAINWINDOW_H
