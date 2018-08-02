#include "bullet_wall.h"
#include <QDebug>

Bullet_Wall_Data::Bullet_Wall_Data(int bullet_wall_data_type)
{
    switch(bullet_wall_data_type) {
    case Bullet_Wall_Data::freeze:
        type = bullet_wall_data_type;
        break;
    default:
        qDebug() << "this ctor is only for: \nBullet_Wall_Data::freeze \nBullet_Wall_Data::magicStone";
    }
}
Bullet_Wall_Data::Bullet_Wall_Data(bool infinite_bounce, bool bounce_when_hit_downside)
    :type(Bullet_Wall_Data::bounce)
{
    this->data.bounce.infinite=infinite_bounce;
    this->data.bounce.bounce_when_hit_downside=bounce_when_hit_downside;
}
Bullet_Wall_Data::Bullet_Wall_Data(double vertical_v, double vertical_a)
    :type(Bullet_Wall_Data::vertical)
{
    this->data.vertical.v=vertical_v;
    this->data.vertical.a=vertical_a;
}
Bullet_Wall_Data::Bullet_Wall_Data(Character* player, double v)
    :type(Bullet_Wall_Data::magicStone)
{
    this->data.magicStone.player=player;
    this->data.magicStone.v=v;
}

Bullet_Wall::Bullet_Wall(QString img, int radius, double x, double y, double xv, double yv, double xa, double ya)
    :Bullet(img,radius,x,y,xv,yv,xa,ya)
{

}

void Bullet_Wall::move() {
    //move object
    if(this->bullet_wall_data_list.size()>0) {
        if(!already_enter && (x>0+radius && x<Game::FrameWidth-radius && y>0+radius && y<Game::FrameHeight-radius)) already_enter=true;
        if(already_enter) {
            switch (this->bullet_wall_data_list.begin()->type) {
            case Bullet_Wall_Data::freeze:
                if(x<0 || x>Game::FrameWidth || y<0 || y>Game::FrameHeight) {
                    this->xa = this->ya = this->xv = this->yv = 0;
                    emit triggered();
                    bullet_wall_data_list.erase(bullet_wall_data_list.begin());
                }
                break;
            case Bullet_Wall_Data::vertical:
                if(x<0+radius || x>Game::FrameWidth-radius || y<0+radius || y>Game::FrameHeight-radius) {
                    if(x<0+radius) {
                        yv=ya=0;
                        this->xv = this->bullet_wall_data_list.begin()->data.vertical.v;
                        this->xa = this->bullet_wall_data_list.begin()->data.vertical.a;
                    } else if(x>Game::FrameWidth-radius) {
                        yv=ya=0;
                        this->xv = - this->bullet_wall_data_list.begin()->data.vertical.v;
                        this->xa = - this->bullet_wall_data_list.begin()->data.vertical.a;
                    }
                    if(y<0+radius) {
                        xv=xa=0;
                        this->yv = this->bullet_wall_data_list.begin()->data.vertical.v;
                        this->ya = this->bullet_wall_data_list.begin()->data.vertical.a;
                    } else if(y>Game::FrameHeight-radius) {
                        xv=xa=0;
                        this->yv = - this->bullet_wall_data_list.begin()->data.vertical.v;
                        this->ya = - this->bullet_wall_data_list.begin()->data.vertical.a;
                    }
                    emit triggered();
                    bullet_wall_data_list.erase(bullet_wall_data_list.begin());
                }
                break;
            case Bullet_Wall_Data::bounce:
                if(x<0+radius || x>Game::FrameWidth-radius || y<0+radius || (this->bullet_wall_data_list.begin()->data.bounce.bounce_when_hit_downside && y>Game::FrameHeight-radius)) {
                    if(x<0+radius || x>Game::FrameWidth-radius) {
                        this->xv *= -1;
                    }
                    if(y<0+radius || (this->bullet_wall_data_list.begin()->data.bounce.bounce_when_hit_downside && y>Game::FrameHeight-radius)) {
                        this->yv *= -1;
                    }
                    this->xa = this->ya = 0;
                    emit triggered();
                    if(!this->bullet_wall_data_list.begin()->data.bounce.infinite)
                        bullet_wall_data_list.erase(bullet_wall_data_list.begin());
                }
                break;
            case Bullet_Wall_Data::magicStone:
                if(x<0+radius || x>Game::FrameWidth-radius || y<0+radius || y>Game::FrameHeight-radius) {
                    if(y<0+radius) {
                        double angle = angleofvector(
                                    bullet_wall_data_list.begin()->data.magicStone.player->getX()-x,
                                    bullet_wall_data_list.begin()->data.magicStone.player->getY()-y);
                        double sin = std::sin(angle);
                        double cos = std::cos(angle);
                        this->xv=this->bullet_wall_data_list.begin()->data.magicStone.v*cos;
                        this->yv=this->bullet_wall_data_list.begin()->data.magicStone.v*sin;
                    }
                    if(x<0+radius || x>Game::FrameWidth-radius) {
                        this->xv *= -1;
                    }
                    if(y>Game::FrameHeight-radius) {
                        this->yv *= -1;
                    }
                    this->xa = this->ya = 0;
                    emit triggered();
                }
                break;
            }
        }
    }
    setPosition(x+xv,y+yv);
    setSpeed(xv+xa,yv+ya);
}
Bullet_Wall& Bullet_Wall::addWallData(int bullet_wall_data_type) {
    Bullet_Wall_Data* bullet_wall_data = new Bullet_Wall_Data(bullet_wall_data_type);
    this->bullet_wall_data_list.push_back(*bullet_wall_data);
    return *this;
}
Bullet_Wall& Bullet_Wall::addWallData(bool infinite_bounce, bool bounce_when_hit_downside) {
    Bullet_Wall_Data* bullet_wall_data = new Bullet_Wall_Data(infinite_bounce, bounce_when_hit_downside);
    this->bullet_wall_data_list.push_back(*bullet_wall_data);
    return *this;
}
Bullet_Wall& Bullet_Wall::addWallData(double vertical_v, double vertical_a) {
    Bullet_Wall_Data* bullet_wall_data = new Bullet_Wall_Data(vertical_v,vertical_a);
    this->bullet_wall_data_list.push_back(*bullet_wall_data);
    return *this;
}
Bullet_Wall& Bullet_Wall::addWallData(Character* player, double v) {
    Bullet_Wall_Data* bullet_wall_data = new Bullet_Wall_Data(player,v);
    this->bullet_wall_data_list.push_back(*bullet_wall_data);
    return *this;
}
