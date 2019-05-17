#include "laser.h"
#include "effect.h"
#include <QDebug>
#include "game.h"
#include "enemy.h"

Laser::Laser(const QString &img, Enemy* shooter, int radius, double angle, double omega, int lifetime, double x, double y, int prepare_time)
    :Bullet(img,radius,x,y)
{
    if(shooter!=nullptr) connect(this,SIGNAL(soundPlay(Game::Sound)),shooter,SIGNAL(soundPlay(Game::Sound)));
    this->setInvulnerable();
    this->angle=angle;
    while(this->angle>=2*M_PI) this->angle-=2*M_PI;
    while(this->angle<0) this->angle+=2*M_PI;
    this->omega=omega;
    this->lifetimer=lifetime;
    this->dead_timer=-1;
    this->dying=false;
    this->img_w=38;
    this->img_h=500;
    this->show_h=1150;
    this->setZValue(Game::ZValueLaser);
    setOpacity(0.3);
    fadein();
    this->prepare_timer=prepare_time;
    preparing=true;
}
Character* Laser::testAttackedBy(Character* attacker) {
    if(preparing||dying) return nullptr;
    if(attacker!=nullptr) {
        double x0=attacker->getX(), y0=attacker->getY(), slope=tan(angle), theta;
        theta = angleofvector(x0-x,y0-y);
        double angle_dif=angle-theta;
        while(angle_dif>M_PI) angle_dif-=2*M_PI;
        while(angle_dif<-M_PI) angle_dif+=2*M_PI;
        if(attacker->isAttackable() && (abs((slope*x0-y0-slope*x+y)/sqrt(pow(slope,2)+1)) <= attacker->getRadius() + radius)
                && (angle_dif<=M_PI/2) && (angle_dif>=-M_PI/2))
        {
            if(health>0 && !invulnerable) {
                health-=1;
                emit healthChanged(health);
            }
            if(health<=0) {
                dead=true;
                emit deadSignal();
            }
            return attacker;
        }
    }
    return nullptr;
}
void Laser::move() {
    //dead fade out
    if(dead_timer!=-1 && !dying) {
        dying=true;
        fadeout(300);
    }
    if(dying) {
        if(--dead_timer==0) {
            dead=true;
            emit deadSignal();
        }
        return;
    }
    //prepare fade in
    if(prepare_timer<=0 && preparing) {
        setOpacity(0.8);
        fadein(1);
        preparing=false;
        emit soundPlay(Game::SoundLaser);
    } else if(prepare_timer == 70) {
        emit soundPlay(Game::SoundMagicSmite);
    }
    //move object
    setPosition(x+xv,y+yv);
    setSpeed(xv+xa,yv+ya);
    angle+=omega;
    while(angle>=2*M_PI) angle-=2*M_PI;
    while(angle<0) angle+=2*M_PI;
    if(!preparing) {
        //lifetimer
        if(lifetimer!=0 && lifetimer!=-1) --lifetimer; //if lifetime is -1, it won't die
        if(lifetimer==0) {
            dead_timer=19;
        }
    } else --prepare_timer;
}
void Laser::img_move() {
    //move image
    setPos(x-show_w/2,y);
    QPixmap oriImg(img);
    setPixmap(oriImg.scaled(show_w,show_h));

    //rotate image
    setTransform(QTransform().translate(show_w/2, 0).rotate((angle-M_PI/2)/M_PI*180).translate(-show_w/2, 0));//martrix transform
}
double Laser::getAngle() const{
    return angle;
}
