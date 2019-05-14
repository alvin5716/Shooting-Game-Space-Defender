#include "player.h"
#include "bullet.h"
#include <QDebug>

double Player::speed=7;

Player::Player(int health, int radius, int shoot_cd, double x, double y, double xv, double yv, double xa, double ya)
    :Character(QString(":/res/player.png"),42,32,50,40,health,radius,x,y,xv,yv,xa,ya)
{
    this->shoot_cd=shoot_cd;
    shooting=false;
    invulnerable_timer=nullptr;
    shoot_timer=0;
    skill_cd=skill_timer=187;
    vulnerable_func_count=0;
    this->setZValue(Game::ZValueBehindDefault);
}
bool Player::isMaxHealth() const {
    return this->health==5;
}
void Player::move() {
    //move object
    Character::move();
    //don't go out of frame
    if(x<0+radius+5) setPosition(0+radius+5,y);
    else if(x>Game::FrameWidth-radius-5) setPosition(Game::FrameWidth-radius-5,y);
    if(y<0+radius+5) setPosition(x,0+radius+5);
    else if(y>Game::FrameHeight-radius-5) setPosition(x,Game::FrameHeight-radius-5);
    //attack cool down
    if(shoot_timer<shoot_cd) ++shoot_timer;
    //skill cool down
    if(skill_timer<skill_cd) ++skill_timer;
    if(invulnerable_timer==nullptr) invulnerable = (skill_timer<skill_cd)?true:false;
}
Character* Player::testAttackedBy(std::vector<Character*> & attackers) {
    if(invulnerable) return nullptr;
    for(int i=0;i<(int)attackers.size();++i) {
        if(attackers.at(i)->isAttackable() && (sqrt(pow(attackers.at(i)->getX() - x,2)+pow(attackers.at(i)->getY() - y,2)) <= attackers.at(i)->getRadius() + radius)) {
            attacked();
            return attackers.at(i);
        }
    }
    return nullptr;
}
void Player::attacked() {
    if(health>0 && !invulnerable) {
        health-=1;
        setInvulnerable();
        emit soundPlay(Game::SoundBd);
        emit healthChanged(health);
    }
    if(health<=0) {
        dead=true;
        emit deadSignal();
    }
}
void Player::setInvulnerable() {
    invulnerable=true;
    if(invulnerable_timer!=nullptr) {
        delete invulnerable_timer;
        invulnerable_timer=nullptr;
    }
    invulnerable_timer = new QTimer;
    invulnerable_timer->start(400);
    connect(invulnerable_timer,SIGNAL(timeout()),this,SLOT(setVulnerable()));
    setVulnerable();
}
void Player::setVulnerable() {
    unsigned int& i = vulnerable_func_count;
    if(i%2==0) {
        setOpacity(0.5);
        if(health<=5) emit healthColorChange("red");
        else healthColorChange("yellow");
    } else {
        setOpacity(1);
        if(health<=5) emit healthColorChange("white");
        else healthColorChange("yellow");
    }
    if(i==7 && invulnerable_timer!=nullptr) {
        invulnerable=false;
        delete invulnerable_timer;
        invulnerable_timer=nullptr;
    }
    if(++i>7) i=0;
}
void Player::gameEndSetting() {
    invulnerable=true;
    if(invulnerable_timer!=nullptr) {
        delete invulnerable_timer;
        invulnerable_timer=nullptr;
    }
}
Bullet *Player::shoot() {
    if(shoot_timer>=shoot_cd) {
        shoot_timer = 0;
        Bullet *new_bullet = new Bullet(QString(":/res/bullet/1/white.png"),3,x,y-show_h/2,0,-14);
        new_bullet->setZValue(Game::ZValueTeammate);
        return new_bullet;
    }
    return nullptr;
}
bool Player::isShooting() const {
    return shooting;
}
void Player::setShooting(bool shooting) {
    this->shooting=shooting;
}
bool Player::isUsingSkill() const{
    return (skill_timer<skill_cd)?true:false;
}
void Player::setHealthTo999() {
    health=999;
    emit healthChanged(999);
    emit healthColorChange("yellow");
}
void Player::coolDown() {
    skill_timer=0;
}
