#include "bullet_time.h"
#include <QDebug>

//Bullet_Time_Data
Bullet_Time_Data::Bullet_Time_Data(int wait_time)
    :wait_time(wait_time), type(Bullet_Time_Data_Type::freeze)
{

}
Bullet_Time_Data::Bullet_Time_Data(int wait_time, double xv, double yv, double xa, double ya)
    :wait_time(wait_time), type(Bullet_Time_Data_Type::updateVA)
{
    this->data.updateVA.xv=xv;
    this->data.updateVA.yv=yv;
    this->data.updateVA.xa=xa;
    this->data.updateVA.ya=ya;
}
Bullet_Time_Data::Bullet_Time_Data(int wait_time, Character* player, double v, double a)
    :wait_time(wait_time), type(Bullet_Time_Data_Type::shootAtPlayer)
{
    this->data.shootAtPlayer.player=player;
    this->data.shootAtPlayer.v=v;
    this->data.shootAtPlayer.a=a;
}
Bullet_Time_Data::Bullet_Time_Data(int wait_time, double x, double y, int time)
    :wait_time(wait_time), type(Bullet_Time_Data_Type::moveTo)
{
    this->data.moveTo.x=x;
    this->data.moveTo.y=y;
    this->data.moveTo.time=time;
}
// Bullet_Time
Bullet_Time::Bullet_Time(QString img, int radius, double x, double y, double xv, double yv, double xa, double ya)
    :Bullet(img,radius,x,y,xv,yv,xa,ya), timer(0)
{

}

void Bullet_Time::move() {
    //move object
    if(this->bullet_time_data_list.size()>0) {
        if(++timer>this->bullet_time_data_list.begin()->wait_time) {
            timer=0;
            switch (this->bullet_time_data_list.begin()->type) {
            case Bullet_Time_Data_Type::freeze:
                this->xa = this->ya = this->xv = this->yv = 0;
                break;
            case Bullet_Time_Data_Type::updateVA:
                this->xv=this->bullet_time_data_list.begin()->data.updateVA.xv;
                this->yv=this->bullet_time_data_list.begin()->data.updateVA.yv;
                this->xa=this->bullet_time_data_list.begin()->data.updateVA.xa;
                this->ya=this->bullet_time_data_list.begin()->data.updateVA.ya;
                break;
            case Bullet_Time_Data_Type::shootAtPlayer:
            {
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
            //These braces are order to limit the scope of the 3 variables above,
            //or it will lead to compile error.
            case Bullet_Time_Data_Type::moveTo:
                this->moveTo(bullet_time_data_list.begin()->data.moveTo.x,bullet_time_data_list.begin()->data.moveTo.y,bullet_time_data_list.begin()->data.moveTo.time);
                break;
            default:
                qDebug() << "error: can't get the type of Bullet_Time_Data";
            }
            bullet_time_data_list.erase(bullet_time_data_list.begin());
            emit triggered();
        }
    }
    Bullet::move();
}
Bullet_Time& Bullet_Time::addTimeData(int wait_time) {
    Bullet_Time_Data* bullet_time_data = new Bullet_Time_Data(wait_time);
    this->bullet_time_data_list.push_back(*bullet_time_data);
    return *this;
}
Bullet_Time& Bullet_Time::addTimeData(int wait_time, double xv, double yv, double xa, double ya) {
    Bullet_Time_Data* bullet_time_data = new Bullet_Time_Data(wait_time,xv,yv,xa,ya);
    this->bullet_time_data_list.push_back(*bullet_time_data);
    return *this;
}
Bullet_Time& Bullet_Time::addTimeData(int wait_time, Character* player, double v, double a) {
    Bullet_Time_Data* bullet_time_data = new Bullet_Time_Data(wait_time,player,v,a);
    this->bullet_time_data_list.push_back(*bullet_time_data);
    return *this;
}
Bullet_Time& Bullet_Time::addTimeData(int wait_time, double x, double y, int time) {
    Bullet_Time_Data* bullet_time_data = new Bullet_Time_Data(wait_time,x,y,time);
    this->bullet_time_data_list.push_back(*bullet_time_data);
    return *this;
}
