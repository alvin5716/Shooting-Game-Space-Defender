#include "enemy_4_blue_5.h"
#include "game.h"
#include <cfloat>
#include "enemy_4_pink.h"

QPointF Enemy_4_Blue_5::corpse_pos(Game::FrameWidth/2,120);

Enemy_4_Blue_5::Enemy_4_Blue_5(Player* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x, double y, double xv, double yv, double xa, double ya, bool bounceable, bool stopable)
    :Enemy_4_Blue(player,150,health,radius,shoot_cd,shoot_cd_init,x,y,xv,yv,xa,ya,bounceable,stopable)
{
    point+=10;
    prep_count=0;
    setDisappearTime(-1);
    setSummonCD(200);
    setFreezeWhenDied(true);
}
void Enemy_4_Blue_5::skill() {
    double angle_seed = qrand()%5;
    //second phase
    testIfSecPhase([this](){
        invulnerable=true;
        img=":/res/enemy/4/blue_2.png";
        shoot_timer = -150;
        shoot_cd = 120;
        skill_timer = -210;
        emit useSkill("「遠距傳送悖論」");
        emit killOtherEnemies(this);
        emit killAllBullets();
    },
    [this](){
        //skill
        if(skill_timer==-110) moveTo(Game::FrameWidth/2,100,125);
        else if(skill_timer==15) emit dialogueStart();
        //skill timer
        if(skill_timer<=15) ++skill_timer;
        else {
            setSpeed(player->getX()>this->x?DBL_MIN:-DBL_MIN,0);
            this->show_img_force_set();
        }
    },
    [this,angle_seed](){
        Enemy_4_Blue::skill();
        if(summon_timer>0) --summon_timer;
        else if(small_enemy==nullptr || small_enemy->isDead()) {
            double angle = ((4.5-angle_seed)/10.0)/5*M_PI;
            angle = (player->getX()<this->x?angle+M_PI/7:-angle-M_PI/7) - M_PI/2;
            double cos = std::cos(angle);
            double sin = std::sin(angle);
            small_enemy = new Enemy_4_Pink(player,6,40,5,5,this->x,this->y,1*cos,1*sin,-0.025*cos,-0.025*sin);
            small_enemy->fadein(1000);
            small_enemy->noPoint();
            connect(this,SIGNAL(useSkill(QString)),small_enemy,SLOT(killItself()));
            connect(small_enemy,SIGNAL(deadSignal()),this,SLOT(small_enemy_died()));
            emit summonEnemy(small_enemy);
        }
    });
}
std::vector<Bullet*>* Enemy_4_Blue_5::shoot2() {
    auto single_shoot = [this](std::vector<Bullet*>* new_bullets, bool snipe)->void{
        Bullet* new_bullet;
        const int bullet_count=7;
        double sin, cos;
        const double angle=angleofvector(player->getX()-x,player->getY()-y) + (snipe?0:M_PI/bullet_count);
        const int bullet_radius = 10;
        std::vector<QPointF> shoot_points = {
            QPointF(14,-9), //head
            QPointF(-12,27), //body 1
            QPointF(16,27), //body 2
            QPointF(56,11), //wing 1
            QPointF(-58,14) //wing 2
        };
        for(QPointF shoot_point: shoot_points) {
            const int rand_seed = qrand()%8;
            const double bullet_a = 0.003+rand_seed/8000.0, bullet_v_t = 1.8+rand_seed/10.0;
            for(int i=-(bullet_count/2);i<=(bullet_count/2-((bullet_count%2==0)?1:0));++i) {
                double shoot_ang = angle+(i+qrand()%50/50.0)*2*M_PI/bullet_count;
                cos = std::cos(shoot_ang);
                sin = std::sin(shoot_ang);
                const int d = qrand()%30;
                new_bullet = new Bullet(QString(":/res/bullet/4/purple.png"),bullet_radius,shoot_point.x()*(face_to_left?-1:1)+this->x+d*cos,shoot_point.y()+this->y+d*sin);
                new_bullet->fadein(500);
                new_bullet->addTimeData(31,0,0,bullet_a*cos,bullet_a*sin);
                new_bullet->setVTerminal(bullet_v_t);
                connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
                new_bullets->push_back(new_bullet);
            }
        }
    };
    if(shoot_timer==shoot_cd) {
        //shoot
        std::vector<Bullet*>* new_bullets=new std::vector<Bullet*>;
        single_shoot(new_bullets, true);
        //move
        this->fadeout(500);
        //return
        return new_bullets;
    } if(shoot_timer>=shoot_cd+31) {
        //shoot
        std::vector<Bullet*>* new_bullets=new std::vector<Bullet*>;
        single_shoot(new_bullets, false);
        //move
        if(prep_count>=2) setVulnerable();
        else ++prep_count;
        double distance = std::sqrt(std::pow(this->x-player->getX(),2) + std::pow(this->y-player->getY(),2));
        if(distance > max_distance) {
            double angle=angleofvector(player->getX()-x,player->getY()-y);
            double cos = std::cos(angle), sin = std::sin(angle);
            setPosition(this->x+max_distance*cos,this->y+max_distance*sin);
        } else setPosition(player->getX(),player->getY());
        this->fadein(500);
        //cd change
        if(shoot_cd==120 && health<=150) shoot_cd=100;
        else if(shoot_cd==100 && health<=100) shoot_cd=85;
        else if(shoot_cd==85 && health<=50) shoot_cd=70;
        //return
        shoot_timer=0;
        return new_bullets;
    }
    return nullptr;
}
Effect* Enemy_4_Blue_5::disappear() {
    if(!player->isMaxHealth()) setDeathImg(":/res/enemy/4/blue_3.png",181,142);
    else setDeathImg(":/res/enemy/4/blue.png",200,153);
    Effect* corpse = Enemy::disappear();
    corpse_pos.setX(corpse->getX());
    corpse_pos.setY(corpse->getY());
    return corpse;
}
QPointF Enemy_4_Blue_5::getCorpsePos() {
    return corpse_pos;
}
