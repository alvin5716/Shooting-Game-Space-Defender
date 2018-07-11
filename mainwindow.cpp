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
    level = 2;
    //scene
    scene = new QGraphicsScene(0,0,Game::FrameWidth,Game::FrameHeight);
    ui->graphicsView->setScene(scene);
    ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView->setViewportUpdateMode(QGraphicsView::NoViewportUpdate);
    //qrand
    qsrand(time(NULL));
    //button
    connect(ui->startButton,SIGNAL(clicked(bool)),this,SLOT(start()));
    connect(ui->QuitButton,SIGNAL(clicked(bool)),qApp,SLOT(quit()));
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
}
void MainWindow::start() {
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
    skill_times=10;
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
    player = new Player(10,5,15,Game::FrameWidth/2,Game::FrameHeight/2);
    ui->PlayerLife->display(player->getHealth());
    scene->addItem(player);
    connect(this,SIGNAL(doMove()),player,SLOT(move()));
    connect(this,SIGNAL(doImgMove()),player,SLOT(img_move()));
    connect(player,SIGNAL(healthColorChange(QString)),this,SLOT(healthColorChange(QString)));
    player->healthColorChange("white");
    //dot
    if(dot!=NULL) delete dot;
    dot = new Shield(":/res/dot.png",50,50,10,10,player,-1,player->getX(),player->getY());
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
        break;
    case 2:
        boss_tick=Game::BossTick2;
        break;
    default:
        boss_tick=Game::BossTick1;
    }
}
void MainWindow::doTick() {
    //focus
    setFocus();
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
            QPixmap *temp = new QPixmap(":/res/bg_boss.png");
            QPainter merger(oriImg2);
            merger.drawPixmap(QRect(0,0,150,(450-172)),*temp,QRect(0,0,150,(450-172)));
            merger.drawPixmap(QRect(0,(450-172),150,172),*cutImg,QRect(0,0,150,172));
        }
        QPainter merger(cutImg);
        int init = (450-172)-((tick-boss_tick)/4+1);
        if(init<0) init = 0;
        if(oriImg2!=NULL) merger.drawPixmap(QRect(0,0,150,172),*oriImg2,QRect(0,init,150,172));
        else merger.drawPixmap(QRect(0,0,150,172),QPixmap(":/res/bg_boss.png"),QRect(0,0,150,172));
    }
    background->setPixmap(cutImg->scaled(Game::FrameWidth,Game::FrameHeight));

    //game
    if(gamestate==GameState::Playing) {
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
        //player skill
        if(use_skill && !player->isUsingSkill() && skill_times>0 && !player->isInvulnerable()) { //init
            player->coolDown();
            new_effect = new Shield(QString(":/res/shield1.png"),70,70,400,400,player,375,player->getX(),player->getY());
            new_effect->fadein();
            newEffectInit(new_effect);
            --skill_times;
            ui->PlayerSkill->display(skill_times);
        }
        if(player->isUsingSkill()) {
            for(int i=0;i<(int)enemy_bullets.size();++i) { //continuous
                if(sqrt(pow(enemy_bullets.at(i)->getX()-player->getX(),2)+pow(enemy_bullets.at(i)->getY()-player->getY(),2))<=200 && !enemy_bullets.at(i)->isInvulnerable())
                    enemy_bullets.at(i)->killItself();
            }
        }
        //move
        emit doMove();
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
        if(real_attacker!=NULL) ui->PlayerLife->display(player->getHealth());
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
                real_attacker->attacked();
                ui->PlayerLife->display(player->getHealth());
            }
        }
        //death
        if(player->isDead()) {
            //game end
            player->gameEndSetting();
            player->setOpacity(0.5);
            ui->EndList->setCurrentIndex(EndListPage::Failed);
            ui->EndList->show();
            //game state
            gamestate=GameState::Failed;
        }
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
        if(tickCheck(500,125,7)) { //500+125i, 7 times
            for(int i=0;i<2;++i) {
                new_enemy = new Enemy(QString(":/res/enemy1.png"),40,37,60,55.5,player,3,27.75,0,0,(i==0)?120:Game::FrameWidth-120,-27,0,0.8,(i==0)?0.0012:-0.0012,0.001);
                newEnemyInit(new_enemy);
            }
        } else if(tickCheck(1500)) { //1500
            for(int i=0;i<2;++i) {
                new_enemy = new Enemy_Red(QString(":/res/enemy2.png"),40,37,80,74,player,15,37,350,200,(i==0)?150:Game::FrameWidth-150,-27,0,1.8,0,-0.008,false,true);
                newEnemyInit(new_enemy);
            }
            tickFreeze();
        } else if(tickCheck(1800,250,4)) { //1800+250i, 4 times
            for(int i=0;i<2;++i) {
                new_enemy = new Enemy_Red(QString(":/res/enemy2.png"),40,37,60,55.5,player,8,27.75,450,100+50*i,Game::FrameWidth/2+((i==0)?-30:30),-27,0,0.6,(i==0)?-0.0008:0.0008,0);
                newEnemyInit(new_enemy);
            }
        } else if(tickCheck(2600)) { //2600
            tickFreeze();
        } else if(tickCheck(2900)) { //2900
            for(int i=0;i<2;++i) {
                new_enemy = new Enemy_Red(QString(":/res/enemy2.png"),40,37,80,74,player,15,37,400,200,(i==0)?-36:Game::FrameWidth+36,40,(i==0)?1:-1,0,0,0,true);
                newEnemyInit(new_enemy);
            }
        } else if(tickCheck(3000,100,3)) { //3000+100i, 3 times
            for(int i=0;i<2;++i) {
                new_enemy = new Enemy(QString(":/res/enemy1.png"),40,37,80,74,player,6,37,0,0,(i==0)?36:Game::FrameWidth-36,-36,((double)(qrand()%101)/100)*((i==0)?1:-1),0,0,0.005,true);
                newEnemyInit(new_enemy);
            }
        } else if(tickCheck(3201)) { //3201
            tickFreeze();
        } else if(tickCheck(3500)) { //3500
            new_boss = new Enemy_Yellow(QString(":/res/enemy3.png"),40,37,100,92.5,player,80,46.25,600,400,Game::FrameWidth/2,-36,0,2.2,0,-0.008,false,true);
            newBossInit(new_boss);
            for(int i=0;i<2;++i) {
                new_enemy = new Enemy_Red(QString(":/res/enemy2.png"),40,37,60,55.5,player,5,27.75,600,300,(i==0)?40:Game::FrameWidth-40,-27,0,1.2,0,-0.005,false,true);
                newEnemyInit(new_enemy);
                new_enemy->setInvulnerable();
                new_effect = new_enemy->showShield();
                newEffectInit(new_effect);
                connect(new_boss,SIGNAL(deadSignal()),new_enemy,SLOT(killItself()));
            }
            tickFreeze();
        } else if(tickCheck(3700,180,10)) { //3700+300i, 10 times
            if(tickCheck(3700,360,5)) //left
                new_enemy = new Enemy_Yellow(QString(":/res/enemy3.png"),40,37,80,74,player,18,37,600,270,160,-36,0,2.2,0,-0.008);
            else //right
                new_enemy = new Enemy_Yellow(QString(":/res/enemy3.png"),40,37,80,74,player,18,37,600,270,Game::FrameWidth-160,-36,0,2.2,0,-0.008);
            newEnemyInit(new_enemy);
        } else if(tickCheck(5800)) { //5800
            new_boss = new Enemy_Pink(QString(":/res/enemy5.png"),40,37,100,92.5,player,90,46.25,500,450,Game::FrameWidth/2,-36,0,1,0,-0.004,false,true);
            newBossInit(new_boss);
            for(int i=0;i<2;++i) {
                new_enemy = new Enemy_Red(QString(":/res/enemy2.png"),40,37,60,55.5,player,5,27.75,400,300,(i==0)?40:Game::FrameWidth-40,-27,0,1.2,0,-0.005,false,true);
                newEnemyInit(new_enemy);
                new_enemy->setInvulnerable();
                new_effect = new_enemy->showShield();
                newEffectInit(new_effect);
                connect(new_boss,SIGNAL(deadSignal()),new_enemy,SLOT(killItself()));
            }
            tickFreeze();
        } else if(tickCheck(6200)) { //6200
            new_boss = new Enemy_Yellow_Tri(QString(":/res/enemy3.png"),40,37,100,92.5,player,60,46.25,325,400,Game::FrameWidth/2,-46,0,1,0,-0.004,false,true);
            newBossInit(new_boss);
            new_enemy = new Enemy_Pink(QString(":/res/enemy5.png"),40,37,60,55.5,player,5,27.75,300,400,Game::FrameWidth/2,-27,0,1.6,0,-0.004,false,true);
            newEnemyInit(new_enemy);
            new_enemy->setInvulnerable();
            new_effect = new_enemy->showShield();
            newEffectInit(new_effect);
            connect(new_boss,SIGNAL(deadSignal()),new_enemy,SLOT(killItself()));
            tickFreeze();
        } else if(tickCheck(6600)) { //6600
            for(int i=0;i<3;++i) {
                new_enemy = new Enemy_Pink(QString(":/res/enemy5.png"),40,37,80,74,player,(i==1)?60:40,37,250,(i==1)?200:325,100+(Game::FrameWidth/2-100)*i,-27,0,(i==1)?1:1.3,0,-0.004,false,true);
                newEnemyInit(new_enemy);
            }
            tickFreeze();
        } else if(tickCheck(7200)) { //7200
            new_boss = new Enemy_Red_Line(QString(":/res/enemy2.png"),40,37,100,92.5,player,60,46.25,10000,400,100,-46,0,1.2,0,-0.004,false,true);
            newBossInit(new_boss);
        } else if(tickCheck(7201,50,3)) { //7201+50i, 3 times
            for(int i=0;i<2;++i) {
                new_enemy = new Enemy(QString(":/res/enemy1.png"),40,37,80,74,player,3,37,0,0,((i==0)?1:3)*Game::FrameWidth/4,-36,1.2,2.2,0,0,true);
                newEnemyInit(new_enemy);
                new_enemy->setInvulnerable();
                new_effect = new_enemy->showShield();
                newEffectInit(new_effect);
                connect(new_boss,SIGNAL(deadSignal()),new_enemy,SLOT(killItself()));
            }
        } else if(tickCheck(7302)) { //7302
            tickFreeze();
        } else if(tickCheck(7405,361,3)) { //7405+361i, 3 times, warning bar fade in
            QGraphicsOpacityEffect *fadein = new QGraphicsOpacityEffect(this);
            ui->WarningBar->setGraphicsEffect(fadein);
            QPropertyAnimation *fadeinAni = new QPropertyAnimation(fadein,"opacity");
            fadeinAni->setDuration(1400);
            fadeinAni->setStartValue(0);
            fadeinAni->setEndValue(1);
            fadeinAni->setEasingCurve(QEasingCurve::InQuad);
            fadeinAni->start(QPropertyAnimation::DeleteWhenStopped);
            ui->WarningBar->show();
        } else if(tickCheck(7590,361,3)) { //7590+361i, 3 times, warning bar fade out
            QGraphicsOpacityEffect *fadeout = new QGraphicsOpacityEffect(this);
            ui->WarningBar->setGraphicsEffect(fadeout);
            QPropertyAnimation *fadeoutAni = new QPropertyAnimation(fadeout,"opacity");
            fadeoutAni->setDuration(1400);
            fadeoutAni->setStartValue(1);
            fadeoutAni->setEndValue(0);
            fadeoutAni->setEasingCurve(QEasingCurve::OutQuad);
            fadeoutAni->start(QPropertyAnimation::DeleteWhenStopped);
        } else if(tickCheck(8550)) { //8550, BOSS 1
            ui->BossLives->show();
            ui->BossHealth->setGeometry(100,40,690,30);
            new_boss = new Enemy_Blue_2(QString(":/res/enemy4.png"),40,47,120,140,player,260,60,350,400,Game::FrameWidth/2,-59,0,0,0,0,false,true);
            connect(new_boss,SIGNAL(deadSignal(int,int)),this,SLOT(bossCorpse(int,int)));
            new_boss->moveTo(Game::FrameWidth/2,200,330);
            newBossInit(new_boss);
            ui->BossLives->setText("5");
            tickFreeze();
        } else if(tickCheck(8552)) {
            ui->BossLives->setText("4");
        } else if(tickCheck(8800)) { //8800, BOSS 2
            new_boss = new Enemy_Blue_4(QString(":/res/enemy4.png"),40,47,120,140,player,260,60,350,400,Game::FrameWidth/2,200,0,0,0,0,false,true);
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
        } else if(tickCheck(8802)) {
            ui->BossLives->setText("3");
        } else if(tickCheck(9050)) { //9050, BOSS 3
            new_boss = new Enemy_Blue_5(QString(":/res/enemy4.png"),40,47,120,140,player,350,60,350,400,Game::FrameWidth/2,200,0,0,0,0,false,true);
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
        } else if(tickCheck(9052)) {
            ui->BossLives->setText("2");
        } else if(tickCheck(9300)) { //9300, BOSS 4
            new_boss = new Enemy_Blue_3(QString(":/res/enemy4.png"),40,47,120,140,player,320,60,350,400,Game::FrameWidth/2,200,0,0,0,0,false,true);
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
        } else if(tickCheck(9302)) {
            ui->BossLives->setText("1");
        } else if(tickCheck(9550)) { //9550, BOSS 5
            new_boss = new Enemy_Blue_1(QString(":/res/enemy4.png"),40,47,120,140,player,400,60,350,400,Game::FrameWidth/2,200,0,0,0,0,false,true);
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
        } else if(tickCheck(9552)) { //9552, END
            ui->BossLives->hide();
            ui->BossSkill->hide();
            if(player!=NULL) player->gameEndSetting();
        } else if(tickCheck(9800)) { //9800, WIN LIST
            ticking=false;
            //Point
            ui->PlayerPoint_list->display(ui->PlayerPoint->value());
            //Health
            ui->PlayerLife_list->display(ui->PlayerLife->value());
            ui->PlayerLife_listBonus->display(5);
            //Shield
            ui->PlayerSkill_list->display(ui->PlayerSkill->value());
            ui->PlayerSkill_listBonus->display(3);
            //Total
            ui->PlayerTotalPoint->display(ui->PlayerPoint->value()+ui->PlayerLife->value()*5+ui->PlayerSkill->value()*3);
            //List
            ui->EndList->setCurrentIndex(EndListPage::Won);
            ui->EndList->show();
            //game state
            gamestate=GameState::Won;
        }
        break;
    //level 2
    case 2:
        if(tickCheck(500,200,7)) { //500+175i, 7 times
            for(int i=0;i<2;++i) {
                new_enemy = new Enemy_2_Green(QString(":/res/enemy6.png"),35,35,70,70,player,3,35,300,200,(i==0)?-35:Game::FrameWidth+35,250,(i==0)?1.2:-1.2,-0.5,0,0.001);
                newEnemyInit(new_enemy);
            }
        } else if(tickCheck(2450)) {
            for(int i=0;i<2;++i) {
                new_enemy = new Enemy_2_Red(QString(":/res/enemy7.png"),35,35,90,90,player,15,45,200,200,(i==0)?-45:Game::FrameWidth+45,80,(i==0)?1.2:-1.2,0,(i==0)?-0.002:0.002,0,false,true);
                newEnemyInit(new_enemy);
            }
            tickFreeze();
        } else if(tickCheck(2625,400,5)) {
            for(int i=0;i<2;++i) {
                new_boss = new Enemy_2_Red(QString(":/res/enemy7.png"),35,35,70,70,player,5,35,220,150,(i==0)?-30:Game::FrameWidth+30,170,(i==0)?1.2:-1.2,0.5,(i==0)?-0.002:0.002,-0.0015);
                newEnemyInit(new_boss);
                new_enemy = new Enemy_2_Green(QString(":/res/enemy6.png"),35,35,60,60,player,5,30,345,150,(i==0)?-30:Game::FrameWidth+30,110,(i==0)?1.2:-1.2,0.5,(i==0)?-0.002:0.002,-0.0015);
                newEnemyInit(new_enemy);
                new_enemy->setInvulnerable();
                connect(new_boss,SIGNAL(deadSignal()),new_enemy,SLOT(killItself()));
            }
        } else if(tickCheck(5200)) {
            new_enemy = new Enemy_2_Yellow(QString(":/res/enemy8.png"),35,35,90,90,player,45,45,150,150,Game::FrameWidth/2,-45,0,3,0,-0.015,false,true);
            newEnemyInit(new_enemy);
            tickFreeze();
        }
        break;
    }
    //draw
    emit doImgMove();
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
    new_effect = new Effect(QString(":/res/lightball.png"),120,120,300,300,308,x,y,0,0,0,0,true);
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
    QString str("color:");
    ui->PlayerLife->setStyleSheet(str.append(color).append(";"));
    ui->PlayerLifeLabel->setStyleSheet(str.append(color).append(";"));
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
            Player::speed=2;
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
            else secret=0;
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
