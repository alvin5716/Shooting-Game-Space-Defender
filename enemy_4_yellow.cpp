#include "enemy_4_yellow.h"
#include "game.h"

Enemy_4_Yellow::Enemy_4_Yellow(Player* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x, double y, double xv, double yv, double xa, double ya, bool bounceable, bool stopable)
:Enemy_4(QString(":/res/enemy/4/yellow.png"),200,153,std::round(3.902*radius),3*radius,player,health,radius,shoot_cd,shoot_cd_init,x,y,xv,yv,xa,ya,bounceable,stopable)
{
    max_speed=0.8;
    invulnerable = true;
    interval = 90;

    player_poses.reserve(63);
    for(unsigned int i=0; i<player_poses.capacity(); ++i) player_poses.push_back(QPointF(-1,-1));
}

void Enemy_4_Yellow::skill() {
    for(std::vector<QPointF>::iterator i=player_poses.begin(); i!=(player_poses.end()-1); ++i) {
        (i+1)->setX(i->x());
        (i+1)->setY(i->y());
    }
    player_poses.begin()->setX(player->getX());
    player_poses.begin()->setY(player->getY());
    if(prep_timer>0) {
        --prep_timer;
        return;
    }
    if(!invulnerable_after_init) invulnerable = false;
    double player_past_x = (player_poses.end()-1)->x();
    if(player_past_x == -1) return;
    constexpr int safe=40;
    if(x>player_past_x-radius-safe && x<player_past_x+radius+safe) {
        double speed;
        if(player_past_x<safe+radius) speed=max_speed;
        else if(player_past_x>Game::FrameWidth-safe-radius) speed=-max_speed;
        else speed=(x>player_past_x?1:-1)*max_speed*std::cos((x-player_past_x)/(radius+safe)*M_PI/2);
        setSpeed(speed,0);
    } else {
        setSpeed(0,0);
    }
}

void Enemy_4_Yellow::setInterval(int interval) {
    this->interval = interval;
}

std::vector<Bullet*>* Enemy_4_Yellow::shoot() {
    if(prep_timer>0) return nullptr;
    constexpr int shoot_count=3, bullet_count=80;
    if(shoot_timer>=shoot_cd && (shoot_timer-shoot_cd)%interval==0) {
        std::vector<Bullet*>* new_bullets=new std::vector<Bullet*>;
        Bullet* new_bullet;
        double sin, cos, angle = angleofvector(player->getX()-x,player->getY()-y), bullet_v=6.4, bullet_a=-0.32, bullet_v_t=1.2;
        const int t=(shoot_timer-shoot_cd)/interval;
        for(int i=0;i<bullet_count;++i) {
            if(t%2==0?i%(bullet_count/4)<12:i%(bullet_count/4)>7) continue;
            sin = std::sin(angle+i*M_PI/(bullet_count/2));
            cos = std::cos(angle+i*M_PI/(bullet_count/2));
            new_bullet = new Bullet(QString(":/res/bullet/4/yellow.png"),16,x,y,bullet_v*cos,bullet_v*sin,bullet_a*cos,bullet_a*sin);
            new_bullet->setVTerminal(bullet_v_t);
            connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
            new_bullets->push_back(new_bullet);
        }
        if(t>=shoot_count) shoot_timer=0;
        return new_bullets;
    }
    return nullptr;
}
