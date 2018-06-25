#include "bullet_terminal.h"
#include <cmath>

Bullet_Terminal::Bullet_Terminal(QString img, int radius, double terminal_v,double x,double y,double xv,double yv,double xa,double ya)
    :Bullet(img,radius,x,y,xv,yv,xa,ya),terminal_v(terminal_v)
{

}
void Bullet_Terminal::move() {
    //move object
    setPosition(x+xv,y+yv);
    if((sqrt(pow(xv,2)+pow(yv,2))<=terminal_v && sqrt(pow(xv+xa,2)+pow(yv+ya,2))<=terminal_v) || (sqrt(pow(xv,2)+pow(yv,2))>=terminal_v && sqrt(pow(xv+xa,2)+pow(yv+ya,2))>=terminal_v)) setSpeed(xv+xa,yv+ya);
}
