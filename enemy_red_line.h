#ifndef ENEMY_RED_LINE_H
#define ENEMY_RED_LINE_H

#include "enemy_red.h"
#include "game.h"

class Enemy_Red_Line: public Enemy_Red {
    public:
    Enemy_Red_Line(QString img,int img_w,int img_h,int show_w,int show_h,Character* player,int health, int radius,int shoot_cd,int shoot_cd_init,double x=0,double y=0,double xv=0,double yv=0,double xa=0,double ya=0,bool bounceable=false,bool stopable=false)
        : Enemy_Red(img,img_w,img_h,show_w,show_h,player,health,radius,shoot_cd,shoot_cd_init,x,y,xv,yv,xa,ya,bounceable,stopable)
    {
        skill_timer=-400;
    }
    public slots:
        void skill() {
            if(skill_timer>=0) {
                if(skill_timer>=1000*2) skill_timer=0;
                if(skill_timer==0) {
                    moveTo(Game::FrameWidth-100,y,500);
                } else if(skill_timer==1000) {
                    moveTo(100,y,500);
                } else if((skill_timer%1000)%50==0&&(skill_timer%1000)<=300) {
                    shoot_timer=shoot_cd;
                }
            }
            ++skill_timer;
        }
};

#endif // ENEMY_RED_LINE_H
