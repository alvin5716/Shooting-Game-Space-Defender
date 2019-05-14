#include "enemy_4_blue_3.h"
#include "enemy_temp.h"
#include "enemy_4_red.h"
#include "game.h"
#include <QDebug>

Enemy_4_Blue_3::Enemy_4_Blue_3(Player* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x, double y, double xv, double yv, double xa, double ya, bool bounceable, bool stopable)
    :Enemy_4_Blue(player,180,health,radius,shoot_cd,shoot_cd_init,x,y,xv,yv,xa,ya,bounceable,stopable)
{
    shoot_timer2=0;
    fall_count=0;
    connect(this,&Enemy_4_Blue_3::bouncedAtY,[this](){
        emit shakeScreenVertical(static_cast<short>(shakeLevel::largeShake));
        shoot_timer2=0;
        setVulnerable();
        if(fall_count<2) ++fall_count;
    });
    this->purple_shooting=false;
}
void Enemy_4_Blue_3::skill() {
    double angle_seed = qrand()%10;
    //second phase
    testIfSecPhase([this](){
        invulnerable=true;
        img=":/res/enemy/4/blue_2.png";
        shoot_timer = 0;
        shoot_cd = 32;
        skill_timer = -210;
        emit useSkill("「膽小鬼賽局」");
        emit killAllBullets();
    },
    [this](){
        //skill
        if(skill_timer==-110) moveTo(Game::FrameWidth/2,80,75);
        else if(skill_timer==-35) emit dialogueStart();
        //skill timer
        if(skill_timer<0) {
            ++skill_timer;
        } else if(skill_timer==0) {
            ++skill_timer;
            this->bounceable = true;
            this->stopable = false;
            this->setAcceleration(0,0.032);
        } else {
            if(player->getX()>this->x+this->radius) this->xa=0.04;
            else if(player->getX()<this->x-this->radius) this->xa=-0.04;
            else if(std::abs(this->xv)>0.15){
                if (this->xv>0) this->xa=-0.06;
                else if (this->xv<0) this->xa=0.06;
            } else {
                this->xa=0.0;
                this->xv = 0;
            }

            if(this->xv>2.4) this->xv=2.4;
            else if(this->xv<-2.4) this->xv=-2.4;

            if(this->y<80) {
                this->y=80;
                this->yv=0;
            }
        }
    },
    [this,angle_seed](){
        Enemy_4_Blue::skill();
        if(summon_timer>0) --summon_timer;
        else if(small_enemy==nullptr || small_enemy->isDead()) {
            double angle = ((angle_seed-4.5)/10.0)/5*M_PI;
            angle += (angle>0?M_PI/3:-M_PI/3) - M_PI/2;
            double cos = std::cos(angle);
            double sin = std::sin(angle);
            small_enemy = new Enemy_4_Red(player,6,40,75,150,this->x,this->y,5*cos,5*sin,-0.08*cos,-0.08*sin);
            small_enemy->fadein(1000);
            small_enemy->noPoint();
            connect(this,SIGNAL(useSkill(QString)),small_enemy,SLOT(killItself()));
            connect(small_enemy,SIGNAL(deadSignal()),this,SLOT(small_enemy_died()));
            emit summonEnemy(small_enemy);
        }
    });
}
std::vector<Bullet*>* Enemy_4_Blue_3::shoot2() {
    std::vector<Bullet*>* new_bullets=new std::vector<Bullet*>;
    if(shoot_timer>=shoot_cd) {
        Bullet* new_bullet;
        if(purple_shooting) {
            double const bullet_v=4.4;
            double sin, cos;
            for(int j=0;j<2;++j) {
                for(int i=0;i<2;++i) {
                    double angle = M_PI/2+(i?1:-1)*M_PI*(3-j)/15;
                    sin = std::sin(angle);
                    cos = std::cos(angle);
                    new_bullet = new Bullet(QString(":/res/bullet/2/purple.png"),16,x+cos*radius,y+sin*radius,bullet_v*cos,bullet_v*sin);
                    connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
                    new_bullets->push_back(new_bullet);
                }
            }
        }
        if(fall_count>=1 && !purple_shooting) {
            purple_shooting=true;
            shoot_timer=-100;
        } else shoot_timer=0;
    }
    const int shoot_cd2 = 100;
    const int prepTime = 70;
    if((fall_count>=2 && shoot_timer2==shoot_cd2-prepTime)||(purple_shooting && shoot_timer==shoot_cd-prepTime)) {
        PrepEffectInfo prepInfo(":/res/bullet/4/purple.png",50,50);
        prepInfo.setTime(prepTime);
        prepEffect(prepInfo);
    }
    if(fall_count>=2) {
        Bullet* new_bullet;
        if(shoot_timer2<=shoot_cd2+16) ++shoot_timer2;
        if(shoot_timer2>=shoot_cd2 && (shoot_timer2-shoot_cd2)%8==0) {
            const int bullet_count = 36;
            for(int i=-(bullet_count/2);i<=(bullet_count/2-1);++i) {
                double angle = qrand()%100/100.0*M_PI/(bullet_count/2);
                double cos = std::cos(angle+i*M_PI/(bullet_count/2));
                double sin = std::sin(angle+i*M_PI/(bullet_count/2));
                const double bullet_v = 2.4, bullet_a = 0.02;
                new_bullet = new Bullet(QString(":/res/bullet/4/purple.png"),13,x,y,bullet_v*cos,bullet_v*sin,bullet_a*cos,bullet_a*sin);
                connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
                new_bullets->push_back(new_bullet);
            }
        }
    }
    return (new_bullets->size()==0 ? nullptr : new_bullets);
}
