#include "enemy_blue_4.h"

Enemy_Blue_4::Enemy_Blue_4(Character* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x, double y, double xv, double yv, double xa, double ya, bool bounceable, bool stopable)
    :Enemy_Blue(player,140,health,radius,shoot_cd,shoot_cd_init,x,y,xv,yv,xa,ya,bounceable,stopable)
{
    circle=false;
}
void Enemy_Blue_4::skill() {
    //second phase
    testIfSecPhase([this](){
        invulnerable=true;
        img=":/res/enemy/1/blue_2.png";
        shoot_timer = -113;
        shoot_cd = 1;
        skill_timer = -88;
        emit useSkill("黑子風暴");
    },
    [this](){
        //skill
        if(skill_timer>=0&&skill_timer%250==0) {
            if(skill_timer>=250*3) skill_timer=0;
            if(skill_timer==250*0) {
                moveTo(250,300);
            } else if(skill_timer==250*1) {
                moveTo(Game::FrameWidth-250,300);
            } else if(skill_timer==250*2) {
                moveTo(Game::FrameWidth/2,100);
            }
        }
        if(skill_timer>=75 && (skill_timer-75)%250==0) {
            circle=true;
        }
        //skill timer
        ++skill_timer;
    });
}
std::vector<Bullet*>* Enemy_Blue_4::shoot2() {
    const int total_t = 200;
    const int interval = 5;
    if(shoot_timer>=shoot_cd && (shoot_timer-shoot_cd)%interval==0) {
        double bullet_v, cos, sin;
        int bullet_radius, t;
        std::vector<Bullet*>* new_bullets = new std::vector<Bullet*>;
        Bullet* new_bullet;
        //bullet v, a
        t = (shoot_timer-shoot_cd)/interval;
        bullet_v = 6;
        bullet_radius = 8;
        //shoot
        for(int i=0;i<2;++i) {
            //purple, left up and right up
            if(t<total_t-8) {
                double angle=angleofvector(((i==0)?0+radius:Game::FrameWidth-radius)-x,-y);
                sin=std::sin(angle);
                cos=std::cos(angle);
                new_bullet = new Bullet(QString(":/res/bullet/1/purple.png"),bullet_radius,(i==0)?x-radius:x+radius,y,bullet_v*cos,bullet_v*sin);
                connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
                new_bullets->push_back(new_bullet);
            }
            //purple, left and right
            if(t>8) {
                new_bullet = new Bullet(QString(":/res/bullet/1/purple.png"),bullet_radius,((i==0)?0+bullet_radius:Game::FrameWidth-bullet_radius),-bullet_radius+1,0,bullet_v);
                connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
                new_bullets->push_back(new_bullet);
            }
            //black ,left and right
            if(t>=20&&(t-20)%5==0) {
                if(t==20) invulnerable=false;
                for(int j=0;j<6;++j) {
                    int init_y = ((t-20)/5*25)%180+60;
                    new_bullet = new Bullet(QString(":/res/bullet/1/black.png"),bullet_radius,((i==0)?0-bullet_radius+1:Game::FrameWidth+bullet_radius-1),init_y+160*j,(i==0)?0.5:-0.5,0,(i==0)?0.006:-0.006,-0.0004);
                    new_bullet->fadein(1500);
                    connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
                    new_bullets->push_back(new_bullet);
                }
            }
        }
        //black, circle
        if(circle && t>50) {
            double rand1 = ((double)(qrand()%10))/10*M_PI/6;
            bool clockwise = (qrand()%2==1)?true:false;
            for(int i=0;i<5;++i) {
                for(int j=-8;j<=7;++j) {
                    double angle = j*M_PI/6+rand1;
                    double cosa = std::cos(angle);
                    double sina = std::sin(angle);
                    new_bullet = new Bullet(QString(":/res/bullet/1/black.png"),16,x,y,(3.2+0.2*i)*cosa,(3.2+0.2*i)*sina);
                    new_bullet->rotateAround(x,y,0.016,clockwise);
                    connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
                    new_bullets->push_back(new_bullet);
                }
            }
        }
        if(circle) circle=false;
        if(shoot_timer==shoot_cd+interval*(total_t-1)) shoot_timer = 0;
        return new_bullets;
    }
    return nullptr;
}
