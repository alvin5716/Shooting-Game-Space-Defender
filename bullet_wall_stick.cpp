#include "bullet_wall_stick.h"

Bullet_Wall_Stick::Bullet_Wall_Stick(QString img, int radius, double x, double y, double xv, double yv, double xa, double ya)
    :Bullet(img,radius,x,y,xv,yv,xa,ya)
{

}
void Bullet_Wall_Stick::move() {
    //move object
    setPosition(x+xv,y+yv);
    setSpeed(xv+xa,yv+ya);
    //touch wall
    if(!already_enter && (x>0+radius && x<Game::FrameWidth-radius && y>0+radius && y<Game::FrameHeight-radius)) already_enter=true;
    if(already_enter) {
        if(x<0 || x>Game::FrameWidth || y<0 || y>Game::FrameHeight) {
                xv=yv=xa=ya=0;
        }
    }
}
