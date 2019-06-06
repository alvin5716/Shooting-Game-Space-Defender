#include "gamerunner.h"
#include "mainwindow.h"

GameRunner::GameRunner(MainWindow* mainwindow, QObject *parent)
    :QObject(parent), endListShowPulse(false), player(mainwindow->player),
    player_bullets(mainwindow->player_bullets),
    enemies(mainwindow->enemies),
    enemy_bullets(mainwindow->enemy_bullets),
    gamestate(mainwindow->gamestate),
    level(mainwindow->level),
    dialogueProcessing(mainwindow->dialogueProcessing)
{

}

void GameRunner::doTick() {
    if((gamestate==Game::GameStatePlaying && !dialogueProcessing)||gamestate==Game::GameStateWon) {
        //player speed
        if(playerLeft() && playerUp()) player->setSpeed(-Player::speed/sqrt(2),-Player::speed/sqrt(2));
        else if(playerLeft() && playerDown()) player->setSpeed(-Player::speed/sqrt(2),Player::speed/sqrt(2));
        else if(playerRight() && playerUp()) player->setSpeed(Player::speed/sqrt(2),-Player::speed/sqrt(2));
        else if(playerRight() && playerDown()) player->setSpeed(Player::speed/sqrt(2),Player::speed/sqrt(2));
        else if(playerLeft()) player->setSpeed(-Player::speed,0);
        else if(playerRight()) player->setSpeed(Player::speed,0);
        else if(playerUp()) player->setSpeed(0,-Player::speed);
        else if(playerDown()) player->setSpeed(0,Player::speed);
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
            soundPlay(Game::SoundMagicShield);
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
        //teammate nearestEnemyPos
        if(teammate!=nullptr) {
            double min_dis = DBL_MAX;
            std::vector<Enemy*>::iterator i;
            for (std::vector<Enemy*>::iterator j=enemies.begin();j!=enemies.end();++j) {
                if((!(*j)->isInvulnerable() || (*j)->isBoss()) && min_dis > teammate->distanceTo(*j)) {
                    min_dis = teammate->distanceTo(*j);
                    i = j;
                }
            }
            if(min_dis==DBL_MAX) teammate->sendNearestEnemyPos(false);
            else teammate->sendNearestEnemyPos(true,QPointF((*i)->getX(),(*i)->getY()),QPointF((*i)->getXV(),(*i)->getYV()));
            if(teammate->isShooting()) { //teammate shoot
                //shoot
                std::vector<Bullet*>* new_mate_bullets;
                new_mate_bullets = teammate->shoot();
                if(new_mate_bullets!=nullptr) {
                    for(std::vector<Bullet*>::iterator j=(*new_mate_bullets).begin();j!=(*new_mate_bullets).end();++j) {
                        scene->addItem(*j);
                        connect(this,SIGNAL(doMove()),*j,SLOT(move()));
                        connect(this,SIGNAL(doImgMove()),*j,SLOT(img_move()));
                        player_bullets.push_back(*j);
                    }
                }
            }
        }
        //damaged
        std::vector<Character*> attackers(enemies.begin(),enemies.end());
        Character* real_attacker = player->testAttackedBy(attackers);
        if(real_attacker!=nullptr) {
            ui->PlayerLife->display(player->getHealth());
            sceneVibrate(static_cast<short>(Enemy::shakeLevel::mediumShake),false);
            redFlash();
        }
        for(Enemy* enemy: enemies) { //enemies
            if(enemy->isDead()) continue;
            //shoot
            std::vector<Bullet*>* new_enemy_bullets;
            if(!(enemy->isBoss() && enemy->isSecPhase())) new_enemy_bullets = enemy->shoot();
            else new_enemy_bullets = enemy->shoot2();
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
            enemy->skill();
            //out of frame
            enemy->deleteIfOutOfFrame();
        }
        { //player bullets
            std::vector<Character*> attackers(enemies.begin(),enemies.end());
            for(Bullet* player_bullet: player_bullets) {
                player_bullet->deleteIfOutOfFrame(); //out
                //damaged
                Character* real_attacker = player_bullet->testAttackedBy(attackers);
                if(real_attacker!=nullptr) real_attacker->attacked();
            }
        }
        {//enemy bullets
            Character* attacker = player;
            for(Bullet* enemy_bullet: enemy_bullets) {
                enemy_bullet->deleteIfOutOfFrame(); //out
                //damaged
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
                dialogueStart({Dialogue("這裡是塔台，防衛者號，聽到請回答。"),
                               Dialogue("是的，長官。",":/res/player.png",QRect(0,0,43,33)),
                               Dialogue("再複習一次你的任務！"),
                               Dialogue("收到。本次任務是要到各個星球探查「智多星」的座標位置，並與他們的首領交流。",":/res/player.png",QRect(0,0,43,33)),
                               Dialogue("很好，在那之後，向我們回報你們的對話，然後我們會給你進一步的指示。"),
                               Dialogue("還有記得，你的駕駛座旁邊有個翻譯裝置，可以用來跟那些外星生物溝通"),
                               Dialogue("但前提是它們本身也要有一定的智力才行，不然就算翻譯出來也只會是一堆雜音而已"),
                               Dialogue("所以你得在這裡先找到能講話的生物談話。"),
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
                this->bossHPShortened = true;
                new_boss = new Enemy_Blue_2(player,240,60,175,200,Game::FrameWidth/2,-59,0,0,0,0,false,true);
                connect(new_boss,SIGNAL(deadSignal(int,int)),this,SLOT(bossCorpse(int,int)));
                connect(new_boss,&Enemy::dialogueStart,[this](){
                    dialogueStart({Dialogue("你是哪個星球來的人啦？",":/res/enemy/1/blue.png",QRect(5,5,35,35)),
                                   Dialogue("(終於出現一個能講話的了...)",":/res/player.png",QRect(0,0,43,33)),
                                   Dialogue("隨便跑來別人家裡，還把人家都打暈了是想怎樣啦",":/res/enemy/1/blue.png",QRect(5,5,35,35)),
                                   Dialogue("不是，我只是想跟你們的頭目談個話，可是其他人都無法溝通，而且都把我當成入侵者了...",":/res/player.png",QRect(0,0,43,33)),
                                   Dialogue("你是入侵者沒有錯啊，而且我不跟入侵者談話啦",":/res/enemy/1/blue.png",QRect(5,5,35,35)),
                                   Dialogue("我要直接把你打下來啦！",":/res/enemy/1/blue.png",QRect(5,5,35,35)),
                                   Dialogue("原來你就是他們的頭目啊！(其實好像也蠻明顯的就是了...)",":/res/player.png",QRect(0,0,43,33))
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
                skillName->hide();
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
                this->bossHPShortened = true;
                new_boss = new Enemy_2_Blue_1(player,340,60,50,200,Game::FrameWidth/2,-59,0,0,0,0,false,true);
                connect(new_boss,SIGNAL(deadSignal(int,int)),this,SLOT(bossCorpse(int,int)));
                connect(new_boss,&Enemy::dialogueStart,[this](){
                    dialogueStart({Dialogue("外星人，走開！",":/res/enemy/2/blue.png",QRect(5,2,25,25)),
                                   Dialogue("抱歉就這樣隨便闖進來了，但我只是想問你們...",":/res/player.png",QRect(0,0,43,33)),
                                   Dialogue("外外！討厭！",":/res/enemy/2/blue.png",QRect(5,2,25,25)),
                                   Dialogue("(...看來是個笨蛋)",":/res/player.png",QRect(0,0,43,33)),
                                   Dialogue("去去去！去去去！",":/res/enemy/2/blue.png",QRect(5,2,25,25)),
                                   Dialogue("好好好，我走就是了",":/res/player.png",QRect(0,0,43,33)),
                                   Dialogue("不要走！去死！",":/res/enemy/2/blue.png",QRect(5,2,25,25)),
                                   Dialogue("??????",":/res/player.png",QRect(0,0,43,33))
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
                skillName->hide();
                if(player!=nullptr) player->gameEndSetting();
            } else if(tickCheck(7163)) {
                dialogueStart({Dialogue("啊呀啊嘎嗚...",":/res/enemy/2/blue_3.png",QRect(5,2,25,25)),
                               Dialogue("(果然問不到東西，反正繼續往這個方向飛看看好了)",":/res/player.png",QRect(0,0,43,33))
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
                dialogueStart({Dialogue("剛剛飛到一半，突然被像傳送門一樣的東西吸到了這個地方。",":/res/player.png",QRect(0,0,43,33)),
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
                new_enemy = new Enemy_3_Yellow(player,45,60,125,60,Game::FrameWidth/2,250);
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
                new_enemy = new Enemy_3_Pink(player,45,60,60,50,Game::FrameWidth/2,250);
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
                this->bossHPShortened = true;
                new_boss = new Enemy_3_Blue_1(player,250,80,18,200,Game::FrameWidth/2,200,0,0,0,0,0,true);
                connect(new_boss,SIGNAL(deadSignal(int,int)),this,SLOT(bossCorpse(int,int)));
                connect(new_boss,&Enemy::dialogueStart,[this](){
                    dialogueStart({Dialogue("哈利，這是我們宿命的對決。",":/res/enemy/3/blue.png",QRect(4,3,39,39)),
                                   Dialogue("蛤？誰是哈利？",":/res/player.png",QRect(0,0,43,33)),
                                   Dialogue("不用裝傻了，你就是命中註定，我最大的敵手",":/res/enemy/3/blue.png",QRect(4,3,39,39)),
                                   Dialogue("(為什麼又是一個笨蛋啊？)",":/res/player.png",QRect(0,0,43,33)),
                                   Dialogue("根據我的占卜，在今天的這個時刻，會有一個外星生物進入我們的維度。",":/res/enemy/3/blue.png",QRect(4,3,39,39)),
                                   Dialogue("只要打敗你，我們就能取得你們的「科技」",":/res/enemy/3/blue.png",QRect(4,3,39,39)),
                                   Dialogue("再加上我們的「魔法」，我們就能到你們的維度裡稱霸！",":/res/enemy/3/blue.png",QRect(4,3,39,39)),
                                   Dialogue("聽起來好像不是笨蛋耶，而且感覺不太妙...",":/res/player.png",QRect(0,0,43,33)),
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
                skillName->hide();
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
                    new_enemy = new Enemy_4_Green(player,5,40,40,63,i?-40:Game::FrameWidth+40,-40,i?3.4:-3.4,3.4,i?-0.028:0.028,-0.028,false);
                    newEnemyInit(new_enemy);
                }
            } else if(tickCheck(310)) {
                dialogueStart({Dialogue("慢著，你是來見我們首領的地球人嗎？",":/res/enemy/4/green.png",QRect(60,25,88,88)),
                               Dialogue("哇，雜魚說話了！",":/res/player.png",QRect(0,0,43,33)),
                               Dialogue("你們星球的人真沒禮貌...",":/res/enemy/4/green.png",QRect(60,25,88,88)),
                               Dialogue("對...對不起，可以帶我去見他嗎？",":/res/player.png",QRect(0,0,43,33)),
                               Dialogue("沒那麼簡單，首領說要我們先測試你一下",":/res/enemy/4/green.png",QRect(60,25,88,88)),
                               Dialogue("不是你們自己要求我們來的嗎！？",":/res/player.png",QRect(0,0,43,33))
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
                new_enemy = new Enemy_4_Green(player,7,40,40,63,(t%2==0)?-40:Game::FrameWidth+40,-40,(t%2==0)?3.4:-3.4,3.4,(t%2==0)?-0.028:0.028,-0.028,false);
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
                static int x = qrand()%(Game::FrameWidth-400)+200;
                x += qrand()%201-100;
                if(x>Game::FrameWidth-200) x = 2*(Game::FrameWidth-200)-x;
                else if(x<200) x = 400-x;
                new_enemy = new Enemy_4_Red(player,t>8?7:4,40,75,200,x,-40,0,2.4,0,-0.0018,true);
                newEnemyInit(new_enemy);
                if(t==17) tickFreeze();
            } else if(tickCheck(5750,240,7)) {
                const int t = timesCount(5750,240);
                new_enemy = new Enemy_4_Pink(player,t>8?7:4,40,5,5,t%2?70:Game::FrameWidth-70,-40,t%2?0.2:-0.2,1.8,t%2?-0.00004:0.00004,0.0018);
                newEnemyInit(new_enemy);
            } else if(tickCheck(7252,181,3)) { //12403+361i, 3 times, warning bar fade in
                this->warningFadeIn();
            } else if(tickCheck(7344,181,3)) { //12588+361i, 3 times, warning bar fade out
                this->warningFadeOut();
            } else if(tickCheck(7824)) { //14048, BOSS 1
                ui->BossLives->show();
                this->bossHPShortened = true;
                new_boss = new Enemy_4_Blue_1(player,270,60,36,200,Game::FrameWidth/2,-60,0,0,0,0,false,true);
                connect(new_boss,SIGNAL(deadSignal(int,int)),this,SLOT(bossCorpse(int,int)));
                connect(new_boss,&Enemy::dialogueStart,[this](){
                    dialogueStart({Dialogue("為什麼每個星球的首領都只是其他隻的放大版而已...",":/res/player.png",QRect(0,0,43,33)),
                                   Dialogue("我所嚮往的是如我們的母星一般，外表黯淡，但內心的光芒隱隱透出。大智若愚，曖曖含光。",":/res/enemy/4/blue.png",QRect(60,25,88,88)),
                                   Dialogue("......",":/res/player.png",QRect(0,0,43,33)),
                                   Dialogue("來談正事，你會來這裡是因為我向你們星球傳了無線電訊號，我需要有人幫忙。",":/res/enemy/4/blue.png",QRect(60,25,88,88)),
                                   Dialogue("幫忙？",":/res/player.png",QRect(0,0,43,33)),
                                   Dialogue("其實說起來有點不好意思...",":/res/enemy/4/blue.png",QRect(60,25,88,88)),
                                   Dialogue("我們之前開發的機械智慧因為不明原因開始暴走了。沒弄好的話，這幾十光年內的星球可能都有危險。",":/res/enemy/4/blue.png",QRect(60,25,88,88)),
                                   Dialogue("幾十光年的話不是也包括我們嗎？那麼那東西在哪裡？",":/res/player.png",QRect(0,0,43,33)),
                                   Dialogue("這個我們留到我打累了以後再說吧。",":/res/enemy/4/blue.png",QRect(60,25,88,88)),
                                   Dialogue("等...等一下，又要打？",":/res/player.png",QRect(0,0,43,33)),
                                   Dialogue("當然啊，如果你沒有一定的能力，帶你去只會扯後腿而已。",":/res/enemy/4/blue.png",QRect(60,25,88,88)),
                                   Dialogue("這問題不是你們搞出來的嗎！",":/res/player.png",QRect(0,0,43,33))
                                  });
                });
                new_boss->moveTo(Game::FrameWidth/2,200,165);
                newBossInit(new_boss);
                ui->BossLives->setText("5");
                tickFreeze();
            } else if(tickCheck(7826)) { //14050
                ui->BossLives->setText("4");
            } else if(tickCheck(7949)) { //14298, BOSS 2
                new_boss = new Enemy_4_Blue_2(player,250,60,41,200,Game::FrameWidth/2,200,0,0,0,0,false,true);
                connect(new_boss,SIGNAL(deadSignal(int,int)),this,SLOT(bossCorpse(int,int)));
                new_boss->fadein(1500);
                newBossInit(new_boss);
                tickFreeze();
            } else if(tickCheck(7951)) { //14300
                ui->BossLives->setText("3");
            } else if(tickCheck(8074)) { //14548, BOSS 3
                new_boss = new Enemy_4_Blue_3(player,330,60,43,200,Game::FrameWidth/2,200,0,0,0,0,false,true);
                connect(new_boss,SIGNAL(deadSignal(int,int)),this,SLOT(bossCorpse(int,int)));
                new_boss->fadein(1500);
                newBossInit(new_boss);
                tickFreeze();
            } else if(tickCheck(8076)) { //14550
                ui->BossLives->setText("2");
            } else if(tickCheck(8199)) { //14798, BOSS 4
                new_boss = new Enemy_4_Blue_4(player,450,60,50,200,Game::FrameWidth/2,200,0,0,0,0,false,true);
                connect(new_boss,SIGNAL(deadSignal(int,int)),this,SLOT(bossCorpse(int,int)));
                new_boss->fadein(1500);
                newBossInit(new_boss);
                tickFreeze();
            } else if(tickCheck(8201)) { //14800
                ui->BossLives->setText("1");
            } else if(tickCheck(8324)) { //8324, BOSS 5
                new_boss = new Enemy_4_Blue_5(player,300,60,45,200,Game::FrameWidth/2,200,0,0,0,0,false,true);
                new_boss->fadein(1500);
                newBossInit(new_boss);
                tickFreeze();
            } else if(tickCheck(8326)) { //8326
                ui->BossLives->hide();
                skillName->hide();
                this->bossHPShortened = false;
                if(player!=nullptr) player->gameEndSetting();
            } else if(tickCheck(8590) && !player->isMaxHealth()) {
                dialogueStart({Dialogue("嗚，雖然有放一點水，不過好久沒動了，好累。",":/res/enemy/4/blue_3.png",QRect(50,0,90,90)),
                               Dialogue("你還真行耶，看來我找對人了。",":/res/enemy/4/blue_3.png",QRect(50,0,90,90)),
                               Dialogue("這是當然的了！所以那個機械智慧在哪裡？到底發生了什麼事？",":/res/player.png",QRect(0,0,43,33)),
                               Dialogue("我們之前嘗試開發機械智慧，但是最後失敗了。",":/res/enemy/4/blue_3.png",QRect(50,0,90,90)),
                               Dialogue("最後我們把失敗品丟到了另一顆無生命的星球上。但最近呢，有很多鄰近的星球卻說他們受到了這些機械的攻擊。",":/res/enemy/4/blue_3.png",QRect(50,0,90,90)),
                               Dialogue("我不清楚它們是怎麼重獲新生的，別說是自主行動了，他們理論上根本已經沒有能源能執行任何動作了。",":/res/enemy/4/blue_3.png",QRect(50,0,90,90)),
                               Dialogue("一開始我也不相信，但受害星球的描述真的都與它們相似。",":/res/enemy/4/blue_3.png",QRect(50,0,90,90)),
                               Dialogue("總之我建議先到他們的據點星球旁的那顆星球，迷霧星。",":/res/enemy/4/blue_3.png",QRect(50,0,90,90)),
                               Dialogue("那裡的大氣很糟，我們可以躲在那裡觀察他們而不容易被發現。",":/res/enemy/4/blue_3.png",QRect(50,0,90,90))
                               });
            } else if(tickCheck(8590) && player->isMaxHealth()) {
                dialogueStart({Dialogue("你比我想像的還要強好多喔...竟然完全沒被打到過...",":/res/enemy/4/blue.png",QRect(60,25,88,88)),
                               Dialogue("不認真打一下好像太失禮了，對吧？",":/res/enemy/4/blue.png",QRect(60,25,88,88))
                               });
                tick = 8645;
            } else if(tickCheck(8640) || tickCheck(8963)) { //8640, WIN LIST
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
            } else if(tickCheck(8650)) { //8600, BOSS 6
                QPointF pos = Enemy_4_Blue_5::getCorpsePos();
                emit killEffectsWithoutDot();
                new_boss = new Enemy_4_Blue_6(player,300,60,55,200,pos.x(),pos.y(),0,0,0,0,false,true);
                newBossInit(new_boss);
                tickFreeze();
            } else if(tickCheck(8913)) {
                dialogueStart({Dialogue("嗚，真好玩，不過好久沒動了，好累。",":/res/enemy/4/blue_3.png",QRect(50,0,90,90)),
                               Dialogue("你還真行耶，看來我找對人了。",":/res/enemy/4/blue_3.png",QRect(50,0,90,90)),
                               Dialogue("這是當然的了！所以那個機械智慧在哪裡？到底發生了什麼事？",":/res/player.png",QRect(0,0,43,33)),
                               Dialogue("我們之前嘗試開發機械智慧，但是最後失敗了。",":/res/enemy/4/blue_3.png",QRect(50,0,90,90)),
                               Dialogue("最後我們把失敗品丟到了另一顆無生命的星球上。但最近呢，有很多鄰近的星球卻說他們受到了這些機械的攻擊。",":/res/enemy/4/blue_3.png",QRect(50,0,90,90)),
                               Dialogue("我不清楚它們是怎麼重新啟動的，別說是自主行動了，他們理論上根本已經沒有能源能執行任何動作了。",":/res/enemy/4/blue_3.png",QRect(50,0,90,90)),
                               Dialogue("一開始我也不相信，但大家的描述真的都與它們相似。",":/res/enemy/4/blue_3.png",QRect(50,0,90,90)),
                               Dialogue("總之我建議先到他們的據點星球旁的那顆星球，迷霧星。",":/res/enemy/4/blue_3.png",QRect(50,0,90,90)),
                               Dialogue("那裡的大氣很糟，我們可以躲在那裡觀察他們而不容易被發現。",":/res/enemy/4/blue_3.png",QRect(50,0,90,90))
                               });
            }
            break;
        //level 5
        case 5:
            if(tickCheck(220)) {
                dialogueStart({Dialogue("呼叫塔台，這裡是防衛者號，聽到請回答。",":/res/player.png",QRect(0,0,43,33)),
                               Dialogue("收到，你找到智多星的首領了嗎？"),
                               Dialogue("是的，我們現在正要前往機械智慧所在地旁的星球，迷霧星。",":/res/player.png",QRect(0,0,43,33)),
                               Dialogue("我們會在那裡靜觀其變、研擬對策。",":/res/player.png",QRect(0,0,43,33)),
                               Dialogue("迷霧星的引力場詭譎難解，大氣活動氣象萬千，星球表面岈然洼然，若垤若穴。",":/res/enemy/4/blue.png",QRect(60,25,88,88)),
                               Dialogue("......",":/res/player.png",QRect(0,0,43,33)),
                               Dialogue("簡單來講就是內營力和外營力都非常劇烈的星球。",":/res/enemy/4/blue.png",QRect(60,25,88,88)),
                               Dialogue("所以沒有複雜生物存在，頂多有微生物吧，雖然危險但是個很好的藏身之處。",":/res/enemy/4/blue.png",QRect(60,25,88,88)),
                               Dialogue("啊，剛說完就有小型隕石群來了，自己小心",":/res/enemy/4/blue.png",QRect(60,25,88,88))
                              });
            } else if(tickCheck(300)) { //300
                new_enemy = new Environment_1(player,28,800);
                newEnemyInit(new_enemy);
            } else if(tickCheck(1500)) { //1500
                new_enemy = new Environment_2(player,65,800);
                newEnemyInit(new_enemy);
            }
            break;
        default:
            qDebug() << "error: can't get what level is selected";
        }
    }
    //player death
    if(player->isDead() && gamestate!=Game::GameStateFailed) {
        //game end
        player->gameEndSetting();
        player->setOpacity(0.5);
        this->endListShowPulse = true;
        //game state
        gamestate=Game::GameStateFailed;
    }
    //ticking
    if(gamestate==Game::GameStatePlaying && ticking && !dialogueProcessing) ++tick;
    if(gamestate==Game::GameStatePlaying) ++gametime;
}
