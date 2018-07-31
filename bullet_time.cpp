#include "bullet_time.h"

Bullet_Time_Data::Bullet_Time_Data(int duration, double xv, double yv, double xa, double ya)
    :duration(duration), type(Bullet_Time_Data::updateVA)
{
    this->data.updateVA.xv=xv;
    this->data.updateVA.yv=yv;
    this->data.updateVA.xa=xa;
    this->data.updateVA.ya=ya;
}
Bullet_Time_Data::Bullet_Time_Data(int duration, Character* player, double v, double a)
    :duration(duration), type(Bullet_Time_Data::shootAtPlayer)
{
    this->data.shootAtPlayer.player=player;
    this->data.shootAtPlayer.v=v;
    this->data.shootAtPlayer.a=a;
}
Bullet_Time::Bullet_Time(QString img, int radius, double x, double y, double xv, double yv, double xa, double ya)
    :Bullet(img,radius,x,y,xv,yv,xa,ya), timer(0)
{

}

void Bullet_Time::move() {
    //move object
    if(this->bullet_time_data_list.size()>0) {
        if(++timer>this->bullet_time_data_list.begin()->duration) {
            timer=0;
            switch (this->bullet_time_data_list.begin()->type) {
            case Bullet_Time_Data::updateVA:
                this->xv=this->bullet_time_data_list.begin()->data.updateVA.xv;
                this->yv=this->bullet_time_data_list.begin()->data.updateVA.yv;
                this->xa=this->bullet_time_data_list.begin()->data.updateVA.xa;
                this->ya=this->bullet_time_data_list.begin()->data.updateVA.ya;
                break;
            case Bullet_Time_Data::shootAtPlayer:
                double angle = angleofvector(
                            bullet_time_data_list.begin()->data.shootAtPlayer.player->getX()-x,
                            bullet_time_data_list.begin()->data.shootAtPlayer.player->getY()-y);
                double sin = std::sin(angle);
                double cos = std::cos(angle);
                this->xv=this->bullet_time_data_list.begin()->data.shootAtPlayer.v*cos;
                this->yv=this->bullet_time_data_list.begin()->data.shootAtPlayer.v*sin;
                this->xa=this->bullet_time_data_list.begin()->data.shootAtPlayer.a*cos;
                this->ya=this->bullet_time_data_list.begin()->data.shootAtPlayer.a*sin;
                break;
            }
            bullet_time_data_list.erase(bullet_time_data_list.begin());
        }
    }
    setPosition(x+xv,y+yv);
    setSpeed(xv+xa,yv+ya);
}

Bullet_Time& Bullet_Time::addTimeData(int duration, double xv, double yv, double xa, double ya) {
    Bullet_Time_Data* bullet_time_data = new Bullet_Time_Data(duration,xv,yv,xa,ya);
    this->bullet_time_data_list.push_back(*bullet_time_data);
    return *this;
}
Bullet_Time& Bullet_Time::addTimeData(int duration, Character* player, double v, double a) {
    Bullet_Time_Data* bullet_time_data = new Bullet_Time_Data(duration,player,v,a);
    this->bullet_time_data_list.push_back(*bullet_time_data);
    return *this;
}
