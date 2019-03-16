#include "enemy_4_blue_2.h"
#include "enemy_temp.h"
#include "game.h"
#include "enemy_4_green.h"

Enemy_4_Blue_2::Enemy_4_Blue_2(Player* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x, double y, double xv, double yv, double xa, double ya, bool bounceable, bool stopable)
    :Enemy_4_Blue(player,140,health,radius,shoot_cd,shoot_cd_init,x,y,xv,yv,xa,ya,bounceable,stopable)
{
    shoot_count=0;
    rng = new SpreadRNG(0,99);
}
void Enemy_4_Blue_2::skill() {
    double angle_seed = qrand()%10;
    //second phase
    testIfSecPhase([this](){
        invulnerable=true;
        img=":/res/enemy/4/blue_2.png";
        shoot_timer = -265;
        shoot_cd = 12;
        skill_timer = -210;
        emit useSkill("「光度佯謬」");
        emit killAllBullets();
    },
    [this](){
        //skill
        if(skill_timer==-110) moveTo(Game::FrameWidth/2,200,75);
        else if(skill_timer==-35) emit dialogueStart();
        else if(skill_timer==-10) {
            Effect* bang = new Effect(":/res/effect/golden_wind.png",120,120,0,0,100,Game::FrameWidth/2,200,0,0,0,0,true);
            bang->zoom(800,800,100);
            bang->rotateStart();
            bang->setOpacity(0.85);
            bang->fadein(500);
            bang->setZValue(-2);
            emit summonEffect(bang);
            emit shakeScreen(static_cast<short>(shakeLevel::largeShake));
        }
        //skill timer
        if(skill_timer<=0) ++skill_timer;
        else Enemy_4_Blue::skill();
    },
    [this,angle_seed](){
        Enemy_4_Blue::skill();
        if(small_enemy_timer>0) --small_enemy_timer;
        else if(small_enemy==nullptr || small_enemy->isDead()) {
            double angle = ((angle_seed-4.5)/10.0)/5*M_PI;
            angle += (angle>0?M_PI/3:-M_PI/3) - M_PI/2;
            double cos = std::cos(angle);
            double sin = std::sin(angle);
            small_enemy = new Enemy_4_Green(player,7,40,40,63,this->x,this->y,5*cos,5*sin,-0.08*cos,-0.08*sin,true);
            small_enemy->fadein(1000);
            small_enemy->noPoint();
            connect(this,SIGNAL(useSkill(QString)),small_enemy,SLOT(killItself()));
            connect(small_enemy,SIGNAL(deadSignal()),this,SLOT(small_enemy_died()));
            emit summonEnemy(small_enemy);
        }
    });
}

std::vector<Bullet*>* Enemy_4_Blue_2::shoot2() {
    const int interval=8;
    if(shoot_timer>=shoot_cd && (shoot_timer-shoot_cd)%interval==0) {
        setVulnerable();
        std::vector<Bullet*>* new_bullets=new std::vector<Bullet*>;
        Bullet* new_bullet;
        int t = (shoot_timer-shoot_cd)/interval;
        double constexpr bullet_a=0.012;
        double bullet_v=1.2, bullet_op=0.9-t*0.15;
        double sin, cos;
        int bullet_radius;
        for(int i=0;i<shoot_times;++i) {
            if(shoot_timer==shoot_cd){
                xp=player->getX();
                yp=player->getY();
                angle[i]=rng->generate()*M_PI/100 + angleofvector(xp-Game::FrameWidth/2,yp-200) - M_PI/2;
                xd[i] = 40+qrand()%120;
            }
            bullet_v+=i*0.2;
            sin = std::sin(angle[i]);
            cos = std::cos(angle[i]);
            double angle_p = angleofvector(xp-Game::FrameWidth/2,yp-200);
            double angle_dif = angle[i]-angle_p;
            bullet_radius = std::max(15-(int)std::abs(angle_dif*5),11)*(1-t*0.06);
            new_bullet = new Bullet(QString(":/res/bullet/other/yellow_hex.png"),bullet_radius,Game::FrameWidth/2+cos*xd[i],200+sin*xd[i],bullet_v*cos,bullet_v*sin);
            double constexpr sc = (double)2/std::sqrt(3);
            new_bullet->setScale(sc);
            double sinp = std::sin(angle_p), cosp = std::cos(angle_p);
            new_bullet->gravityFrom(Game::FrameWidth/2+cosp*(xd[i]+400),200+sinp*(xd[i]+400),bullet_a*std::pow(std::abs(angle_dif),2.7*std::sin(angle_dif)+0.15));
            //new_bullet->setVTerminal(1);
            connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
            new_bullet->setOpacity(bullet_op);
            new_bullet->fadein();
            new_bullets->push_back(new_bullet);
        }
        if(shoot_timer==shoot_cd+interval*2) shoot_timer = 0;
        return new_bullets;
    }
    return nullptr;
}
