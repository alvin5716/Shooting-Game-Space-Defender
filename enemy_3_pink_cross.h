#ifndef ENEMY_3_PINK_CROSS_H
#define ENEMY_3_PINK_CROSS_H

#include "enemy_3_pink.h"
#include "game.h"

class Enemy_3_Pink_Cross: public Enemy_3_Pink {
    public:
    Enemy_3_Pink_Cross(QString img,int img_w,int img_h,int show_w,int show_h,Character* player,int health, int radius,int shoot_cd,int shoot_cd_init,double x=0,double y=0,double xv=0,double yv=0,double xa=0,double ya=0,bool bounceable=false,bool stopable=false)
        : Enemy_3_Pink(img,img_w,img_h,show_w,show_h,player,health,radius,shoot_cd,shoot_cd_init,x,y,xv,yv,xa,ya,bounceable,stopable)
    {
        skill_timer=-100;
        setInvulnerable();
    }
    public slots:
        void skill() {
            const int interval = 500;
            if(skill_timer>=0&&skill_timer%interval==0) {
                if(skill_timer>=interval*4) skill_timer=0;
                if(skill_timer==0) {
                    moveTo(Game::FrameWidth-200,100,250);
                } else if(skill_timer==interval*1) {
                    moveTo(200,200,250);
                } else if(skill_timer==interval*2) {
                    moveTo(Game::FrameWidth-200,200,250);
                } else if(skill_timer==interval*3) {
                    moveTo(200,100,250);
                }
            }
            if(skill_timer==150) setVulnerable();
            ++skill_timer;
        }
};

#endif // ENEMY_3_PINK_CROSS_H
