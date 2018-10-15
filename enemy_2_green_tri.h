#ifndef ENEMY_2_GREEN_TRI_H
#define ENEMY_2_GREEN_TRI_H

#include "enemy_2_green.h"
#include "game.h"

class Enemy_2_Green_Tri: public Enemy_2_Green {
    public:
    Enemy_2_Green_Tri(Character* player,int health, int radius,int shoot_cd,int shoot_cd_init,double x=0,double y=0,double xv=0,double yv=0,double xa=0,double ya=0,bool bounceable=false,bool stopable=false)
        : Enemy_2_Green(player,health,radius,shoot_cd,shoot_cd_init,x,y,xv,yv,xa,ya,bounceable,stopable)
    {
        skill_timer=-400;
    }
    public slots:
        void skill() {
            if(skill_timer>=0&&skill_timer%375==0) {
                if(skill_timer>=375*3) skill_timer=0;
                if(skill_timer==375*0) {
                    moveTo(Game::FrameWidth/2+200,100,350);
                } else if(skill_timer==375*1) {
                    moveTo(Game::FrameWidth/2-200,100,350);
                } else if(skill_timer==375*2) {
                    moveTo(Game::FrameWidth/2,460,350);
                }
            }
            ++skill_timer;
        }
};

#endif // ENEMY_2_GREEN_TRI_H
