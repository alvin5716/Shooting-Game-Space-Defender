#include "enemy_2_blue_3.h"
#include <QDebug>
#include "bullet_rotate.h"
#include "bullet_bounce.h"

Enemy_2_Blue_3::Enemy_2_Blue_3(QString img, int img_w, int img_h, int show_w, int show_h, Character* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x, double y, double xv, double yv, double xa, double ya, bool bounceable, bool stopable)
    :Enemy_2_Blue(img,img_w,img_h,show_w,show_h,player,health,radius,shoot_cd,shoot_cd_init,x,y,xv,yv,xa,ya,bounceable,stopable)
{
    shoot_count=0;
}
void Enemy_2_Blue_3::skill() {
    //second phase
    if(health<=260 && !secPhase) {
        secPhase = true;
        img=":/res/enemy10_2.png";
        shoot_timer = -120;
        shoot_cd = 125;
        skill_timer = -200;
        emit useSkill("幻彩環");
    }
    if(secPhase) {
        //skill
        if(skill_timer==0) moveTo(Game::FrameWidth/2,150,240);
        //skill timer
        if(skill_timer<=0) ++skill_timer;
    }
}
std::vector<Bullet*>* Enemy_2_Blue_3::shoot2() {
    if(shoot_timer==shoot_cd) {
        std::vector<Bullet*>* new_bullets=new std::vector<Bullet*>;
        Bullet* new_bullet;
        double bullet_v, bullet_a, tana, sina, cosa, sinb, cosb, sin, cos, angle;
        int bullet_count, bullet_count_2;
        //angle
        tana = (y-player->getY()) / (x-player->getX());
        if(std::isinf(tana)) cosa = 0;
        else cosa = ((player->getX()>x)?1:-1)/ sqrt(tana*tana+1);
        if(std::isinf(tana)) sina = 1;
        else sina = tana*cosa;
        //bullet v, a
        bullet_v = 1.2;
        bullet_a = 0.00008;
        bullet_count = 16;
        bullet_count_2 = 36;
        //shoot
        switch (shoot_count) {
        case 15:
            for(int j=2;j<=3;++j) {
                for(int i=-(bullet_count_2/2);i<=(bullet_count_2/2-1);++i) {
                    if(i>=-7 && i<=4) continue;
                    cosb = std::cos((i+j*0.5+0.25)*M_PI/(bullet_count_2/2));
                    sinb = std::sin((i+j*0.5+0.25)*M_PI/(bullet_count_2/2));
                    cos = cosa*cosb-sina*sinb;
                    sin = sina*cosb+cosa*sinb;
                    new_bullet = new Bullet_Bounce(rainbowBullet(j),1,9,x,y+radius,bullet_v*cos,bullet_v*sin,bullet_a*cos,bullet_a*sin);
                    connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
                    new_bullets->push_back(new_bullet);
                }
            }
        case 14:
        case 13:
        case 12:
        case 11:
        case 10:
            for(int j=0;j<=1;++j) {
                for(int i=-(bullet_count/2);i<=(bullet_count/2-1);++i) {
                    cos = std::cos((i+j*0.5+0.25)*M_PI/(bullet_count/2));
                    sin = std::sin((i+j*0.5+0.25)*M_PI/(bullet_count/2));
                    new_bullet = new Bullet(rainbowBullet(j),8,x,y+radius,bullet_v*cos,bullet_v*sin,bullet_a*cos,bullet_a*sin);
                    connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
                    new_bullets->push_back(new_bullet);
                }
            }
        case 9:
        case 8:
        case 7:
        case 6:
        case 5:
            for(int i=-(bullet_count/2);i<=(bullet_count/2-1);++i) {
                cosb = std::cos(i*M_PI/(bullet_count/2));
                sinb = std::sin(i*M_PI/(bullet_count/2));
                cos = cosa*cosb-sina*sinb;
                sin = sina*cosb+cosa*sinb;
                new_bullet = new Bullet(rainbowBullet(-1),7,x,y+radius,bullet_v*cos,bullet_v*sin,bullet_a*cos,bullet_a*sin);
                connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
                new_bullets->push_back(new_bullet);
            }
        case 4:
        case 3:
        case 2:
        case 1:
        case 0:
            angle = qrand()%180*M_PI/180;
            for(int j=-3;j<=-2;++j) {
                for(int i=-(bullet_count/2);i<=(bullet_count/2-1);++i) {
                    cos = std::cos(angle+i*M_PI/(bullet_count/2));
                    sin = std::sin(angle+i*M_PI/(bullet_count/2));
                    new_bullet = new Bullet_Rotate(rainbowBullet(j),x,y+radius,0.0015,j==-3,6,x,y+radius,bullet_v*cos,bullet_v*sin);
                    connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
                    new_bullets->push_back(new_bullet);
                }
            }
            if(shoot_count<15) ++shoot_count;
            break;
        default:
            qDebug() << "error";
            break;
        }
        shoot_timer = 0;
        return new_bullets;
    }
    return NULL;
}
