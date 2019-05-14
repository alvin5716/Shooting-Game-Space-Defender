#include "environment_2.h"
#include "bullet_effect.h"
#include "spreadrng.h"

Environment_2::Environment_2(Player *player, int shoot_cd, int lifetime)
    :Environment(player,shoot_cd,lifetime)
{
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
    rng = new SpreadRNG(0,4);
}

std::vector<Bullet*>* Environment_2::shoot() {
    if(shoot_timer==shoot_cd) {
        static const int period=8.7;
        std::vector<Bullet*>* new_bullets=new std::vector<Bullet*>;
        Bullet* new_bullet;
        //bullet v, a and count
        double bias_k = (qrand()%100/100.0)*period;
        double bias_i = qrand()%100/100.0;
        int r = rng->generate();
        int center_x = qrand()%160+r*160-400+Game::FrameWidth/2, center_y = qrand()%50-130;
        //shoot
        for(int j=0; j<3; ++j) {
            for(int i=0;i<=25;++i) {
                //trig func
                double k = i+bias_k;
                double pos_len_scale = std::sin(k/period*M_PI*2);
                //vec
                double shoot_x_vec = 30 * (i-12.5+j*0.5+bias_i);
                double shoot_y_vec = 30*pos_len_scale + (30+j*10)*(-std::pow(i/12.5-1.0,2)+1);
                //rotate
                double center_std_ang = angleofvector(Game::FrameWidth/2-center_x, Game::FrameHeight-center_y);
                double center_rotate_ang = center_std_ang - M_PI/2;
                double cosr = std::cos(center_rotate_ang), sinr = std::sin(center_rotate_ang);
                double shoot_x_vec_rotate = cosr * shoot_x_vec - sinr * shoot_y_vec;
                double shoot_y_vec_rotate = sinr * shoot_x_vec + cosr * shoot_y_vec;
                double bullet_x = center_x + shoot_x_vec_rotate, bullet_y = center_y + shoot_y_vec_rotate;
                //radius
                double bullet_radius = 15 - j*3;
                //shoot angle
                double shoot_ang = center_std_ang + ((i+bias_i)>12.5?-1:1)*std::abs((i+bias_i)/12.5-1.0)*M_PI*(70/180.0);
                double cos = std::cos(shoot_ang), sin = std::sin(shoot_ang);
                if(bullet_y<0 && sin<=0) continue;
                double land_x = bullet_x + (-bullet_y/sin*cos);
                double land_x_2 = bullet_x + ((-bullet_y+Game::FrameHeight)/sin*cos);
                if(bullet_y<0 && !((land_x>0-bullet_radius && land_x<Game::FrameWidth+bullet_radius)
                        || (land_x_2>0-bullet_radius && land_x_2<Game::FrameWidth+bullet_radius))) continue;
                //v, a
                double bullet_v = 2.8 + j*0.35 + pos_len_scale*0.035, bullet_a = 0.0028 + j*0.0035 + pos_len_scale*0.000245;
                Bullet_Effect* new_bullet_effect;
                new_bullet_effect = new Bullet_Effect(QString(":/res/bullet/5/white.png"),bullet_radius,this->scene(),":/res/bullet/5/water.png",2.4,
                                                      bullet_x,bullet_y,bullet_v*cos,bullet_v*sin,bullet_a*cos,bullet_a*sin);
                Effect* eff = new_bullet_effect->getEffect();
                if(eff!=nullptr) {
                    eff->setOpacity(0.75);
                    eff->setRotation(shoot_ang/M_PI*180);
                }
                new_bullet = new_bullet_effect;
                new_bullet->waitUntilInFrame(250);
                connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
                new_bullets->push_back(new_bullet);
            }
        }
        shoot_timer = 0;
        return new_bullets;
    }
    return nullptr;
}
