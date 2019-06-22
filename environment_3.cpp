#include "environment_3.h"
#include "bullet_effect.h"
#include <QDebug>

Environment_3::Environment_3(Player *player, int shoot_cd, int lifetime)
    :Environment(ImgPixmap::Level5::enemy_5_environment_1,player,shoot_cd,lifetime),
      seeds()
{
    rng_x = new SpreadRNG(0,9);
    rng_cd = new SpreadRNG(0,4);
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
}

std::vector<Bullet*>* Environment_3::shoot() {
    const int bullet_radius = 15;
    const int area_count = 10, base_x = bullet_radius/2+5;
    const int area_w = (Game::FrameWidth-2*base_x)/area_count;
    const int bullet_v = 4.6, bullet_a = 0.04;
    const int bullet_count = 16;
    const int bullet_y = -bullet_radius*3;
    std::vector<Bullet*>* new_bullets=new std::vector<Bullet*>;
    Bullet* new_bullet;
    Bullet_Effect* new_bullet_effect;
    //growing
    for(std::vector<Bullet_Seed*>::iterator i=seeds.begin();i!=seeds.end();) {
        if((*i)->died_early) {
            //delete and erase
            delete (*i);
            i = seeds.erase(i);
        } else if((*i)->triggered) {
            //kill bullet and shoot more
            (*i)->bullet->killItself();
            double angle_gen = qrand()%100/100.0*M_PI/(bullet_count/2);
            for(int j=-(bullet_count/2);j<=(bullet_count/2-1);++j) {
                double angle = angle_gen + j*M_PI/(bullet_count/2);
                double cos = std::cos(angle);
                double sin = std::sin(angle);
                new_bullet_effect = new Bullet_Effect(ImgPixmap::Level5::bullet_5_white_leaf,bullet_radius,this->scene(),ImgPixmap::Level5::bullet_5_plant,2.4,
                                                      (*i)->pos.x(),(*i)->pos.y(),bullet_v*cos,bullet_v*sin,bullet_a*cos,bullet_a*sin);
                double img_angle = angle/M_PI*180;
                new_bullet_effect->setRotation(img_angle);
                Effect* eff = new_bullet_effect->getEffect();
                if(eff!=nullptr) {
                    eff->setOpacity(0.75);
                    eff->setRotation(img_angle);
                }
                new_bullet = new_bullet_effect;
                connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
                new_bullets->push_back(new_bullet);
            }
            //delete and erase
            delete (*i);
            i = seeds.erase(i);
        } else ++i;
    }
    //seed shooting
    if(shoot_timer>=shoot_cd && lifetimer>=((double)Game::FrameHeight-bullet_y)/bullet_v) {
        //
        int x_gen = rng_x->generate();
        int bullet_x = x_gen * area_w + qrand() % area_w + base_x;
        //shoot
        new_bullet_effect = new Bullet_Effect(ImgPixmap::Level5::bullet_5_white_seed,bullet_radius,this->scene(),ImgPixmap::Level5::bullet_5_seed,3,
                                              bullet_x,bullet_y,0,bullet_v,0,bullet_a);
        double img_angle = 90;
        new_bullet_effect->setRotation(img_angle);
        new_bullet_effect->setEffectImgSize(150,150);
        Effect* eff = new_bullet_effect->getEffect();
        if(eff!=nullptr) {
            eff->setOpacity(0.75);
            eff->setRotation(img_angle);
        }
        new_bullet = new_bullet_effect;
        new_bullet->waitUntilInFrame();
        new_bullet->addWallData();
        seeds.push_back(new Bullet_Seed(new_bullet));
        connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
        new_bullets->push_back(new_bullet);
        //cd
        shoot_timer = (rng_cd->generate()-2.25+qrand()%100/200.0)*shoot_cd/6;
    }
    if(new_bullets->size()==0) return nullptr;
    return new_bullets;
}

Environment_3::~Environment_3() {
    delete this->rng_cd;
    delete this->rng_x;
}
