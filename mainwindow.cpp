#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QScrollArea>
#include <QDebug>
#include <cmath>
#include <iterator>
#include <ctime>
#include "game.h"
#include <QDir>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    timer(nullptr),freezeTimer(nullptr),
    ticking(false),left(false),right(false),up(false),down(false),use_skill(false),
    player(nullptr),dot(nullptr),secret(0),oriImg2(nullptr)
{
    ui->setupUi(this);
    this->setGamePage(Game::GamePageMenu);
    //boss objects
    bossHealthOpacityEff = new QGraphicsOpacityEffect(this);
    bossLivesOpacityEff = new QGraphicsOpacityEffect(this);
    ui->BossHealth->setGraphicsEffect(bossHealthOpacityEff);
    ui->BossLives->setGraphicsEffect(bossLivesOpacityEff);
    //level
    level = 1;
    //scene
    scene = new QGraphicsScene(0,0,Game::FrameWidth,Game::FrameHeight);
    ui->graphicsView->setScene(scene);
    ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView->setViewportUpdateMode(QGraphicsView::NoViewportUpdate);
    //qrand
    qsrand(time(nullptr));
    //button
    connect(ui->startButton,SIGNAL(clicked(bool)),this,SLOT(levelSelect()));
    connect(ui->QuitButton,SIGNAL(clicked(bool)),qApp,SLOT(quit()));
    connect(ui->LevelButton_1,SIGNAL(clicked(bool)),this,SLOT(start1()));
    connect(ui->LevelButton_2,SIGNAL(clicked(bool)),this,SLOT(start2()));
    connect(ui->LevelButton_3,SIGNAL(clicked(bool)),this,SLOT(start3()));
    connect(ui->LevelButton_4,SIGNAL(clicked(bool)),this,SLOT(start4()));
    //key control button
    auto keyControlButtonsConnect = [](std::vector<KeyControlButton*>& buttons, KeyControlButton::arrowPos arrow_pos) {
        for(std::vector<KeyControlButton*>::iterator i=buttons.begin(); i!=buttons.end(); ++i) {
            connect(*i,SIGNAL(downSelect()),
                    i==buttons.end()-1?*(buttons.begin()):*(i+1),SLOT(selectThis()));
            connect(*i,SIGNAL(upSelect()),
                    i==buttons.begin()?*(buttons.end()-1):*(i-1),SLOT(selectThis()));
            (*i)->setArrowPos(arrow_pos);
        }
    };
    std::vector<KeyControlButton*> menuButtons({ui->startButton, ui->QuitButton});
    keyControlButtonsConnect(menuButtons,KeyControlButton::arrowPosLeft);
    std::vector<KeyControlButton*> levelButtons({ui->LevelButton_1, ui->LevelButton_2, ui->LevelButton_3, ui->LevelButton_4});
    keyControlButtonsConnect(levelButtons,KeyControlButton::arrowPosDown);
    //focus policy
    setFocusPolicy(Qt::NoFocus);
    //flash
    flash = new Flash(ui->GamePage);
    //end list
    QGraphicsOpacityEffect *half_opacity = new QGraphicsOpacityEffect(this);
    half_opacity->setOpacity(0.6);
    ui->EndList->setGraphicsEffect(half_opacity);
    ui->EndList->setAutoFillBackground(true);
    //boss skill name animation
    bossSkillOpacityEff = new QGraphicsOpacityEffect(this);
    ui->BossSkill->setGraphicsEffect(bossSkillOpacityEff);
    bossSkillFadeinAni = new QPropertyAnimation(bossSkillOpacityEff,"opacity");
    bossSkillFadeinAni->setDuration(3500);
    bossSkillFadeinAni->setStartValue(0);
    bossSkillFadeinAni->setEndValue(1);
    bossSkillFadeinAni->setEasingCurve(QEasingCurve::OutCubic);
    bossSkillMoveInAni = new QPropertyAnimation(ui->BossSkill,"geometry");
    bossSkillMoveInAni->setDuration(2000);
    bossSkillMoveInAni->setStartValue(QRect(200,80,280,50));
    bossSkillMoveInAni->setEndValue(QRect(510,80,280,50));
    bossSkillMoveInAni->setEasingCurve(QEasingCurve::OutCubic);
    //game state
    gamestate=Game::GameStateMenu;
    //level intro
    ui->levelIntro->hide();
    //audioer
    audioers.resize(7);
    for(QMediaPlayer*& audioer: audioers) {
        audioer = new QMediaPlayer(nullptr,QMediaPlayer::LowLatency);
    }
    auto soundSet = [this](Game::Sound sound, int vol, QString url) {
        audioers.at((int)sound)->setMedia(QUrl::fromLocalFile("C:/Users/USER/Documents/GitHub/Shooting-Game-Space-Defender/"+url));
        audioers.at((int)sound)->setVolume(vol);
    };
    soundSet(Game::SoundCymbal,10,"res/sound/cymbal.wav");
    soundSet(Game::SoundFire,30,"res/sound/fire.wav");
    soundSet(Game::SoundShoot01,30,"res/sound/shoot01.wav");
    soundSet(Game::SoundHit,20,"res/sound/hit.wav");
    soundSet(Game::SoundBd,70,"res/sound/bd.wav");
    soundSet(Game::SoundWarning,30,"res/sound/warning.wav");
    soundSet(Game::SoundWarning02,20,"res/sound/warning02.wav");
}

void MainWindow::setGamePage(Game::GamePage page) {
    ui->stackedWidget->setCurrentIndex((int)page);
    KeyControlButton::unselect();
    switch (page) {
    case Game::GamePageMenu:
        ui->startButton->selectThis();
        break;
    case Game::GamePageLevelSelecting:
        ui->LevelButton_1->selectThis();
        break;
    default:
        break;
    }
}

void MainWindow::levelSelect() {
    this->setGamePage(Game::GamePageLevelSelecting);
}
void MainWindow::start1() {
    level=1;
    start();
}
void MainWindow::start2() {
    level=2;
    start();
}
void MainWindow::start3() {
    level=3;
    start();
}
void MainWindow::start4() {
    level=4;
    start();
}
void MainWindow::start() {
    //level intro
    if(StartTick==0) ui->levelIntro->show();
    ui->levelIntro->setCurrentIndex(level-1);
    QGraphicsOpacityEffect *fadein = new QGraphicsOpacityEffect(this);
    ui->levelIntro->setGraphicsEffect(fadein);
    QPropertyAnimation *fadeinAni = new QPropertyAnimation(fadein,"opacity");
    fadeinAni->setDuration(800);
    fadeinAni->setStartValue(0);
    fadeinAni->setEndValue(1);
    fadeinAni->setEasingCurve(QEasingCurve::InQuad);
    fadeinAni->start(QPropertyAnimation::DeleteWhenStopped);
    levelIntroShowing = true;
    //level label
    ui->levelLabel->setText(QString("LEVEL ").append(QString().setNum(level)));
    //boss lives count
    ui->BossLives->hide();
    ui->BossLives->setText("");
    bossLivesOpacityEff->setOpacity(0.7);
    //boss skill name
    ui->BossSkill->hide();
    ui->BossSkill->setText("");
    bossSkillOpacityEff->setOpacity(0.7);
    //boss health bar
    ui->BossHealth->hide();
    ui->BossHealth->setGeometry(70,40,720,30);
    ui->BossHealth->setFormat("");
    ui->BossHealth->setValue(0);
    bossHealthOpacityEff->setOpacity(0.7);
    //warning bar
    ui->WarningBar->hide();
    //skill times
    skill_times=5;
    ui->PlayerSkill->display(skill_times);
    //point
    ui->PlayerPoint->display(0);
    //end list
    ui->EndList->hide();
    //background
    background = new QGraphicsPixmapItem;
    scene->addItem(background);
    background->setPos(0,0);
    background->setZValue(-100);
    cutImg = new QPixmap(150,172);
    oriImg = new QPixmap(":/res/bg/normal.png");
    //player
    if(player!=nullptr) delete player;
    player = new Player(5,5,7,Game::FrameWidth/2,Game::FrameHeight-100);
    connect(player,SIGNAL(soundPlay(Game::Sound)),this,SLOT(soundPlay(Game::Sound)));
    isPlayerPosHigh=false;
    ui->PlayerLife->display(player->getHealth());
    scene->addItem(player);
    connect(this,SIGNAL(doMove()),player,SLOT(move()));
    connect(this,SIGNAL(doImgMove()),player,SLOT(img_move()));
    connect(player,SIGNAL(healthColorChange(QString)),this,SLOT(healthColorChange(QString)));
    player->healthColorChange("white");
    //dot
    if(dot!=nullptr) delete dot;
    dot = new Shield(":/res/effect/dot.png",50,50,10,10,player,-1,player->getX(),player->getY());
    dot->setZValue(100);
    newEffectInit(dot);
    //page
    this->setGamePage(Game::GamePagePlaying);
    //game state
    gamestate=Game::GameStatePlaying;
    //boss tick
    switch(level) {
    case 1:
        boss_tick=Game::BossTick1;
        strBossBG=":/res/bg/boss.png";
        break;
    case 2:
        boss_tick=Game::BossTick2;
        strBossBG=":/res/bg/boss_2.png";
        break;
    case 3:
        boss_tick=Game::BossTick3;
        strBossBG=":/res/bg/boss_3.png";
        break;
    case 4:
        boss_tick=Game::BossTick4;
        strBossBG=":/res/bg/boss_4.png";
        break;
    default:
        boss_tick=Game::BossTick1;
        strBossBG=":/res/bg/boss.png";
        qDebug() << "error: can't get what level is selected";
    }
    //timer
    gametime=tick=StartTick;
    ticking=true;
    if(timer!=nullptr) delete timer;
    timer=new QTimer;
    timer->start(16); //62.5 tick per sec
    connect(timer,SIGNAL(timeout()),this,SLOT(doTick()));
    connect(timer,SIGNAL(timeout()),ui->graphicsView,SLOT(update()));
}
void MainWindow::warningFadeIn() {
    soundPlay(Game::SoundWarning);
    QGraphicsOpacityEffect *fadein = new QGraphicsOpacityEffect(this);
    ui->WarningBar->setGraphicsEffect(fadein);
    QPropertyAnimation *fadeinAni = new QPropertyAnimation(fadein,"opacity");
    fadeinAni->setDuration(1400);
    fadeinAni->setStartValue(0);
    fadeinAni->setEndValue(1);
    fadeinAni->setEasingCurve(QEasingCurve::InQuad);
    fadeinAni->start(QPropertyAnimation::DeleteWhenStopped);
    ui->WarningBar->show();
}
void MainWindow::warningFadeOut() {
    soundPlay(Game::SoundWarning02);
    QGraphicsOpacityEffect *fadeout = new QGraphicsOpacityEffect(this);
    ui->WarningBar->setGraphicsEffect(fadeout);
    QPropertyAnimation *fadeoutAni = new QPropertyAnimation(fadeout,"opacity");
    fadeoutAni->setDuration(1400);
    fadeoutAni->setStartValue(1);
    fadeoutAni->setEndValue(0);
    fadeoutAni->setEasingCurve(QEasingCurve::OutQuad);
    fadeoutAni->start(QPropertyAnimation::DeleteWhenStopped);
}

void MainWindow::doTick() {
    //focus
    setFocus();
    //level intro
    if((tick==250 || gamestate!=Game::GameStatePlaying) && levelIntroShowing) {
        levelIntroShowing = false;
        QGraphicsOpacityEffect* levelIntroOpacityEff = new QGraphicsOpacityEffect(this);
        ui->levelIntro->setGraphicsEffect(levelIntroOpacityEff);
        QPropertyAnimation* levelIntroFadeoutAni = new QPropertyAnimation(levelIntroOpacityEff,"opacity");
        levelIntroFadeoutAni->setDuration(tick==250?6000:1000);
        levelIntroFadeoutAni->setStartValue(1);
        levelIntroFadeoutAni->setEndValue(0);
        levelIntroFadeoutAni->setEasingCurve(QEasingCurve::OutQuint);
        connect(levelIntroFadeoutAni,SIGNAL(finished()),ui->levelIntro,SLOT(hide()));
        levelIntroFadeoutAni->start(QAbstractAnimation::DeleteWhenStopped);
    }
    //bg
    if(tick<boss_tick) {
        int init = (450-172)-(gametime*2/7+1);
        while(init<0) init+=450;
        QPainter merger(cutImg);
        if(init>(450-172)) {
            merger.drawPixmap(QRect(0,0,150,450-init),*oriImg,QRect(0,init,150,450-init));
            merger.drawPixmap(QRect(0,450-init,150,init-(450-172)),*oriImg,QRect(0,0,150,init-(450-172)));
        } else {
            merger.drawPixmap(QRect(0,0,150,172),*oriImg,QRect(0,init,150,172));
        }
    } else {
        if(tick==boss_tick) {
            oriImg2 = new QPixmap(150,450);
            QPixmap *temp = new QPixmap(strBossBG);
            QPainter merger(oriImg2);
            merger.drawPixmap(QRect(0,0,150,(450-172)),*temp,QRect(0,0,150,(450-172)));
            merger.drawPixmap(QRect(0,(450-172),150,172),*cutImg,QRect(0,0,150,172));
        }
        QPainter merger(cutImg);
        int init = (450-172)-((tick-boss_tick)/2+1);
        if(init<0) init = 0;
        if(oriImg2!=nullptr) merger.drawPixmap(QRect(0,0,150,172),*oriImg2,QRect(0,init,150,172));
        else merger.drawPixmap(QRect(0,0,150,172),QPixmap(strBossBG),QRect(0,0,150,172));
    }
    background->setPixmap(cutImg->scaled(Game::FrameWidth,Game::FrameHeight));

    //game
    if(gamestate==Game::GameStatePlaying||gamestate==Game::GameStateWon) {
        //player speed
        if(left && up) player->setSpeed(-Player::speed/sqrt(2),-Player::speed/sqrt(2));
        else if(left && down) player->setSpeed(-Player::speed/sqrt(2),Player::speed/sqrt(2));
        else if(right && up) player->setSpeed(Player::speed/sqrt(2),-Player::speed/sqrt(2));
        else if(right && down) player->setSpeed(Player::speed/sqrt(2),Player::speed/sqrt(2));
        else if(left) player->setSpeed(-Player::speed,0);
        else if(right) player->setSpeed(Player::speed,0);
        else if(up) player->setSpeed(0,-Player::speed);
        else if(down) player->setSpeed(0,Player::speed);
        else player->setSpeed(0,0);
        //move
        emit doMove();
    }
    if(gamestate==Game::GameStatePlaying) {
        //boss lives, skill bar and health bar
        if(!isPlayerPosHigh && player->getY()<280) {
            isPlayerPosHigh=true;
            bossHealthOpacityEff->setOpacity(0.3);
            bossLivesOpacityEff->setOpacity(0.3);
            bossSkillOpacityEff->setOpacity(0.3);
        } else if(isPlayerPosHigh && player->getY()>=280) {
            isPlayerPosHigh=false;
            bossHealthOpacityEff->setOpacity(0.7);
            bossLivesOpacityEff->setOpacity(0.7);
            bossSkillOpacityEff->setOpacity(0.7);
        }
        //player skill
        if(use_skill && !player->isUsingSkill() && skill_times>0 && !player->isInvulnerable()) { //init
            player->coolDown();
            new_effect = new Shield(QString(":/res/effect/shield1.png"),70,70,500,500,player,187,player->getX(),player->getY());
            new_effect->fadein();
            newEffectInit(new_effect);
            --skill_times;
            ui->PlayerSkill->display(skill_times);
        }
        if(player->isUsingSkill()) {
            for(Bullet* enemy_bullet : enemy_bullets) { //continuous
                if(sqrt(pow(enemy_bullet->getX()-player->getX(),2)+pow(enemy_bullet->getY()-player->getY(),2))<=250 && !enemy_bullet->isInvulnerable())
                    enemy_bullet->killItself();
            }
        }
        //shoot ,skill
        if(player->isShooting()) { //player
            //shoot
            Bullet* new_player_bullet = player->shoot();
            if(new_player_bullet!=nullptr) {
                scene->addItem(new_player_bullet);
                player_bullets.push_back(new_player_bullet);
                connect(this,SIGNAL(doMove()),new_player_bullet,SLOT(move()));
                connect(this,SIGNAL(doImgMove()),new_player_bullet,SLOT(img_move()));
            }
        }
        //damaged
        std::vector<Character*> attackers(enemies.begin(),enemies.end());
        Character* real_attacker = player->testAttackedBy(attackers);
        if(real_attacker!=nullptr) {
            ui->PlayerLife->display(player->getHealth());
            sceneVibrate(static_cast<short>(Enemy::shakeLevel::mediumShake));
            redFlash();
        }
        for(int i=0;i<(int)enemies.size();++i) { //enemies
            if(enemies.at(i)->isDead()) continue;
            //shoot
            std::vector<Bullet*>* new_enemy_bullets;
            if(!(enemies.at(i)->isBoss() && enemies.at(i)->isSecPhase())) new_enemy_bullets = enemies.at(i)->shoot();
            else new_enemy_bullets = enemies.at(i)->shoot2();
            if(new_enemy_bullets!=nullptr) {
                for (std::vector<Bullet*>::iterator j=(*new_enemy_bullets).begin();j!=(*new_enemy_bullets).end();++j) {
                    scene->addItem(*j);
                    connect(this,SIGNAL(doMove()),*j,SLOT(move()));
                    connect(this,SIGNAL(doImgMove()),*j,SLOT(img_move()));
                    enemy_bullets.push_back(*j);
                }
                new_enemy_bullets->clear();
                new_enemy_bullets->shrink_to_fit();
                delete new_enemy_bullets;
            }
            //skill
            enemies.at(i)->skill();
            //out of frame
            enemies.at(i)->outOfFrame();
        }
        for(int i=0;i<(int)player_bullets.size();++i) { //player bullets
            //damaged
            std::vector<Character*> attackers(enemies.begin(),enemies.end());
            Character* real_attacker = player_bullets.at(i)->testAttackedBy(attackers);
            if(real_attacker!=nullptr) real_attacker->attacked();
        }
        for(Bullet* enemy_bullet: enemy_bullets) { //enemy bullets
            //damaged
            Character* attacker = player;
            Character* real_attacker = enemy_bullet->testAttackedBy(attacker);
            if(real_attacker!=nullptr) {
                if(!player->isInvulnerable()) {
                    sceneVibrate();
                    redFlash();
                }
                real_attacker->attacked();
                ui->PlayerLife->display(player->getHealth());
            }
        }
        //death
        for(std::vector<Bullet*>::iterator i=player_bullets.begin();i!=player_bullets.end();) {
            if((*i)->isDead()) {
                //delete and erase
                delete (*i);
                i = player_bullets.erase(i);
            } else ++i;
        }
        for(std::vector<Enemy*>::iterator i=enemies.begin();i!=enemies.end();) {
            if((*i)->isDead()) {
                //corpse fade-out
                new_effect = (*i)->disappear();
                newEffectInit(new_effect);
                //delete and erase
                delete (*i);
                i = enemies.erase(i);
            } else ++i;
        }
        for(std::list<Bullet*>::iterator i=enemy_bullets.begin();i!=enemy_bullets.end();) {
            if((*i)->isDead()) {
                //delete and erase
                delete (*i);
                i = enemy_bullets.erase(i);
            } else ++i;
        }
        //spawn
        switch(level) {
        //level 1
        case 1:
            if(tickCheck(250,63,7)) {
                //250+63i, 7 times
                for(int i=0;i<2;++i) {
                    new_enemy = new Enemy_Green(player,3,27.75,0,0,(i==0)?120:Game::FrameWidth-120,-27,0,1.6,(i==0)?0.0048:-0.0048,0.004);
                    newEnemyInit(new_enemy);
                }
            } else if(tickCheck(750)) {
                for(int i=0;i<2;++i) {
                    new_enemy = new Enemy_Red(player,15,37,175,100,(i==0)?150:Game::FrameWidth-150,-27,0,3.6,0,-0.032,false,true);
                    newEnemyInit(new_enemy);
                }
                tickFreeze();
            } else if(tickCheck(900,125,4)) { //1800+250i, 4 times
                for(int i=0;i<2;++i) {
                    new_enemy = new Enemy_Red(player,4,27.75,225,50+25*i,Game::FrameWidth/2+((i==0)?-30:30),-27,0,1.2,(i==0)?-0.0032:0.0032,0);
                    newEnemyInit(new_enemy);
                }
            } else if(tickCheck(1300)) {//2600
                tickFreeze();
            } else if(tickCheck(1450)) { //2900
                for(int i=0;i<2;++i) {
                    new_enemy = new Enemy_Red(player,9,37,200,100,(i==0)?-36:Game::FrameWidth+36,40,(i==0)?2:-2,0,0,0,true);
                    newEnemyInit(new_enemy);
                }
            } else if(tickCheck(1500,50,3)) { //3000+100i, 3 times
                for(int i=0;i<2;++i) {
                    new_enemy = new Enemy_Green(player,6,37,0,0,(i==0)?36:Game::FrameWidth-36,-36,((double)(qrand()%101)/100)*((i==0)?2:-2),0,0,0.02,true);
                    newEnemyInit(new_enemy);
                }
            } else if(tickCheck(1601)) { //3201
                tickFreeze();
            } else if(tickCheck(1750)) { //3500
                new_enemy = new Enemy_Yellow(player,40,46.25,75,150,Game::FrameWidth/2,-36,0,4.4,0,-0.032,false,true);
                newEnemyInit(new_enemy);
                tickFreeze();
            } else if(tickCheck(1850,180,5)) { //3700+360i, 5 times, left
                new_enemy = new Enemy_Yellow(player,7,37,300,135,160,-36,0,4.4,0,-0.032);
                newEnemyInit(new_enemy);
                break;
            } else if(tickCheck(1940,180,5)) { //3700+180+360i, 5 times, right
                new_enemy = new Enemy_Yellow(player,7,37,300,135,Game::FrameWidth-160,-36,0,4.4,0,-0.032);
                newEnemyInit(new_enemy);
            } else if(tickCheck(2900)) { //5800
                new_enemy = new Enemy_Pink(player,90,46.25,50,150,Game::FrameWidth/2,-36,0,3,0,-0.016,false,true);
                newEnemyInit(new_enemy);
                tickFreeze();
            } else if(tickCheck(3100)) { //6200
                new_boss = new Enemy_Yellow_Tri(player,60,46.25,163,200,Game::FrameWidth/2,-46,0,2,0,-0.016,false,true);
                newBossInit(new_boss);
                new_enemy = new Enemy_Pink(player,5,27.75,150,200,Game::FrameWidth/2,-27,0,3.2,0,-0.016,false,true);
                newEnemyInit(new_enemy);
                new_enemy->setInvulnerable();
                new_effect = new_enemy->showShield();
                newEffectInit(new_effect);
                connect(new_boss,SIGNAL(deadSignal()),new_enemy,SLOT(killItself()));
                tickFreeze();
            } else if(tickCheck(3300)) { //6600
                for(int i=0;i<3;++i) {
                    new_enemy = new Enemy_Pink(player,(i==1)?40:20,37,125,(i==1)?100:163,100+(Game::FrameWidth/2-100)*i,-27,0,(i==1)?2:2.6,0,-0.016,false,true);
                    newEnemyInit(new_enemy);
                }
                tickFreeze();
            } else if(tickCheck(3352,181,3)) { //6703+361i, 3 times, warning bar fade in
                this->warningFadeIn();
            } else if(tickCheck(3444,181,3)) { //6888+361i, 3 times, warning bar fade out
                this->warningFadeOut();
            } else if(tickCheck(3925)) { //7850, BOSS 1
                ui->BossLives->show();
                ui->BossHealth->setGeometry(100,40,690,30);
                new_boss = new Enemy_Blue_2(player,240,60,175,200,Game::FrameWidth/2,-59,0,0,0,0,false,true);
                connect(new_boss,SIGNAL(deadSignal(int,int)),this,SLOT(bossCorpse(int,int)));
                new_boss->moveTo(Game::FrameWidth/2,200,165);
                newBossInit(new_boss);
                ui->BossLives->setText("5");
                tickFreeze();
            } else if(tickCheck(3927)) {
                ui->BossLives->setText("4");
            } else if(tickCheck(4050)) { //8100, BOSS 2
                new_boss = new Enemy_Blue_4(player,210,60,175,200,Game::FrameWidth/2,200,0,0,0,0,false,true);
                connect(new_boss,SIGNAL(deadSignal(int,int)),this,SLOT(bossCorpse(int,int)));
                newBossInit(new_boss);
                for(int i=0;i<4;++i) {
                    new_enemy = new Enemy_Green(player,5,27.75,0,0,Game::FrameWidth/5*(i+1),-27,0,1.6,0,0.02,true);
                    newEnemyInit(new_enemy);
                    new_enemy->setInvulnerable();
                    new_effect = new_enemy->showShield();
                    newEffectInit(new_effect);
                    connect(new_boss,SIGNAL(useSkill(QString)),new_enemy,SIGNAL(killItsBullets()));
                    connect(new_boss,SIGNAL(useSkill(QString)),new_enemy,SLOT(killItself()));
                }
                tickFreeze();
            } else if(tickCheck(4052)) {
                ui->BossLives->setText("3");
            } else if(tickCheck(4175)) { //8350, BOSS 3
                new_boss = new Enemy_Blue_5(player,310,60,175,200,Game::FrameWidth/2,200,0,0,0,0,false,true);
                connect(new_boss,SIGNAL(deadSignal(int,int)),this,SLOT(bossCorpse(int,int)));
                newBossInit(new_boss);
                for(int i=0;i<2;++i) {
                    new_enemy = new Enemy_Red(player,5,27.75,235,100,(i==0)?60:Game::FrameWidth-60,-27,0,2.4,0,-0.02,false,true);
                    newEnemyInit(new_enemy);
                    new_enemy->setInvulnerable();
                    new_effect = new_enemy->showShield();
                    newEffectInit(new_effect);
                    connect(new_boss,SIGNAL(useSkill(QString)),new_enemy,SIGNAL(killItsBullets()));
                    connect(new_boss,SIGNAL(useSkill(QString)),new_enemy,SLOT(killItself()));
                }
                tickFreeze();
            } else if(tickCheck(4177)) {
                ui->BossLives->setText("2");
            } else if(tickCheck(4300)) { //8600, BOSS 4
                new_boss = new Enemy_Blue_3(player,300,60,175,200,Game::FrameWidth/2,200,0,0,0,0,false,true);
                connect(new_boss,SIGNAL(deadSignal(int,int)),this,SLOT(bossCorpse(int,int)));
                newBossInit(new_boss);
                for(int i=0;i<2;++i) {
                    new_enemy = new Enemy_Yellow(player,5,27.75,210,135,(i==0)?60:Game::FrameWidth-60,-27,0,2.4,0,-0.02,false,true);
                    newEnemyInit(new_enemy);
                    new_enemy->setInvulnerable();
                    new_effect = new_enemy->showShield();
                    newEffectInit(new_effect);
                    connect(new_boss,SIGNAL(useSkill(QString)),new_enemy,SIGNAL(killItsBullets()));
                    connect(new_boss,SIGNAL(useSkill(QString)),new_enemy,SLOT(killItself()));
                }
                tickFreeze();
            } else if(tickCheck(4302)) {
                ui->BossLives->setText("1");
            } else if(tickCheck(4425)) { //8850, BOSS 5
                new_boss = new Enemy_Blue_1(player,380,60,175,200,Game::FrameWidth/2,200,0,0,0,0,false,true);
                newBossInit(new_boss);
                for(int i=0;i<2;++i) {
                    new_enemy = new Enemy_Pink(player,5,27.75,170,55,(i==0)?100:Game::FrameWidth-100,-27,0,2.4,0,-0.02,false,true);
                    newEnemyInit(new_enemy);
                    new_enemy->setInvulnerable();
                    new_effect = new_enemy->showShield();
                    newEffectInit(new_effect);
                    connect(new_boss,SIGNAL(useSkill(QString)),new_enemy,SIGNAL(killItsBullets()));
                    connect(new_boss,SIGNAL(useSkill(QString)),new_enemy,SLOT(killItself()));
                }
                tickFreeze();
            } else if(tickCheck(4427)) { //8852, END
                ui->BossLives->hide();
                ui->BossSkill->hide();
                if(player!=nullptr) player->gameEndSetting();
            } else if(tickCheck(4700)) { //9400, WIN LIST
                ticking=false;
                //Point
                ui->PlayerPoint_list->display(ui->PlayerPoint->value());
                //Health
                ui->PlayerLife_list->display(ui->PlayerLife->value());
                ui->PlayerLife_listBonus->display(5);
                //Shield
                ui->PlayerSkill_list->display(ui->PlayerSkill->value());
                ui->PlayerSkill_listBonus->display(2);
                //Total
                ui->PlayerTotalPoint->display(ui->PlayerPoint->value()+ui->PlayerLife->value()*5+ui->PlayerSkill->value()*2);
                //List
                ui->EndList->setCurrentIndex(Game::EndListPageWon);
                ui->EndList->show();
                //game state
                gamestate=Game::GameStateWon;
            }
            break;
        //level 2
        case 2:
            if(tickCheck(250,131,5)) { //500+262i, 5 times
                for(int i=0;i<2;++i) {
                    new_enemy = new Enemy_2_Green(player,3,35,150,100,(i==0)?-35:Game::FrameWidth+35,250,(i==0)?2.4:-2.4,-1,0,0.004);
                    newEnemyInit(new_enemy);
                }
            } else if(tickCheck(1225)) {//2450
                for(int i=0;i<2;++i) {
                    new_enemy = new Enemy_2_Red(player,15,45,100,100,(i==0)?-45:Game::FrameWidth+45,80,(i==0)?2.4:-2.4,0,(i==0)?-0.008:0.008,0,false,true);
                    newEnemyInit(new_enemy);
                }
                tickFreeze();
            } else if(tickCheck(1313,200,5)) { //2625+400i, 5 times
                for(int i=0;i<2;++i) {
                    new_boss = new Enemy_2_Red(player,5,35,110,75,(i==0)?-30:Game::FrameWidth+30,170,(i==0)?2.4:-2.4,1,(i==0)?-0.008:0.008,-0.0062);
                    newEnemyInit(new_boss);
                    new_enemy = new Enemy_2_Green(player,5,30,173,75,(i==0)?-30:Game::FrameWidth+30,110,(i==0)?2.4:-2.4,1,(i==0)?-0.008:0.008,-0.0062);
                    newEnemyInit(new_enemy);
                    new_enemy->setInvulnerable();
                    connect(new_boss,SIGNAL(deadSignal()),new_enemy,SLOT(killItself()));
                }
            } else if(tickCheck(2600)) { //5200
                new_enemy = new Enemy_2_Yellow(player,55,45,100,100,Game::FrameWidth/2,-45,0,6,0,-0.06,false,true);
                newEnemyInit(new_enemy);
                tickFreeze();
            } else if(tickCheck(2750,300,4)) { //5500+600i, 4 times
                for(int i=0;i<2;++i) {
                    new_enemy = new Enemy_2_Yellow(player,5,35,225,75,(i==0)?-35:Game::FrameWidth+35,-35,(i==0)?1.6:-1.6,2,(i==0)?-0.01:0.01,0,false,true);
                    newEnemyInit(new_enemy);
                }
            } else if(tickCheck(4250)) { //8500
                new_boss = new Enemy_2_Red(player,70,50,175,60,Game::FrameWidth/2,-50,0,0,0,0,false,true);
                new_boss->moveTo(new_boss->getX(),190,100);
                newBossInit(new_boss);
                new_enemy = new Enemy_2_Green(player,3,35,150,80,Game::FrameWidth/2-50,-35,0,0,0,0,false,true);
                new_enemy->moveTo(new_enemy->getX(),95,200);
                newEnemyInit(new_enemy);
                new_enemy->setInvulnerable();
                new_effect = new_enemy->showShield();
                newEffectInit(new_effect);
                connect(new_boss,SIGNAL(deadSignal()),new_enemy,SLOT(killItself()));
                new_enemy = new Enemy_2_Yellow(player,3,35,150,140,Game::FrameWidth/2+50,-35,0,0,0,0,false,true);
                new_enemy->moveTo(new_enemy->getX(),95,200);
                newEnemyInit(new_enemy);
                new_enemy->setInvulnerable();
                new_effect = new_enemy->showShield();
                newEffectInit(new_effect);
                connect(new_boss,SIGNAL(deadSignal()),new_enemy,SLOT(killItself()));
                tickFreeze();
                new_enemy = new Enemy_2_Green(player,3,35,150,100,Game::FrameWidth/2+115,-35,0,0,0,0,false,true);
                new_enemy->moveTo(new_enemy->getX(),170,200);
                newEnemyInit(new_enemy);
                new_enemy->setInvulnerable();
                new_effect = new_enemy->showShield();
                newEffectInit(new_effect);
                connect(new_boss,SIGNAL(deadSignal()),new_enemy,SLOT(killItself()));
                new_enemy = new Enemy_2_Yellow(player,3,35,150,120,Game::FrameWidth/2-115,-35,0,0,0,0,false,true);
                new_enemy->moveTo(new_enemy->getX(),170,200);
                newEnemyInit(new_enemy);
                new_enemy->setInvulnerable();
                new_effect = new_enemy->showShield();
                newEffectInit(new_effect);
                connect(new_boss,SIGNAL(deadSignal()),new_enemy,SLOT(killItself()));
                tickFreeze();
            } else if(tickCheck(4400)) { //8800
                new_enemy = new Enemy_2_Pink(player,55,45,75,75,Game::FrameWidth/2,-45,0,6,0,-0.06,false,true);
                newEnemyInit(new_enemy);
                tickFreeze();
            } else if(tickCheck(4550,275,3)) { //9100+550i, 3 times
                const int t = timesCount(4550,275);
                new_enemy = new Enemy_2_Pink(player,11,30,20,70,Game::FrameWidth/2,-30,0,0,0,0,false,true);
                new_enemy->moveTo(80+t*60,50+t*60);
                newEnemyInit(new_enemy);
            } else if(tickCheck(4688,275,3)) { //9375+550i, 3 times
                const int t = timesCount(4688,275);
                new_enemy = new Enemy_2_Pink(player,11,30,20,70,Game::FrameWidth/2,-30,0,0,0,0,false,true);
                new_enemy->moveTo(Game::FrameWidth-80-t*60,50+t*60);
                newEnemyInit(new_enemy);
            } else if(tickCheck(5750)) { //11500
                new_boss = new Enemy_2_Green_Tri(player,80,50,163,210,Game::FrameWidth/2,-50,0,0,0,0,false,true);
                new_boss->moveTo(new_boss->getX(),460,125);
                newBossInit(new_boss);
                new_enemy = new Enemy_2_Pink(player,5,28,150,175,Game::FrameWidth/2,-28,0,0,0,0,false,true);
                new_enemy->moveTo(new_enemy->getX(),290,175);
                new_enemy->setInvulnerable();
                newEnemyInit(new_enemy);
                new_effect = new_enemy->showShield();
                newEffectInit(new_effect);
                connect(new_boss,SIGNAL(deadSignal()),new_enemy,SLOT(killItself()));
                new_enemy = new Enemy_2_Yellow(player,5,28,300,175,Game::FrameWidth/2+40,-28,0,0,0,0,false,true);
                new_enemy->moveTo(new_enemy->getX(),220,175);
                new_enemy->setInvulnerable();
                newEnemyInit(new_enemy);
                new_effect = new_enemy->showShield();
                newEffectInit(new_effect);
                connect(new_boss,SIGNAL(deadSignal()),new_enemy,SLOT(killItself()));
                new_enemy = new Enemy_2_Red(player,5,28,250,175,Game::FrameWidth/2-40,-28,0,0,0,0,false,true);
                new_enemy->moveTo(new_enemy->getX(),220,175);
                new_enemy->setInvulnerable();
                newEnemyInit(new_enemy);
                new_effect = new_enemy->showShield();
                newEffectInit(new_effect);
                connect(new_boss,SIGNAL(deadSignal()),new_enemy,SLOT(killItself()));
                tickFreeze();
            } else if(tickCheck(5802,181,3)) { //11603+361i, 3 times, warning bar fade in
                this->warningFadeIn();
            } else if(tickCheck(5894,181,3)) { //11788+361i, 3 times, warning bar fade out
                this->warningFadeOut();
            } else if(tickCheck(6374)) { //12748, BOSS 1
                ui->BossLives->show();
                ui->BossHealth->setGeometry(100,40,690,30);
                new_boss = new Enemy_2_Blue_1(player,340,60,50,200,Game::FrameWidth/2,-59,0,0,0,0,false,true);
                connect(new_boss,SIGNAL(deadSignal(int,int)),this,SLOT(bossCorpse(int,int)));
                new_boss->moveTo(Game::FrameWidth/2,200,165);
                newBossInit(new_boss);
                ui->BossLives->setText("5");
                tickFreeze();
            } else if(tickCheck(6376)) { //12750
                ui->BossLives->setText("4");
            } else if(tickCheck(6499)) { //12998, BOSS 2
                new_boss = new Enemy_2_Blue_2(player,370,60,150,200,Game::FrameWidth/2,200,0,0,0,0,false,true);
                connect(new_boss,SIGNAL(deadSignal(int,int)),this,SLOT(bossCorpse(int,int)));
                newBossInit(new_boss);
                for(int i=0;i<2;++i) {
                    new_enemy = new Enemy_2_Green(player,5,30,223,200,Game::FrameWidth/2,200,(i==0)?-2.4:2.4,1.8,0,-0.02,true);
                    newEnemyInit(new_enemy);
                    new_enemy->setInvulnerable();
                    new_enemy->fadein(1000);
                    new_effect = new_enemy->showShield();
                    newEffectInit(new_effect);
                    connect(new_boss,SIGNAL(useSkill(QString)),new_enemy,SIGNAL(killItsBullets()));
                    connect(new_boss,SIGNAL(useSkill(QString)),new_enemy,SLOT(killItself()));
                }
                tickFreeze();
            } else if(tickCheck(6501)) { //13000
                ui->BossLives->setText("3");
            } else if(tickCheck(6624)) { //13248, BOSS 3
                new_boss = new Enemy_2_Blue_5(player,310,60,150,200,Game::FrameWidth/2,200,0,0,0,0,false,true);
                connect(new_boss,SIGNAL(deadSignal(int,int)),this,SLOT(bossCorpse(int,int)));
                newBossInit(new_boss);
                for(int i=0;i<2;++i) {
                    new_enemy = new Enemy_2_Red(player,5,30,223,200,Game::FrameWidth/2,200,(i==0)?-1.6:1.6,-2.4,0,0.02,true,true);
                    newEnemyInit(new_enemy);
                    new_enemy->setInvulnerable();
                    new_enemy->fadein(1000);
                    new_effect = new_enemy->showShield();
                    newEffectInit(new_effect);
                    connect(new_boss,SIGNAL(useSkill(QString)),new_enemy,SIGNAL(killItsBullets()));
                    connect(new_boss,SIGNAL(useSkill(QString)),new_enemy,SLOT(killItself()));
                }
                tickFreeze();
            } else if(tickCheck(6626)) { //13250
                ui->BossLives->setText("2");
            } else if(tickCheck(6749)) { //13498, BOSS 4
                new_boss = new Enemy_2_Blue_4(player,390,60,150,200,Game::FrameWidth/2,200,0,0,0,0,false,true);
                connect(new_boss,SIGNAL(deadSignal(int,int)),this,SLOT(bossCorpse(int,int)));
                newBossInit(new_boss);
                for(int i=0;i<2;++i) {
                    new_enemy = new Enemy_2_Yellow(player,5,30,125,250,Game::FrameWidth/2,200,(i==0)?-1.6:1.6,-2.4,0,0.02,true,true);
                    newEnemyInit(new_enemy);
                    new_enemy->setInvulnerable();
                    new_enemy->fadein(1000);
                    new_effect = new_enemy->showShield();
                    newEffectInit(new_effect);
                    connect(new_boss,SIGNAL(useSkill(QString)),new_enemy,SIGNAL(killItsBullets()));
                    connect(new_boss,SIGNAL(useSkill(QString)),new_enemy,SLOT(killItself()));
                }
                tickFreeze();
            } else if(tickCheck(6751)) { //13500
                ui->BossLives->setText("1");
            } else if(tickCheck(6874)) { //13748, BOSS 5
                new_boss = new Enemy_2_Blue_3(player,460,60,200,200,Game::FrameWidth/2,200,0,0,0,0,false,true);
                newBossInit(new_boss);
                for(int i=0;i<2;++i) {
                    new_enemy = new Enemy_2_Pink(player,5,30,150,150,Game::FrameWidth/2,200,(i==0)?-1.6:1.6,-2.4,0,0.02,true,true);
                    newEnemyInit(new_enemy);
                    new_enemy->setInvulnerable();
                    new_enemy->fadein(1000);
                    new_effect = new_enemy->showShield();
                    newEffectInit(new_effect);
                    connect(new_boss,SIGNAL(useSkill(QString)),new_enemy,SIGNAL(killItsBullets()));
                    connect(new_boss,SIGNAL(useSkill(QString)),new_enemy,SLOT(killItself()));
                }
                tickFreeze();
            } else if(tickCheck(6876)) { //13750, END
                ui->BossLives->hide();
                ui->BossSkill->hide();
                if(player!=nullptr) player->gameEndSetting();
            } else if(tickCheck(7213)) { //14425, WIN LIST
                ticking=false;
                //Point
                ui->PlayerPoint_list->display(ui->PlayerPoint->value());
                //Health
                ui->PlayerLife_list->display(ui->PlayerLife->value());
                ui->PlayerLife_listBonus->display(5);
                //Shield
                ui->PlayerSkill_list->display(ui->PlayerSkill->value());
                ui->PlayerSkill_listBonus->display(2);
                //Total
                ui->PlayerTotalPoint->display(ui->PlayerPoint->value()+ui->PlayerLife->value()*5+ui->PlayerSkill->value()*2);
                //List
                ui->EndList->setCurrentIndex(Game::EndListPageWon);
                ui->EndList->show();
                //game state
                gamestate=Game::GameStateWon;
            }
            break;
        //level 3
        case 3:
            if(tickCheck(250,125,5)) {  //500+250i, 5 times
                const int t = timesCount(250,125);
                for(int i=0;i<2;++i) newMagicEffect(120,120,(i==0)?300-60*t:Game::FrameWidth-300+60*t,80+100*t);
            } else if(tickCheck(350,125,5)) {  //700+250i, 5 times
                for(int i=0;i<2;++i) {
                    const int t = timesCount(350,125);
                    new_enemy = new Enemy_3_Green(player,3,40,140,25,(i==0)?300-60*t:Game::FrameWidth-300+60*t,80+100*t,(i==0)?0.2:-0.2,0,0,-0.0024);
                    new_enemy->fadein();
                    newEnemyInit(new_enemy);
                }
            } else if(tickCheck(1350)) {  //2700
                for(int i=0;i<2;++i) newMagicEffect(150,150,(i==0)?100:Game::FrameWidth-100,150);
            } else if(tickCheck(1450)) {  //2900
                for(int i=0;i<2;++i) {
                    new_enemy = new Enemy_3_Red(player,14,50,125,60,(i==0)?100:Game::FrameWidth-100,150,(i==0)?0:-0.002,0,(i==0)?0:0.004,0,false,true);
                    new_enemy->fadein();
                    newEnemyInit(new_enemy);
                }
            } else if(tickCheck(1900)) {  //3800
                newMagicEffect(180,180,Game::FrameWidth/2,220,363);
                for(int t=0;t<2;++t) for(int i=0;i<2;++i) newMagicEffect(120,120,(i==0)?300-80*t:Game::FrameWidth-300+80*t,80+100*t,363);
            } else if(tickCheck(2000)) {  //4000
                new_boss = new Enemy_3_Green(player,45,60,25,188,Game::FrameWidth/2,220);
                new_boss->setInvulnerable();
                new_boss->fadein();
                newBossInit(new_boss);
            } else if(tickCheck(2063,63,2)) {  //4125+125i, 2 times
                const int t=timesCount(2063,63);
                for(int i=0;i<2;++i) {
                    new_enemy = new Enemy_3_Red(player,5,40,125,50+88*(1-t),(i==0)?300-80*t:Game::FrameWidth-300+80*t,80+100*t);
                    new_enemy->setInvulnerable();
                    new_effect = new_enemy->showShield();
                    newEffectInit(new_effect);
                    connect(new_boss,SIGNAL(deadSignal()),new_enemy,SLOT(killItself()));
                    new_enemy->fadein();
                    newEnemyInit(new_enemy);
                }
            } else if(tickCheck(2127)) { //4251
                tickFreeze();
                new_boss->setVulnerable();
            } else if(tickCheck(2250)) { //4500
                newMagicEffect(180,180,Game::FrameWidth/2,250);
            } else if(tickCheck(2350)) { //4700
                new_enemy = new Enemy_3_Yellow(player,30,60,125,60,Game::FrameWidth/2,250);
                new_enemy->fadein();
                newEnemyInit(new_enemy);
                tickFreeze();
            } else if(tickCheck(2475,125,8)) { //4950+250i, 8 times
                const int t=timesCount(2475,125);
                new_enemy = new Enemy_3_Yellow(player,9,40,250,25,(t%2==0)?-40:Game::FrameWidth+40,50+60*((t>=4)?7-t:t),(t%2==0)?3:-3,0,0,0,true);
                newEnemyInit(new_enemy);
                if(t==7) tickFreeze();
            } else if(tickCheck(3500)) { //7000
                newMagicEffect(180,180,Game::FrameWidth/2,100);
                for(int i=0;i<2;++i) newMagicEffect(120,120,(i==0)?80:Game::FrameWidth-80,Game::FrameHeight-70);
            } else if(tickCheck(3600)) { //7200
                new_boss = new Enemy_3_Yellow(player,60,60,150,25,Game::FrameWidth/2,100);
                new_boss->fadein();
                newBossInit(new_boss);
                for(int i=0;i<2;++i) {
                    new_enemy = new Enemy_3_Green(player,5,40,125,60,(i==0)?80:Game::FrameWidth-80,Game::FrameHeight-70,(i==0)?2.6:-2.6,0,0,0,true);
                    new_enemy->setInvulnerable();
                    new_effect = new_enemy->showShield();
                    newEffectInit(new_effect);
                    connect(new_boss,SIGNAL(deadSignal()),new_enemy,SLOT(killItself()));
                    new_enemy->fadein();
                    newEnemyInit(new_enemy);
                }
                tickFreeze();
            } else if(tickCheck(3750)) { //7500
                newMagicEffect(180,180,Game::FrameWidth/2,250);
            } else if(tickCheck(3850)) {  //7700
                new_enemy = new Enemy_3_Pink(player,40,60,60,50,Game::FrameWidth/2,250);
                new_enemy->fadein();
                newEnemyInit(new_enemy);
                tickFreeze();
            } else if(tickCheck(3950,188,4)) { //7900+375i, 4 times
                if(timesCount(3950,188)==3) tickFreeze();
                for(int i=-1;i<=1;++i) {
                    new_enemy = new Enemy_3_Pink(player,(i==0)?6:3,40,400,150,Game::FrameWidth/2+i*(Game::FrameWidth/2+40),-40+std::abs(i)*100);
                    new_enemy->moveTo(Game::FrameWidth/2+i*100,240+std::abs(i)*87,300);
                    newEnemyInit(new_enemy);
                }
            } else if(tickCheck(4638)) { //9275
                newMagicEffect(180,180,Game::FrameWidth/2,100);
                for(int i=0;i<2;++i) newMagicEffect(120,120,(i==0)?80:Game::FrameWidth-80,70);
            } else if(tickCheck(4738)) { //9475
                new_boss = new Enemy_3_Pink_Cross(player,35,60,55,25,Game::FrameWidth/2,100);
                new_boss->fadein();
                newBossInit(new_boss);
                for(int i=0;i<2;++i) {
                    new_enemy = new Enemy_3_Green(player,5,40,65,75,(i==0)?80:Game::FrameWidth-80,70);
                    new_enemy->setInvulnerable();
                    new_effect = new_enemy->showShield();
                    newEffectInit(new_effect);
                    connect(new_boss,SIGNAL(deadSignal()),new_enemy,SLOT(killItself()));
                    new_enemy->fadein();
                    newEnemyInit(new_enemy);
                }
                tickFreeze();
            } else if(tickCheck(4789,181,3)) {  //9578+361i, 3 times, warning bar fade in
                this->warningFadeIn();
            } else if(tickCheck(4881,181,3)) {  //9763+361i, 3 times, warning bar fade out
                this->warningFadeOut();
            } else if(tickCheck(5363)) {  //10725
                new_effect = new Effect(QString(":/res/effect/magic_blue.png"),120,120,256,256,175,Game::FrameWidth/2,200,0,0,0,0,true);
                new_effect->setOpacity(0.6);
                new_effect->fadein();
                newEffectInit(new_effect);
            } else if(tickCheck(5463)) {  //10925, BOSS 1
                ui->BossLives->show();
                ui->BossHealth->setGeometry(100,40,690,30);
                new_boss = new Enemy_3_Blue_1(player,250,80,18,200,Game::FrameWidth/2,200,0,0,0,0,0,true);
                connect(new_boss,SIGNAL(deadSignal(int,int)),this,SLOT(bossCorpse(int,int)));
                new_boss->fadein(1500);
                newBossInit(new_boss);
                ui->BossLives->setText("5");
                tickFreeze();
            } else if(tickCheck(5465)) {  //10927
                ui->BossLives->setText("4");
                for(int i=0;i<2;++i) newMagicEffect(100,100,(i==0)?160:Game::FrameWidth-160,150,200);
            } else if(tickCheck(5588)) {  //11175, BOSS 2
                new_boss = new Enemy_3_Blue_2(player,320,80,50,200,Game::FrameWidth/2,200,0,0,0,0,0,true);
                connect(new_boss,SIGNAL(deadSignal(int,int)),this,SLOT(bossCorpse(int,int)));
                new_boss->fadein(1500);
                newBossInit(new_boss);
                for(int i=0;i<2;++i) {
                    new_enemy = new Enemy_3_Green(player,5,30,115,150,(i==0)?160:Game::FrameWidth-160,150);
                    newEnemyInit(new_enemy);
                    new_enemy->setInvulnerable();
                    new_enemy->fadein(1000);
                    new_effect = new_enemy->showShield();
                    newEffectInit(new_effect);
                    connect(new_boss,SIGNAL(useSkill(QString)),new_enemy,SIGNAL(killItsBullets()));
                    connect(new_boss,SIGNAL(useSkill(QString)),new_enemy,SLOT(killItself()));
                }
                tickFreeze();
            } else if(tickCheck(5590)) {  //11177
                ui->BossLives->setText("3");
                for(int i=0;i<2;++i) newMagicEffect(100,100,(i==0)?160:Game::FrameWidth-160,150,200);
            } else if(tickCheck(5713)) {  //11425, BOSS 3
                new_boss = new Enemy_3_Blue_3(player,370,80,50,200,Game::FrameWidth/2,200,0,0,0,0,0,true);
                connect(new_boss,SIGNAL(deadSignal(int,int)),this,SLOT(bossCorpse(int,int)));
                new_boss->fadein(1500);
                newBossInit(new_boss);
                for(int i=0;i<2;++i) {
                    new_enemy = new Enemy_3_Red(player,5,30,125,75,(i==0)?160:Game::FrameWidth-160,150);
                    newEnemyInit(new_enemy);
                    new_enemy->setInvulnerable();
                    new_enemy->fadein(1000);
                    new_effect = new_enemy->showShield();
                    newEffectInit(new_effect);
                    connect(new_boss,SIGNAL(useSkill(QString)),new_enemy,SIGNAL(killItsBullets()));
                    connect(new_boss,SIGNAL(useSkill(QString)),new_enemy,SLOT(killItself()));
                }
                tickFreeze();
            } else if(tickCheck(5715)) {  //11427
                ui->BossLives->setText("2");
                for(int i=0;i<2;++i) newMagicEffect(100,100,(i==0)?160:Game::FrameWidth-160,150,200);
            } else if(tickCheck(5838)) {  //11675, BOSS 4
                new_boss = new Enemy_3_Blue_4(player,395,80,50,200,Game::FrameWidth/2,200,0,0,0,0,0,true);
                connect(new_boss,SIGNAL(deadSignal(int,int)),this,SLOT(bossCorpse(int,int)));
                new_boss->fadein(1500);
                newBossInit(new_boss);
                for(int i=0;i<2;++i) {
                    new_enemy = new Enemy_3_Yellow(player,5,30,250,150,(i==0)?160:Game::FrameWidth-160,150);
                    newEnemyInit(new_enemy);
                    new_enemy->setInvulnerable();
                    new_enemy->fadein(1000);
                    new_effect = new_enemy->showShield();
                    newEffectInit(new_effect);
                    connect(new_boss,SIGNAL(useSkill(QString)),new_enemy,SIGNAL(killItsBullets()));
                    connect(new_boss,SIGNAL(useSkill(QString)),new_enemy,SLOT(killItself()));
                }
                tickFreeze();
            } else if(tickCheck(5840)) {  //11677
                ui->BossLives->setText("1");
                for(int i=0;i<2;++i) newMagicEffect(100,100,(i==0)?160:Game::FrameWidth-160,150,200);
            } else if(tickCheck(5963)) {  //11925, BOSS 5
                new_boss = new Enemy_3_Blue_5(player,500,80,60,200,Game::FrameWidth/2,200,0,0,0,0,0,true);
                new_boss->fadein(1500);
                newBossInit(new_boss);
                for(int i=0;i<2;++i) {
                    new_enemy = new Enemy_3_Pink(player,5,30,150,150,(i==0)?160:Game::FrameWidth-160,150);
                    newEnemyInit(new_enemy);
                    new_enemy->setInvulnerable();
                    new_enemy->fadein(1000);
                    new_effect = new_enemy->showShield();
                    newEffectInit(new_effect);
                    connect(new_boss,SIGNAL(useSkill(QString)),new_enemy,SIGNAL(killItsBullets()));
                    connect(new_boss,SIGNAL(useSkill(QString)),new_enemy,SLOT(killItself()));
                }
                tickFreeze();
            } else if(tickCheck(5965)) {  //11927, END
                ui->BossLives->hide();
                ui->BossSkill->hide();
                if(player!=nullptr) player->gameEndSetting();
            } else if(tickCheck(6300)) {  //12600, WIN LIST
                ticking=false;
                //Point
                ui->PlayerPoint_list->display(ui->PlayerPoint->value());
                //Health
                ui->PlayerLife_list->display(ui->PlayerLife->value());
                ui->PlayerLife_listBonus->display(5);
                //Shield
                ui->PlayerSkill_list->display(ui->PlayerSkill->value());
                ui->PlayerSkill_listBonus->display(2);
                //Total
                ui->PlayerTotalPoint->display(ui->PlayerPoint->value()+ui->PlayerLife->value()*5+ui->PlayerSkill->value()*2);
                //List
                ui->EndList->setCurrentIndex(Game::EndListPageWon);
                ui->EndList->show();
                //game state
                gamestate=Game::GameStateWon;
            }
            break;
        //level 4
        case 4:
            if(tickCheck(250,225,3)) {
                for(int i=0;i<2;++i) {
                    new_enemy = new Enemy_4_Green(player,5,40,40,63,i?-40:Game::FrameWidth+40,-40,i?3.4:-3.4,3.4,i?-0.028:0.028,-0.028,false,true);
                    newEnemyInit(new_enemy);
                }
            } else if(tickCheck(1100,500,3)) {
                const int t = timesCount(1100,500);
                for(int i=0;i<2;++i) {
                    new_enemy = new Enemy_4_Red(player,7+3*t,40,75,200,i?-40:Game::FrameWidth+40,-40,i?3.4:-3.4,3.4,i?-0.028:0.028,-0.028,false);
                    newEnemyInit(new_enemy);
                }
            } else if(tickCheck(2600)) {
                for(int i=0;i<2;++i) {
                    new_enemy = new Enemy_4_Yellow(player,12,40,200,100,i?-40:Game::FrameWidth+40,-40,i?3.4:-3.4,3.4,i?-0.028:0.028,-0.028,false,true);
                    newEnemyInit(new_enemy);
                }
                tickFreeze();
            } else if(tickCheck(2750,150,10)) {
                const int t = timesCount(5500,200);
                new_enemy = new Enemy_4_Green(player,7,40,40,63,(t%2==0)?-40:Game::FrameWidth+40,-40,(t%2==0)?3.4:-3.4,3.4,(t%2==0)?-0.028:0.028,-0.028,false,true);
                newEnemyInit(new_enemy);
                if(tickCheck(2750+150*9)) tickFreeze();
            } else if(tickCheck(3075)) {
                new_boss = new Enemy_4_Yellow(player,25,55,175,100,Game::FrameWidth/2,-40,0,3.4,0,-0.028,false,true);
                newBossInit(new_boss);
            } else if(tickCheck(4300,250,3)) {
                const int t = timesCount(4300,250);
                for(int i=0;i<2;++i) {
                    new_enemy = new Enemy_4_Pink(player,9,40,5,5,i?220-t*80:Game::FrameWidth-220+t*80,-40,0,1.8,0,0.0018);
                    newEnemyInit(new_enemy);
                }
            } else if(tickCheck(5500,100,18)) {
                int t = timesCount(5500,100);
                new_enemy = new Enemy_4_Red(player,t>8?7:4,40,75,200,qrand()%(Game::FrameWidth-80)+40,-40,0,2.4,0,-0.0018,true);
                newEnemyInit(new_enemy);
                if(t==17) tickFreeze();
            } else if(tickCheck(5750,200,7)) {
                const int t = timesCount(5750,200);
                new_enemy = new Enemy_4_Pink(player,t>8?7:4,40,5,5,t%2?70:Game::FrameWidth-70,-40,t%2?0.2:-0.2,1.8,t%2?-0.00004:0.00004,0.0018);
                newEnemyInit(new_enemy);
            } else if(tickCheck(7252,181,3)) { //12403+361i, 3 times, warning bar fade in
                this->warningFadeIn();
            } else if(tickCheck(7344,181,3)) { //12588+361i, 3 times, warning bar fade out
                this->warningFadeOut();
            } else if(tickCheck(7824)) { //14048, BOSS 1
                ui->BossLives->show();
                ui->BossHealth->setGeometry(100,40,690,30);
                new_boss = new Enemy_4_Blue_1(player,270,60,35,200,Game::FrameWidth/2,-60,0,0,0,0,false,true);
                connect(new_boss,SIGNAL(deadSignal(int,int)),this,SLOT(bossCorpse(int,int)));
                new_boss->moveTo(Game::FrameWidth/2,200,165);
                newBossInit(new_boss);
                ui->BossLives->setText("5");
                tickFreeze();
            } else if(tickCheck(7826)) { //14050
                ui->BossLives->setText("4");
            } else if(tickCheck(7949)) { //14298, BOSS 2
                new_boss = new Enemy_4_Blue_2(player,250,60,45,200,Game::FrameWidth/2,200,0,0,0,0,false,true);
                connect(new_boss,SIGNAL(deadSignal(int,int)),this,SLOT(bossCorpse(int,int)));
                new_boss->fadein(1500);
                newBossInit(new_boss);
                tickFreeze();
            } else if(tickCheck(7951)) { //14300
                ui->BossLives->setText("3");
            } else if(tickCheck(8074)) { //14548, BOSS 3
                new_boss = new Enemy_4_Blue_3(player,330,60,45,200,Game::FrameWidth/2,200,0,0,0,0,false,true);
                connect(new_boss,SIGNAL(deadSignal(int,int)),this,SLOT(bossCorpse(int,int)));
                new_boss->fadein(1500);
                newBossInit(new_boss);
                tickFreeze();
            } else if(tickCheck(8076)) { //14550
                ui->BossLives->setText("2");
            } else if(tickCheck(8199)) { //14798, BOSS 4
                new_boss = new Enemy_4_Blue_4(player,450,60,55,200,Game::FrameWidth/2,200,0,0,0,0,false,true);
                connect(new_boss,SIGNAL(deadSignal(int,int)),this,SLOT(bossCorpse(int,int)));
                new_boss->fadein(1500);
                newBossInit(new_boss);
                tickFreeze();
            } else if(tickCheck(8201)) { //14800
                ui->BossLives->setText("1");
            } else if(tickCheck(8324)) { //8324, BOSS 5
                new_boss = new Enemy_4_Blue_5(player,280,60,55,200,Game::FrameWidth/2,200,0,0,0,0,false,true);
                new_boss->fadein(1500);
                newBossInit(new_boss);
                tickFreeze();
            } else if(tickCheck(8326)) { //8326
                ui->BossLives->hide();
                ui->BossSkill->hide();
                if(player!=nullptr) player->gameEndSetting();
            } else if(tickCheck(8640)) { //8640, WIN LIST
                ticking=false;
                //Point
                ui->PlayerPoint_list->display(ui->PlayerPoint->value());
                //Health
                ui->PlayerLife_list->display(ui->PlayerLife->value());
                ui->PlayerLife_listBonus->display(5);
                //Shield
                ui->PlayerSkill_list->display(ui->PlayerSkill->value());
                ui->PlayerSkill_listBonus->display(2);
                //Total
                ui->PlayerTotalPoint->display(ui->PlayerPoint->value()+ui->PlayerLife->value()*5+ui->PlayerSkill->value()*2);
                //List
                ui->EndList->setCurrentIndex(Game::EndListPageWon);
                ui->EndList->show();
                //game state
                gamestate=Game::GameStateWon;
            }
            break;
        default:
            qDebug() << "error: can't get what level is selected";
        }
    }
    //draw
    if(gamestate!=Game::GameStatePaused) emit doImgMove();
    //player death
    if(player->isDead()) {
        //game end
        player->gameEndSetting();
        player->setOpacity(0.5);
        ui->EndList->setCurrentIndex(Game::EndListPageFailed);
        ui->EndList->show();
        //game state
        gamestate=Game::GameStateFailed;
    }
    //ticking
    if(gamestate==Game::GameStatePlaying && ticking) ++tick;
    if(gamestate==Game::GameStatePlaying) ++gametime;
}
void MainWindow::backToMenu() {
    ticking=false;
    //kill timer
    delete timer;
    timer=nullptr;
    delete freezeTimer;
    freezeTimer=nullptr;
    //kill all objects
    for(std::vector<Bullet*>::iterator i=player_bullets.begin();i!=player_bullets.end();) {
        delete (*i);
        i = player_bullets.erase(i);
    }
    for(std::vector<Enemy*>::iterator i=enemies.begin();i!=enemies.end();) {
        delete (*i);
        i = enemies.erase(i);
    }
    for(std::list<Bullet*>::const_iterator i=enemy_bullets.begin();i!=enemy_bullets.end();) {
        delete (*i);
        i = enemy_bullets.erase(i);
    }
    delete player;
    player=nullptr;
    emit killEffects();
    dot=nullptr;
    //menu
    this->setGamePage(Game::GamePageMenu);
    //game state
    gamestate=Game::GameStateMenu;
}
void MainWindow::newEffectInit(Effect* new_effect) {
    if(new_effect!=nullptr) {
        scene->addItem(new_effect);
        connect(this,SIGNAL(doMove()),new_effect,SLOT(move()));
        connect(this,SIGNAL(doImgMove()),new_effect,SLOT(img_move()));
        connect(this,SIGNAL(killEffects()),new_effect,SLOT(killItself()));
    }
}
void MainWindow::newEnemyInit(Enemy* new_enemy) {
    if(new_enemy!=nullptr) {
        scene->addItem(new_enemy);
        enemies.push_back(new_enemy);
        connect(this,SIGNAL(doMove()),new_enemy,SLOT(move()));
        connect(this,SIGNAL(doImgMove()),new_enemy,SLOT(img_move()));
        connect(new_enemy,SIGNAL(pointGive(int)),this,SLOT(pointAdd(int)));
        connect(new_enemy,SIGNAL(summonEffect(Effect*)),this,SLOT(newEffectInit(Effect*)));
        connect(new_enemy,SIGNAL(summonEnemy(Enemy*)),this,SLOT(newEnemyInit(Enemy*)));
        connect(new_enemy,SIGNAL(soundPlay(Game::Sound)),this,SLOT(soundPlay(Game::Sound)));
    }
}
void MainWindow::newBossInit(Enemy* new_boss) {
    newEnemyInit(new_boss);
    //boss bar and lives
    ui->BossHealth->show();
    ui->BossHealth->setMaximum(new_boss->getHealth());
    ui->BossHealth->setValue(new_boss->getHealth());
    ui->BossSkill->setText("");
    connect(new_boss,SIGNAL(healthChanged(int)),ui->BossHealth,SLOT(setValue(int)));
    connect(new_boss,SIGNAL(useSkill(QString)),bossSkillFadeinAni,SLOT(start()));
    connect(new_boss,SIGNAL(useSkill(QString)),this,SLOT(bossSkillLengthSetting(QString)));
    connect(new_boss,SIGNAL(useSkill(QString)),bossSkillMoveInAni,SLOT(start()));
    connect(new_boss,SIGNAL(useSkill(QString)),ui->BossSkill,SLOT(show()));
    connect(new_boss,SIGNAL(useSkill(QString)),ui->BossSkill,SLOT(setText(QString)));
    connect(new_boss,SIGNAL(useSkill(QString)),flash,SLOT(flash()));
    connect(new_boss,SIGNAL(deadSignal()),ui->BossHealth,SLOT(hide()));
    connect(new_boss,SIGNAL(deadSignal()),ui->BossSkill,SLOT(hide()));
    connect(new_boss,SIGNAL(shakeScreen(short)),this,SLOT(sceneVibrate(short)));
    connect(new_boss,SIGNAL(shakeScreenVertical(short)),this,SLOT(sceneVibrateVertical(short)));
    connect(new_boss,SIGNAL(killAllBullets()),this,SLOT(killAllBullets()));
    connect(new_boss,SIGNAL(killOtherEnemies(Enemy*)),this,SLOT(killOtherEnemies(Enemy*)));
}
void MainWindow::killAllBullets() {
    for(Bullet* enemy_bullet: enemy_bullets) {
        enemy_bullet->killItself();
    }
}
void MainWindow::killOtherEnemies(Enemy* this_enemy) {
    for(Enemy* enemy: enemies) {
        if(enemy!=this_enemy) enemy->killItself();
    }
}
void MainWindow::bossSkillLengthSetting(QString skill) {
    int length=skill.length();
    bossSkillMoveInAni->setStartValue(QRect(480-length*50,80,length*50,50));
    bossSkillMoveInAni->setEndValue(QRect(790-length*50,80,length*50,50));
}
void MainWindow::newMagicEffect(int show_w, int show_h, double x, double y, int lifetime) {
    new_effect = new Effect(QString(":/res/effect/magic.png"),100,100,show_w,show_h,lifetime,x,y,0,0,0,0,true);
    new_effect->setOpacity(0.6);
    new_effect->fadein();
    newEffectInit(new_effect);
}
void MainWindow::pauseAndResume() {
    if(gamestate==Game::GameStatePlaying) {
        ui->EndList->setCurrentIndex(Game::EndListPagePaused);
        ui->EndList->show();
        gamestate=Game::GameStatePaused;
    } else {
        ui->EndList->hide();
        gamestate=Game::GameStatePlaying;
    }
    return;
}
void MainWindow::soundPlay(Game::Sound sound) {
    QMediaPlayer*& audioer = audioers.at((int)sound);
    if(!(audioer->state()==QMediaPlayer::PlayingState)) audioer->play();
    else {
        audioer->stop();
        audioer->play();
    }
}
void MainWindow::bossCorpse(int x, int y) {
    QString str;
    switch(level) {
    case 1:
        str = ":/res/effect/lightball.png";
        new_effect = new Effect(str,120,120,300,300,154,x,y,0,0,0,0,true);
        new_effect->rotateStart();
        break;
    case 2:
        str = ":/res/effect/stars.png";
        new_effect = new Effect(str,120,120,300,300,154,x,y,0,0,0,0,true);
        new_effect->rotateStart();
        break;
    case 3:
        str = ":/res/effect/magic_blue.png";
        new_effect = new Effect(str,120,120,256,256,154,x,y,0,0,0,0,true);
        new_effect->setOpacity(0.6);
        break;
    case 4:
        str = ":/res/effect/wind.png";
        new_effect = new Effect(str,120,120,300,300,154,x,y,0,0,0,0,true);
        new_effect->rotateStart();
        break;
    default:
        str = ":/res/effect/lightball.png";
        new_effect = new Effect(str,120,120,300,300,154,x,y,0,0,0,0,true);
        new_effect->rotateStart();
        qDebug() << "Error: can't find boss corpse img because can't get what level is";
        break;
    }
    new_effect->fadein();
    new_effect->moveTo(Game::FrameWidth/2,200,123);
    newEffectInit(new_effect);
}
void MainWindow::pointAdd(int point) {
    ui->PlayerPoint->display(ui->PlayerPoint->value()+point);
}
void MainWindow::tickFreeze() {
    ticking=false;
    ++tick;
    if(freezeTimer==nullptr) freezeTimer = new QTimer;
    freezeTimer->start(100);
    connect(freezeTimer,SIGNAL(timeout()),this,SLOT(tickContinueCheck()));
}
inline bool MainWindow::tickCheck(unsigned int tick) {
    return this->tick==tick;
}
inline bool MainWindow::tickCheck(unsigned int tick_init, unsigned int interval, unsigned int times) {
    return tick>=tick_init&&!((tick-tick_init)%interval)&&(tick-tick_init)/interval<times;
}
inline int MainWindow::timesCount(unsigned int tick_init, unsigned int interval) {
    return (tick-tick_init)/interval;
}
void MainWindow::tickContinueCheck() {
    if(enemies.size()==0 && freezeTimer!=nullptr) {
        delete freezeTimer;
        freezeTimer = nullptr;
        ticking=true;
    }
}
void MainWindow::healthColorChange(QString color) {
    //change health color
    QString str("color:");
    ui->PlayerLife->setStyleSheet(str.append(color).append(";"));
    ui->PlayerLifeLabel->setStyleSheet(str.append(color).append(";"));
    //change gameFrame border color
    QString str2("border: 6px solid ");
    ui->gameFrame->setStyleSheet(str2.append(color).append(";"));
}
void MainWindow::redFlash() {
    Flash* red_flash = new Flash(ui->GamePage);
    red_flash->setStyleSheet("background-color: red;");
    red_flash->setOpacity(0.3);
    red_flash->setFlashTime(400,400);
    red_flash->flash(true);
}
void MainWindow::sceneVibrate(short vibrate_count) {
    static QPropertyAnimation* vibrateAni=nullptr;
    static short vibrate_count_temp=0;
    bool firstVib=false;
    vibrate_count_temp = vibrate_count+1;
    if(vibrate_count<10) {
        if(vibrateAni!=nullptr) delete vibrateAni;
        else firstVib=true;
        vibrateAni = new QPropertyAnimation(scene,"sceneRect");
        vibrateAni->setDuration((50+(vibrate_count>0?1:-1)*(vibrate_count*vibrate_count)*2.2)/((firstVib||vibrate_count==9)?2:1));
        vibrateAni->setEasingCurve(vibrate_count==9?QEasingCurve::InSine:(firstVib?QEasingCurve::OutSine:QEasingCurve::InOutSine));
        vibrateAni->setStartValue(this->scene->sceneRect());
        if(vibrate_count==9) {
            vibrateAni->setEndValue(QRect(0,0,Game::FrameWidth,Game::FrameHeight));
        } else {
            vibrateAni->setEndValue(QRect((vibrate_count%2==0?-1:1)*(5-vibrate_count*vibrate_count*0.05),0,Game::FrameWidth,Game::FrameHeight));
        }
        connect(vibrateAni,&QPropertyAnimation::finished,[=](){
            sceneVibrate(vibrate_count_temp);
        });
        vibrateAni->start(QAbstractAnimation::KeepWhenStopped);
    } else {
        if(vibrateAni!=nullptr) delete vibrateAni;
        vibrateAni=nullptr;
    }
}
void MainWindow::sceneVibrateVertical(short vibrate_count) {
    static QPropertyAnimation* vibrateAni=nullptr;
    static short vibrate_count_temp=0;
    bool firstVib=false;
    vibrate_count_temp = vibrate_count+1;
    if(vibrate_count<10) {
        if(vibrateAni!=nullptr) delete vibrateAni;
        else firstVib=true;
        vibrateAni = new QPropertyAnimation(scene,"sceneRect");
        vibrateAni->setDuration((50+(vibrate_count>0?1:-1)*(vibrate_count*vibrate_count)*2.2)/((firstVib||vibrate_count==9)?2:1));
        vibrateAni->setEasingCurve(vibrate_count==9?QEasingCurve::InSine:(firstVib?QEasingCurve::OutSine:QEasingCurve::InOutSine));
        vibrateAni->setStartValue(this->scene->sceneRect());
        if(vibrate_count==9) {
            vibrateAni->setEndValue(QRect(0,0,Game::FrameWidth,Game::FrameHeight));
        } else {
            vibrateAni->setEndValue(QRect(0,(vibrate_count%2==0?-1:1)*(5-vibrate_count*vibrate_count*0.05),Game::FrameWidth,Game::FrameHeight));
        }
        connect(vibrateAni,&QPropertyAnimation::finished,[=](){
            sceneVibrateVertical(vibrate_count_temp);
        });
        vibrateAni->start(QAbstractAnimation::KeepWhenStopped);
    } else {
        if(vibrateAni!=nullptr) delete vibrateAni;
        vibrateAni=nullptr;
    }
}
void MainWindow::keyPressEvent(QKeyEvent *e) {
    if (!e->isAutoRepeat()) {
        switch (e->key()) {
        case Qt::Key_A:
        case Qt::Key_Left:
            right=false;
            left=true;
            break;
        case Qt::Key_D:
        case Qt::Key_Right:
            left=false;
            right=true;
            break;
        case Qt::Key_W:
        case Qt::Key_Up:
            down=false;
            up=true;
            break;
        case Qt::Key_S:
        case Qt::Key_Down:
            up=false;
            down=true;
            break;
        case Qt::Key_Z:
            if(gamestate!=Game::GameStateMenu) player->setShooting(true);
            Player::speed=Player::shootingSpeed;
            break;
        case Qt::Key_X:
            use_skill=true;
            break;
        case Qt::Key_Escape:
        case Qt::Key_P:
            if(gamestate==Game::GameStatePlaying||gamestate==Game::GameStatePaused) pauseAndResume();
            break;
        case Qt::Key_M:
            if(gamestate==Game::GameStateWon||gamestate==Game::GameStateFailed||gamestate==Game::GameStatePaused) backToMenu();
            break;
        case Qt::Key_R:
            if(gamestate==Game::GameStateWon||gamestate==Game::GameStateFailed||gamestate==Game::GameStatePaused) {
                backToMenu();
                start();
            }
            break;
        case Qt::Key_F11:
            if(!(this->windowState()==Qt::WindowFullScreen)) this->setWindowState(Qt::WindowFullScreen);
            else this->setWindowState(Qt::WindowMaximized);
            break;
        }
    }
    //secret up-up-down-down-left-right-left-right-B-A
    if (!e->isAutoRepeat()) {
        switch (e->key()) {
        case Qt::Key_Up:
            if(secret==0 || secret==1) ++secret;
            else if(secret!=2) secret=0;
            break;
        case Qt::Key_Down:
            if(secret==2 || secret==3) ++secret;
            else secret=0;
            break;
        case Qt::Key_Left:
            if(secret==4 || secret==6) ++secret;
            else secret=0;
            break;
        case Qt::Key_Right:
            if(secret==5 || secret==7) ++secret;
            else secret=0;
            break;
        case Qt::Key_B:
            if(secret==8) ++secret;
            else secret=0;
            break;
        case Qt::Key_A:
            if(secret==9) {
                if(gamestate==Game::GameStateMenu) start();
                player->setHealthTo999();
                ui->PlayerLife->display(player->getHealth());
            }
            secret=0;
            break;
        default:
            secret=0;
            break;
        }
    }
}
void MainWindow::keyReleaseEvent(QKeyEvent *e) {
    if (!e->isAutoRepeat()) {
        switch (e->key()) {
        case Qt::Key_A:
        case Qt::Key_Left:
            left=false;
            break;
        case Qt::Key_D:
        case Qt::Key_Right:
            right=false;
            break;
        case Qt::Key_W:
        case Qt::Key_Up:
            up=false;
            break;
        case Qt::Key_S:
        case Qt::Key_Down:
            down=false;
            break;
        case Qt::Key_Z:
            if(gamestate!=Game::GameStateMenu) player->setShooting(false);
            Player::speed=Player::nonShootingSpeed;
            break;
        case Qt::Key_X:
            use_skill=false;
            break;
        }
    }
}
MainWindow::~MainWindow()
{
    delete ui;
}
