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
#include <QKeyEvent>
#include <vector>
#include <list>
#include "flash.h"
#include "effect.h"
#include "shield.h"
#include <QMediaPlayer>
#include "widgetanimationer.h"
#include "dialoguewidget.h"

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
    void newMagicEffect(int show_w, int show_h, double x, double y, int lifetime=175, Game::MagicType type=Game::MagicTypeWhite);
    void tickFreeze();
    inline bool tickCheck(unsigned int tick);
    inline bool tickCheck(unsigned int tick_init, unsigned int interval, unsigned int times);
    inline int timesCount(unsigned int tick_init, unsigned int interval);
    void setGamePage(Game::GamePage page);
    void setEndListPage(Game::EndListPage page);
    void warningFadeIn();
    void warningFadeOut();
    void redFlash();
    void dialogueStart(std::initializer_list<Dialogue> list);
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
    std::list<Bullet*> enemy_bullets;
    Enemy* new_enemy, *new_boss;
    Effect* new_effect;
    Flash* flash;
    int skill_times;
    Shield *dot;
    int secret;
    QPixmap *oriImg2, *cutImg, *oriImg;
    int gamestate, level;
    QString strBossBG;
    bool isPlayerPosHigh, levelIntroShowing;
    QGraphicsOpacityEffect bossHealthOpacityEff, bossLivesOpacityEff;
    std::vector<QMediaPlayer*> audioers;
    WidgetAnimationer* EndListAni, *levelSelectAni, *bossSkillAni;
    bool dialogueProcessing;
    static const int StartTick = 0;
    DialogueWidget* dialogueWidget;

private slots:
    void newEnemyInit(Enemy* new_enemy);
    void newBossInit(Enemy* new_boss);
    void newEffectInit(Effect* new_effect);
    void levelSelect();
    void start();
    void start1();
    void start2();
    void start3();
    void start4();
    void doTick();
    void restart();
    void pauseAndResume();
    void backToMenu();
    void tickContinueCheck();
    void pointAdd(int);
    void bossCorpse(int,int);
    void healthColorChange(QString);
    void sceneVibrate(short vibrate_count = 0, bool withSound = true);
    void sceneVibrateVertical(short vibrate_count = 0, bool withSound = true);
    void bossSkillLengthSetting(QString skill);
    void soundPlay(Game::Sound sound);
    void killAllBullets();
    void killOtherEnemies(Enemy*);
    void dialogueEnd();

signals:
    void doMove();
    void doImgMove();
    void killEffects();
    void killEffectsWithoutDot();
};

#endif // MAINWINDOW_H
