#include "bullet_wall.h"
#include "game.h"
#include <QDebug>

Bullet_Wall::Bullet_Wall(QString img, int radius, int bounce_time, double vt, double at, double x, double y, double xv, double yv, double xa, double ya)
    :Bullet(img,radius,x,y,xv,yv,xa,ya),bounce_time(bounce_time),vt(vt),at(at)
{

}
void Bullet_Wall::move() {
    //move object
    setPosition(x+xv,y+yv);
    setSpeed(xv+xa,yv+ya);
    //touch wall
    if(!already_enter && (x>0+radius && x<Game::FrameWidth-radius && y>0+radius && y<Game::FrameHeight-radius)) already_enter=true;
    if(already_enter && bounce_time>0) {
        if(x<0+radius || x>Game::FrameWidth-radius || y<0+radius || y>Game::FrameHeight-radius) {
            if(x<0+radius) {
                yv=ya=0;
                xv=vt;
                xa=at;
            } else if(x>Game::FrameWidth-radius) {
                yv=ya=0;
                xv=-vt;
                xa=-at;
            }
            if(y<0+radius) {
                xv=xa=0;
                yv=vt;
                ya=at;
            } else if(y>Game::FrameHeight-radius) {
                xv=xa=0;
                yv=-vt;
                ya=-at;
            }
            --bounce_time;
        }
    }
}
