#include "environment.h"

Environment::Environment(Player* player, int shoot_cd, int lifetime)
    :Enemy_Temp(nullptr,nullptr,player,shoot_cd,1,Game::FrameWidth/2,Game::FrameHeight/2),
      lifetimer(lifetime)
{

}

void Environment::move() {
    Enemy_Temp::move();
    if(--lifetimer<=0) dead=true;
}
