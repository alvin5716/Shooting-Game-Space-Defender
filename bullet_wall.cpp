#include "bullet_wall.h"
#include <QDebug>

Bullet_Wall_Data::Bullet_Wall_Data()
    :type(Bullet_Wall_Data::freeze)
{

}
Bullet_Wall_Data::Bullet_Wall_Data(bool infinite_bounce)
    :type(Bullet_Wall_Data::bounce)
{
    this->data.bounce.infinite=infinite_bounce;
}
Bullet_Wall_Data::Bullet_Wall_Data(double v, double a)
    :type(Bullet_Wall_Data::vertical)
{
    this->data.vertical.v=v;
    this->data.vertical.a=a;
}
/*
Bullet_Wall_Data::Bullet_Wall_Data(Character* player, double v, double a)
    :type(Bullet_Wall_Data::shootAtPlayer)
{
    this->data.shootAtPlayer.player=player;
    this->data.shootAtPlayer.v=v;
    this->data.shootAtPlayer.a=a;
}
*/
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
                    bullet_wall_data_list.erase(bullet_wall_data_list.begin());
                }
                break;
            case Bullet_Wall_Data::bounce:
                if(x<0+radius || x>Game::FrameWidth-radius || y<0+radius) {
                    if(x<0+radius || x>Game::FrameWidth-radius) {
                        this->xv *= -1;
                    }
                    if(y<0+radius) {
                        this->yv *= -1;
                    }
                    this->xa = this->ya = 0;
                    if(!this->bullet_wall_data_list.begin()->data.bounce.infinite)
                        bullet_wall_data_list.erase(bullet_wall_data_list.begin());
                }
                break;
                /*
            case Bullet_Wall_Data::shootAtPlayer:
                double angle = angleofvector(
                            bullet_Wall_data_list.begin()->data.shootAtPlayer.player->getX()-x,
                            bullet_Wall_data_list.begin()->data.shootAtPlayer.player->getY()-y);
                double sin = std::sin(angle);
                double cos = std::cos(angle);
                this->xv=this->bullet_Wall_data_list.begin()->data.shootAtPlayer.v*cos;
                this->yv=this->bullet_Wall_data_list.begin()->data.shootAtPlayer.v*sin;
                this->xa=this->bullet_Wall_data_list.begin()->data.shootAtPlayer.a*cos;
                this->ya=this->bullet_Wall_data_list.begin()->data.shootAtPlayer.a*sin;
                break;
                */
            }
        }
    }
    setPosition(x+xv,y+yv);
    setSpeed(xv+xa,yv+ya);
}
Bullet_Wall& Bullet_Wall::addWallData() {
    Bullet_Wall_Data* bullet_Wall_data = new Bullet_Wall_Data();
    this->bullet_wall_data_list.push_back(*bullet_Wall_data);
    return *this;
}
Bullet_Wall& Bullet_Wall::addWallData(bool infinite_bounce) {
    Bullet_Wall_Data* bullet_Wall_data = new Bullet_Wall_Data(infinite_bounce);
    this->bullet_wall_data_list.push_back(*bullet_Wall_data);
    return *this;
}
Bullet_Wall& Bullet_Wall::addWallData(double v, double a) {
    Bullet_Wall_Data* bullet_Wall_data = new Bullet_Wall_Data(v,a);
    this->bullet_wall_data_list.push_back(*bullet_Wall_data);
    return *this;
}
/*
Bullet_Wall& Bullet_Wall::addWallData(Character* player, double v, double a) {
    Bullet_Wall_Data* bullet_Wall_data = new Bullet_Wall_Data(wait_Wall,player,v,a);
    this->bullet_wall_data_list.push_back(*bullet_Wall_data);
    return *this;
}
*/
