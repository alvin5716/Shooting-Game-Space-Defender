#include "enemy_4_blue_2.h"
#include "enemy_temp.h"
#include "game.h"

Enemy_4_Blue_2::Enemy_4_Blue_2(Character* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x, double y, double xv, double yv, double xa, double ya, bool bounceable, bool stopable)
    :Enemy_4_Blue(player,140,health,radius,shoot_cd,shoot_cd_init,x,y,xv,yv,xa,ya,bounceable,stopable)
{
    shoot_count=0;
    rng = new SpreadRNG(0,99);
}
void Enemy_4_Blue_2::skill() {
    //second phase
    testIfSecPhase([this](){
        invulnerable=true;
        img=":/res/enemy/4/blue_2.png";
        shoot_timer = -265;
        shoot_cd = 9;
        skill_timer = -210;
        emit useSkill("奧伯斯悖論-寰宇之光");
    },
    [this](){
        //skill
        if(skill_timer==-110) moveTo(Game::FrameWidth/2,120,75);
        else if(skill_timer==-10) {
            Effect* bang = new Effect(":/res/effect/golden_wind.png",120,120,0,0,100,Game::FrameWidth/2,120,0,0,0,0,true);
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
    [this](){
        Enemy_4_Blue::skill();
    });
}
std::vector<Bullet*>* Enemy_4_Blue_2::shoot2() {
    const int interval=6;
    if(shoot_timer>=shoot_cd && (shoot_timer-shoot_cd)%interval==0) {
        setVulnerable();
        std::vector<Bullet*>* new_bullets=new std::vector<Bullet*>;
        Bullet* new_bullet;
        int t = (shoot_timer-shoot_cd)/interval;
        double constexpr bullet_a=0.012;
        double bullet_v=1.4, bullet_op=0.9-t*0.15;
        double sin, cos;
        int bullet_radius;
        for(int i=0;i<shoot_times;++i) {
            if(shoot_timer==shoot_cd){
                xp=player->getX();
                yp=player->getY();
                angle[i]=rng->generate()*M_PI/100;
                xd[i] = 40+qrand()%120;
            }
            bullet_v+=i*0.2;
            sin = std::sin(angle[i]);
            cos = std::cos(angle[i]);
            double angle_p = angleofvector(xp-Game::FrameWidth/2,yp);
            double angle_dif = angle[i]-angle_p;
            bullet_radius = std::max(15-(int)std::abs(angle_dif*5),11)*(1-t*0.06);
            new_bullet = new Bullet(QString(":/res/bullet/other/yellow_hex.png"),bullet_radius,Game::FrameWidth/2+cos*xd[i],sin*xd[i],bullet_v*cos,bullet_v*sin);
            double constexpr sc = (double)2/std::sqrt(3);
            new_bullet->setScale(sc);
            double sinp = std::sin(angle_p), cosp = std::cos(angle_p);
            new_bullet->gravityFrom(Game::FrameWidth/2+cosp*(xd[i]+600),sinp*(xd[i]+600),bullet_a*std::pow(std::abs(angle_dif),3.2*std::sin(angle_dif)+0.15));
            new_bullet->setVTerminal(6.4);
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
