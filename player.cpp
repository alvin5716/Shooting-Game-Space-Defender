#include "player.h"
#include "bullet.h"
#include <QDebug>
#include "game.h"
double Player::speed=3.5;
Player::Player(int health, int radius, int shoot_cd, double x, double y, double xv, double yv, double xa, double ya)
    :Character(QString(":/res/player.png"),42,32,50,40,health,radius,x,y,xv,yv,xa,ya)
{
    this->shoot_cd=shoot_cd;
    shooting=false;
    invulnerable_timer=NULL;
    shoot_timer=0;
    skill_cd=skill_timer=375;
}
void Player::move() {
    //move object
    setPosition(x+xv,y+yv);
    setSpeed(xv+xa,yv+ya);
    //don't go out of frame
    if(x<0+radius) setPosition(0+radius,y);
    else if(x>Game::FrameWidth-radius) setPosition(Game::FrameWidth-radius,y);
    if(y<0+radius) setPosition(x,0+radius);
    else if(y>Game::FrameHeight-radius) setPosition(x,Game::FrameHeight-radius);
    //attack cool down
    if(shoot_timer<shoot_cd) ++shoot_timer;
    //skill cool down
    if(skill_timer<skill_cd) ++skill_timer;
    if(invulnerable_timer==NULL) invulnerable = (skill_timer<skill_cd)?true:false;
}
Character* Player::testAttackedBy(std::vector<Character*> & attackers) {
    for(int i=0;i<(int)attackers.size();++i) {
        if(sqrt(pow(attackers.at(i)->getX() - x,2)+pow(attackers.at(i)->getY() - y,2)) <= attackers.at(i)->getRadius() + radius) {
            if(health>0 && !invulnerable) {
                health-=1;
                setInvulnerable();
                emit healthChanged(health);
            }
            if(health<=0) {
                dead=true;
                emit deadSignal();
            }
            return attackers.at(i);
        }
    }
    return NULL;
}
void Player::attacked() {
    if(health>0 && !invulnerable) {
        health-=1;
        setInvulnerable();
        emit healthChanged(health);
    }
    if(health<=0) {
        dead=true;
        emit deadSignal();
    }
}
void Player::setInvulnerable() {
    invulnerable=true;
    if(invulnerable_timer!=NULL) {
        delete invulnerable_timer;
        invulnerable_timer=NULL;
    }
    invulnerable_timer = new QTimer;
    invulnerable_timer->start(400);
    connect(invulnerable_timer,SIGNAL(timeout()),this,SLOT(setVulnerable()));
    setVulnerable();
}
void Player::setVulnerable() {
    static unsigned int i=0;
    if(i%2==0) {
        setOpacity(0.5);
        if(health<=10) emit healthColorChange("red");
        else healthColorChange("yellow");
    } else {
        setOpacity(1);
        if(health<=10) emit healthColorChange("white");
        else healthColorChange("yellow");
    }
    if(i==7 && invulnerable_timer!=NULL) {
        invulnerable=false;
        delete invulnerable_timer;
        invulnerable_timer=NULL;
    }
    if(++i>7) i=0;
}
void Player::gameEndSetting() {
    invulnerable=true;
    if(invulnerable_timer!=NULL) {
        delete invulnerable_timer;
        invulnerable_timer=NULL;
    }
}
Bullet *Player::shoot() {
    if(shoot_timer>=shoot_cd) {
        shoot_timer = 0;
        Bullet *new_bullet = new Bullet(QString(":/res/bullet_white.png"),3,x,y-show_h/2,0,-6);
        return new_bullet;
    }
    return NULL;
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
