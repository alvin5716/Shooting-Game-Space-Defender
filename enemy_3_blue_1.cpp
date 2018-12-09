#include "enemy_3_blue_1.h"
#include "bullet_sin.h"
#include <QDebug>
#include "game.h"

Enemy_3_Blue_1::Enemy_3_Blue_1(Character* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x, double y, double xv, double yv, double xa, double ya, bool bounceable, bool stopable)
    :Enemy_3_Blue(player,135,health,radius,shoot_cd,shoot_cd_init,x,y,xv,yv,xa,ya,bounceable,stopable)
{
    mode=false;
    magicstone=nullptr;
    shoot_small_bullets=false;
    triggered_count=0;
}
void Enemy_3_Blue_1::skill() {
    //second phase
    testIfSecPhase([this](){
        invulnerable=true;
        img=":/res/enemy/3/blue_2.png";
        shoot_timer = -425;
        shoot_cd = 300;
        skill_timer = -100;
        emit useSkill("神秘的魔法石");
        this->redMagicShield();
    },
    [this](){
        //skill
        if(skill_timer==0) moveTo(Game::FrameWidth/2,220,120);
        //skill timer
        if(skill_timer<=0) ++skill_timer;
    },
    [this](){
        Enemy_3_Blue::skill();
    });
}
std::vector<Bullet*>* Enemy_3_Blue_1::shoot2() {
    //magic stone
    if(shoot_timer==-225) {
        double bullet_v, bullet_a, cos, sin;
        int bullet_radius;
        std::vector<Bullet*>* new_bullets=new std::vector<Bullet*>;
        Bullet* new_bullet;
        bullet_radius = 60;
        bullet_v = 0;
        bullet_a = 0.06;
        cos = std::cos(face_to_left?-M_PI/6:-M_PI*5/6);
        sin = std::sin(face_to_left?-M_PI/6:-M_PI*5/6);
        new_bullet = magicstone = new Bullet(":/res/bullet/other/magicball.png",0,shootXPos(),shootYPos());
        magicstone->addTimeData(63,bullet_radius)
                ->addTimeData(25,bullet_v*cos,bullet_v*sin,bullet_a*cos,bullet_a*sin)
                ->addWallData(player,5.2);
        connect(magicstone,SIGNAL(triggered()),this,SLOT(shootSmallBullet()));
        new_bullet->setInvulnerable();
        connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
        new_bullets->push_back(new_bullet);
        return new_bullets;
    }
    std::vector<Bullet*>* new_bullets=new std::vector<Bullet*>;
    Bullet* new_bullet;
    //small magic stone bullets
    if(shoot_small_bullets) {
        double bullet_v, bullet_v_2, bullet_a, cos, sin, init_pos;
        int bullet_count;
        //bullet v, a and count
        bullet_v_2 = 0.6;
        bullet_count = 14;
        angle = angleofvector(player->getX()-x,player->getY()-y);
        //shoot
        for(int j=0;j<2;++j) {
            for(int i=-(bullet_count/2);i<=(bullet_count/2-1);++i) {
                bullet_v = (j==0)?0.2:0;
                bullet_a = (j==0)?0.032:0.024;
                init_pos = (j==0)?0.5:0;
                double shoot_angle = angle+(i+(mode?0.5:0)+0.5*j)*M_PI/(bullet_count/2);
                cos = std::cos(shoot_angle);
                sin = std::sin(shoot_angle);
                new_bullet = new Bullet(QString(":/res/bullet/other/magicball.png"),12,
                                        magicstone->getX()+magicstone->getRadius()*cos*init_pos,
                                        magicstone->getY()+magicstone->getRadius()*sin*init_pos,
                                        bullet_v*cos,bullet_v*sin,bullet_a*cos,bullet_a*sin);
                new_bullet->addTimeData(125,bullet_v_2*cos,bullet_v_2*sin,0,0);
                new_bullet->fadein(1500);
                connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
                new_bullets->push_back(new_bullet);
            }
        }
        shoot_small_bullets=false;
        mode=!mode;
    }
    //move
    const int interval=90;
    if(shoot_timer==shoot_cd-100 || shoot_timer==shoot_cd/2-50 || shoot_timer==shoot_cd+5 || shoot_timer==shoot_cd+95) {
        if(player->getX()<0+radius+10) moveTo(0+radius+10,this->y,(shoot_timer==shoot_cd+5 || shoot_timer==shoot_cd+95)?75:110);
        else if(player->getX()>Game::FrameWidth-radius-10) moveTo(Game::FrameWidth-radius-10,this->y,(shoot_timer==shoot_cd+5 || shoot_timer==shoot_cd+95)?75:110);
        else moveTo(player->getX(),this->y,110);
        setVulnerable();
    }
    //thunder
    if(shoot_timer>=shoot_cd && (shoot_timer-shoot_cd)%interval==0) {
        double bullet_v, bullet_a;
        int bullet_radius;
        //bullet
        bullet_radius = 8;
        //shoot
        for(int i=0;i<19;++i) {
            bullet_v = 0.4;
            bullet_a = 0.1;
            Bullet* new_bullet;
            new_bullet = new Bullet(":/res/bullet/1/yellow.png",bullet_radius,shootXPos(),shootYPos());
            int aim_x, aim_y;
            if(i<=6) {
                aim_x = this->shootXPos()-i*6;
                aim_y = this->shootYPos()-(6-i)*20;
            } else if(i>=12) {
                aim_x = this->shootXPos()+(18-i)*8;
                aim_y = this->shootYPos()+(i-12)*20;
            } else {
                aim_x = this->shootXPos()+(i-9)*16;
                aim_y = this->shootYPos();
            }
            new_bullet->moveTo(aim_x,aim_y,25);
            new_bullet->addTimeData(25)
                    ->addTimeData(10,0,bullet_v,0,bullet_a);
            connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
            new_bullet->fadein();
            new_bullets->push_back(new_bullet);
        }
        if(shoot_timer==shoot_cd+interval*2) shoot_timer = 0;
    }
    if(new_bullets->size()>0) return new_bullets;
    return nullptr;
}
void Enemy_3_Blue_1::shootSmallBullet() {
    if(triggered_count<2) ++triggered_count;
    else {
        shoot_small_bullets=true;
        emit shakeScreen(static_cast<short>(shakeLevel::smallShake));
    }
}
