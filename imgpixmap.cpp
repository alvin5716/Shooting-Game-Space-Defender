#include "imgpixmap.h"
#include <QDebug>

ImgPixmap::ImgPixmap(int level):
    level(0), pixmaps(all_count)
{
    for(QPixmap*& pixmap: pixmaps) pixmap = nullptr;
    this->loadLevel(level);
}

void ImgPixmap::deletePixmaps() {
    for(QPixmap*& pixmap: pixmaps) {
        if(pixmap != nullptr) {
            delete pixmap;
            pixmap = nullptr;
        }
    }
}

void ImgPixmap::loadLevelInit(int level) {
    this->level = level;
    this->deletePixmaps();
    pixmaps.at((int) Global::bg_normal) = new QPixmap(":/res/bg/normal.png");
    pixmaps.at((int) Global::effect_dot) = new QPixmap(":/res/effect/dot.png");
    pixmaps.at((int) Global::player) = new QPixmap(":/res/player.png");
    pixmaps.at((int) Global::bullet_1_white) = new QPixmap(":/res/bullet/1/white.png");
    pixmaps.at((int) Global::effect_shield1) = new QPixmap(":/res/effect/shield1.png");
    pixmaps.at((int) Global::effect_shield2) = new QPixmap(":/res/effect/shield2.png");
}

void ImgPixmap::loadLevel(int level) {
    switch(level) {
    case 0:
        this->level = level;
        this->deletePixmaps();
        break;
    case 1:
        this->loadLevelInit(level);
        pixmaps.at((int) Level1::bg_boss) = new QPixmap(":/res/bg/boss.png");
        pixmaps.at((int) Level1::bullet_1_black) = new QPixmap(":/res/bullet/1/black.png");
        pixmaps.at((int) Level1::bullet_1_blue) = new QPixmap(":/res/bullet/1/blue.png");
        pixmaps.at((int) Level1::bullet_1_green) = new QPixmap(":/res/bullet/1/green.png");
        pixmaps.at((int) Level1::bullet_1_pink) = new QPixmap(":/res/bullet/1/pink.png");
        pixmaps.at((int) Level1::bullet_1_purple) = new QPixmap(":/res/bullet/1/purple.png");
        pixmaps.at((int) Level1::bullet_1_red) = new QPixmap(":/res/bullet/1/red.png");
        pixmaps.at((int) Level1::bullet_1_yellow) = new QPixmap(":/res/bullet/1/yellow.png");
        pixmaps.at((int) Level1::bullet_laser_blue) = new QPixmap(":/res/bullet/laser/blue.png");
        pixmaps.at((int) Level1::bullet_laser_black) = new QPixmap(":/res/bullet/laser/black.png");
        pixmaps.at((int) Level1::effect_lightball) = new QPixmap(":/res/effect/lightball.png");
        pixmaps.at((int) Level1::enemy_1_blue) = new QPixmap(":/res/enemy/1/blue.png");
        pixmaps.at((int) Level1::enemy_1_blue_2) = new QPixmap(":/res/enemy/1/blue_2.png");
        pixmaps.at((int) Level1::enemy_1_blue_3) = new QPixmap(":/res/enemy/1/blue_3.png");
        pixmaps.at((int) Level1::enemy_1_green) = new QPixmap(":/res/enemy/1/green.png");
        pixmaps.at((int) Level1::enemy_1_pink) = new QPixmap(":/res/enemy/1/pink.png");
        pixmaps.at((int) Level1::enemy_1_red) = new QPixmap(":/res/enemy/1/red.png");
        pixmaps.at((int) Level1::enemy_1_yellow) = new QPixmap(":/res/enemy/1/yellow.png");
        break;
    case 2: case 3: case 4:
        this->loadLevelInit(level);
        break;
    case 5:
        this->loadLevelInit(level);
        pixmaps.at((int) Level5::bg_boss_5) = new QPixmap(":/res/bg/boss_5.png");
        pixmaps.at((int) Level5::enemy_4_blue) = new QPixmap(":/res/enemy/4/blue.png");
        pixmaps.at((int) Level5::bullet_4_blue) = new QPixmap(":/res/bullet/4/blue.png");
        pixmaps.at((int) Level5::bullet_5_fire) = new QPixmap(":/res/bullet/5/fire.png");
        pixmaps.at((int) Level5::bullet_5_water) = new QPixmap(":/res/bullet/5/water.png");
        pixmaps.at((int) Level5::bullet_5_seed) = new QPixmap(":/res/bullet/5/seed.png");
        pixmaps.at((int) Level5::bullet_5_plant) = new QPixmap(":/res/bullet/5/plant.png");
        pixmaps.at((int) Level5::bullet_5_magic) = new QPixmap(":/res/bullet/5/magic.png");
        pixmaps.at((int) Level5::bullet_5_star) = new QPixmap(":/res/bullet/5/star.png");
        pixmaps.at((int) Level5::bullet_5_white_circle) = new QPixmap(":/res/bullet/5/white_circle.png");
        pixmaps.at((int) Level5::bullet_5_white_melt) = new QPixmap(":/res/bullet/5/white_melt.png");
        pixmaps.at((int) Level5::bullet_5_white_reflect) = new QPixmap(":/res/bullet/5/white_reflect.png");
        pixmaps.at((int) Level5::bullet_5_white_seed) = new QPixmap(":/res/bullet/5/white_seed.png");
        pixmaps.at((int) Level5::bullet_5_white_leaf) = new QPixmap(":/res/bullet/5/white_leaf.png");
        pixmaps.at((int) Level5::bullet_5_white_magic) = new QPixmap(":/res/bullet/5/white_magic.png");
        pixmaps.at((int) Level5::bullet_5_white_star) = new QPixmap(":/res/bullet/5/white_star.png");
        pixmaps.at((int) Level5::enemy_5_environment_1) = new QPixmap(":/res/enemy/5/environment_1.png");
        pixmaps.at((int) Level5::enemy_5_environment_2) = new QPixmap(":/res/enemy/5/environment_2.png");
        break;
    default:
        qDebug() << "Can't load level: " << level;
    }
}

QPixmap* ImgPixmap::getPixmap(int pixmap) const{
    if(pixmap >= level1_index) {
        if((unsigned int) pixmap >= pixmaps.size()) {
            qDebug() << "Pixmap doesn't exist: " << pixmap;
            return nullptr;
        } else if(pixmap < level2_index) {
            if(this->level != 1) {
                qDebug() << "Level doesn't fit:\n"
                         << "Level now is " << this->level
                         << ", but the pixmap you use is for level 1";
                return nullptr;
            }
        } else if(pixmap < all_count) {
            if(this->level != 5) {
                qDebug() << "Level doesn't fit:\n"
                         << "Level now is " << this->level
                         << ", but the pixmap you use is for level 5";
                return nullptr;
            }
        }
    } else if(pixmap < 0) {
        qDebug() << "Pixmap doesn't exist: " << pixmap;
        return nullptr;
    }
    return pixmaps.at(pixmap);
}

ImgPixmap::~ImgPixmap() {
    this->deletePixmaps();
}
