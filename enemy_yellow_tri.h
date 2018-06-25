#ifndef ENEMY_YELLOW_TRI_H
#define ENEMY_YELLOW_TRI_H

#include "enemy_yellow.h"
#include "game.h"

class Enemy_Yellow_Tri: public Enemy_Yellow {
    public:
    Enemy_Yellow_Tri(QString img,int img_w,int img_h,int show_w,int show_h,Character* player,int health, int radius,int shoot_cd,int shoot_cd_init,double x=0,double y=0,double xv=0,double yv=0,double xa=0,double ya=0,bool bounceable=false,bool stopable=false)
        : Enemy_Yellow(img,img_w,img_h,show_w,show_h,player,health,radius,shoot_cd,shoot_cd_init,x,y,xv,yv,xa,ya,bounceable,stopable)
    {
        skill_timer=-400;
    }
    public slots:
        void skill() {
            if(skill_timer>=0&&skill_timer%375==0) {
                if(skill_timer>=375*3) skill_timer=0;
                if(skill_timer==375*0) {
                    moveTo(200,300,250);
                } else if(skill_timer==375*1) {
                    moveTo(Game::FrameWidth-200,300,250);
                } else if(skill_timer==375*2) {
                    moveTo(Game::FrameWidth/2,100,250);
                }
            }
            ++skill_timer;
        }
};

#endif // ENEMY_YELLOW_TRI_H
