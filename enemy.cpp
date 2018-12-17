#include "enemy.h"
#include <QString>
#include "bullet.h"
#include <QDebug>
#include "game.h"

Enemy::Enemy(QString img, int img_w, int img_h, int show_w, int show_h, Character* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x, double y, double xv, double yv, double xa, double ya, bool bounceable, bool stopable)
    :Character(img,img_w,img_h,show_w,show_h,health,radius,x,y,xv,yv,xa,ya),
      death_img_w(img_w), death_img_h(img_h)
{
    disappearTime=200;
    point=2;
    skill_timer=0;
    shoot_timer=shoot_cd-shoot_cd_init;
    this->bounceable=bounceable;
    this->stopable=stopable;
    this->player=player;
    this->shoot_cd=shoot_cd;
    already_enter=false;
    boss=false;
    secPhase=false;
    shield=nullptr;
    connect(this,SIGNAL(deadSignal()),this,SLOT(diedFromPlayer()));
    floatable=true;
    bossSkillHP=0;
    death_img="";
}
void Enemy::diedFromPlayer() {
    emit pointGive((int)point);
    emit soundPlay(Game::SoundCymbal);
}
void Enemy::outOfFrame() {
    if(x<0-radius || x>Game::FrameWidth+radius || y<0-radius || y>Game::FrameHeight+radius) {
        dead=true;
    }
}
void Enemy::noPoint() {
    this->point=0;
}
std::vector<Bullet*>* Enemy::shoot() {
    if(shoot_timer>=shoot_cd) shoot_timer = 0;
    return nullptr;
}
std::vector<Bullet*>* Enemy::shoot2() {
    if(shoot_timer>=shoot_cd) shoot_timer = 0;
    return nullptr;
}
Shield* Enemy::showShield(QString str,int img_w, int img_h) {
    shield = new Shield(str,img_w,img_h,radius*3.2,radius*3.2,this,-1,x,y);
    if(floatable) {
        shield->setFloatable();
        shield->float_timer = this->float_timer;
    }
    return shield;
}
void Enemy::setFloatable(bool floatable) {
    Character::setFloatable(floatable);
    if(this->shield!=nullptr) this->shield->setFloatable(floatable);
}
void Enemy::move() {
    //move object
    Character::move();
    //attack cool down
    ++shoot_timer;
    //bounceable
    if(bounceable) {
        if(!already_enter && (x>0+radius && x<Game::FrameWidth-radius && y>0+radius && y<Game::FrameHeight-radius)) already_enter=true;
        if(already_enter) {
            if((x<0+radius && xv<0) || (x>Game::FrameWidth-radius && xv>0)) bounceAtX();
            if((y<0+radius && yv<0) || (y>Game::FrameHeight-radius && yv>0)) bounceAtY();
        }
    }
    //stopable
    if(stopable) {
        if(xa*xv>0) {
            xv=0;
            xa=0;
        }
        if(ya*yv>0) {
            yv=0;
            ya=0;
        }
    }
}
void Enemy::beABoss(int bossSkillHP) {
    this->bossSkillHP=bossSkillHP;
    this->boss=true;
}
void Enemy::setBossHPToSkill() {
    if(!this->isBoss()) {
        qDebug() << "only boss enemy can use this function: setBossHPToSkill";
        return;
    }
    this->health=this->bossSkillHP;
}
int Enemy::getBossSkillHP() {
    if(!this->isBoss()) {
        qDebug() << "only boss enemy can use this function: setBossHPToSkill";
        return 0;
    }
    return this->bossSkillHP;
}
void Enemy::testIfSecPhase(skillFunc initialize, skillFunc secPhaseSkill) {
    if(!this->isBoss()) {
        qDebug() << "only boss enemy can use this function: setBossHPToSkill";
        return;
    }
    if(health<=bossSkillHP && !secPhase) {
        secPhase = true;
        initialize();
    }
    if(secPhase) secPhaseSkill();
}
void Enemy::testIfSecPhase(skillFunc initialize, skillFunc secPhaseSkill, skillFunc FirPhaseSkill) {
    if(!this->isBoss()) {
        qDebug() << "only boss enemy can use this function: setBossHPToSkill";
        return;
    }
    if(health<=bossSkillHP && !secPhase) {
        secPhase = true;
        initialize();
    }
    if(secPhase) secPhaseSkill();
    else FirPhaseSkill();
}
void Enemy::setDisappearTime(int disappearTime) {
    this->disappearTime=disappearTime;
}
void Enemy::attacked() {
    Character::attacked();
    if(!invulnerable) emit soundPlay(Game::SoundHit);
}
void Enemy::skill() {return;}
bool Enemy::isBoss() const{
    return boss;
}
bool Enemy::isSecPhase() const{
    return secPhase;
}
void Enemy::setDeathImg(QString death_img) {
    this->death_img = death_img;
}
void Enemy::setDeathImg(QString death_img, int death_img_w, int death_img_h) {
    this->death_img = death_img;
    this->death_img_w = death_img_w;
    this->death_img_h = death_img_h;
}
Effect* Enemy::disappear() const {
    Effect* corpse = new Effect((death_img=="")?img:death_img,
                                death_img_w,death_img_h,
                                show_w * death_img_w / img_w, show_h * death_img_h / img_h,
                                disappearTime/8,imgX()+show_w/2,imgY()+show_h/2,(death_img=="")?xv:0,(death_img=="")?yv:0,(death_img=="")?xa:0,(death_img=="")?ya:0);
    corpse->setZValue(-1);
    //if needed, face to left
    if(canBeMirrored&&face_to_left) {
        corpse->setCanBeMirrored();
        corpse->setFaceToLeft();
    }
    //if needed, float
    if(floatable) {
        corpse->setFloatable(true);
        corpse->float_timer = this->float_timer;
    }
    //disappear time
    corpse->fadeout(disappearTime);
    return corpse;
}
Enemy::~Enemy() {
    if(shield!=nullptr) delete shield;
}
