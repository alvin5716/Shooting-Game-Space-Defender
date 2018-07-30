#include "enemy_blue_4.h"
#include "bullet_rotate.h"

Enemy_Blue_4::Enemy_Blue_4(QString img, int img_w, int img_h, int show_w, int show_h, Character* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x, double y, double xv, double yv, double xa, double ya, bool bounceable, bool stopable)
    :Enemy_Blue(img,img_w,img_h,show_w,show_h,player,health,radius,shoot_cd,shoot_cd_init,x,y,xv,yv,xa,ya,bounceable,stopable)
{
    circle=false;
}
void Enemy_Blue_4::skill() {
    //second phase
    if(health<=140 && !secPhase) {
        secPhase = true;
        invulnerable=true;
        img=":/res/enemy4_2.png";
        shoot_timer = -225;
        shoot_cd = 1;
        skill_timer = -175;
        emit useSkill("黑子風暴");
    }
    if(secPhase) {
        //skill
        if(skill_timer>=0&&skill_timer%500==0) {
            if(skill_timer>=500*3) skill_timer=0;
            if(skill_timer==500*0) {
                moveTo(250,300);
            } else if(skill_timer==500*1) {
                moveTo(Game::FrameWidth-250,300);
            } else if(skill_timer==500*2) {
                moveTo(Game::FrameWidth/2,100);
            }
        }
        if(skill_timer>=150 && (skill_timer-150)%500==0) {
            circle=true;
        }
        //skill timer
        ++skill_timer;
    }
}
std::vector<Bullet*>* Enemy_Blue_4::shoot2() {
    const int total_t = 400;
    const int interval = 10;
    if(shoot_timer>=shoot_cd && (shoot_timer-shoot_cd)%interval==0) {
        double bullet_v, cos, sin;
        int bullet_radius, t;
        std::vector<Bullet*>* new_bullets = new std::vector<Bullet*>;
        Bullet* new_bullet;
        //bullet v, a
        t = (shoot_timer-shoot_cd)/interval;
        bullet_v = 3;
        bullet_radius = 8;
        //shoot
        for(int i=0;i<2;++i) {
            //purple, left up and right up
            if(t<total_t-15) {
                double angle=angleofvector(((i==0)?0+radius:Game::FrameWidth-radius)-x,-y);
                sin=std::sin(angle);
                cos=std::cos(angle);
                new_bullet = new Bullet(QString(":/res/bullet_purple.png"),bullet_radius,(i==0)?x-radius:x+radius,y,bullet_v*cos,bullet_v*sin);
                connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
                new_bullets->push_back(new_bullet);
            }
            //purple, left and right
            if(t>15) {
                new_bullet = new Bullet(QString(":/res/bullet_purple.png"),bullet_radius,((i==0)?0+bullet_radius:Game::FrameWidth-bullet_radius),-bullet_radius+1,0,bullet_v);
                connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
                new_bullets->push_back(new_bullet);
            }
            //black ,left and right
            if(t>=40&&(t-40)%10==0) {
                if(t==40) invulnerable=false;
                for(int j=0;j<6;++j) {
                    int init_y = ((t-40)/10*25)%180+60;
                    new_bullet = new Bullet(QString(":/res/bullet_black.png"),bullet_radius,((i==0)?0-bullet_radius+1:Game::FrameWidth+bullet_radius-1),init_y+160*j,(i==0)?0.25:-0.25,0,(i==0)?0.0015:-0.0015,-0.0001);
                    new_bullet->fadein(1500);
                    connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
                    new_bullets->push_back(new_bullet);
                }
            }
        }
        //black, circle
        if(circle && t>100) {
            double rand1 = ((double)(qrand()%10))/10*M_PI/6;
            bool clockwise = (qrand()%2==1)?true:false;
            for(int i=0;i<5;++i) {
                for(int j=-8;j<=7;++j) {
                    double angle = j*M_PI/6+rand1;
                    double cosa = std::cos(angle);
                    double sina = std::sin(angle);
                    new_bullet = new Bullet_Rotate(QString(":/res/bullet_black.png"),x,y,0.004,clockwise,16,x,y,(1.6+0.1*i)*cosa,(1.6+0.1*i)*sina);
                    connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
                    new_bullets->push_back(new_bullet);
                }
            }
        }
        if(circle) circle=false;
        if(shoot_timer==shoot_cd+interval*(total_t-1)) shoot_timer = 0;
        return new_bullets;
    }
    return NULL;
}
