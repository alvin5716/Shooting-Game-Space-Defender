#include "environment_2.h"
#include "bullet_effect.h"
#include "spreadrng.h"

Environment_2::Environment_2(Player *player, int shoot_cd, int lifetime)
    :Environment(ImgPixmap::Level5::enemy_5_environment_2,player,shoot_cd,lifetime)
{
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
    rng = new SpreadRNG(0,4);
}

std::vector<Bullet*>* Environment_2::shoot() {
    const int total_t = 3;
    const int interval = 2;
    if(shoot_timer>=shoot_cd && (shoot_timer-shoot_cd)%interval==0) {
        int t = (shoot_timer-shoot_cd)/interval;
        int j = total_t-1-t;
        static const int period=8.7;
        std::vector<Bullet*>* new_bullets=new std::vector<Bullet*>;
        Bullet* new_bullet;
        //bullet v, a and count
        if(t==0) {
            bias_k = (qrand()%100/100.0)*period;
            bias_i = qrand()%100/100.0;
            r = rng->generate();
            center_x = qrand()%160+r*160-400+Game::FrameWidth/2;
            center_y = qrand()%50-160;
        }
        //rotate
        double center_std_ang = angleofvector(Game::FrameWidth/2-center_x, Game::FrameHeight-center_y);
        double center_rotate_ang = center_std_ang - M_PI/2;
        double cosr = std::cos(center_rotate_ang), sinr = std::sin(center_rotate_ang);
        //radius
        double bullet_radius = 15 - j*3;
        //shoot
        for(int i=0;i<=25;++i) {
            //trig func
            double k = i+bias_k;
            double pos_len_scale = std::sin(k/period*M_PI*2);
            //vec
            double shoot_x_vec = 30 * (i-12.5+j*0.5+bias_i);
            double shoot_y_vec = 30*pos_len_scale + (30+j*7)*(-std::pow(i/12.5-1.0,2)+1);
            //shoot angle
            double shoot_ang = center_std_ang + ((i+bias_i)>12.5?-1:1)*std::abs((i+bias_i)/12.5-1.0)*M_PI*(70/180.0);
            double cos = std::cos(shoot_ang), sin = std::sin(shoot_ang);
            //a, v
            double bullet_v = 2.8 + j*0.35 + pos_len_scale*0.035;
            double bullet_a = 0.0028 + j*0.0035 + pos_len_scale*0.000245;
            double bullet_v_x = bullet_v * cos, bullet_v_y = bullet_v * sin;
            double bullet_a_x = bullet_a * cos, bullet_a_y = bullet_a * sin;
            //rotate
            double shoot_x_vec_rotate = cosr * shoot_x_vec - sinr * shoot_y_vec;
            double shoot_y_vec_rotate = sinr * shoot_x_vec + cosr * shoot_y_vec;
            double bullet_x = center_x + shoot_x_vec_rotate;
            double bullet_y = center_y + shoot_y_vec_rotate;
            //delete bullets that will never get in the frame
            if(bullet_y<0 && sin<=0) continue;
            double land_x = bullet_x + (-bullet_y/sin*cos);
            double land_x_2 = bullet_x + ((-bullet_y+Game::FrameHeight)/sin*cos);
            if(bullet_y<0 && !((land_x>0-bullet_radius && land_x<Game::FrameWidth+bullet_radius)
                    || (land_x_2>0-bullet_radius && land_x_2<Game::FrameWidth+bullet_radius))) continue;
            //v, a
            Bullet_Effect* new_bullet_effect;
            new_bullet_effect = new Bullet_Effect(ImgPixmap::Level5::bullet_5_white_reflect,bullet_radius,this->scene(),ImgPixmap::Level5::bullet_5_water,2.4,
                                                  bullet_x,bullet_y,bullet_v_x,bullet_v_y,bullet_a_x,bullet_a_y);
            double img_angle = shoot_ang/M_PI*180;
            new_bullet_effect->setRotation(img_angle);
            Effect* eff = new_bullet_effect->getEffect();
            if(eff!=nullptr) {
                eff->setOpacity(0.75);
                eff->setRotation(img_angle);
            }
            new_bullet = new_bullet_effect;
            new_bullet->waitUntilInFrame(250);
            connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
            new_bullets->push_back(new_bullet);
        }
        if(t>=total_t-1) shoot_timer = 0;
        return new_bullets;
    }
    return nullptr;
}
