#include "bullet_nether.h"
#include "effect.h"

Bullet_Nether::Bullet_Nether(QString img, int radius, Character* nether_center, int lifetime, double x, double y, double xv, double yv, double xa, double ya)
    :Bullet(img,radius,x,y,xv,yv,xa,ya)
{
    lifetimer=lifetime;
    this->nether_center=nether_center;
    setOpacity(0.8);
}
void Bullet_Nether::move() {
    //move object
    setPosition(x+xv,y+yv);
    setSpeed(xv+xa,yv+ya);
    //attack by nether center
    if(nether_center!=NULL) {
        if(sqrt(pow(nether_center->getX() - x,2)+pow(nether_center->getY() - y,2)) <= nether_center->getRadius() + radius) {
            if(health>0 && !invulnerable) health-=1;
            if(health<=0 && !invulnerable) {
                fadeout(500);
                lifetimer=500/8;
                setInvulnerable();
            }
        }
    }

    //lifetimer
    --lifetimer;
    if(lifetimer<=0) dead=true;
}
Character* Bullet_Nether::testAttackedBy(std::vector<Character*> & attackers) {
    for(int i=0;i<(int)attackers.size();++i) {
        if(sqrt(pow(attackers.at(i)->getX() - x,2)+pow(attackers.at(i)->getY() - y,2)) <= attackers.at(i)->getRadius() + radius) {
            if(health>0 && !invulnerable) {
                health-=1;
                emit healthChanged(health);
            }
            if(health<=0) {
                dead=true;
                emit deadSignal();
            }
            return attackers.at(i);
        }
    }
    return NULL;
}
Character* Bullet_Nether::testAttackedBy(Character* attacker) {
    if(sqrt(pow(attacker->getX() - x,2)+pow(attacker->getY() - y,2)) <= attacker->getRadius() + radius) {
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
    return NULL;
}
