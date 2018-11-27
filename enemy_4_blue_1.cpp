#include "enemy_4_blue_1.h"
#include "enemy_temp.h"
#include "game.h"

Enemy_4_Blue_1::Enemy_4_Blue_1(Character* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x, double y, double xv, double yv, double xa, double ya, bool bounceable, bool stopable)
    :Enemy_4_Blue(player,160,health,radius,shoot_cd,shoot_cd_init,x,y,xv,yv,xa,ya,bounceable,stopable)
{
    first = true;
    rng = new SpreadRNG(0,bullet_count-1);
    for(int i=0;i<bullet_count;++i) {
        boat_bullets[i]=nullptr;
        img_num[i]=6;
    }
    this->setBossHPToSkill();
}
void Enemy_4_Blue_1::skill() {
    //second phase
    testIfSecPhase([this](){
        invulnerable=true;
        img=":/res/enemy/4/blue_2.png";
        shoot_timer = -200;
        shoot_cd = 350;
        skill_timer = -420;
        emit useSkill("忒修斯之船");
    },
    [this](){
        //skill
        if(skill_timer==-220) moveTo(Game::FrameWidth/2,250,150);
        //skill timer
        if(skill_timer<=0) ++skill_timer;
    },
    [this](){
        Enemy_4_Blue::skill();
    });
}

QPoint Enemy_4_Blue_1::bulletPos(int bulletNum) const {
    switch(bulletNum) {
    default:
        return QPoint((bulletNum+1)*(Game::FrameWidth/(bullet_count+1)),this->y);
    }
}

std::vector<Bullet*>* Enemy_4_Blue_1::shoot2() {
    if(shoot_timer>=shoot_cd) {
        int constexpr bullet_radius=12;
        std::vector<Bullet*>* new_bullets=new std::vector<Bullet*>;
        Bullet* new_bullet;
        if(first) {
            emit shakeScreen(static_cast<short>(shakeLevel::largeShake));
            for(int i=0;i<bullet_count;++i) {
                boat_bullets[i] = new_bullet = new Bullet(QString(":/res/bullet/2/purple.png"),bullet_radius,x,y);
                new_bullet->moveTo(this->bulletPos(i),250);
                new_bullet->addTimeData(250);
                new_bullet->setInvulnerable();
                connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
                new_bullets->push_back(new_bullet);
            }
            first=false;
        } else {
            this->setVulnerable();
            int launchPosNum[3];
            for(int i=0;i<3;++i) {
                //choose 3 launch points
                bool rechoose;
                do {
                    launchPosNum[i] = rng->generate();
                    rechoose=false;
                    for(int j=0;j<i;++j) if(launchPosNum[j] == launchPosNum[i]) rechoose=true;
                } while(rechoose);
                //spawn bullets
                int bullet_x = boat_bullets[launchPosNum[i]]->getX(), bullet_y = boat_bullets[launchPosNum[i]]->getY();
                double angle = angleofvector(player->getX()-bullet_x,player->getY()-bullet_y);
                double sin = std::sin(angle), cos = std::cos(angle);
                double bullet_v = qrand()%10/10.0, bullet_a =0.006;
                QString bullet_img = rainbowBullet(img_num[launchPosNum[i]]);
                new_bullet = new Bullet(bullet_img,bullet_radius,bullet_x,bullet_y,bullet_v*cos,bullet_v*sin,bullet_a*cos,bullet_a*sin);
                connect(this,SIGNAL(killItsBullets()),new_bullet,SLOT(killItself()));
                new_bullets->push_back(new_bullet);
                //change old bullet
                Bullet *oldBullet = boat_bullets[launchPosNum[i]];
                if(++img_num[launchPosNum[i]]>=7) img_num[launchPosNum[i]]=0;
                oldBullet->setImg(rainbowBullet(img_num[launchPosNum[i]]));
                oldBullet->fadein();
            }
            this->shoot_cd = 30;
        }
        shoot_timer = 0;
        return new_bullets;
    }
    return nullptr;
}

QString Enemy_4_Blue_1::rainbowBullet(int i) const{
    QString str;
    switch(i) {
    case 0:
        str=":/res/bullet/2/pink.png";
        break;
    case 1:
        str=":/res/bullet/2/red.png";
        break;
    case 2:
        str=":/res/bullet/2/yellow.png";
        break;
    case 3:
        str=":/res/bullet/2/green.png";
        break;
    case 4:
        str=":/res/bullet/2/blue.png";
        break;
    case 5:
        str=":/res/bullet/2/indigo.png";
        break;
    case 6:
        str=":/res/bullet/2/purple.png";
        break;
    }
    return str;
}
