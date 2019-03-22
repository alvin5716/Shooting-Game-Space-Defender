#include "bulletdatawall.h"
#include "bullet.h"
#include "game.h"
#include <QDebug>

BulletDataWall::BulletDataWall(Bullet* bullet)
    :BulletData(bullet), type(BulletDataWallType::freeze)
{
}
BulletDataWall::BulletDataWall(Bullet* bullet, bool infinite_bounce, bool bounce_when_hit_downside)
    :BulletData(bullet), type(BulletDataWallType::bounce)
{
    this->infinite = infinite_bounce;
    this->data.bounce.infinite=infinite_bounce;
    this->data.bounce.bounce_when_hit_downside=bounce_when_hit_downside;
}
BulletDataWall::BulletDataWall(Bullet* bullet, double vertical_v, double vertical_a)
    :BulletData(bullet), type(BulletDataWallType::vertical)
{
    this->data.vertical.v=vertical_v;
    this->data.vertical.a=vertical_a;
}
BulletDataWall::BulletDataWall(Bullet* bullet, Player* player, double v)
    :BulletData(bullet), type(BulletDataWallType::magicStone)
{
    this->infinite = true;
    this->data.magicStone.player=player;
    this->data.magicStone.v=v;
}
BulletDataWall::BulletDataWall(Bullet* bullet, Player* player, double v, double a)
    :BulletData(bullet), type(BulletDataWallType::shootAtPlayer)
{
    this->data.shootAtPlayer.player=player;
    this->data.shootAtPlayer.v=v;
    this->data.shootAtPlayer.a=a;
}
bool BulletDataWall::skill() {
    //move object
    const int x = this->bullet->getX(), y = this->bullet->getY(), radius = this->bullet->getRadius();
    if(!already_enter && (x>0+radius && x<Game::FrameWidth-radius && y>0+radius && y<Game::FrameHeight-radius)) already_enter=true;
    if(already_enter) {
        switch (this->type) {
        case BulletDataWallType::freeze:
            if(x<0 || x>Game::FrameWidth || y<0 || y>Game::FrameHeight) {
                this->bullet->setSpeed(0,0);
                this->bullet->setAcceleration(0,0);
                return true;
            }
            break;
        case BulletDataWallType::vertical:
            if(x<0+radius || x>Game::FrameWidth-radius || y<0+radius || y>Game::FrameHeight-radius) {
                if(x<0+radius) {
                    this->bullet->setSpeed(this->data.vertical.v,0);
                    this->bullet->setAcceleration(this->data.vertical.a,0);
                } else if(x>Game::FrameWidth-radius) {
                    this->bullet->setSpeed(-this->data.vertical.v,0);
                    this->bullet->setAcceleration(-this->data.vertical.a,0);
                }
                if(y<0+radius) {
                    this->bullet->setSpeed(0,this->data.vertical.v);
                    this->bullet->setAcceleration(0,this->data.vertical.a);
                } else if(y>Game::FrameHeight-radius) {
                    this->bullet->setSpeed(0,-this->data.vertical.v);
                    this->bullet->setAcceleration(0,-this->data.vertical.a);
                }
                return true;
            }
            break;
        case BulletDataWallType::bounce:
            if(x<0+radius || x>Game::FrameWidth-radius || y<0+radius || (this->data.bounce.bounce_when_hit_downside && y>Game::FrameHeight-radius)) {
                if(x<0+radius || x>Game::FrameWidth-radius) {
                    this->bullet->bounceAtX();
                }
                if(y<0+radius || (this->data.bounce.bounce_when_hit_downside && y>Game::FrameHeight-radius)) {
                    this->bullet->bounceAtY();
                }
                this->bullet->setAcceleration(0,0);
                return true;
            }
            break;
        case BulletDataWallType::magicStone:
            if(x<0+radius || x>Game::FrameWidth-radius || y<0+radius || y>Game::FrameHeight-radius) {
                if(y<0+radius) {
                    double angle = this->bullet->angleofvector(
                                this->data.magicStone.player->getX()-x,
                                this->data.magicStone.player->getY()-y);
                    if(!(angle>M_PI/20 && angle<M_PI*19/20)) angle = M_PI*(qrand()%19+1)/20;
                    double sin = std::sin(angle);
                    double cos = std::cos(angle);
                    this->bullet->setSpeed(this->data.magicStone.v*cos,this->data.magicStone.v*sin);
                }
                if(x<0+radius || x>Game::FrameWidth-radius) {
                    this->bullet->bounceAtX();
                }
                if(y>Game::FrameHeight-radius) {
                    this->bullet->bounceAtY();
                }
                this->bullet->setAcceleration(0,0);
                return true;
            }
            break;
        case BulletDataWallType::shootAtPlayer:
            if(x<0+radius || x>Game::FrameWidth-radius || y<0+radius || (this->data.bounce.bounce_when_hit_downside && y>Game::FrameHeight-radius)) {
                double angle = this->bullet->angleofvector(
                            this->data.shootAtPlayer.player->getX()-x,
                            this->data.shootAtPlayer.player->getY()-y);
                double sin = std::sin(angle);
                double cos = std::cos(angle);
                this->bullet->setSpeed(this->data.shootAtPlayer.v*cos,this->data.shootAtPlayer.v*sin);
                this->bullet->setAcceleration(this->data.shootAtPlayer.a*cos,this->data.shootAtPlayer.a*sin);
                return true;
            }
            break;
        default:
            qDebug() << "error: can't get the type of BulletDataWall";
        }
    }
    return false;
}
