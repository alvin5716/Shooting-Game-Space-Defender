#include "environment.h"

Environment::Environment(const QString &img, Player* player, int shoot_cd, int lifetime)
    :Enemy(img,375,450,0,0,player,1,1,shoot_cd,1,Game::FrameWidth/2,Game::FrameHeight/2),
      lifetimer(lifetime)
{
    this->attackable=false;
    this->setInvulnerable();
    this->fadein(2000);
    this->setDisappearTime(2000);
    this->show_img_set();
    this->setZValue(Game::ZValueImportantBackEffect);
}

Environment::Environment(int pixmap, Player* player, int shoot_cd, int lifetime)
    :Enemy(pixmap,375,450,0,0,player,1,1,shoot_cd,1,Game::FrameWidth/2,Game::FrameHeight/2),
      lifetimer(lifetime)
{
    this->attackable=false;
    this->setInvulnerable();
    this->fadein(2000);
    this->setDisappearTime(2000);
    this->show_img_set();
    this->setZValue(Game::ZValueImportantBackEffect);
}

void Environment::move() {
    Enemy::move();
    if(--lifetimer<=0) dead=true;
}

void Environment::img_move() {
    return;
}
