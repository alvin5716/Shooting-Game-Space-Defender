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

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    timer(NULL),freezeTimer(NULL),
    ticking(false),left(false),right(false),up(false),down(false),use_skill(false),
    player(NULL),dot(NULL),secret(0),oriImg2(NULL)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(GamePage::Menu);
    //level
    level = 1;
    //scene
    scene = new QGraphicsScene(0,0,Game::FrameWidth,Game::FrameHeight);
    ui->graphicsView->setScene(scene);
    ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView->setViewportUpdateMode(QGraphicsView::NoViewportUpdate);
    //qrand
    qsrand(time(NULL));
    //button
    connect(ui->startButton,SIGNAL(clicked(bool)),this,SLOT(levelSelect()));
    connect(ui->QuitButton,SIGNAL(clicked(bool)),qApp,SLOT(quit()));
    connect(ui->LevelButton_1,SIGNAL(clicked(bool)),this,SLOT(start1()));
    connect(ui->LevelButton_2,SIGNAL(clicked(bool)),this,SLOT(start2()));
    connect(ui->LevelButton_3,SIGNAL(clicked(bool)),this,SLOT(start3()));
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
    gamestate=GameState::Menu;
    //level intro
    ui->levelIntro->hide();
}
void MainWindow::levelSelect() {
    ui->stackedWidget->setCurrentIndex(GamePage::LevelSelecting);
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
void MainWindow::start() {
    //level intro
    if(Game::StartTick==0) ui->levelIntro->show();
    ui->levelIntro->setCurrentIndex(level-1);
    QGraphicsOpacityEffect *fadein = new QGraphicsOpacityEffect(this);
    ui->levelIntro->setGraphicsEffect(fadein);
    QPropertyAnimation *fadeinAni = new QPropertyAnimation(fadein,"opacity");
    fadeinAni->setDuration(800);
    fadeinAni->setStartValue(0);
    fadeinAni->setEndValue(1);
    fadeinAni->setEasingCurve(QEasingCurve::InQuad);
    fadeinAni->start(QPropertyAnimation::DeleteWhenStopped);
    //level label
    ui->levelLabel->setText(QString("LEVEL ").append(QString().setNum(level)));
    //boss lives count
    ui->BossLives->hide();
    ui->BossLives->setText("");
    //boss skill name
    ui->BossSkill->hide();
    ui->BossSkill->setText("");
    //boss health bar
    ui->BossHealth->hide();
    ui->BossHealth->setGeometry(70,40,720,30);
    ui->BossHealth->setFormat("");
    ui->BossHealth->setValue(0);
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
    oriImg = new QPixmap(":/res/bg_normal.png");
    //player
    if(player!=NULL) delete player;
    player = new Player(5,5,15,Game::FrameWidth/2,Game::FrameHeight-100);
    ui->PlayerLife->display(player->getHealth());
    scene->addItem(player);
    connect(this,SIGNAL(doMove()),player,SLOT(move()));
    connect(this,SIGNAL(doImgMove()),player,SLOT(img_move()));
    connect(player,SIGNAL(healthColorChange(QString)),this,SLOT(healthColorChange(QString)));
    player->healthColorChange("white");
    //dot
    if(dot!=NULL) delete dot;
    dot = new Shield(":/res/dot.png",50,50,10,10,player,-1,player->getX(),player->getY());
    dot->setZValue(100);
    newEffectInit(dot);
    //timer
    gametime=tick=Game::StartTick;
    ticking=true;
    if(timer!=NULL) delete timer;
    timer=new QTimer;
    timer->start(8); //125 tick per sec
    connect(timer,SIGNAL(timeout()),this,SLOT(doTick()));
    connect(timer,SIGNAL(timeout()),ui->graphicsView,SLOT(update()));
    //page
    ui->stackedWidget->setCurrentIndex(GamePage::Playing);
    //game state
    gamestate=GameState::Playing;
    //boss tick
    switch(level) {
    case 1:
        boss_tick=Game::BossTick1;
        strBossBG=":/res/bg_boss.png";
        break;
    case 2:
        boss_tick=Game::BossTick2;
        strBossBG=":/res/bg_boss_2.png";
        break;
    case 3:
        boss_tick=Game::BossTick3;
        strBossBG=":/res/bg_boss_3.png";
        break;
    default:
        boss_tick=Game::BossTick1;
        strBossBG=":/res/bg_boss.png";
        qDebug() << "error: can't get what level is selected";
    }
}
void MainWindow::doTick() {
    //focus
    setFocus();
    //level intro
    if(tick==500) {
        QGraphicsOpacityEffect* levelIntroOpacityEff = new QGraphicsOpacityEffect(this);
        ui->levelIntro->setGraphicsEffect(levelIntroOpacityEff);
        QPropertyAnimation* levelIntroFadeoutAni = new QPropertyAnimation(levelIntroOpacityEff,"opacity");
        levelIntroFadeoutAni->setDuration(6000);
        levelIntroFadeoutAni->setStartValue(1);
        levelIntroFadeoutAni->setEndValue(0);
        levelIntroFadeoutAni->setEasingCurve(QEasingCurve::OutQuint);
        connect(levelIntroFadeoutAni,SIGNAL(finished()),ui->levelIntro,SLOT(hide()));
        levelIntroFadeoutAni->start(QAbstractAnimation::DeleteWhenStopped);
    }
    //bg
    if(tick<boss_tick) {
        int init = (450-172)-(gametime/7+1);
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
        int init = (450-172)-((tick-boss_tick)/4+1);
        if(init<0) init = 0;
        if(oriImg2!=NULL) merger.drawPixmap(QRect(0,0,150,172),*oriImg2,QRect(0,init,150,172));
        else merger.drawPixmap(QRect(0,0,150,172),QPixmap(strBossBG),QRect(0,0,150,172));
    }
    background->setPixmap(cutImg->scaled(Game::FrameWidth,Game::FrameHeight));

    //game
    if(gamestate==GameState::Playing||gamestate==GameState::Won) {
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
    if(gamestate==GameState::Playing) {
        //player skill
        if(use_skill && !player->isUsingSkill() && skill_times>0 && !player->isInvulnerable()) { //init
            player->coolDown();
            new_effect = new Shield(QString(":/res/shield1.png"),70,70,500,500,player,375,player->getX(),player->getY());
            new_effect->fadein();
            newEffectInit(new_effect);
            --skill_times;
            ui->PlayerSkill->display(skill_times);
        }
        if(player->isUsingSkill()) {
            for(int i=0;i<(int)enemy_bullets.size();++i) { //continuous
                if(sqrt(pow(enemy_bullets.at(i)->getX()-player->getX(),2)+pow(enemy_bullets.at(i)->getY()-player->getY(),2))<=250 && !enemy_bullets.at(i)->isInvulnerable())
                    enemy_bullets.at(i)->killItself();
            }
        }
        //shoot ,skill
        if(player->isShooting()) { //player
            //shoot
            Bullet* new_player_bullet = player->shoot();
            if(new_player_bullet!=NULL) {
                scene->addItem(new_player_bullet);
                player_bullets.push_back(new_player_bullet);
                connect(this,SIGNAL(doMove()),new_player_bullet,SLOT(move()));
                connect(this,SIGNAL(doImgMove()),new_player_bullet,SLOT(img_move()));
            }
        }
        //damaged
        std::vector<Character*> attackers(enemies.begin(),enemies.end());
        Character* real_attacker = player->testAttackedBy(attackers);
        if(real_attacker!=NULL) {
            ui->PlayerLife->display(player->getHealth());
            sceneVibrate();
            redFlash();
        }
        for(int i=0;i<(int)enemies.size();++i) { //enemies
            //shoot
            std::vector<Bullet*>* new_enemy_bullets;
            if(!(enemies.at(i)->isBoss() && enemies.at(i)->isSecPhase())) new_enemy_bullets = enemies.at(i)->shoot();
            else new_enemy_bullets = enemies.at(i)->shoot2();
            if(new_enemy_bullets!=NULL) {
                for (std::vector<Bullet*>::iterator j=(*new_enemy_bullets).begin();j!=(*new_enemy_bullets).end();++j) {
                    scene->addItem(*j);
                    enemy_bullets.push_back(*j);
                    connect(this,SIGNAL(doMove()),*j,SLOT(move()));
                    connect(this,SIGNAL(doImgMove()),*j,SLOT(img_move()));
                }
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
            if(real_attacker!=NULL) real_attacker->attacked();
        }
        for(int i=0;i<(int)enemy_bullets.size();++i) { //enemy bullets
            //damaged
            Character* attacker = player;
            Character* real_attacker = enemy_bullets.at(i)->testAttackedBy(attacker);
            if(real_attacker!=NULL) {
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
        for(std::vector<Bullet*>::iterator i=enemy_bullets.begin();i!=enemy_bullets.end();) {
            if((*i)->isDead()) {
                //delete and erase
                delete (*i);
                i = enemy_bullets.erase(i);
            } else ++i;
        }
    }
    //spawn
    switch(level) {
    //level 1
    case 1:
        switch(tick) {
        case 500: case 500+125: case 500+125*2: case 500+125*3: case 500+125*4: case 500+125*5: case 500+125*6:
            //500+125i, 7 times
            for(int i=0;i<2;++i) {
                new_enemy = new Enemy(QString(":/res/enemy1.png"),40,37,60,55.5,player,3,27.75,0,0,(i==0)?120:Game::FrameWidth-120,-27,0,0.8,(i==0)?0.0012:-0.0012,0.001);
                newEnemyInit(new_enemy);
            }
            break;
        case 1500: //1500
            for(int i=0;i<2;++i) {
                new_enemy = new Enemy_Red(QString(":/res/enemy2.png"),40,37,80,74,player,15,37,350,200,(i==0)?150:Game::FrameWidth-150,-27,0,1.8,0,-0.008,false,true);
                newEnemyInit(new_enemy);
            }
            tickFreeze();
            break;
        case 1800: case 1800+250: case 1800+250*2: case 1800+250*3: //1800+250i, 4 times
            for(int i=0;i<2;++i) {
                new_enemy = new Enemy_Red(QString(":/res/enemy2.png"),40,37,60,55.5,player,4,27.75,450,100+50*i,Game::FrameWidth/2+((i==0)?-30:30),-27,0,0.6,(i==0)?-0.0008:0.0008,0);
                newEnemyInit(new_enemy);
            }
            break;
        case 2600: //2600
            tickFreeze();
            break;
        case 2900: //2900
            for(int i=0;i<2;++i) {
                new_enemy = new Enemy_Red(QString(":/res/enemy2.png"),40,37,80,74,player,9,37,400,200,(i==0)?-36:Game::FrameWidth+36,40,(i==0)?1:-1,0,0,0,true);
                newEnemyInit(new_enemy);
            }
            break;
        case 3000: case 3000+100: case 3000+100*2: //3000+100i, 3 times
            for(int i=0;i<2;++i) {
                new_enemy = new Enemy(QString(":/res/enemy1.png"),40,37,80,74,player,6,37,0,0,(i==0)?36:Game::FrameWidth-36,-36,((double)(qrand()%101)/100)*((i==0)?1:-1),0,0,0.005,true);
                newEnemyInit(new_enemy);
            }
            break;
        case 3201: //3201
            tickFreeze();
            break;
        case 3500: //3500
            new_enemy = new Enemy_Yellow(QString(":/res/enemy3.png"),40,37,100,92.5,player,40,46.25,150,300,Game::FrameWidth/2,-36,0,2.2,0,-0.008,false,true);
            newEnemyInit(new_enemy);
            tickFreeze();
            break;
        case 3700: case 3700+360: case 3700+360*2: case 3700+360*3: case 3700+360*4:
            //3700+360i, 5 times, left
            new_enemy = new Enemy_Yellow(QString(":/res/enemy3.png"),40,37,80,74,player,7,37,600,270,160,-36,0,2.2,0,-0.008);
            newEnemyInit(new_enemy);
            break;
        case 3880: case 3880+360: case 3880+360*2: case 3880+360*3: case 3880+360*4:
            //3700+180+360i, 5 times, right
            new_enemy = new Enemy_Yellow(QString(":/res/enemy3.png"),40,37,80,74,player,7,37,600,270,Game::FrameWidth-160,-36,0,2.2,0,-0.008);
            newEnemyInit(new_enemy);
            break;
        case 5800: //5800
            new_enemy = new Enemy_Pink(QString(":/res/enemy5.png"),40,37,100,92.5,player,90,46.25,100,300,Game::FrameWidth/2,-36,0,1.5,0,-0.004,false,true);
            newEnemyInit(new_enemy);
            tickFreeze();
            break;
        case 6200: //6200
            new_boss = new Enemy_Yellow_Tri(QString(":/res/enemy3.png"),40,37,100,92.5,player,60,46.25,325,400,Game::FrameWidth/2,-46,0,1,0,-0.004,false,true);
            newBossInit(new_boss);
            new_enemy = new Enemy_Pink(QString(":/res/enemy5.png"),40,37,60,55.5,player,5,27.75,300,400,Game::FrameWidth/2,-27,0,1.6,0,-0.004,false,true);
            newEnemyInit(new_enemy);
            new_enemy->setInvulnerable();
            new_effect = new_enemy->showShield();
            newEffectInit(new_effect);
            connect(new_boss,SIGNAL(deadSignal()),new_enemy,SLOT(killItself()));
            tickFreeze();
            break;
        case 6600: //6600
            for(int i=0;i<3;++i) {
                new_enemy = new Enemy_Pink(QString(":/res/enemy5.png"),40,37,80,74,player,(i==1)?40:20,37,250,(i==1)?200:325,100+(Game::FrameWidth/2-100)*i,-27,0,(i==1)?1:1.3,0,-0.004,false,true);
                newEnemyInit(new_enemy);
            }
            tickFreeze();
            break;
        case 6703: case 6703+361: case 6703+361*2://6703+361i, 3 times, warning bar fade in
            {
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
            break;
        case 6888: case 6888+361: case 6888+361*2: //6888+361i, 3 times, warning bar fade out
            {
                QGraphicsOpacityEffect *fadeout = new QGraphicsOpacityEffect(this);
                ui->WarningBar->setGraphicsEffect(fadeout);
                QPropertyAnimation *fadeoutAni = new QPropertyAnimation(fadeout,"opacity");
                fadeoutAni->setDuration(1400);
                fadeoutAni->setStartValue(1);
                fadeoutAni->setEndValue(0);
                fadeoutAni->setEasingCurve(QEasingCurve::OutQuad);
                fadeoutAni->start(QPropertyAnimation::DeleteWhenStopped);
            }
            break;
        case 7850: //7850, BOSS 1
            ui->BossLives->show();
            ui->BossHealth->setGeometry(100,40,690,30);
            new_boss = new Enemy_Blue_2(QString(":/res/enemy4.png"),40,47,120,140,player,240,60,350,400,Game::FrameWidth/2,-59,0,0,0,0,false,true);
            connect(new_boss,SIGNAL(deadSignal(int,int)),this,SLOT(bossCorpse(int,int)));
            new_boss->moveTo(Game::FrameWidth/2,200,330);
            newBossInit(new_boss);
            ui->BossLives->setText("5");
            tickFreeze();
            break;
        case 7852:
            ui->BossLives->setText("4");
            break;
        case 8100: //8100, BOSS 2
            new_boss = new Enemy_Blue_4(QString(":/res/enemy4.png"),40,47,120,140,player,210,60,350,400,Game::FrameWidth/2,200,0,0,0,0,false,true);
            connect(new_boss,SIGNAL(deadSignal(int,int)),this,SLOT(bossCorpse(int,int)));
            newBossInit(new_boss);
            for(int i=0;i<4;++i) {
                new_enemy = new Enemy(QString(":/res/enemy1.png"),40,37,60,55.5,player,5,27.75,0,0,Game::FrameWidth/5*(i+1),-27,0,0.8,0,0.005,true);
                newEnemyInit(new_enemy);
                new_enemy->setInvulnerable();
                new_effect = new_enemy->showShield();
                newEffectInit(new_effect);
                connect(new_boss,SIGNAL(useSkill(QString)),new_enemy,SIGNAL(killItsBullets()));
                connect(new_boss,SIGNAL(useSkill(QString)),new_enemy,SLOT(killItself()));
            }
            tickFreeze();
            break;
        case 8102:
            ui->BossLives->setText("3");
            break;
        case 8350: //8350, BOSS 3
            new_boss = new Enemy_Blue_5(QString(":/res/enemy4.png"),40,47,120,140,player,310,60,350,400,Game::FrameWidth/2,200,0,0,0,0,false,true);
            connect(new_boss,SIGNAL(deadSignal(int,int)),this,SLOT(bossCorpse(int,int)));
            newBossInit(new_boss);
            for(int i=0;i<2;++i) {
                new_enemy = new Enemy_Red(QString(":/res/enemy2.png"),40,37,60,55.5,player,5,27.75,470,200,(i==0)?60:Game::FrameWidth-60,-27,0,1.2,0,-0.005,false,true);
                newEnemyInit(new_enemy);
                new_enemy->setInvulnerable();
                new_effect = new_enemy->showShield();
                newEffectInit(new_effect);
                connect(new_boss,SIGNAL(useSkill(QString)),new_enemy,SIGNAL(killItsBullets()));
                connect(new_boss,SIGNAL(useSkill(QString)),new_enemy,SLOT(killItself()));
            }
            tickFreeze();
            break;
        case 8352:
            ui->BossLives->setText("2");
            break;
        case 8600: //8600, BOSS 4
            new_boss = new Enemy_Blue_3(QString(":/res/enemy4.png"),40,47,120,140,player,300,60,350,400,Game::FrameWidth/2,200,0,0,0,0,false,true);
            connect(new_boss,SIGNAL(deadSignal(int,int)),this,SLOT(bossCorpse(int,int)));
            newBossInit(new_boss);
            for(int i=0;i<2;++i) {
                new_enemy = new Enemy_Yellow(QString(":/res/enemy3.png"),40,37,60,55.5,player,5,27.75,420,270,(i==0)?60:Game::FrameWidth-60,-27,0,1.2,0,-0.005,false,true);
                newEnemyInit(new_enemy);
                new_enemy->setInvulnerable();
                new_effect = new_enemy->showShield();
                newEffectInit(new_effect);
                connect(new_boss,SIGNAL(useSkill(QString)),new_enemy,SIGNAL(killItsBullets()));
                connect(new_boss,SIGNAL(useSkill(QString)),new_enemy,SLOT(killItself()));
            }
            tickFreeze();
            break;
        case 8602:
            ui->BossLives->setText("1");
            break;
        case 8850: //8850, BOSS 5
            new_boss = new Enemy_Blue_1(QString(":/res/enemy4.png"),40,47,120,140,player,380,60,350,400,Game::FrameWidth/2,200,0,0,0,0,false,true);
            newBossInit(new_boss);
            for(int i=0;i<2;++i) {
                new_enemy = new Enemy_Pink(QString(":/res/enemy5.png"),40,37,60,55.5,player,5,27.75,340,110,(i==0)?100:Game::FrameWidth-100,-27,0,1.2,0,-0.005,false,true);
                newEnemyInit(new_enemy);
                new_enemy->setInvulnerable();
                new_effect = new_enemy->showShield();
                newEffectInit(new_effect);
                connect(new_boss,SIGNAL(useSkill(QString)),new_enemy,SIGNAL(killItsBullets()));
                connect(new_boss,SIGNAL(useSkill(QString)),new_enemy,SLOT(killItself()));
            }
            tickFreeze();
            break;
        case 8852: //8852, END
            ui->BossLives->hide();
            ui->BossSkill->hide();
            if(player!=NULL) player->gameEndSetting();
            break;
        case 9400: //9400, WIN LIST
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
            ui->EndList->setCurrentIndex(EndListPage::Won);
            ui->EndList->show();
            //game state
            gamestate=GameState::Won;
        }
        break;
    //level 2
    case 2:
        switch(tick) {
        case 500: case 500+175: case 500+175*2: case 500+175*3: case 500+175*4: case 500+175*5: case 500+175*6:
            //500+175i, 7 times
            for(int i=0;i<2;++i) {
                new_enemy = new Enemy_2_Green(QString(":/res/enemy6.png"),35,35,70,70,player,3,35,300,200,(i==0)?-35:Game::FrameWidth+35,250,(i==0)?1.2:-1.2,-0.5,0,0.001);
                newEnemyInit(new_enemy);
            }
            break;
        case 2450: //2450
            for(int i=0;i<2;++i) {
                new_enemy = new Enemy_2_Red(QString(":/res/enemy7.png"),35,35,90,90,player,15,45,200,200,(i==0)?-45:Game::FrameWidth+45,80,(i==0)?1.2:-1.2,0,(i==0)?-0.002:0.002,0,false,true);
                newEnemyInit(new_enemy);
            }
            tickFreeze();
            break;
        case 2625: case 2625+400: case 2625+400*2: case 2625+400*3: case 2625+400*4: //2625+400i, 5 times
            for(int i=0;i<2;++i) {
                new_boss = new Enemy_2_Red(QString(":/res/enemy7.png"),35,35,70,70,player,5,35,220,150,(i==0)?-30:Game::FrameWidth+30,170,(i==0)?1.2:-1.2,0.5,(i==0)?-0.002:0.002,-0.00155);
                newEnemyInit(new_boss);
                new_enemy = new Enemy_2_Green(QString(":/res/enemy6.png"),35,35,60,60,player,5,30,345,150,(i==0)?-30:Game::FrameWidth+30,110,(i==0)?1.2:-1.2,0.5,(i==0)?-0.002:0.002,-0.00155);
                newEnemyInit(new_enemy);
                new_enemy->setInvulnerable();
                connect(new_boss,SIGNAL(deadSignal()),new_enemy,SLOT(killItself()));
            }
            break;
        case 5200: //5200
            new_enemy = new Enemy_2_Yellow(QString(":/res/enemy8.png"),35,35,90,90,player,55,45,200,200,Game::FrameWidth/2,-45,0,3,0,-0.015,false,true);
            newEnemyInit(new_enemy);
            tickFreeze();
            break;
        case 5500: case 5500+600: case 5500+600*2: case 5500+600*3: //5500+600i, 4 times
            for(int i=0;i<2;++i) {
                new_enemy = new Enemy_2_Yellow(QString(":/res/enemy8.png"),35,35,70,70,player,5,35,450,150,(i==0)?-35:Game::FrameWidth+35,-35,(i==0)?0.8:-0.8,1,(i==0)?-0.0025:0.0025,0,false,true);
                newEnemyInit(new_enemy);
            }
            break;
        case 8500: //8500
            new_boss = new Enemy_2_Red(QString(":/res/enemy7.png"),35,35,100,100,player,70,50,350,120,Game::FrameWidth/2,-50,0,0,0,0,false,true);
            new_boss->moveTo(new_boss->getX(),190,100);
            newBossInit(new_boss);
            new_enemy = new Enemy_2_Green(QString(":/res/enemy6.png"),35,35,70,70,player,3,35,300,160,Game::FrameWidth/2-50,-35,0,0,0,0,false,true);
            new_enemy->moveTo(new_enemy->getX(),95,200);
            newEnemyInit(new_enemy);
            new_enemy->setInvulnerable();
            new_effect = new_enemy->showShield();
            newEffectInit(new_effect);
            connect(new_boss,SIGNAL(deadSignal()),new_enemy,SLOT(killItself()));
            new_enemy = new Enemy_2_Yellow(QString(":/res/enemy8.png"),35,35,70,70,player,3,35,300,280,Game::FrameWidth/2+50,-35,0,0,0,0,false,true);
            new_enemy->moveTo(new_enemy->getX(),95,200);
            newEnemyInit(new_enemy);
            new_enemy->setInvulnerable();
            new_effect = new_enemy->showShield();
            newEffectInit(new_effect);
            connect(new_boss,SIGNAL(deadSignal()),new_enemy,SLOT(killItself()));
            tickFreeze();
            new_enemy = new Enemy_2_Green(QString(":/res/enemy6.png"),35,35,70,70,player,3,35,300,200,Game::FrameWidth/2+115,-35,0,0,0,0,false,true);
            new_enemy->moveTo(new_enemy->getX(),170,200);
            newEnemyInit(new_enemy);
            new_enemy->setInvulnerable();
            new_effect = new_enemy->showShield();
            newEffectInit(new_effect);
            connect(new_boss,SIGNAL(deadSignal()),new_enemy,SLOT(killItself()));
            new_enemy = new Enemy_2_Yellow(QString(":/res/enemy8.png"),35,35,70,70,player,3,35,300,240,Game::FrameWidth/2-115,-35,0,0,0,0,false,true);
            new_enemy->moveTo(new_enemy->getX(),170,200);
            newEnemyInit(new_enemy);
            new_enemy->setInvulnerable();
            new_effect = new_enemy->showShield();
            newEffectInit(new_effect);
            connect(new_boss,SIGNAL(deadSignal()),new_enemy,SLOT(killItself()));
            tickFreeze();
            break;
        case 8800: //8800
            new_enemy = new Enemy_2_Pink(QString(":/res/enemy9.png"),35,35,90,90,player,55,45,150,150,Game::FrameWidth/2,-45,0,3,0,-0.015,false,true);
            newEnemyInit(new_enemy);
            tickFreeze();
            break;
        case 9100: case 9100+550: case 9100+550*2: //9100+550i, 3 times
            new_enemy = new Enemy_2_Pink(QString(":/res/enemy9.png"),35,35,60,60,player,11,30,40,60,80,-30,0,1.2);
            newEnemyInit(new_enemy);
            break;
        case 9375: case 9375+550: case 9375+550*2: //9100+550i, 3 times
            new_enemy = new Enemy_2_Pink(QString(":/res/enemy9.png"),35,35,60,60,player,11,30,40,60,Game::FrameWidth-80,-30,0,1.2);
            newEnemyInit(new_enemy);
            break;
        case 11500: //11500
            new_boss = new Enemy_2_Green_Tri(QString(":/res/enemy6.png"),35,35,100,100,player,80,50,325,420,Game::FrameWidth/2,-50,0,0,0,0,false,true);
            new_boss->moveTo(new_boss->getX(),460,250);
            newBossInit(new_boss);
            new_enemy = new Enemy_2_Pink(QString(":/res/enemy9.png"),35,35,60,60,player,5,28,300,350,Game::FrameWidth/2,-28,0,0,0,0,false,true);
            new_enemy->moveTo(new_enemy->getX(),290,350);
            new_enemy->setInvulnerable();
            newEnemyInit(new_enemy);
            new_effect = new_enemy->showShield();
            newEffectInit(new_effect);
            connect(new_boss,SIGNAL(deadSignal()),new_enemy,SLOT(killItself()));
            new_enemy = new Enemy_2_Yellow(QString(":/res/enemy8.png"),35,35,60,60,player,5,28,600,350,Game::FrameWidth/2+40,-28,0,0,0,0,false,true);
            new_enemy->moveTo(new_enemy->getX(),220,350);
            new_enemy->setInvulnerable();
            newEnemyInit(new_enemy);
            new_effect = new_enemy->showShield();
            newEffectInit(new_effect);
            connect(new_boss,SIGNAL(deadSignal()),new_enemy,SLOT(killItself()));
            new_enemy = new Enemy_2_Red(QString(":/res/enemy7.png"),35,35,60,60,player,5,28,500,350,Game::FrameWidth/2-40,-28,0,0,0,0,false,true);
            new_enemy->moveTo(new_enemy->getX(),220,350);
            new_enemy->setInvulnerable();
            newEnemyInit(new_enemy);
            new_effect = new_enemy->showShield();
            newEffectInit(new_effect);
            connect(new_boss,SIGNAL(deadSignal()),new_enemy,SLOT(killItself()));
            tickFreeze();
            break;
        case 11603: case 11603+361: case 11603+361*2: //11603+361i, 3 times, warning bar fade in
            {
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
            break;
        case 11788: case 11788+361: case 11788+361*2: //11788+361i, 3 times, warning bar fade out
            {
                QGraphicsOpacityEffect *fadeout = new QGraphicsOpacityEffect(this);
                ui->WarningBar->setGraphicsEffect(fadeout);
                QPropertyAnimation *fadeoutAni = new QPropertyAnimation(fadeout,"opacity");
                fadeoutAni->setDuration(1400);
                fadeoutAni->setStartValue(1);
                fadeoutAni->setEndValue(0);
                fadeoutAni->setEasingCurve(QEasingCurve::OutQuad);
                fadeoutAni->start(QPropertyAnimation::DeleteWhenStopped);
            }
            break;
        case 12748: //12748, BOSS 1
            ui->BossLives->show();
            ui->BossHealth->setGeometry(100,40,690,30);
            new_boss = new Enemy_2_Blue_1(QString(":/res/enemy10.png"),35,35,130,130,player,340,60,100,400,Game::FrameWidth/2,-59,0,0,0,0,false,true);
            connect(new_boss,SIGNAL(deadSignal(int,int)),this,SLOT(bossCorpse(int,int)));
            new_boss->moveTo(Game::FrameWidth/2,200,330);
            newBossInit(new_boss);
            ui->BossLives->setText("5");
            tickFreeze();
            break;
        case 12750: //12750
            ui->BossLives->setText("4");
            break;
        case 12998: //12998, BOSS 2
            new_boss = new Enemy_2_Blue_2(QString(":/res/enemy10.png"),35,35,130,130,player,370,60,300,400,Game::FrameWidth/2,200,0,0,0,0,false,true);
            connect(new_boss,SIGNAL(deadSignal(int,int)),this,SLOT(bossCorpse(int,int)));
            newBossInit(new_boss);
            for(int i=0;i<2;++i) {
                new_enemy = new Enemy_2_Green(QString(":/res/enemy6.png"),35,35,60,60,player,5,30,445,400,Game::FrameWidth/2,200,(i==0)?-1.2:1.2,0.9,0,-0.005,true);
                newEnemyInit(new_enemy);
                new_enemy->setInvulnerable();
                new_enemy->fadein(1000);
                new_effect = new_enemy->showShield();
                newEffectInit(new_effect);
                connect(new_boss,SIGNAL(useSkill(QString)),new_enemy,SIGNAL(killItsBullets()));
                connect(new_boss,SIGNAL(useSkill(QString)),new_enemy,SLOT(killItself()));
            }
            tickFreeze();
            break;
        case 13000: //13000
            ui->BossLives->setText("3");
            break;
        case 13248: //13248, BOSS 3
            new_boss = new Enemy_2_Blue_5(QString(":/res/enemy10.png"),35,35,130,130,player,310,60,300,400,Game::FrameWidth/2,200,0,0,0,0,false,true);
            connect(new_boss,SIGNAL(deadSignal(int,int)),this,SLOT(bossCorpse(int,int)));
            newBossInit(new_boss);
            for(int i=0;i<2;++i) {
                new_enemy = new Enemy_2_Red(QString(":/res/enemy7.png"),35,35,60,60,player,5,30,445,400,Game::FrameWidth/2,200,(i==0)?-0.8:0.8,-1.2,0,0.005,true,true);
                newEnemyInit(new_enemy);
                new_enemy->setInvulnerable();
                new_enemy->fadein(1000);
                new_effect = new_enemy->showShield();
                newEffectInit(new_effect);
                connect(new_boss,SIGNAL(useSkill(QString)),new_enemy,SIGNAL(killItsBullets()));
                connect(new_boss,SIGNAL(useSkill(QString)),new_enemy,SLOT(killItself()));
            }
            tickFreeze();
            break;
        case 13250: //13250
            ui->BossLives->setText("2");
            break;
        case 13498: //13498, BOSS 4
            new_boss = new Enemy_2_Blue_4(QString(":/res/enemy10.png"),35,35,130,130,player,390,60,300,400,Game::FrameWidth/2,200,0,0,0,0,false,true);
            connect(new_boss,SIGNAL(deadSignal(int,int)),this,SLOT(bossCorpse(int,int)));
            newBossInit(new_boss);
            for(int i=0;i<2;++i) {
                new_enemy = new Enemy_2_Yellow(QString(":/res/enemy8.png"),35,35,60,60,player,5,30,250,500,Game::FrameWidth/2,200,(i==0)?-0.8:0.8,-1.2,0,0.005,true,true);
                newEnemyInit(new_enemy);
                new_enemy->setInvulnerable();
                new_enemy->fadein(1000);
                new_effect = new_enemy->showShield();
                newEffectInit(new_effect);
                connect(new_boss,SIGNAL(useSkill(QString)),new_enemy,SIGNAL(killItsBullets()));
                connect(new_boss,SIGNAL(useSkill(QString)),new_enemy,SLOT(killItself()));
            }
            tickFreeze();
            break;
        case 13500: //13500
            ui->BossLives->setText("1");
            break;
        case 13748: //13748, BOSS 5
            new_boss = new Enemy_2_Blue_3(QString(":/res/enemy10.png"),35,35,130,130,player,450,60,400,400,Game::FrameWidth/2,200,0,0,0,0,false,true);
            newBossInit(new_boss);
            for(int i=0;i<2;++i) {
                new_enemy = new Enemy_2_Pink(QString(":/res/enemy9.png"),35,35,60,60,player,5,30,300,300,Game::FrameWidth/2,200,(i==0)?-0.8:0.8,-1.2,0,0.005,true,true);
                newEnemyInit(new_enemy);
                new_enemy->setInvulnerable();
                new_enemy->fadein(1000);
                new_effect = new_enemy->showShield();
                newEffectInit(new_effect);
                connect(new_boss,SIGNAL(useSkill(QString)),new_enemy,SIGNAL(killItsBullets()));
                connect(new_boss,SIGNAL(useSkill(QString)),new_enemy,SLOT(killItself()));
            }
            tickFreeze();
            break;
        case 13750: //13750, END
            ui->BossLives->hide();
            ui->BossSkill->hide();
            if(player!=NULL) player->gameEndSetting();
            break;
        case 14425: //14425, WIN LIST
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
            ui->EndList->setCurrentIndex(EndListPage::Won);
            ui->EndList->show();
            //game state
            gamestate=GameState::Won;
        }
        break;
    //level 3
    case 3:
        switch(tick) {
        case 500: case 500+250: case 500+250*2: case 500+250*3: case 500+250*4: //500+250i, 5 times
            for(int i=0;i<2;++i) {
                new_effect = new Effect(QString(":/res/magic.png"),50,50,120,120,350,(i==0)?300-60*((tick-500)/250):Game::FrameWidth-300+60*((tick-500)/250),80+100*((tick-500)/250),0,0,0,0,true);
                new_effect->setOpacity(0.6);
                new_effect->fadein();
                newEffectInit(new_effect);
            }
            break;
        case 700: case 700+250: case 700+250*2: case 700+250*3: case 700+250*4: //700+250i, 5 times
            for(int i=0;i<2;++i) {
                new_enemy = new Enemy_3_Green(QString(":/res/enemy11.png"),54,43,100,80,player,3,40,280,50,(i==0)?300-60*((tick-700)/250):Game::FrameWidth-300+60*((tick-700)/250),80+100*((tick-700)/250),(i==0)?0.1:-0.1,0,0,-0.0006);
                new_enemy->fadein();
                newEnemyInit(new_enemy);
            }
            break;
        case 2700: //2700
            for(int i=0;i<2;++i) {
                new_effect = new Effect(QString(":/res/magic.png"),50,50,150,150,350,(i==0)?100:Game::FrameWidth-100,150,0,0,0,0,true);
                new_effect->setOpacity(0.6);
                new_effect->fadein();
                newEffectInit(new_effect);
            }
            break;
        case 2900: //2900
            for(int i=0;i<2;++i) {
                new_enemy = new Enemy_3_Red(QString(":/res/enemy12.png"),54,43,125,100,player,14,50,250,120,(i==0)?100:Game::FrameWidth-100,150,(i==0)?0:-0.001,0,(i==0)?0:0.001,0,false,true);
                new_enemy->fadein();
                newEnemyInit(new_enemy);
            }
            break;
        case 3800: //3800
            new_effect = new Effect(QString(":/res/magic.png"),50,50,180,180,725,Game::FrameWidth/2,220,0,0,0,0,true);
            new_effect->setOpacity(0.6);
            new_effect->fadein();
            newEffectInit(new_effect);
            for(int t=0;t<2;++t) {
                for(int i=0;i<2;++i) {
                    new_effect = new Effect(QString(":/res/magic.png"),50,50,120,120,725,(i==0)?300-80*t:Game::FrameWidth-300+80*t,80+100*t,0,0,0,0,true);
                    new_effect->setOpacity(0.6);
                    new_effect->fadein();
                    newEffectInit(new_effect);
                }
            }
            break;
        case 4000: //4000
            new_boss = new Enemy_3_Green(QString(":/res/enemy11.png"),54,43,150,120,player,45,60,50,375,Game::FrameWidth/2,220);
            new_boss->setInvulnerable();
            new_boss->fadein();
            newBossInit(new_boss);
            break;
        case 4125: case 4250: //4125+125i, 2 times
            {
                const int t=(tick-4125)/125;
                for(int i=0;i<2;++i) {
                    new_enemy = new Enemy_3_Red(QString(":/res/enemy12.png"),54,43,100,80,player,5,40,250,100+175*(1-t),(i==0)?300-80*t:Game::FrameWidth-300+80*t,80+100*t);
                    new_enemy->setInvulnerable();
                    new_effect = new_enemy->showShield();
                    newEffectInit(new_effect);
                    connect(new_boss,SIGNAL(deadSignal()),new_enemy,SLOT(killItself()));
                    new_enemy->fadein();
                    newEnemyInit(new_enemy);
                }
            }
            break;
        case 4251: //4251
            tickFreeze();
            new_boss->setVulnerable();
            break;
        case 4500: //4500
            new_effect = new Effect(QString(":/res/magic.png"),50,50,180,180,350,Game::FrameWidth/2,250,0,0,0,0,true);
            new_effect->setOpacity(0.6);
            new_effect->fadein();
            newEffectInit(new_effect);
            break;
        case 4700: //4700
            new_enemy = new Enemy_3_Yellow(QString(":/res/enemy13.png"),54,43,150,120,player,30,60,250,120,Game::FrameWidth/2,250);
            new_enemy->fadein();
            newEnemyInit(new_enemy);
            tickFreeze();
            break;
        case 4950: case 4950+250: case 4950+250*2: case 4950+250*3:
        case 4950+250*4: case 4950+250*5: case 4950+250*6: case 4950+250*7:
            //4950+250i, 8 times
            {
                const int t=(tick-4950)/250;
                new_enemy = new Enemy_3_Yellow(QString(":/res/enemy13.png"),54,43,100,80,player,9,40,500,50,(t%2==0)?-40:Game::FrameWidth+40,50+60*((t>=4)?7-t:t),(t%2==0)?1.5:-1.5,0,0,0,true);
                newEnemyInit(new_enemy);
                if(t==7) tickFreeze();
            }
            break;
        case 7000: //7000
            new_effect = new Effect(QString(":/res/magic.png"),50,50,180,180,350,Game::FrameWidth/2,100,0,0,0,0,true);
            new_effect->setOpacity(0.6);
            new_effect->fadein();
            newEffectInit(new_effect);
            for(int i=0;i<2;++i) {
                new_effect = new Effect(QString(":/res/magic.png"),50,50,120,120,350,(i==0)?80:Game::FrameWidth-80,Game::FrameHeight-70,0,0,0,0,true);
                new_effect->setOpacity(0.6);
                new_effect->fadein();
                newEffectInit(new_effect);
            }
            break;
        case 7200: //7200
            new_boss = new Enemy_3_Yellow(QString(":/res/enemy13.png"),54,43,150,120,player,60,60,300,50,Game::FrameWidth/2,100);
            new_boss->fadein();
            newBossInit(new_boss);
            for(int i=0;i<2;++i) {
                new_enemy = new Enemy_3_Green(QString(":/res/enemy11.png"),54,43,100,80,player,5,40,250,120,(i==0)?80:Game::FrameWidth-80,Game::FrameHeight-70,(i==0)?1.3:-1.3,0,0,0,true);
                new_enemy->setInvulnerable();
                new_effect = new_enemy->showShield();
                newEffectInit(new_effect);
                connect(new_boss,SIGNAL(deadSignal()),new_enemy,SLOT(killItself()));
                new_enemy->fadein();
                newEnemyInit(new_enemy);
            }
            tickFreeze();
            break;
        case 7500: //7500
            new_effect = new Effect(QString(":/res/magic.png"),50,50,180,180,350,Game::FrameWidth/2,250,0,0,0,0,true);
            new_effect->setOpacity(0.6);
            new_effect->fadein();
            newEffectInit(new_effect);
            break;
        case 7700: //7700
            new_enemy = new Enemy_3_Pink(QString(":/res/enemy14.png"),54,43,150,120,player,40,60,120,100,Game::FrameWidth/2,250);
            new_enemy->fadein();
            newEnemyInit(new_enemy);
            tickFreeze();
            break;
        //7900+375i, 4 times
        case 7900+375*3:
            tickFreeze();
        case 7900: case 7900+375: case 7900+375*2:
            for(int i=-1;i<=1;++i) {
                new_enemy = new Enemy_3_Pink(QString(":/res/enemy14.png"),54,43,100,80,player,(i==0)?6:3,40,400,150,Game::FrameWidth/2+i*(Game::FrameWidth/2+40),-40+std::abs(i)*100);
                new_enemy->moveTo(Game::FrameWidth/2+i*100,240+std::abs(i)*87,400);
                newEnemyInit(new_enemy);
            }
            break;
        case 9275: //9275
            new_effect = new Effect(QString(":/res/magic.png"),50,50,180,180,350,Game::FrameWidth/2,100,0,0,0,0,true);
            new_effect->setOpacity(0.6);
            new_effect->fadein();
            newEffectInit(new_effect);
            for(int i=0;i<2;++i) {
                new_effect = new Effect(QString(":/res/magic.png"),50,50,120,120,350,(i==0)?80:Game::FrameWidth-80,70,0,0,0,0,true);
                new_effect->setOpacity(0.6);
                new_effect->fadein();
                newEffectInit(new_effect);
            }
            break;
        case 9475: //9475
            new_boss = new Enemy_3_Pink_Cross(QString(":/res/enemy14.png"),54,43,150,120,player,35,60,110,50,Game::FrameWidth/2,100);
            new_boss->fadein();
            newBossInit(new_boss);
            for(int i=0;i<2;++i) {
                new_enemy = new Enemy_3_Green(QString(":/res/enemy11.png"),54,43,100,80,player,5,40,130,150,(i==0)?80:Game::FrameWidth-80,70);
                new_enemy->setInvulnerable();
                new_effect = new_enemy->showShield();
                newEffectInit(new_effect);
                connect(new_boss,SIGNAL(deadSignal()),new_enemy,SLOT(killItself()));
                new_enemy->fadein();
                newEnemyInit(new_enemy);
            }
            tickFreeze();
            break;
        case 9578: case 9578+361: case 9578+361*2: //9578+361i, 3 times, warning bar fade in
            {
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
            break;
        case 9763: case 9763+361: case 9763+361*2: //9763+361i, 3 times, warning bar fade out
            {
                QGraphicsOpacityEffect *fadeout = new QGraphicsOpacityEffect(this);
                ui->WarningBar->setGraphicsEffect(fadeout);
                QPropertyAnimation *fadeoutAni = new QPropertyAnimation(fadeout,"opacity");
                fadeoutAni->setDuration(1400);
                fadeoutAni->setStartValue(1);
                fadeoutAni->setEndValue(0);
                fadeoutAni->setEasingCurve(QEasingCurve::OutQuad);
                fadeoutAni->start(QPropertyAnimation::DeleteWhenStopped);
            }
            break;
        case 10725: //10725
            new_effect = new Effect(QString(":/res/magic.png"),50,50,180,180,350,Game::FrameWidth/2,200,0,0,0,0,true);
            new_effect->setOpacity(0.6);
            new_effect->fadein();
            newEffectInit(new_effect);
            break;
        case 10925: //10925, BOSS 1
            ui->BossLives->show();
            ui->BossHealth->setGeometry(100,40,690,30);
            new_boss = new Enemy_3_Blue_1(QString(":/res/enemy15.png"),54,55,160,160,player,290,80,35,400,Game::FrameWidth/2,200,0,0,0,0,0,true);
            connect(new_boss,SIGNAL(deadSignal(int,int)),this,SLOT(bossCorpse(int,int)));
            new_boss->fadein(1500);
            newBossInit(new_boss);
            ui->BossLives->setText("5");
            tickFreeze();
            break;
        case 10927: //10927
            ui->BossLives->setText("4");
            for(int i=0;i<2;++i) {
                new_effect = new Effect(QString(":/res/magic.png"),50,50,100,100,400,(i==0)?160:Game::FrameWidth-160,150,0,0,0,0,true);
                new_effect->setOpacity(0.6);
                new_effect->fadein();
                newEffectInit(new_effect);
            }
            break;
        case 11175: //11175, BOSS 2
            new_boss = new Enemy_3_Blue_2(QString(":/res/enemy15.png"),54,55,160,160,player,330,80,100,400,Game::FrameWidth/2,200,0,0,0,0,0,true);
            connect(new_boss,SIGNAL(deadSignal(int,int)),this,SLOT(bossCorpse(int,int)));
            new_boss->fadein(1500);
            newBossInit(new_boss);
            for(int i=0;i<2;++i) {
                new_enemy = new Enemy_3_Green(QString(":/res/enemy11.png"),54,55,75,60,player,5,30,230,300,(i==0)?160:Game::FrameWidth-160,150);
                newEnemyInit(new_enemy);
                new_enemy->setInvulnerable();
                new_enemy->fadein(1000);
                new_effect = new_enemy->showShield();
                newEffectInit(new_effect);
                connect(new_boss,SIGNAL(useSkill(QString)),new_enemy,SIGNAL(killItsBullets()));
                connect(new_boss,SIGNAL(useSkill(QString)),new_enemy,SLOT(killItself()));
            }
            tickFreeze();
            break;
        case 11177: //11177
            ui->BossLives->setText("3");
            for(int i=0;i<2;++i) {
                new_effect = new Effect(QString(":/res/magic.png"),50,50,100,100,400,(i==0)?160:Game::FrameWidth-160,150,0,0,0,0,true);
                new_effect->setOpacity(0.6);
                new_effect->fadein();
                newEffectInit(new_effect);
            }
            break;
        case 11425: //11425, BOSS 3
            new_boss = new Enemy_3_Blue_3(QString(":/res/enemy15.png"),54,55,160,160,player,370,80,100,400,Game::FrameWidth/2,200,0,0,0,0,0,true);
            connect(new_boss,SIGNAL(deadSignal(int,int)),this,SLOT(bossCorpse(int,int)));
            new_boss->fadein(1500);
            newBossInit(new_boss);
            for(int i=0;i<2;++i) {
                new_enemy = new Enemy_3_Red(QString(":/res/enemy12.png"),54,55,75,60,player,5,30,250,300,(i==0)?160:Game::FrameWidth-160,150);
                newEnemyInit(new_enemy);
                new_enemy->setInvulnerable();
                new_enemy->fadein(1000);
                new_effect = new_enemy->showShield();
                newEffectInit(new_effect);
                connect(new_boss,SIGNAL(useSkill(QString)),new_enemy,SIGNAL(killItsBullets()));
                connect(new_boss,SIGNAL(useSkill(QString)),new_enemy,SLOT(killItself()));
            }
            tickFreeze();
            break;
        case 11427: //11427
            ui->BossLives->setText("2");
            for(int i=0;i<2;++i) {
                new_effect = new Effect(QString(":/res/magic.png"),50,50,100,100,400,(i==0)?160:Game::FrameWidth-160,150,0,0,0,0,true);
                new_effect->setOpacity(0.6);
                new_effect->fadein();
                newEffectInit(new_effect);
            }
            break;
        case 11675: //11675, BOSS 4
            new_boss = new Enemy_3_Blue_4(QString(":/res/enemy15.png"),54,55,160,160,player,395,80,100,400,Game::FrameWidth/2,200,0,0,0,0,0,true);
            connect(new_boss,SIGNAL(deadSignal(int,int)),this,SLOT(bossCorpse(int,int)));
            new_boss->fadein(1500);
            newBossInit(new_boss);
            for(int i=0;i<2;++i) {
                new_enemy = new Enemy_3_Yellow(QString(":/res/enemy13.png"),54,55,75,60,player,5,30,500,300,(i==0)?160:Game::FrameWidth-160,150);
                newEnemyInit(new_enemy);
                new_enemy->setInvulnerable();
                new_enemy->fadein(1000);
                new_effect = new_enemy->showShield();
                newEffectInit(new_effect);
                connect(new_boss,SIGNAL(useSkill(QString)),new_enemy,SIGNAL(killItsBullets()));
                connect(new_boss,SIGNAL(useSkill(QString)),new_enemy,SLOT(killItself()));
            }
            tickFreeze();
            break;
        case 11677: //11677
            ui->BossLives->setText("1");
            for(int i=0;i<2;++i) {
                new_effect = new Effect(QString(":/res/magic.png"),50,50,100,100,400,(i==0)?160:Game::FrameWidth-160,150,0,0,0,0,true);
                new_effect->setOpacity(0.6);
                new_effect->fadein();
                newEffectInit(new_effect);
            }
            break;
        case 11925: //11925, BOSS 5
            new_boss = new Enemy_3_Blue_5(QString(":/res/enemy15.png"),54,55,160,160,player,550,80,70,400,Game::FrameWidth/2,200,0,0,0,0,0,true);
            new_boss->fadein(1500);
            newBossInit(new_boss);
            for(int i=0;i<2;++i) {
                new_enemy = new Enemy_3_Pink(QString(":/res/enemy14.png"),54,55,75,60,player,5,30,300,300,(i==0)?160:Game::FrameWidth-160,150);
                newEnemyInit(new_enemy);
                new_enemy->setInvulnerable();
                new_enemy->fadein(1000);
                new_effect = new_enemy->showShield();
                newEffectInit(new_effect);
                connect(new_boss,SIGNAL(useSkill(QString)),new_enemy,SIGNAL(killItsBullets()));
                connect(new_boss,SIGNAL(useSkill(QString)),new_enemy,SLOT(killItself()));
            }
            tickFreeze();
            break;
        case 11927: //11927, END
            ui->BossLives->hide();
            ui->BossSkill->hide();
            if(player!=NULL) player->gameEndSetting();
            break;
        case 12600: //12600, WIN LIST
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
            ui->EndList->setCurrentIndex(EndListPage::Won);
            ui->EndList->show();
            //game state
            gamestate=GameState::Won;
        }
        break;
    default:
        qDebug() << "error: can't get what level is selected";
    }
    //draw
    if(gamestate!=GameState::Paused) emit doImgMove();
    //player death
    if(player->isDead()) {
        //game end
        player->gameEndSetting();
        player->setOpacity(0.5);
        ui->EndList->setCurrentIndex(EndListPage::Failed);
        ui->EndList->show();
        //game state
        gamestate=GameState::Failed;
    }
    //ticking
    if(gamestate==GameState::Playing && ticking) ++tick;
    if(gamestate==GameState::Playing) ++gametime;
}
void MainWindow::backToMenu() {
    ticking=false;
    //kill timer
    delete timer;
    timer=NULL;
    delete freezeTimer;
    freezeTimer=NULL;
    //kil all objects
    for(std::vector<Bullet*>::iterator i=player_bullets.begin();i!=player_bullets.end();) {
        delete (*i);
        i = player_bullets.erase(i);
    }
    for(std::vector<Enemy*>::iterator i=enemies.begin();i!=enemies.end();) {
        delete (*i);
        i = enemies.erase(i);
    }
    for(std::vector<Bullet*>::iterator i=enemy_bullets.begin();i!=enemy_bullets.end();) {
        delete (*i);
        i = enemy_bullets.erase(i);
    }
    delete player;
    player=NULL;
    emit killEffects();
    dot=NULL;
    //menu
    ui->stackedWidget->setCurrentIndex(GamePage::Menu);
    //game state
    gamestate=GameState::Menu;
}
void MainWindow::newEffectInit(Effect* new_effect) {
    if(new_effect!=NULL) {
        scene->addItem(new_effect);
        connect(this,SIGNAL(doMove()),new_effect,SLOT(move()));
        connect(this,SIGNAL(doImgMove()),new_effect,SLOT(img_move()));
        connect(this,SIGNAL(killEffects()),new_effect,SLOT(killItself()));
    }
}
void MainWindow::newEnemyInit(Enemy* new_enemy) {
    if(new_enemy!=NULL) {
        scene->addItem(new_enemy);
        enemies.push_back(new_enemy);
        connect(this,SIGNAL(doMove()),new_enemy,SLOT(move()));
        connect(this,SIGNAL(doImgMove()),new_enemy,SLOT(img_move()));
        connect(new_enemy,SIGNAL(pointGive(int)),this,SLOT(pointAdd(int)));
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
    connect(new_boss,SIGNAL(useSkill(QString)),bossSkillMoveInAni,SLOT(start()));
    connect(new_boss,SIGNAL(useSkill(QString)),ui->BossSkill,SLOT(show()));
    connect(new_boss,SIGNAL(useSkill(QString)),ui->BossSkill,SLOT(setText(QString)));
    connect(new_boss,SIGNAL(useSkill(QString)),flash,SLOT(flash()));
    connect(new_boss,SIGNAL(deadSignal()),ui->BossHealth,SLOT(hide()));
    connect(new_boss,SIGNAL(deadSignal()),ui->BossSkill,SLOT(hide()));
    connect(new_boss,SIGNAL(summonEffect(Effect*)),this,SLOT(newEffectInit(Effect*)));
}
void MainWindow::pauseAndResume() {
    if(gamestate==GameState::Playing) {
        ui->EndList->setCurrentIndex(EndListPage::Paused);
        ui->EndList->show();
        gamestate=GameState::Paused;
    } else {
        ui->EndList->hide();
        gamestate=GameState::Playing;
    }
    return;
}
void MainWindow::bossCorpse(int x, int y) {
    QString str;
    switch(level) {
    case 1:
        str = ":/res/lightball.png";
        break;
    case 2:
        str = ":/res/stars.png";
        break;
    case 3:
        str = ":/res/magic_wind.png";
        break;
    default:
        str = ":/res/lightball.png";
        qDebug() << "Error: can't find boss corpse img because can't get what level is";
        break;
    }
    new_effect = new Effect(str,120,120,300,300,308,x,y,0,0,0,0,true);
    new_effect->fadein();
    new_effect->rotateStart();
    new_effect->moveTo(Game::FrameWidth/2,200,246);
    newEffectInit(new_effect);
}
void MainWindow::pointAdd(int point) {
    ui->PlayerPoint->display(ui->PlayerPoint->value()+point);
}
void MainWindow::tickFreeze() {
    ticking=false;
    ++tick;
    if(freezeTimer==NULL) freezeTimer = new QTimer;
    freezeTimer->start(100);
    connect(freezeTimer,SIGNAL(timeout()),this,SLOT(tickContinueCheck()));
}
bool MainWindow::tickCheck(unsigned int tick) {
    if(this->tick==tick) return true;
    return false;
}
bool MainWindow::tickCheck(unsigned int tick_init, unsigned int interval, unsigned int times) {
    if(tick>=tick_init&&(tick-tick_init)%interval==0&&(tick-tick_init)/interval<times) return true;
    return false;
}
void MainWindow::tickContinueCheck() {
    if(enemies.size()==0 && freezeTimer!=NULL) {
        delete freezeTimer;
        freezeTimer = NULL;
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
    static QPropertyAnimation* vibrateAni=NULL;
    static short vibrate_count_temp=0;
    vibrate_count_temp = vibrate_count+1;
    if(vibrate_count<10) {
        if(vibrateAni!=NULL) delete vibrateAni;
        vibrateAni = new QPropertyAnimation(scene,"sceneRect");
        vibrateAni->setDuration((50+vibrate_count*vibrate_count*2.2)/((vibrate_count==0||vibrate_count==9)?2:1));
        vibrateAni->setEasingCurve(vibrate_count==9?QEasingCurve::InSine:(vibrate_count==0?QEasingCurve::OutSine:QEasingCurve::InOutSine));
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
        if(vibrateAni!=NULL) delete vibrateAni;
        vibrateAni=NULL;
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
            if(gamestate!=GameState::Menu) player->setShooting(true);
            Player::speed=1.8;
            break;
        case Qt::Key_X:
            use_skill=true;
            break;
        case Qt::Key_Escape:
        case Qt::Key_P:
            if(gamestate==GameState::Playing||gamestate==GameState::Paused) pauseAndResume();
            break;
        case Qt::Key_M:
            if(gamestate==GameState::Won||gamestate==GameState::Failed||gamestate==GameState::Paused) backToMenu();
            break;
        case Qt::Key_R:
            if(gamestate==GameState::Won||gamestate==GameState::Failed||gamestate==GameState::Paused) {
                backToMenu();
                start();
            }
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
                if(gamestate==GameState::Menu) start();
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
            if(gamestate!=GameState::Menu) player->setShooting(false);
            Player::speed=3.5;
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
