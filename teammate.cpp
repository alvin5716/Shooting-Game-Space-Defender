#include "teammate.h"
#include <QDebug>

Teammate::Teammate(const QString &img, int img_w, int img_h, int show_w, int show_h, Player *player, int shoot_cd, double x, double y)
    :Enemy(img,img_w,img_h,show_w,show_h,player,100,0,shoot_cd,0,x,y)
{
    hasEnemy = false;
    aim_pos = QPointF(-1,-1);
    aim_speed = QPointF(-1,-1);
    shooting = false;
    this->setZValue(Game::ZValueTeammate);
    this->setOpacity(0.7);
}

void Teammate::sendNearestEnemyPos(bool hasEnemy, QPointF nearestEnemyPos, QPointF speed) {
    this->hasEnemy = hasEnemy;
    this->aim_pos = nearestEnemyPos;
    this->aim_speed = speed;
}

void Teammate::setShooting(bool shooting) {
    this->shooting = shooting;
}

bool Teammate::isShooting() {
    return shooting;
}

std::vector<Bullet*>* Teammate::shoot() {
    if(++shoot_timer>=shoot_cd) {
        shoot_timer = 0;
        const int bullet_v = 8;
        double angle;
        if(!hasEnemy) angle = M_PI*1.5;
        else {
            double vx = aim_speed.x(), vy = aim_speed.y();
            double px = aim_pos.x() - this->x, py = aim_pos.y() - this->y;
            double a = vx*vx+vy*vy-bullet_v*bullet_v;
            double b = 2*(px*vx+py*vy);
            double c = px*px+py*py;
            double t;
            if(b*b-4*a*c >= 0) {
                double t1 = (-b+sqrt(b*b-4*a*c)) / (2*a);
                double t2 = (-b-sqrt(b*b-4*a*c)) / (2*a);
                if(t1>=0) t = t1;
                else if(t2>=0) t = t2;
                else t = 0;
            } else {
                t = 0;
            }
            double shoot_x = aim_pos.x() + vx*t;
            double shoot_y = aim_pos.y() + vy*t;
            angle=angleofvector(shoot_x-x,shoot_y-y);
        }
        double cos = std::cos(angle);
        double sin = std::sin(angle);
        std::vector<Bullet*>* new_bullets=new std::vector<Bullet*>;
        for(int j=0;j<2;++j) {
            for(int i=0;i<3;++i) {
                if(i==0 && j==0) continue;
                double cos_rotate = std::cos(angle+(j?M_PI/2:-M_PI/2));
                double sin_rotate = std::sin(angle+(j?M_PI/2:-M_PI/2));
                double bullet_x = x + (-25*i+30)*cos + 25*i*cos_rotate;
                double bullet_y = y + (-25*i+30)*sin + 25*i*sin_rotate;
                Bullet *new_bullet = new Bullet(QString(":/res/bullet/4/blue.png"),3,bullet_x,bullet_y,bullet_v*cos,bullet_v*sin);
                new_bullet->setOpacity(0.7);
                new_bullet->setZValue(Game::ZValueTeammate);
                new_bullet->waitUntilInFrame(63);
                new_bullets->push_back(new_bullet);
            }
        }
        return new_bullets;
    }
    return nullptr;
}

void Teammate::move() {
    static const double max_speed = 3.0;
    static const double max_acce = 0.08;
    static const double force_acce = 0.2;
    double speed = std::sqrt(std::pow(xv,2)+std::pow(yv,2));
    if(speed>max_speed) {
        xv *= max_speed / speed;
        yv *= max_speed / speed;
        speed = max_speed;
    }
    double dis = this->distanceTo(player);
    if(dis > 150) {
        double angle = angleofvector(player->getX()-x,player->getY()-y);
        double cos = std::cos(angle);
        double sin = std::sin(angle);
        setAcceleration(max_acce*cos, max_acce*sin);
    } else if(dis < 100) {
        double angle = angleofvector(player->getX()-x,player->getY()-y);
        double cos = -std::cos(angle);
        double sin = -std::sin(angle);
        setAcceleration(max_acce*cos, max_acce*sin);
    } else if(speed > max_acce) {
        double angle = angleofvector(xv,yv);
        double cos = -std::cos(angle);
        double sin = -std::sin(angle);
        setAcceleration(max_acce*cos, max_acce*sin);
    } else {
        setAcceleration(0,0);
        setSpeed(0,0);
    }
    //always in frame
    const int half_w = show_w/2;
    const int half_h = show_h/2;
    if(x<0+half_w) setAcceleration(xa+force_acce,ya);
    else if(y<0+half_h) setAcceleration(xa,ya+force_acce);
    else if(x>Game::FrameWidth-half_w) setAcceleration(xa-force_acce,ya);
    else if(y>Game::FrameHeight-half_h) setAcceleration(xa,ya-force_acce);
    Enemy::move();
}
