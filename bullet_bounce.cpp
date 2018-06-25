#include "bullet_bounce.h"
#include "game.h"

Bullet_Bounce::Bullet_Bounce(QString img, int bounce_time, int radius, double x, double y, double xv, double yv, double xa, double ya)
    :Bullet(img,radius,x,y,xv,yv,xa,ya),bounce_time(bounce_time)
{

}
void Bullet_Bounce::move() {
    //move object
    setPosition(x+xv,y+yv);
    setSpeed(xv+xa,yv+ya);
    //bounce
    if(!already_enter && (x>0+radius && x<Game::FrameWidth-radius && y>0+radius && y<Game::FrameHeight-radius)) already_enter=true;
    if(already_enter && bounce_time>0) {
        if(x<0+radius || x>Game::FrameWidth-radius || y<0+radius) {
            if(x<0+radius || x>Game::FrameWidth-radius) {
                xv *= -1;
            }
            if(y<0+radius) {
                yv *= -1;
            }
            xa = 0;
            ya = 0;
            --bounce_time;
        }
    }
}
