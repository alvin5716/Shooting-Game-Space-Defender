#include "environment_4.h"
#include "bullet_effect.h"

Environment_4::Environment_4(Player *player, int shoot_cd, int lifetime)
    :Environment(ImgPixmap::Level5::enemy_5_environment_1,player,shoot_cd,lifetime)
{
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
}

std::vector<Bullet*>* Environment_4::shoot() {
    const int area_x = 6;
    int t = 0;
    if(shoot_timer>=shoot_cd) {
        std::vector<Bullet*>* new_bullets=new std::vector<Bullet*>;
        Bullet* new_bullet;
        //bullet v, a and count
        static const double bullet_v = 2.8, bullet_a = 0.005;
        static const int bullet_r = 14;
        static const int sum_x = Game::FrameWidth;
        static const double len_x = sum_x/area_x;
        static const double len_y = 1.5*(shoot_cd*bullet_v+shoot_cd*(shoot_cd-1)/2*bullet_a);
        static int random_max_x = (int)std::round(len_x*100);
        static int random_max_y = (int)std::round(len_y*100);
        static int random_max_x_2 = (int)std::round(len_x-2*bullet_r);
        double bias_x = qrand()%random_max_x/100.0;
        for(int j=0; j<area_x; ++j) {
            int bullet_rand = qrand();
            double x_pre = len_x*j + bullet_rand%random_max_x_2 + bias_x;
            if(x_pre>len_x*area_x) x_pre -= len_x*area_x;
            double bullet_x = x_pre;
            double y_pre = len_y + bullet_rand%random_max_y/100.0;
            if(y_pre>len_y) y_pre -= len_y;
            double bullet_y = -y_pre-bullet_r*2.4;
            Bullet_Effect* new_bullet_effect;
            new_bullet_effect = new Bullet_Effect(ImgPixmap::Level5::bullet_5_white_magic,bullet_r,this->scene(),ImgPixmap::Level5::bullet_5_magic,2.4,
                                                  bullet_x,bullet_y,0,bullet_v,0,bullet_a);
            int T = 220;
            new_bullet_effect->moveAsTrigFunction(T,40,(bool)((t%(2*T))<T),t%T);
            t += T/10;
            Effect* eff = new_bullet_effect->getEffect();
            if(eff!=nullptr) {
                eff->setOpacity(0.75);
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
