#ifndef ENEMY_2_GREEN_TRI_H
#define ENEMY_2_GREEN_TRI_H

#include "enemy_2_green.h"
#include "game.h"

class Enemy_2_Green_Tri: public Enemy_2_Green {
    public:
    Enemy_2_Green_Tri(Player* player,int health, int radius,int shoot_cd,int shoot_cd_init,double x=0,double y=0,double xv=0,double yv=0,double xa=0,double ya=0,bool bounceable=false,bool stopable=false)
        : Enemy_2_Green(player,health,radius,shoot_cd,shoot_cd_init,x,y,xv,yv,xa,ya,bounceable,stopable)
    {
        skill_timer=-200;
    }
    public slots:
        void skill() {
            if(skill_timer>=0&&skill_timer%188==0) {
                if(skill_timer>=188*3) skill_timer=0;
                if(skill_timer==188*0) {
                    moveTo(Game::FrameWidth/2+200,100,175);
                } else if(skill_timer==188*1) {
                    moveTo(Game::FrameWidth/2-200,100,175);
                } else if(skill_timer==188*2) {
                    moveTo(Game::FrameWidth/2,460,175);
                }
            }
            ++skill_timer;
        }
};

#endif // ENEMY_2_GREEN_TRI_H
