#include "bullet_collection.h"

Bullet_Collection::Bullet_Collection(int bullet_count, const QString &img, int radius, double x, double y, double xv, double yv, double xa, double ya)
    :Bullet(img, radius, x, y, xv, yv, xa, ya), children()
{
    children.reserve(bullet_count);
    this->invulnerable = true;
    this->attackable = false;
}
void Bullet_Collection::addBullet(int x, int y, int xv, int yv, int xa, int ya, int enter_timer) {
    Bullet* child = new Bullet("",0,x,y,xv,yv,xa,ya);
    child->waitUntilInFrame(enter_timer);
    children.push_back(child);
}

void Bullet_Collection::move() {
    //collection center move
    Bullet::move();
    //bullets move
    for(Bullet* child: children) {
        if(terminal_v==0) {
            child->Character::move();
        } else {
            //move object
            child->setPosition(child->x + child->xv + this->xv,
                               child->y + child->yv + this->yv);
            double old_v = sqrt(pow(child->xv,2)+pow(child->yv,2));
            double new_vx = child->xv + child->xa + this->xa;
            double new_vy = child->yv + child->ya + this->ya;
            double new_v = sqrt(pow(new_vx,2)+pow(new_vy,2));
            if((old_v <= terminal_v && new_v <= terminal_v) ||
               (old_v >= terminal_v && new_v >=terminal_v))
                child->setSpeed(new_vx,new_vy);
            //enter_timer
            if(child->enter_timer > 0) --(child->enter_timer);
        }
    }
}

bool Bullet_Collection::outOfFrame() {
    for(std::vector<Bullet*>::iterator i=children.begin();i!=children.end();) {
        if((*i)->enter_timer!=0) {
            if(!(*i)->Character::outOfFrame()) (*i)->enter_timer=0;
            ++i;
        } else if(Character::outOfFrame()) {
            (*i)->killItself();
            delete (*i);
            i = player_bullets.erase(i);
        } else ++i;
    }
    return false;
}

void Bullet::img_move() {
    for(Bullet* child: children) {
        child->img_move();
    }
}

//img_move
//attacked
