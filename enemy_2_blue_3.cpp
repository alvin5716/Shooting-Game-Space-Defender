#include "enemy_2_blue_3.h"
#include <QDebug>
#include "bullet_rotate.h"
#include "bullet_wall.h"

Enemy_2_Blue_3::Enemy_2_Blue_3(QString img, int img_w, int img_h, int show_w, int show_h, Character* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x, double y, double xv, double yv, double xa, double ya, bool bounceable, bool stopable)
    :Enemy_2_Blue(img,img_w,img_h,show_w,show_h,player,health,radius,shoot_cd,shoot_cd_init,x,y,xv,yv,xa,ya,bounceable,stopable)
{
    shoot_count=0;
    rotater=0;
    setDisappearTime(5000);
    death_img=":/res/enemy10_3.png";
}
void Enemy_2_Blue_3::skill() {
    //second phase
    if(health<=300 && !secPhase) {
        secPhase = true;
        invulnerable=true;
        img=":/res/enemy10_2.png";
        shoot_timer = -170;
        shoot_cd = 120;
        skill_timer = -200;
        emit useSkill("幻彩色波紋疾走");
    }
    if(secPhase) {
        //skill
        if(skill_timer==0) moveTo(Game::FrameWidth/2,190,75);
        //skill timer
        if(skill_timer<=0) ++skill_timer;
    }
}
std::vector<Bullet*>* Enemy_2_Blue_3::shoot2() {
    if(shoot_timer==shoot_cd) {
        invulnerable=false;
        std::vector<Bullet*>* new_bullets=new std::vector<Bullet*>;
        Bullet* new_bullet;
        double bullet_v, bullet_a, sin, cos, angle;
        int bullet_count, bullet_count_2;
        //bullet v, a
        bullet_v = 1.6;
        bullet_a = 0.00017;
        bullet_count = 16;
        bullet_count_2 = 38;
        //shoot
        switch (shoot_count) {
        case 21:
            for(int j=2;j<=3;++j) {
                for(int i=-(bullet_count_2/2);i<=(bullet_count_2/2-((bullet_count_2%2==0)?1:0));++i) {
                    if(i>=-12 && i<=10) continue;
                    cos = std::cos((i+j*0.5-0.25+rotater*0.15)*M_PI/bullet_count_2*2+M_PI/2);
                    sin = std::sin((i+j*0.5-0.25+rotater*0.15)*M_PI/bullet_count_2*2+M_PI/2);
                    Bullet_Wall *new_bullet_wall;
                    new_bullet = new_bullet_wall = new Bullet_Wall(rainbowBullet(j),9,x,y+radius,bullet_v*cos,bullet_v*sin,bullet_a*cos,bullet_a*sin);
                    new_bullet_wall->addWallData(false);
                    connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
                    new_bullets->push_back(new_bullet);
                }
            }
            if(++rotater>3) rotater=-3;
        case 20:
        case 19:
        case 18:
        case 17:
        case 16:
        case 15:
        case 14:
            for(int j=0;j<=1;++j) {
                for(int i=-(bullet_count/2);i<=(bullet_count/2-((bullet_count%2==0)?1:0));++i) {
                    cos = std::cos((i+j*0.5+0.25)*M_PI/bullet_count*2);
                    sin = std::sin((i+j*0.5+0.25)*M_PI/bullet_count*2);
                    new_bullet = new Bullet(rainbowBullet(j),8,x,y+radius,bullet_v*cos,bullet_v*sin,bullet_a*cos,bullet_a*sin);
                    connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
                    new_bullets->push_back(new_bullet);
                }
            }
        case 13:
        case 12:
        case 11:
        case 10:
        case 9:
        case 8:
        case 7:
            angle = angleofvector(player->getX()-x,player->getY()-y);
            for(int i=-(bullet_count/2);i<=(bullet_count/2-((bullet_count%2==0)?1:0));++i) {
                cos = std::cos(angle+i*M_PI/bullet_count*2);
                sin = std::sin(angle+i*M_PI/bullet_count*2);
                new_bullet = new Bullet(rainbowBullet(-1),7,x,y+radius,bullet_v*cos,bullet_v*sin,bullet_a*cos,bullet_a*sin);
                connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
                new_bullets->push_back(new_bullet);
            }
        case 6:
        case 5:
        case 4:
        case 3:
        case 2:
        case 1:
        case 0:
            angle = qrand()%180*M_PI/180;
            for(int j=-3;j<=-2;++j) {
                for(int i=-(bullet_count/2);i<=(bullet_count/2-((bullet_count%2==0)?1:0));++i) {
                    cos = std::cos(angle+i*M_PI/bullet_count*2);
                    sin = std::sin(angle+i*M_PI/bullet_count*2);
                    new_bullet = new Bullet_Rotate(rainbowBullet(j),x,y+radius,0.0032,j==-3,6,x,y+radius,bullet_v*cos,bullet_v*sin);
                    connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
                    new_bullets->push_back(new_bullet);
                }
            }
            if(shoot_count<21) ++shoot_count;
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
