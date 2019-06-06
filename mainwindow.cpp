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
#include <QDesktopWidget>
#include <float.h>
#include "gamerunner.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    timer(nullptr),freezeTimer(nullptr),
    ticking(false),left(false),right(false),up(false),down(false),use_skill(false),
    cur_left(false),cur_up(false),
    player(nullptr),teammate(nullptr),dot(nullptr),secret(0),
    oriImg2(nullptr),cutImg(nullptr),oriImg(nullptr),
    skillName(new TextItem("",nullptr)),
    bossHealthOpacityEff(this), bossLivesOpacityEff(this),
    dialogueProcessing(false), bossHPShortened(false),
    dialogueWidget(nullptr),
    gameFrameContentGeo()
{
    ui->setupUi(this);
    this->setGamePage(Game::GamePageMenu);
    //level
    level = 1;
    //scene
    scene = new QGraphicsScene(0,0,Game::FrameWidth,Game::FrameHeight);
    ui->graphicsView->setScene(scene);
    ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView->setViewportUpdateMode(QGraphicsView::NoViewportUpdate);
    ui->graphicsView->setRenderHints(QPainter::SmoothPixmapTransform);
    ui->graphicsView->setOptimizationFlag(QGraphicsView::DontAdjustForAntialiasing);
    //boss objects
    ui->BossHealth->setGraphicsEffect(&bossHealthOpacityEff);
    ui->BossLives->setGraphicsEffect(&bossLivesOpacityEff);
    skillName->setPos(20,55);
    skillName->setFont(QFont("Microsoft Jhenghei", 18, QFont::Bold));
    skillName->setDefaultTextColor(Qt::white);
    skillName->setZValue(Game::ZValueUI);
    scene->addItem(skillName);
    //qrand
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
    //endlist
    ui->EndList->hide();
    EndListAni = new WidgetAnimationer(ui->EndList);
    EndListAni->setOpacity(0.6);
    EndListAni->setFadeDir(WidgetAnimationer::FadeDirectionNone);
    EndListAni->setExpandDir(WidgetAnimationer::ExpandDirectionHor);
    //button
    connect(ui->startButton,SIGNAL(clicked(bool)),this,SLOT(levelSelect()));
    connect(ui->QuitButton,SIGNAL(clicked(bool)),qApp,SLOT(quit()));
    connect(ui->LevelButton_1,SIGNAL(clicked(bool)),this,SLOT(start1()));
    connect(ui->LevelButton_2,SIGNAL(clicked(bool)),this,SLOT(start2()));
    connect(ui->LevelButton_3,SIGNAL(clicked(bool)),this,SLOT(start3()));
    connect(ui->LevelButton_4,SIGNAL(clicked(bool)),this,SLOT(start4()));
    connect(ui->LevelButton_5,SIGNAL(clicked(bool)),this,SLOT(start5()));
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
                                                ui->optionsButton,
                                                ui->QuitButton});
    keyControlButtonsConnect(menuButtons,KeyControlButton::ArrowPosLeft);
    std::vector<KeyControlButton*> levelButtons({ui->LevelButton_1,
                                                 ui->LevelButton_2,
                                                 ui->LevelButton_3,
                                                 ui->LevelButton_4,
                                                 ui->LevelButton_5});
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
    flash = new Flash(QRect(-50,-50,Game::FrameWidth+100,Game::FrameHeight+100));
    scene->addItem(flash);
    //boss skill name animation
    bossSkillAni = new ItemAnimationer(skillName);
    bossSkillAni->setFadeDir(ItemAnimationer::FadeDirectionRight);
    //game state
    gamestate=Game::GameStateMenu;
    //level intro
    ui->levelIntro->hide();
    //level
    levelSelectAni = new WidgetAnimationer(ui->selectFrame);
    levelSelectAni->setFadeDir(WidgetAnimationer::FadeDirectionNone);
    levelSelectAni->setExpandDir(WidgetAnimationer::ExpandDirectionHor);
    //dialogue
    //audioer
    audioers.resize(12);
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
    soundSet(Game::SoundShake,40,"res/sound/shake.wav");
    soundSet(Game::SoundMagicShield,40,"res/sound/magic_shield.wav");
    soundSet(Game::SoundMagicSmite,40,"res/sound/magic_smite.wav");
    soundSet(Game::SoundLaser,40,"res/sound/laser.wav");
    //frame content geometry set
    gameFrameContentGeo.resize(18);
    gameFrameContentGeo.at(Game::UIBaseGeometryDialogue) = QRect(60,669,731,211);
    gameFrameContentGeo.at(Game::UIBaseGeometryHP) = QRect(20,20,720,30);
    gameFrameContentGeo.at(Game::UIBaseGeometryHPShort) = QRect(50,20,690,30);
    std::initializer_list<QRect> geos = {
        ui->EndList->geometry(),
        ui->WarningBar->geometry(),
        ui->levelIntro->geometry(),
        ui->BossLives->geometry(),
        ui->WinWidget->geometry(),
        ui->restartButton_3->geometry(),
        ui->menuButton_3->geometry(),
        ui->LoseLabel->geometry(),
        ui->restartButton_2->geometry(),
        ui->menuButton_2->geometry(),
        ui->PauseLabel->geometry(),
        ui->restartButton->geometry(),
        ui->menuButton->geometry(),
        ui->continueButton->geometry()
    };
    {
        unsigned int i=3;
        for(const QRect& geo: geos) {
            gameFrameContentGeo.at(i) = geo;
            if(++i==UINT_MAX) break;
        }
    }
    this->triggerResize(ui->centralWidget->height());
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
void MainWindow::start5() {
    level=5;
    start();
}
void MainWindow::start() {
    //pixmap
    Character::imgPixmap.loadLevel(level);
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
    skillName->hide();
    skillName->setPlainText("");
    bossSkillAni->setOpacity(0.7);
    //boss health bar
    ui->BossHealth->hide();
    this->bossHPShortened = false;
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
    background->setZValue(Game::ZValueBackground);
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
    //teammate
    if(level>=5) {
        if(teammate!=nullptr) delete teammate;
        teammate = new Teammate(":/res/enemy/4/blue.png",200,153,120,92,player,50,player->getX()-120,player->getY());
        scene->addItem(teammate);
        teammate->setCanBeMirrored();
        connect(this,SIGNAL(doMove()),teammate,SLOT(move()));
        connect(this,SIGNAL(doImgMove()),teammate,SLOT(img_move()));
    }
    //dot
    if(dot!=nullptr) delete dot;
    dot = new Shield(":/res/effect/dot.png",50,50,10,10,player,-1,player->getX(),player->getY());
    dot->setZValue(Game::ZValueDot);
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
    case 5:
        boss_tick=Game::BossTick5;
        strBossBG=":/res/bg/boss_5.png";
        break;
    default:
        boss_tick=Game::BossTick1;
        strBossBG=":/res/bg/boss.png";
        qDebug() << "error: can't get what level is selected";
    }
    //gameRunner
    gameRunner = new GameRunner(this);
    //timer
    gametime=tick=StartTick;
    ticking=true;
    if(timer!=nullptr) delete timer;
    timer=new QTimer;
    timer->start(16); //62.5 tick per sec
    connect(timer,SIGNAL(timeout()),this,SLOT(doTick()));
    //timer2
    if(timer2!=nullptr) delete timer;
    timer2=new QTimer;
    timer2->start(16); //62.5 tick per sec
    connect(timer2,SIGNAL(timeout()),gameRunner,SLOT(doTick()));
    //resize
    this->triggerResize(ui->centralWidget->height());
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

void MainWindow::resizeEvent(QResizeEvent *e) {
    double central_h = e->size().height();
    this->triggerResize(central_h);
}

void MainWindow::triggerResize(double central_h) {
    constexpr int margin = 5, border = 6;
    if(gamestate==Game::GameStateMenu) {
        ui->stackedWidget->setGeometry((ui->centralWidget->width()-1150)/2,(central_h-940)/2,1150,940);
    } else {
        //frame
        double frame_h = central_h-2*margin;
        double frame_w = (frame_h-2*border)*Game::FrameWidth/Game::FrameHeight+2*border;
        ui->gameFrame->setGeometry(margin,margin,frame_w,frame_h);
        ui->graphicsView->setGeometry(ui->graphicsView->x(),ui->graphicsView->y(),frame_w-2*border,frame_h-2*border);
        //frame content
        QSize frameBaseSize(Game::FrameWidth+2*border,Game::FrameHeight+2*border);
        double frame_sc = frame_h/frameBaseSize.height();
        auto scaleAsFrame = [frame_sc](QWidget* uiItem, QRect baseGeo){
            QRect scaledGeo(baseGeo.x()*frame_sc,baseGeo.y()*frame_sc,baseGeo.width()*frame_sc,baseGeo.height()*frame_sc);
            uiItem->setGeometry(scaledGeo);
        };
        auto scaleAsFrame_Ani = [frame_sc](WidgetAnimationer* uiItem_Ani, QRect baseGeo){
            QRect scaledGeo(baseGeo.x()*frame_sc,baseGeo.y()*frame_sc,baseGeo.width()*frame_sc,baseGeo.height()*frame_sc);
            uiItem_Ani->setGeometry(scaledGeo);
        };
        auto scaleAsFrame_Dia = [frame_sc](DialogueWidget* uiItem_Dia, QRect baseGeo){
            QRect scaledGeo(baseGeo.x()*frame_sc,baseGeo.y()*frame_sc,baseGeo.width()*frame_sc,baseGeo.height()*frame_sc);
            uiItem_Dia->setAniGeometry(scaledGeo);
        };
        scaleAsFrame(ui->WarningBar,gameFrameContentGeo.at(Game::UIBaseGeometryWarning));
        scaleAsFrame(ui->levelIntro,gameFrameContentGeo.at(Game::UIBaseGeometryIntro));
        scaleAsFrame_Ani(EndListAni,gameFrameContentGeo.at(Game::UIBaseGeometryEndList));
        if(dialogueWidget!=nullptr) scaleAsFrame_Dia(dialogueWidget,gameFrameContentGeo.at(Game::UIBaseGeometryDialogue));
        scaleAsFrame(ui->BossHealth,gameFrameContentGeo.at(this->bossHPShortened?Game::UIBaseGeometryHPShort:Game::UIBaseGeometryHP));
        scaleAsFrame(ui->BossLives,gameFrameContentGeo.at(Game::UIBaseGeometryLife));
        //end list
        scaleAsFrame(ui->WinWidget,gameFrameContentGeo.at(Game::UIBaseGeometryWinWidget));
        scaleAsFrame(ui->restartButton_3,gameFrameContentGeo.at(Game::UIBaseGeometryRestartButton_3));
        scaleAsFrame(ui->menuButton_3,gameFrameContentGeo.at(Game::UIBaseGeometryMenuButton_3));
        scaleAsFrame(ui->LoseLabel,gameFrameContentGeo.at(Game::UIBaseGeometryLoseLabel));
        scaleAsFrame(ui->restartButton_2,gameFrameContentGeo.at(Game::UIBaseGeometryRestartButton_2));
        scaleAsFrame(ui->menuButton_2,gameFrameContentGeo.at(Game::UIBaseGeometryMenuButton_2));
        scaleAsFrame(ui->PauseLabel,gameFrameContentGeo.at(Game::UIBaseGeometryPauseLabel));
        scaleAsFrame(ui->restartButton,gameFrameContentGeo.at(Game::UIBaseGeometryRestartButton));
        scaleAsFrame(ui->menuButton,gameFrameContentGeo.at(Game::UIBaseGeometryMenuButton));
        scaleAsFrame(ui->continueButton,gameFrameContentGeo.at(Game::UIBaseGeometryContinueButton));
        //view
        QTransform zoom;
        double sc = (double)ui->graphicsView->height()/Game::FrameHeight;
        zoom.scale(sc,sc);
        ui->graphicsView->setTransform(zoom);
        //right
        QRect geo(ui->rightWidget->geometry());
        geo.setX(frame_w+2*margin+25);
        geo.setY(margin-10);
        geo.setHeight(891/872.0 * frame_h);
        geo.setWidth(311);
        ui->rightWidget->setGeometry(geo);
        //stacked
        double stacked_w = geo.x() + geo.width() + margin;
        double& stacked_h = central_h;
        ui->stackedWidget->setGeometry((ui->centralWidget->width()-stacked_w)/2,0,stacked_w,stacked_h);
    }
}

inline bool MainWindow::playerLeft() {
    return left&&right ? cur_left : left;
}

inline bool MainWindow::playerRight() {
    return left&&right ? !cur_left : right;
}

inline bool MainWindow::playerUp() {
    return up&&down ? cur_up : up;
}

inline bool MainWindow::playerDown() {
    return up&&down ? !cur_up : down;
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
    //draw
    if(gamestate!=Game::GameStatePaused) emit doImgMove();
    //
    if(gameRunner->endListShowPulse) {
        gameRunner->endListShowPulse = false;
        this->setEndListPage(Game::EndListPageFailed);
        ui->EndList->show();
    }
    //update
    ui->graphicsView->update();
}
void MainWindow::backToMenu() {
    if(gamestate==Game::GameStatePlaying||gamestate==Game::GameStateMenu) return;
    ticking=false;
    //kill timer
    delete timer;
    timer=nullptr;
    delete timer2;
    timer2=nullptr;
    delete freezeTimer;
    freezeTimer=nullptr;
    //kill gameRunner
    delete gameRunner;
    gameRunner=nullptr;
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
    delete teammate;
    teammate=nullptr;
    //menu
    this->setGamePage(Game::GamePageMenu);
    //game state
    gamestate=Game::GameStateMenu;
    //resize
    this->triggerResize(ui->centralWidget->height());
}
void MainWindow::dialogueStart(std::initializer_list<Dialogue> list) {
    if(dialogueWidget!=nullptr) delete dialogueWidget;
    dialogueWidget = new DialogueWidget(gameFrameContentGeo.at(Game::UIBaseGeometryDialogue),0.8,ui->GamePage);
    this->triggerResize(ui->centralWidget->height());
    connect(dialogueWidget,SIGNAL(soundPlay(Game::Sound)),this,SLOT(soundPlay(Game::Sound)));
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
        if(this->dot!=nullptr && new_effect!=this->dot) connect(this,SIGNAL(killEffectsWithoutDot()),new_effect,SLOT(killItself()));
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
    this->triggerResize(ui->centralWidget->height());
    ui->BossHealth->show();
    ui->BossHealth->setMaximum(new_boss->getHealth());
    ui->BossHealth->setValue(new_boss->getHealth());
    skillName->setPlainText("");
    connect(new_boss,SIGNAL(healthChanged(int)),ui->BossHealth,SLOT(setValue(int)));
    connect(new_boss,SIGNAL(useSkill(QString)),this,SLOT(bossSkillLengthSetting(QString)));
    connect(new_boss,SIGNAL(useSkill(QString)),bossSkillAni,SLOT(animationStart3500()));
    connect(new_boss,SIGNAL(useSkill(QString)),skillName,SLOT(setTextSlot(QString)));
    connect(new_boss,SIGNAL(useSkill(QString)),skillName,SLOT(showSlot()));
    connect(new_boss,SIGNAL(useSkill(QString)),flash,SLOT(flash()));
    connect(new_boss,SIGNAL(deadSignal()),ui->BossHealth,SLOT(hide()));
    connect(new_boss,SIGNAL(deadSignal()),skillName,SLOT(hideSlot()));
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
    QRect new_geo(740-length*50,60,length*50,50);
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
    Flash* red_flash = new Flash(QRect(-50,-50,Game::FrameWidth+100,Game::FrameHeight+100));
    red_flash->setBrush(QBrush(Qt::red));
    red_flash->setOpacity(0.3);
    red_flash->setFlashTime(400,400);
    red_flash->flash(true);
    scene->addItem(red_flash);
}
void MainWindow::sceneVibrate(short vibrate_count, bool withSound) {
    static QPropertyAnimation* vibrateAni=nullptr;
    static short vibrate_count_temp=0;
    bool firstVib=false;
    vibrate_count_temp = vibrate_count+1;
    if(withSound) soundPlay(Game::SoundShake);
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
            vibrateAni->setEndValue(QRect((vibrate_count%2==0?-1:1)*(5-(vibrate_count<0?-1:1)*vibrate_count*vibrate_count*0.05),0,Game::FrameWidth,Game::FrameHeight));
        }
        connect(vibrateAni,&QPropertyAnimation::finished,[=](){
            sceneVibrate(vibrate_count_temp,false);
        });
        vibrateAni->start(QAbstractAnimation::KeepWhenStopped);
    } else {
        if(vibrateAni!=nullptr) delete vibrateAni;
        vibrateAni=nullptr;
    }
}
void MainWindow::sceneVibrateVertical(short vibrate_count, bool withSound) {
    static QPropertyAnimation* vibrateAni=nullptr;
    static short vibrate_count_temp=0;
    bool firstVib=false;
    vibrate_count_temp = vibrate_count+1;
    if(withSound) soundPlay(Game::SoundShake);
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
            vibrateAni->setEndValue(QRect(0,(vibrate_count%2==0?-1:1)*(5-(vibrate_count<0?-1:1)*vibrate_count*vibrate_count*0.05),Game::FrameWidth,Game::FrameHeight));
        }
        connect(vibrateAni,&QPropertyAnimation::finished,[=](){
            sceneVibrateVertical(vibrate_count_temp,false);
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
            cur_left = true;
            left=true;
            break;
        case Qt::Key_D:
        case Qt::Key_Right:
            cur_left = false;
            right=true;
            break;
        case Qt::Key_W:
        case Qt::Key_Up:
            cur_up = true;
            up=true;
            break;
        case Qt::Key_S:
        case Qt::Key_Down:
            cur_up = false;
            down=true;
            break;
        case Qt::Key_Z:
            if(gamestate!=Game::GameStateMenu) {
                player->setShooting(true);
                if(teammate!=nullptr) teammate->setShooting(true);
            }
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
            if(!(this->windowState()==Qt::WindowFullScreen)) {
                this->setWindowState(Qt::WindowFullScreen);
                this->triggerResize(ui->centralWidget->height());
            } else this->setWindowState(Qt::WindowMaximized);
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
            if(gamestate!=Game::GameStateMenu) {
                player->setShooting(false);
                if(teammate!=nullptr) teammate->setShooting(false);
            }
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
