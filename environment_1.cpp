#include "environment_1.h"
#include "bullet_effect.h"

Environment_1::Environment_1(Player *player, int shoot_cd, int lifetime)
    :Environment(ImgPixmap::Level5::enemy_5_environment_1,player,shoot_cd,lifetime)
{
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
}

std::vector<Bullet*>* Environment_1::shoot() {
    const int area_x = 10;
    if(shoot_timer>=shoot_cd) {
        std::vector<Bullet*>* new_bullets=new std::vector<Bullet*>;
        Bullet* new_bullet;
        //bullet v, a and count
        static const double bullet_v = 2.6, bullet_a = 0.025;
        static const int bullet_r = 14;
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
            double x_pre = len_x*j + qrand()%random_max_x_2 + bias_x;
            if(x_pre>len_x*area_x) x_pre -= len_x*area_x;
            x_pre = x_pre - sum_x/2;
            double y_pre = len_y + qrand()%random_max_y/100.0;
            if(y_pre>len_y) y_pre -= len_y;
            y_pre = -(y_pre + bullet_r + Game::FrameHeight/2 + Game::FrameWidth/2*sin);
            double bullet_x = cos*x_pre - sin*y_pre + Game::FrameWidth/2;
            double bullet_y = sin*x_pre + cos*y_pre + Game::FrameHeight/2;
            Bullet_Effect* new_bullet_effect;
            new_bullet_effect = new Bullet_Effect(ImgPixmap::Level5::bullet_5_white_melt,bullet_r,this->scene(),ImgPixmap::Level5::bullet_5_fire,2.4,
                                                  bullet_x,bullet_y,bullet_v*-sin,bullet_v*cos,bullet_a*-sin,bullet_a*cos);
            double img_angle = bias_angle/M_PI*180 + 90;
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
        return new_bullets;
    }
    return nullptr;
}
