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
    player(nullptr),dot(nullptr),secret(0),
    oriImg2(nullptr),cutImg(nullptr),oriImg(nullptr),
    bossHealthOpacityEff(this), bossLivesOpacityEff(this),
    dialogueProcessing(false), dialogueWidget(nullptr)
{
    ui->setupUi(this);
    this->setGamePage(Game::GamePageMenu);
    //boss objects
    ui->BossHealth->setGraphicsEffect(&bossHealthOpacityEff);
    ui->BossLives->setGraphicsEffect(&bossLivesOpacityEff);
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
    //endlist
    ui->EndList->hide();
    EndListAni = new WidgetAnimationer(ui->EndList);
    EndListAni->setOpacity(0.6);
    //button
    connect(ui->startButton,SIGNAL(clicked(bool)),this,SLOT(levelSelect()));
    connect(ui->QuitButton,SIGNAL(clicked(bool)),qApp,SLOT(quit()));
    connect(ui->LevelButton_1,SIGNAL(clicked(bool)),this,SLOT(start1()));
    connect(ui->LevelButton_2,SIGNAL(clicked(bool)),this,SLOT(start2()));
    connect(ui->LevelButton_3,SIGNAL(clicked(bool)),this,SLOT(start3()));
    connect(ui->LevelButton_4,SIGNAL(clicked(bool)),this,SLOT(start4()));
    auto buttonsConnect = [this](std::vector<KeyControlButton*>& buttons, const char* slot) {
        for(KeyControlButton* button: buttons) {
            connect(button,SIGNAL(clicked()),this,slot);
        }
    };
    std::vector<KeyControlButton*> restartButtons({ui->restartButton,
                                                ui->restartButton_2,
                                                ui->restartButton_3});
    buttonsConnect(restartButtons,SLOT(restart()));
    std::vector<KeyControlButton*> backtomenuButtons({ui->menuButton,
                                                ui->menuButton_2,
                                                ui->menuButton_3});
    buttonsConnect(backtomenuButtons,SLOT(backToMenu()));
    connect(ui->continueButton,SIGNAL(clicked(bool)),this,SLOT(pauseAndResume()));
    //key control button
    auto keyControlButtonsConnect = [this](std::vector<KeyControlButton*>& buttons, KeyControlButton::ArrowPos arrow_pos) {
        for(std::vector<KeyControlButton*>::iterator i=buttons.begin(); i!=buttons.end(); ++i) {
            connect(*i,SIGNAL(downSelect()),
                    i==buttons.end()-1?*(buttons.begin()):*(i+1),SLOT(selectThis()));
            connect(*i,SIGNAL(upSelect()),
                    i==buttons.begin()?*(buttons.end()-1):*(i-1),SLOT(selectThis()));
            (*i)->setArrowPos(arrow_pos);
            connect((*i),SIGNAL(soundPlay(Game::Sound)),this,SLOT(soundPlay(Game::Sound)));
        }
    };
    std::vector<KeyControlButton*> menuButtons({ui->startButton,
                                                ui->QuitButton});
    keyControlButtonsConnect(menuButtons,KeyControlButton::ArrowPosLeft);
    std::vector<KeyControlButton*> levelButtons({ui->LevelButton_1,
                                                 ui->LevelButton_2,
                                                 ui->LevelButton_3,
                                                 ui->LevelButton_4});
    keyControlButtonsConnect(levelButtons,KeyControlButton::ArrowPosDown);
    std::vector<KeyControlButton*> pauseListButtons({ui->menuButton,
                                                ui->restartButton,
                                                ui->continueButton});
    keyControlButtonsConnect(pauseListButtons,KeyControlButton::ArrowPosLeft);
    std::vector<KeyControlButton*> loseListButtons({ui->menuButton_2,
                                                ui->restartButton_2});
    keyControlButtonsConnect(loseListButtons,KeyControlButton::ArrowPosLeft);
    std::vector<KeyControlButton*> winListButtons({ui->menuButton_3,
                                                ui->restartButton_3});
    keyControlButtonsConnect(winListButtons,KeyControlButton::ArrowPosLeft);
    //focus policy
    setFocusPolicy(Qt::NoFocus);
    //flash
    flash = new Flash(ui->GamePage);
    //boss skill name animation
    bossSkillAni = new WidgetAnimationer(ui->BossSkill);
    bossSkillAni->setFadeDir(WidgetAnimationer::FadeDirectionLeft);
    //game state
    gamestate=Game::GameStateMenu;
    //level intro
    ui->levelIntro->hide();
    //level
    levelSelectAni = new WidgetAnimationer(ui->selectFrame);
    levelSelectAni->setFadeDir(WidgetAnimationer::FadeDirectionDown);
    //dialogue
    //audioer
    audioers.resize(8);
    for(QMediaPlayer*& audioer: audioers) {
        audioer = new QMediaPlayer(nullptr,QMediaPlayer::LowLatency);
    }
    auto soundSet = [this](Game::Sound sound, int vol, QString url) {
        audioers.at((int)sound)->setMedia(QUrl::fromLocalFile("C:/Users/USER/Documents/GitHub/Shooting-Game-Space-Defender/"+url));
        audioers.at((int)sound)->setVolume(vol);
    };
    soundSet(Game::SoundCymbal,10,"res/sound/cymbal.wav");
    soundSet(Game::SoundFire,10,"res/sound/fire.wav");
    soundSet(Game::SoundShoot01,30,"res/sound/shoot01.wav");
    soundSet(Game::SoundHit,20,"res/sound/hit.wav");
    soundSet(Game::SoundBd,70,"res/sound/bd.wav");
    soundSet(Game::SoundWarning,30,"res/sound/warning.wav");
    soundSet(Game::SoundWarning02,20,"res/sound/warning02.wav");
    soundSet(Game::SoundSnare,15,"res/sound/snare.wav");
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
        levelSelectAni->animationStart();
        break;
    default:
        break;
    }
}

void MainWindow::setEndListPage(Game::EndListPage page) {
    ui->EndList->setCurrentIndex((int)page);
    EndListAni->animationStart();
    KeyControlButton::unselect();
    switch (page) {
    case Game::EndListPagePaused:
        ui->continueButton->selectThis();
        ui->continueButton->disableClick(1000);
        break;
    case Game::EndListPageFailed:
        ui->restartButton_2->selectThis();
        ui->restartButton_2->disableClick(1000);
        break;
    case Game::EndListPageWon:
        ui->restartButton_3->selectThis();
        ui->restartButton_3->disableClick(1000);
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
    //dialogue
    if(dialogueProcessing) dialogueEnd();
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
    bossLivesOpacityEff.setOpacity(0.7);
    //boss skill name
    ui->BossSkill->hide();
    ui->BossSkill->setText("");
    bossSkillAni->setOpacity(0.7);
    //boss health bar
    ui->BossHealth->hide();
    ui->BossHealth->setGeometry(70,40,720,30);
    ui->BossHealth->setFormat("");
    ui->BossHealth->setValue(0);
    bossHealthOpacityEff.setOpacity(0.7);
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
    if(gamestate==Game::GameStatePlaying) {
        if(dialogueProcessing) dialogueWidget->setFocus();
        else this->setFocus();
    } else KeyControlButton::setFocusOnSelectedOne();
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
    if((gamestate==Game::GameStatePlaying && !dialogueProcessing)||gamestate==Game::GameStateWon) {
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
    if(gamestate==Game::GameStatePlaying && !dialogueProcessing) {
        //boss lives, skill bar and health bar
        if(!isPlayerPosHigh && player->getY()<280) {
            isPlayerPosHigh=true;
            bossHealthOpacityEff.setOpacity(0.3);
            bossLivesOpacityEff.setOpacity(0.3);
            bossSkillAni->setOpacity(0.3);
        } else if(isPlayerPosHigh && player->getY()>=280) {
            isPlayerPosHigh=false;
            bossHealthOpacityEff.setOpacity(0.7);
            bossLivesOpacityEff.setOpacity(0.7);
            bossSkillAni->setOpacity(0.7);
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
            if(tickCheck(220)) {
                dialogueStart({Dialogue("E-842，聽到請回答。",":/res/player.png",QRect(0,0,43,33)),
                               Dialogue("是，長官。",":/res/player.png",QRect(0,0,43,33)),
                               Dialogue("再複習一次你的任務！",":/res/player.png",QRect(0,0,43,33)),
                               Dialogue("收到。本次任務是要到各個星球探查「智多星」的座標位置，並與他們的首領交流。",":/res/player.png",QRect(0,0,43,33)),
                               Dialogue("很好，在那之後，向我們回報你們的對話。",":/res/player.png",QRect(0,0,43,33)),
                               Dialogue("還有記得，你的駕駛座旁邊有個翻譯裝置，可以用來跟那些外星生物溝通",":/res/player.png",QRect(0,0,43,33)),
                               Dialogue("前提是它們本身也要有一定的智力才行，不然就算翻譯出來也只會是一堆雜音而已",":/res/player.png",QRect(0,0,43,33)),
                               Dialogue("所以你得在這裡先找到能講話的生物談話。",":/res/player.png",QRect(0,0,43,33)),
                               Dialogue("收到！",":/res/player.png",QRect(0,0,43,33)),
                              });
            } if(tickCheck(250,63,7)) {
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
                connect(new_boss,&Enemy::dialogueStart,[this](){
                    dialogueStart({Dialogue("你是哪個星球來的人啦？",":/res/enemy/1/blue.png",QRect(5,5,35,35)),
                                   Dialogue("(終於出現一個能講話的了...)",":/res/player.png",QRect(0,0,43,33)),
                                   Dialogue("隨便跑來別人家裡，還把人家都打暈了是想怎樣啦",":/res/enemy/1/blue.png",QRect(5,5,35,35)),
                                   Dialogue("不是，我只是想跟你們的頭目談個話，可是他們都無法溝通，而且都把我當成入侵者了...",":/res/player.png",QRect(0,0,43,33)),
                                   Dialogue("你是入侵者沒有錯啊，而且我不跟入侵者談話啦",":/res/enemy/1/blue.png",QRect(5,5,35,35)),
                                   Dialogue("我要直接把你打下來啦！",":/res/enemy/1/blue.png",QRect(5,5,35,35)),
                                   Dialogue("原來你就是他們的頭目啊，不過好像也蠻明顯的就是了...",":/res/player.png",QRect(0,0,43,33))
                                  });
                });
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
            } else if(tickCheck(4650)) {
                dialogueStart({Dialogue("住手，不要再打了啦...",":/res/enemy/1/blue_3.png",QRect(5,5,35,35)),
                               Dialogue("好好好，可以問正經事了吧",":/res/player.png",QRect(0,0,43,33)),
                               Dialogue("你到底是想問什麼啦",":/res/enemy/1/blue_3.png",QRect(5,5,35,35)),
                               Dialogue("你們知道智多星在哪個方向嗎？",":/res/player.png",QRect(0,0,43,33)),
                               Dialogue("智多星啊...我們當初有辦法找到這顆星球落腳，就是他們幫忙的啦。",":/res/enemy/1/blue_3.png",QRect(5,5,35,35)),
                               Dialogue("不過我也不確定他們住哪，大概是那顆七彩的星球的方向吧，你自己往那裡找看看啦！",":/res/enemy/1/blue_3.png",QRect(5,5,35,35))
                              });
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
                this->setEndListPage(Game::EndListPageWon);
                ui->EndList->show();
                //game state
                gamestate=Game::GameStateWon;
            }
            break;
        //level 2
        case 2:
            if(tickCheck(220)) {
                dialogueStart({Dialogue("前面就是那個彩色的星球了。",":/res/player.png",QRect(0,0,43,33)),
                               Dialogue("不過他們好像已經發現我了，要小心一點。",":/res/player.png",QRect(0,0,43,33))
                              });
            } else if(tickCheck(250,131,5)) { //500+262i, 5 times
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
                connect(new_boss,&Enemy::dialogueStart,[this](){
                    dialogueStart({Dialogue("外星人，走開！",":/res/enemy/2/blue.png",QRect(5,2,25,25)),
                                   Dialogue("抱歉就這樣隨便闖進來了，但我只是想問你們...",":/res/player.png",QRect(0,0,43,33)),
                                   Dialogue("外外！討厭！",":/res/enemy/2/blue.png",QRect(5,2,25,25)),
                                   Dialogue("(喔不，他是智障)",":/res/player.png",QRect(0,0,43,33)),
                                   Dialogue("去去去！",":/res/enemy/2/blue.png",QRect(5,2,25,25)),
                                   Dialogue("好好好，我滾就是了",":/res/player.png",QRect(0,0,43,33)),
                                   Dialogue("不要走！去死！",":/res/enemy/2/blue.png",QRect(5,2,25,25)),
                                   Dialogue("(??????)",":/res/player.png",QRect(0,0,43,33))
                                  });
                });
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
            } else if(tickCheck(7163)) {
                dialogueStart({Dialogue("啊呀啊嘎嗚...",":/res/enemy/2/blue_3.png",QRect(5,2,25,25)),
                               Dialogue("(看來是問不到東西了，繼續往這個方向飛看看好了)",":/res/player.png",QRect(0,0,43,33))
                                });
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
                this->setEndListPage(Game::EndListPageWon);
                ui->EndList->show();
                //game state
                gamestate=Game::GameStateWon;
            }
            break;
        //level 3
        case 3:
            if(tickCheck(220)) {
                dialogueStart({Dialogue("剛剛飛到一半，突然被吸到了這個奇怪的地方.",":/res/player.png",QRect(0,0,43,33)),
                               Dialogue("被吸進來後，儀器突然都失靈了，連塔台的訊號都收不到",":/res/player.png",QRect(0,0,43,33)),
                               Dialogue("這裡到底是哪裡啊...",":/res/player.png",QRect(0,0,43,33)),
                              });
            } else if(tickCheck(250,125,5)) {  //500+250i, 5 times
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
                newMagicEffect(256,256,Game::FrameWidth/2,200,175,Game::MagicTypeBlue);
            } else if(tickCheck(5463)) {  //10925, BOSS 1
                ui->BossLives->show();
                ui->BossHealth->setGeometry(100,40,690,30);
                new_boss = new Enemy_3_Blue_1(player,250,80,18,200,Game::FrameWidth/2,200,0,0,0,0,0,true);
                connect(new_boss,SIGNAL(deadSignal(int,int)),this,SLOT(bossCorpse(int,int)));
                connect(new_boss,&Enemy::dialogueStart,[this](){
                    dialogueStart({Dialogue("哈利，這是我們宿命的對決。",":/res/enemy/3/blue.png",QRect(4,3,39,39)),
                                   Dialogue("蛤？誰是哈利？",":/res/player.png",QRect(0,0,43,33)),
                                   Dialogue("不用裝傻了，你就是命中註定，我最大的敵手",":/res/enemy/3/blue.png",QRect(4,3,39,39)),
                                   Dialogue("(為什麼又是一個智障啊？)",":/res/player.png",QRect(0,0,43,33)),
                                   Dialogue("根據我的占卜，在今天的這個時刻，會有一個外星生物進入我們的維度。",":/res/enemy/3/blue.png",QRect(4,3,39,39)),
                                   Dialogue("只要打敗你，我們就能取得你們的「科技」",":/res/enemy/3/blue.png",QRect(4,3,39,39)),
                                   Dialogue("再加上我們的「魔法」，我們就能到你們的維度裡稱霸！",":/res/enemy/3/blue.png",QRect(4,3,39,39)),
                                   Dialogue("(他好像不是智障啊，而且怎麼聽起來不太妙的感覺...)",":/res/player.png",QRect(0,0,43,33)),
                                   Dialogue("成為我們進步的踏腳石吧，哈利！",":/res/enemy/3/blue.png",QRect(4,3,39,39)),
                                   Dialogue("所以說那個名字到底是誰啊",":/res/player.png",QRect(0,0,43,33))
                                  });
                });
                new_boss->fadein(1500);
                newBossInit(new_boss);
                ui->BossLives->setText("5");
                tickFreeze();
            } else if(tickCheck(5465)) {  //10927
                ui->BossLives->setText("4");
                for(int i=0;i<2;++i) newMagicEffect(100,100,(i==0)?80:Game::FrameWidth-80,150,200,Game::MagicTypeBlue);
            } else if(tickCheck(5588)) {  //11175, BOSS 2
                new_boss = new Enemy_3_Blue_2(player,320,80,50,200,Game::FrameWidth/2,200,0,0,0,0,0,true);
                connect(new_boss,SIGNAL(deadSignal(int,int)),this,SLOT(bossCorpse(int,int)));
                new_boss->fadein(1500);
                newBossInit(new_boss);
                for(int i=0;i<2;++i) {
                    new_enemy = new Enemy_3_Green(player,5,30,115,150,(i==0)?80:Game::FrameWidth-80,150,0,0,0,0,false,true);
                    newEnemyInit(new_enemy);
                    new_enemy->setInvulnerable();
                    new_enemy->fadein(1000);
                    new_enemy->moveTo((i==0)?160:Game::FrameWidth-160,150,100);
                    new_effect = new_enemy->showShield();
                    newEffectInit(new_effect);
                    connect(new_boss,SIGNAL(useSkill(QString)),new_enemy,SIGNAL(killItsBullets()));
                    connect(new_boss,SIGNAL(useSkill(QString)),new_enemy,SLOT(killItself()));
                }
                tickFreeze();
            } else if(tickCheck(5590)) {  //11177
                ui->BossLives->setText("3");
                for(int i=0;i<2;++i) newMagicEffect(100,100,(i==0)?80:Game::FrameWidth-80,150,200,Game::MagicTypeBlue);
            } else if(tickCheck(5713)) {  //11425, BOSS 3
                new_boss = new Enemy_3_Blue_3(player,370,80,50,200,Game::FrameWidth/2,200,0,0,0,0,0,true);
                connect(new_boss,SIGNAL(deadSignal(int,int)),this,SLOT(bossCorpse(int,int)));
                new_boss->fadein(1500);
                newBossInit(new_boss);
                for(int i=0;i<2;++i) {
                    new_enemy = new Enemy_3_Red(player,5,30,125,120,(i==0)?80:Game::FrameWidth-80,150,0,0,0,0,false,true);
                    newEnemyInit(new_enemy);
                    new_enemy->setInvulnerable();
                    new_enemy->fadein(1000);
                    new_enemy->moveTo((i==0)?160:Game::FrameWidth-160,150,100);
                    new_effect = new_enemy->showShield();
                    newEffectInit(new_effect);
                    connect(new_boss,SIGNAL(useSkill(QString)),new_enemy,SIGNAL(killItsBullets()));
                    connect(new_boss,SIGNAL(useSkill(QString)),new_enemy,SLOT(killItself()));
                }
                tickFreeze();
            } else if(tickCheck(5715)) {  //11427
                ui->BossLives->setText("2");
                for(int i=0;i<2;++i) newMagicEffect(100,100,(i==0)?80:Game::FrameWidth-80,150,200,Game::MagicTypeBlue);
            } else if(tickCheck(5838)) {  //11675, BOSS 4
                new_boss = new Enemy_3_Blue_4(player,395,80,50,200,Game::FrameWidth/2,200,0,0,0,0,0,true);
                connect(new_boss,SIGNAL(deadSignal(int,int)),this,SLOT(bossCorpse(int,int)));
                new_boss->fadein(1500);
                newBossInit(new_boss);
                for(int i=0;i<2;++i) {
                    new_enemy = new Enemy_3_Yellow(player,5,30,250,150,(i==0)?80:Game::FrameWidth-80,150,0,0,0,0,false,true);
                    newEnemyInit(new_enemy);
                    new_enemy->setInvulnerable();
                    new_enemy->fadein(1000);
                    new_enemy->moveTo((i==0)?160:Game::FrameWidth-160,150,100);
                    new_effect = new_enemy->showShield();
                    newEffectInit(new_effect);
                    connect(new_boss,SIGNAL(useSkill(QString)),new_enemy,SIGNAL(killItsBullets()));
                    connect(new_boss,SIGNAL(useSkill(QString)),new_enemy,SLOT(killItself()));
                }
                tickFreeze();
            } else if(tickCheck(5840)) {  //11677
                ui->BossLives->setText("1");
                for(int i=0;i<2;++i) newMagicEffect(100,100,(i==0)?80:Game::FrameWidth-80,150,200,Game::MagicTypeBlue);
            } else if(tickCheck(5963)) {  //11925, BOSS 5
                new_boss = new Enemy_3_Blue_5(player,500,80,60,200,Game::FrameWidth/2,200,0,0,0,0,0,true);
                new_boss->fadein(1500);
                newBossInit(new_boss);
                for(int i=0;i<2;++i) {
                    new_enemy = new Enemy_3_Pink(player,5,30,150,150,(i==0)?80:Game::FrameWidth-80,150,0,0,0,0,false,true);
                    newEnemyInit(new_enemy);
                    new_enemy->setInvulnerable();
                    new_enemy->fadein(1000);
                    new_enemy->moveTo((i==0)?160:Game::FrameWidth-160,150,100);
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
            } else if(tickCheck(6250)) {
                dialogueStart({Dialogue("占卜的結果...不是這樣的啊...",":/res/enemy/3/blue_3.png",QRect(4,3,39,39)),
                               Dialogue("別說那個了，你知道「智多星」的位置嗎？",":/res/player.png",QRect(0,0,43,33)),
                               Dialogue("智多星啊，他們星球的生物是很長壽的種族。",":/res/enemy/3/blue_3.png",QRect(4,3,39,39)),
                               Dialogue("他們的首領幾千年前有來過我們這裡，教了我的祖先很多魔法以外的實戰技巧",":/res/enemy/3/blue_3.png",QRect(4,3,39,39)),
                               Dialogue("本來想說以後稱霸宇宙以後，好好感謝他們的。",":/res/enemy/3/blue_3.png",QRect(4,3,39,39)),
                               Dialogue("所以你知道要怎麼到他們那裡嗎？",":/res/player.png",QRect(0,0,43,33)),
                               Dialogue("要去那裡那還不簡單，我們有跟他們星球往來，所以我有設直達的傳送陣。",":/res/enemy/3/blue_3.png",QRect(4,3,39,39)),
                              });
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
                this->setEndListPage(Game::EndListPageWon);
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
            } else if(tickCheck(310)) {
                dialogueStart({Dialogue("慢著，你是來見我們首領的地球人嗎？",":/res/enemy/4/green.png",QRect(87,39,58,58)),
                               Dialogue("哇，雜魚說話了！",":/res/player.png",QRect(0,0,43,33)),
                               Dialogue("你們星球的人真沒禮貌...",":/res/enemy/4/green.png",QRect(87,39,58,58)),
                               Dialogue("對...對不起，可以帶我去見他嗎？",":/res/player.png",QRect(0,0,43,33)),
                               Dialogue("沒那麼簡單，首領說要我們先測試你一下",":/res/enemy/4/green.png",QRect(87,39,58,58)),
                               Dialogue("居然還要測試...？",":/res/player.png",QRect(0,0,43,33))
                              });
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
                connect(new_boss,&Enemy::dialogueStart,[this](int x){
                    switch(x) {
                    case 0:
                        dialogueStart({Dialogue("你是首領嗎？怎麼好像只是其他隻的放大版而已...",":/res/player.png",QRect(0,0,43,33)),
                                       Dialogue("我所嚮往的是如我們的母星一般，外表黯淡，但內心的光芒隱隱透出。大智若愚，曖曖含光。",":/res/enemy/4/blue.png",QRect(87,39,58,58)),
                                       Dialogue("......",":/res/player.png",QRect(0,0,43,33)),
                                       Dialogue("重點不是這個，你會在這裡是因為我向你們星球傳了無線電訊號，我需要有人幫忙。",":/res/enemy/4/blue.png",QRect(87,39,58,58)),
                                       Dialogue("幫忙？",":/res/player.png",QRect(0,0,43,33)),
                                       Dialogue("其實說起來有點不好意思...",":/res/enemy/4/blue.png",QRect(87,39,58,58)),
                                       Dialogue("我們之前開發的機械智慧因為不明原因開始暴走了。沒弄好的話，這幾十光年內的星球可能都有危險。",":/res/enemy/4/blue.png",QRect(87,39,58,58)),
                                       Dialogue("幾十光年的話不是也包括我們嗎？那麼那東西在哪裡？",":/res/player.png",QRect(0,0,43,33)),
                                       Dialogue("這個我們留到我打累了以後再說吧。",":/res/enemy/4/blue.png",QRect(87,39,58,58)),
                                       Dialogue("等...等一下，又要打？",":/res/player.png",QRect(0,0,43,33)),
                                       Dialogue("當然啊，如果你沒有一定的能力，帶你去只會扯後腿而已。",":/res/enemy/4/blue.png",QRect(87,39,58,58)),
                                       Dialogue("這問題不是你們搞出來的嗎！",":/res/player.png",QRect(0,0,43,33))
                                      });
                        break;
                    case 1:
                        dialogueStart({Dialogue("當一艘船的各個零件被漸漸汰換，最後所有零件都被更換時，它還是原本那艘船嗎？",":/res/enemy/4/blue_2.png",QRect(87,39,58,58))});
                    }
                });
                new_boss->moveTo(Game::FrameWidth/2,200,165);
                newBossInit(new_boss);
                ui->BossLives->setText("5");
                tickFreeze();
            } else if(tickCheck(7826)) { //14050
                ui->BossLives->setText("4");
            } else if(tickCheck(7949)) { //14298, BOSS 2
                new_boss = new Enemy_4_Blue_2(player,250,60,45,200,Game::FrameWidth/2,200,0,0,0,0,false,true);
                connect(new_boss,SIGNAL(deadSignal(int,int)),this,SLOT(bossCorpse(int,int)));
                connect(new_boss,&Enemy::dialogueStart,[this](){
                    dialogueStart({Dialogue("宇宙幾乎是無限大的，且到處都充滿了星體，那為什麼宇宙不是一片光亮的呢？",":/res/enemy/4/blue_2.png",QRect(87,39,58,58))});
                });
                new_boss->fadein(1500);
                newBossInit(new_boss);
                tickFreeze();
            } else if(tickCheck(7951)) { //14300
                ui->BossLives->setText("3");
            } else if(tickCheck(8074)) { //14548, BOSS 3
                new_boss = new Enemy_4_Blue_3(player,330,60,45,200,Game::FrameWidth/2,200,0,0,0,0,false,true);
                connect(new_boss,SIGNAL(deadSignal(int,int)),this,SLOT(bossCorpse(int,int)));
                connect(new_boss,&Enemy::dialogueStart,[this](){
                    dialogueStart({Dialogue("兩個人開著車子朝彼此前進，到最後一刻仍不轉彎的人便是贏家，但如果兩個人都不轉彎......",":/res/enemy/4/blue_2.png",QRect(87,39,58,58)),
                                  Dialogue("這種賽局模式可以套用到許多實力之中，來解釋為何賽局雙方通常都寧願當「膽小鬼」。",":/res/enemy/4/blue_2.png",QRect(87,39,58,58))
                                  });
                });
                new_boss->fadein(1500);
                newBossInit(new_boss);
                tickFreeze();
            } else if(tickCheck(8076)) { //14550
                ui->BossLives->setText("2");
            } else if(tickCheck(8199)) { //14798, BOSS 4
                new_boss = new Enemy_4_Blue_4(player,450,60,55,200,Game::FrameWidth/2,200,0,0,0,0,false,true);
                connect(new_boss,SIGNAL(deadSignal(int,int)),this,SLOT(bossCorpse(int,int)));
                connect(new_boss,&Enemy::dialogueStart,[this](){
                    dialogueStart({Dialogue("一支飛行中的箭矢在每個瞬間都是靜止的，代表這跟箭矢是靜止箭矢的集合，",":/res/enemy/4/blue_2.png",QRect(87,39,58,58)),
                                   Dialogue("所以飛行的箭矢是靜止的。這個敘述明顯有錯才會造成這個矛盾結論，那是錯在哪裡呢？",":/res/enemy/4/blue_2.png",QRect(87,39,58,58))
                                  });
                });
                new_boss->fadein(1500);
                newBossInit(new_boss);
                tickFreeze();
            } else if(tickCheck(8201)) { //14800
                ui->BossLives->setText("1");
            } else if(tickCheck(8324)) { //8324, BOSS 5
                new_boss = new Enemy_4_Blue_5(player,280,60,55,200,Game::FrameWidth/2,200,0,0,0,0,false,true);
                connect(new_boss,&Enemy::dialogueStart,[this](){
                    dialogueStart({Dialogue("當你進入機器A，它將你身上的粒子全部掃瞄並在機器B用不一樣的粒子做出來，",":/res/enemy/4/blue_2.png",QRect(87,39,58,58)),
                                   Dialogue("然後把機器A裡的你拆解回粒子。這樣算是同一個你被「傳送」過去的嗎？",":/res/enemy/4/blue_2.png",QRect(87,39,58,58)),
                                   Dialogue("你話也太多了吧...",":/res/player.png",QRect(0,0,43,33))
                                  });
                });
                new_boss->fadein(1500);
                newBossInit(new_boss);
                tickFreeze();
            } else if(tickCheck(8326)) { //8326
                ui->BossLives->hide();
                ui->BossSkill->hide();
                if(player!=nullptr) player->gameEndSetting();
            } else if(tickCheck(8590)) {
                dialogueStart({Dialogue("嗚，雖然我有放水，不過好久沒動了，好累。",":/res/enemy/4/blue_3.png",QRect(74,17,58,58)),
                               Dialogue("你還真行耶，看來我找對人了。",":/res/enemy/4/blue_3.png",QRect(74,17,58,58)),
                               Dialogue("這是當然的了！所以那個機械智慧在哪裡？到底發生了什麼事？",":/res/player.png",QRect(0,0,43,33)),
                               Dialogue("我們之前嘗試開發機械智慧，但是最後失敗了。",":/res/enemy/4/blue_3.png",QRect(74,17,58,58)),
                               Dialogue("最後我們把失敗品丟到了另一顆無生命的星球上。但最近呢，有很多鄰近的星球卻說他們受到了這些機械的攻擊。",":/res/enemy/4/blue_3.png",QRect(74,17,58,58)),
                               Dialogue("我不清楚它們是怎麼重獲新生的，別說是自主行動了，他們理論上根本已經沒有能源能執行任何動作了。",":/res/enemy/4/blue_3.png",QRect(74,17,58,58)),
                               Dialogue("一開始我也不相信，但受害星球的描述真的都與它們相似。",":/res/enemy/4/blue_3.png",QRect(74,17,58,58)),
                               Dialogue("總之我建議先到他們的據點星球旁的那顆星球，迷霧星。",":/res/enemy/4/blue_3.png",QRect(74,17,58,58)),
                               Dialogue("那裡的大氣很糟，我們可以躲在那裡觀察他們而不容易被發現。",":/res/enemy/4/blue_3.png",QRect(74,17,58,58))
                               });
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
                this->setEndListPage(Game::EndListPageWon);
                ui->EndList->show();
                //game state
                gamestate=Game::GameStateWon;
            }
            break;
        case 5:
            break;
        default:
            qDebug() << "error: can't get what level is selected";
        }
    }
    //draw
    if(gamestate!=Game::GameStatePaused) emit doImgMove();
    //player death
    if(player->isDead() && gamestate!=Game::GameStateFailed) {
        //game end
        player->gameEndSetting();
        player->setOpacity(0.5);
        this->setEndListPage(Game::EndListPageFailed);
        ui->EndList->show();
        //game state
        gamestate=Game::GameStateFailed;
    }
    //ticking
    if(gamestate==Game::GameStatePlaying && ticking && !dialogueProcessing) ++tick;
    if(gamestate==Game::GameStatePlaying) ++gametime;
}
void MainWindow::backToMenu() {
    if(gamestate==Game::GameStatePlaying||gamestate==Game::GameStateMenu) return;
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
void MainWindow::dialogueStart(std::initializer_list<Dialogue> list) {
    if(dialogueWidget!=nullptr) delete dialogueWidget;
    dialogueWidget = new DialogueWidget(QRect(60,669,731,211),0.8,ui->GamePage);
    connect(dialogueWidget,SIGNAL(end()),this,SLOT(dialogueEnd()));
    this->dialogueProcessing = true;
    dialogueWidget->show();
    dialogueWidget->start(list);
}
void MainWindow::dialogueEnd() {
    if(ticking) ++tick;
    this->dialogueProcessing = false;
    dialogueWidget->hide();
    if(dialogueWidget!=nullptr) delete dialogueWidget;
    dialogueWidget=nullptr;
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
    connect(new_boss,SIGNAL(useSkill(QString)),this,SLOT(bossSkillLengthSetting(QString)));
    connect(new_boss,SIGNAL(useSkill(QString)),bossSkillAni,SLOT(animationStart3500()));
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
    bossSkillAni->setGeometry(QRect(790-length*50,80,length*50,50));
}
void MainWindow::newMagicEffect(int show_w, int show_h, double x, double y, int lifetime, Game::MagicType type) {
    bool white = type==Game::MagicTypeWhite;
    new_effect = new Effect(QString(white?":/res/effect/magic.png":":/res/effect/magic_blue.png"),white?100:120,white?100:120,show_w,show_h,lifetime,x,y,0,0,0,0,true);
    new_effect->setOpacity(0.6);
    new_effect->fadein();
    newEffectInit(new_effect);
}
void MainWindow::pauseAndResume() {
    if(gamestate==Game::GameStatePlaying) {
        this->setEndListPage(Game::EndListPagePaused);
        ui->EndList->show();
        gamestate=Game::GameStatePaused;
    } else if(gamestate==Game::GameStatePaused) {
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
    ++tick;
    ticking=false;
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
            if(gamestate==Game::GameStatePlaying) pauseAndResume();
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
void MainWindow::restart() {
    if(gamestate==Game::GameStatePlaying||gamestate==Game::GameStateMenu) return;
    backToMenu();
    start();
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
