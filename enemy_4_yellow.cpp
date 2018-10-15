#include "enemy_4_yellow.h"
#include "game.h"

Enemy_4_Yellow::Enemy_4_Yellow(Character* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x, double y, double xv, double yv, double xa, double ya, bool bounceable, bool stopable)
:Enemy_4(QString(":/res/enemy18.png"),199,153,std::round(3.902*radius),3*radius,player,health,radius,shoot_cd,shoot_cd_init,x,y,xv,yv,xa,ya,bounceable,stopable)
{
    move_speed=0.4;
    setInvulnerable();
}

void Enemy_4_Yellow::skill() {
    if(prep_timer>0) {
        --prep_timer;
        return;
    }
    setVulnerable();
    constexpr int safe=40;
    if(x>player->getX()-radius-safe && x<player->getX()+radius+safe) {
        double speed;
        if(player->getX()<safe+radius) speed=move_speed;
        else if(player->getX()>Game::FrameWidth-safe-radius) speed=-move_speed;
        else speed=(x>player->getX()?1:-1)*move_speed*std::cos((x-player->getX())/(radius+safe)*M_PI/2);
        setSpeed(speed,0);
    } else {
        setSpeed(0,0);
    }
}

std::vector<Bullet*>* Enemy_4_Yellow::shoot() {
    if(prep_timer>0) return nullptr;
    constexpr int interval=180, shoot_count=3, bullet_count=80;
    if(shoot_timer>=shoot_cd && (shoot_timer-shoot_cd)%interval==0) {
        std::vector<Bullet*>* new_bullets=new std::vector<Bullet*>;
        Bullet* new_bullet;
        double sin, cos, angle = angleofvector(player->getX()-x,player->getY()-y), bullet_v=3.2, bullet_a=-0.08, bullet_v_t=0.6;
        int t=(shoot_timer-shoot_cd)/interval;
        for(int i=0;i<bullet_count;++i) {
            if(t%2==0?i%(bullet_count/4)<12:i%(bullet_count/4)>7) continue;
            sin = std::sin(angle+i*M_PI/(bullet_count/2));
            cos = std::cos(angle+i*M_PI/(bullet_count/2));
            new_bullet = new Bullet(QString(":/res/bullet_yellow.png"),16,x,y,bullet_v*cos,bullet_v*sin,bullet_a*cos,bullet_a*sin);
            new_bullet->setVTerminal(bullet_v_t);
            connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
            new_bullets->push_back(new_bullet);
        }
        if(t>=shoot_count) shoot_timer=0;
        return new_bullets;
    }
    return nullptr;
}
