#include "laser.h"
#include "effect.h"
#include <QDebug>
#include "game.h"

Laser::Laser(QString img, int radius, double angle, double omega, int lifetime, double x, double y, int prepare_time)
    :Bullet(img,radius,x,y)
{
    this->setInvulnerable();
    this->angle=angle+M_PI/2;
    while(this->angle>=2*M_PI) this->angle-=2*M_PI;
    while(this->angle<0) this->angle+=2*M_PI;
    this->omega=omega;
    this->lifetimer=lifetime;
    this->img_w=38;
    this->img_h=500;
    this->show_h=1150;
    setOpacity(0.3);
    fadein();
    this->prepare_timer=prepare_time;
    preparing=true;
}
Character* Laser::testAttackedBy(Character* attacker) {
    if(preparing) return NULL;
    if(x<0-radius || x>Game::FrameWidth+radius || y<0-radius || y>Game::FrameHeight+radius) {
        dead=true;
    }
    if(attacker!=NULL) {
        double x0=attacker->getX(), y0=attacker->getY(), slope=tan(angle), sin, cos;
        sincostoxy(sin,cos,x0,y0);
        double angle_dif=angle-((y0<y)?acos(cos):2*M_PI-acos(cos));
        if(angle_dif>M_PI) angle_dif-=2*M_PI;
        else if(angle_dif<-M_PI) angle_dif+=2*M_PI;
        if((angle/M_PI*180>90&&angle/M_PI*180<=180)||(angle/M_PI*180>270&&angle/M_PI*180<=360)) angle_dif*=-1;
        if(((int)round(angle/M_PI*180))%180==0) angle_dif+=M_PI/2;
        if(((int)round(angle/M_PI*180)+90)%180==0) angle_dif-=M_PI/2;
        if(angle_dif>M_PI) angle_dif-=2*M_PI;
        else if(angle_dif<-M_PI) angle_dif+=2*M_PI;
        if((abs((slope*x0-y0-slope*x+y)/sqrt(pow(slope,2)+1)) <= attacker->getRadius() + radius)
                && (angle_dif<=M_PI) && (angle_dif>=0))
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
    return NULL;
}
void Laser::move() {
    if(prepare_timer<=0 && preparing) {
        setOpacity(0.8);
        fadein(1);
        preparing=false;
    }
    //move object
    setPosition(x+xv,y+yv);
    setSpeed(xv+xa,yv+ya);
    angle+=omega;
    while(angle>=2*M_PI) angle-=2*M_PI;
    while(angle<0) angle+=2*M_PI;
    if(!preparing) {
        //lifetimer
        if(lifetimer!=-1) --lifetimer; //if lifetime is -1, it won't die
        if(lifetimer==0) {
            dead=true;
            emit deadSignal();
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
