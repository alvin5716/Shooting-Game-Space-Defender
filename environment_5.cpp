#include "environment_5.h"
#include "bullet_effect.h"
#include "laser.h"

Environment_5::Environment_5(Player *player, int shoot_cd, int lifetime)
    :Environment(ImgPixmap::Level5::enemy_5_environment_1,player,shoot_cd,lifetime)
{
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
    this->laser_timer = 0;
}

std::vector<Bullet*>* Environment_5::shoot() {
    const int area_x = 7, laser_num = 6;
    const int laser_time = this->shoot_cd*3;
    static const double laser_bias_angle = M_PI/3;
    static const double frame_angle = this->angleofvector(Game::FrameWidth, Game::FrameHeight);
    static const double laser_h_ratio = std::cos(laser_bias_angle) / std::sin(laser_bias_angle+frame_angle);
    static const double laser_w_ratio = std::sin(M_PI-laser_bias_angle-frame_angle) / std::sin(laser_bias_angle);
    if(shoot_timer>=shoot_cd) {
        std::vector<Bullet*>* new_bullets=new std::vector<Bullet*>;
        Bullet* new_bullet;
        //bullet v, a and count
        static const double bullet_v = 5.6, bullet_a = 0.035;
        static const int bullet_r = 12;
        static double bias_angle = M_PI/6;
        static const int sum_x = std::sqrt(Game::FrameWidth*Game::FrameWidth + Game::FrameHeight*Game::FrameHeight)
                * std::cos(angleofvector(Game::FrameWidth,Game::FrameHeight) - bias_angle);
        static const double len_x = sum_x/area_x;
        static const double len_y = 1.5*(shoot_cd*bullet_v+shoot_cd*(shoot_cd-1)/2*bullet_a);
        static int random_max_x = (int)std::round(len_x*100);
        static int random_max_y = (int)std::round(len_y*100);
        static int random_max_x_2 = (int)std::round(len_x-2*bullet_r);
        static double cos = std::cos(bias_angle), sin = std::sin(bias_angle);
        double bias_x = qrand()%random_max_x/100.0;
        for(int j=0; j<area_x; ++j) {
            int rng = qrand();
            double x_pre = len_x*j + rng % random_max_x_2 + bias_x;
            if(x_pre>len_x*area_x) x_pre -= len_x*area_x;
            x_pre = x_pre - sum_x/2;
            double y_pre = len_y + rng % random_max_y/100.0;
            if(y_pre>len_y) y_pre -= len_y;
            y_pre = -(y_pre + bullet_r + Game::FrameHeight/2 + Game::FrameWidth/2*sin);
            double bullet_x = Game::FrameWidth/2 - cos*x_pre + sin*y_pre;
            double bullet_y = sin*x_pre + cos*y_pre + Game::FrameHeight/2;
            Bullet_Effect* new_bullet_effect;
            new_bullet_effect = new Bullet_Effect(ImgPixmap::Level5::bullet_5_white_star,bullet_r,this->scene(),ImgPixmap::Level5::bullet_5_star,2.6,
                                                  bullet_x,bullet_y,bullet_v*sin,bullet_v*cos,bullet_a*sin,bullet_a*cos);
            double img_angle = rng%360;
            new_bullet_effect->setRotation(img_angle);
            Effect* eff = new_bullet_effect->getEffect();
            if(eff!=nullptr) {
                eff->setOpacity(0.75);
                eff->setRotation(img_angle);
            }
            new_bullet = new_bullet_effect;
            new_bullet->waitUntilInFrame();
            connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
            new_bullets->push_back(new_bullet);
        }
        shoot_timer = 0;
        //laser
        if(++laser_timer > 6) {
            static const int hypo = std::sqrt(Game::FrameWidth*Game::FrameWidth + Game::FrameHeight*Game::FrameHeight);
            static const int hypo_area = hypo/laser_num;
            this->laser_timer = 0;
            for(int i=0; i < laser_num; ++i) {
                int pos_seed;
                switch (i) {
                case 0:
                    pos_seed = 50 + qrand() % (hypo_area-70);
                    break;
                case laser_num-1:
                    pos_seed = hypo_area*i + 20 + qrand() % (hypo_area-70);
                    break;
                default:
                    pos_seed = hypo_area*i + 20 + qrand() % (hypo_area-40);
                }
                double laser_x, laser_y;
                if(pos_seed <= Game::FrameHeight * laser_h_ratio) {
                    laser_x = -20;
                    laser_y = Game::FrameHeight - pos_seed / laser_h_ratio;
                } else {
                    laser_x = Game::FrameWidth - (hypo - pos_seed) * laser_w_ratio;
                    laser_y = -20;
                }
                Laser* new_laser = new Laser(":/res/bullet/laser/yellow.png",this,bullet_r,laser_bias_angle,0,laser_time,laser_x,laser_y,laser_time);
                connect(this,SIGNAL(killItsBullets()),new_laser,SLOT(killItself()));
                new_bullets->push_back(new_laser);
                if(i == 0) new_laser->setSoundEnabled();
            }
        }
        return new_bullets;
    }
    return nullptr;
}
