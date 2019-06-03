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

void ImgPixmap::loadLevel(int level) {
    switch(level) {
    case 0:
        this->level = level;
        this->deletePixmaps();
        break;
    case 1:
        this->level = level;
        this->deletePixmaps();
        pixmaps.reserve(level1_count);
        pixmaps.at((int) Global::bg_normal) = new QPixmap(":/res/bg/normal");
        pixmaps.at((int) Global::effect_dot) = new QPixmap(":/res/effect/dot");
        pixmaps.at((int) Global::effect_shield1) = new QPixmap(":/res/effect/shield1");
        pixmaps.at((int) Global::effect_shield2) = new QPixmap(":/res/effect/shield2");
        pixmaps.at((int) Level1::bg_boss) = new QPixmap(":/res/bg/boss");
        pixmaps.at((int) Level1::bullet_1_black) = new QPixmap(":/res/bullet/1/black");
        pixmaps.at((int) Level1::bullet_1_blue) = new QPixmap(":/res/bullet/1/blue");
        pixmaps.at((int) Level1::bullet_1_green) = new QPixmap(":/res/bullet/1/green");
        pixmaps.at((int) Level1::bullet_1_pink) = new QPixmap(":/res/bullet/1/pink");
        pixmaps.at((int) Level1::bullet_1_pink) = new QPixmap(":/res/bullet/1/purple");
        pixmaps.at((int) Level1::bullet_1_red) = new QPixmap(":/res/bullet/1/red");
        pixmaps.at((int) Level1::bullet_1_white) = new QPixmap(":/res/bullet/1/white");
        pixmaps.at((int) Level1::bullet_1_yellow) = new QPixmap(":/res/bullet/1/yellow");
        pixmaps.at((int) Level1::bullet_laser_blue) = new QPixmap(":/res/bullet/laser/blue");
        pixmaps.at((int) Level1::bullet_laser_black) = new QPixmap(":/res/bullet/laser/black");
        pixmaps.at((int) Level1::effect_lightball) = new QPixmap(":/res/effect/lightball");
        pixmaps.at((int) Level1::enemy_1_blue) = new QPixmap(":/res/enemy/1/blue");
        pixmaps.at((int) Level1::enemy_1_blue_2) = new QPixmap(":/res/enemy/1/blue_2");
        pixmaps.at((int) Level1::enemy_1_blue_3) = new QPixmap(":/res/enemy/1/blue_3");
        pixmaps.at((int) Level1::enemy_1_green) = new QPixmap(":/res/enemy/1/green");
        pixmaps.at((int) Level1::enemy_1_pink) = new QPixmap(":/res/enemy/1/pink");
        pixmaps.at((int) Level1::enemy_1_red) = new QPixmap(":/res/enemy/1/red");
        pixmaps.at((int) Level1::enemy_1_yellow) = new QPixmap(":/res/enemy/1/yellow");
        break;
    case 2: case 3: case 4: case 5:
        this->level = level;
        this->deletePixmaps();
        pixmaps.reserve(level1_count);
        pixmaps.at((int) Global::bg_normal) = new QPixmap(":/res/bg/normal");
        pixmaps.at((int) Global::effect_dot) = new QPixmap(":/res/effect/dot");
        pixmaps.at((int) Global::effect_shield1) = new QPixmap(":/res/effect/shield1");
        pixmaps.at((int) Global::effect_shield2) = new QPixmap(":/res/effect/shield2");
        break;
    default:
        qDebug() << "Can't load level: " << level;
    }
}

QPixmap* ImgPixmap::getPixmap(int pixmap) const{
    if(pixmap >= global_count) {
        if((unsigned int) pixmap >= pixmaps.size()) {
            qDebug() << "Pixmap doesn't exist: " << pixmap;
            return nullptr;
        } else if(pixmap < global_count+level1_count) {
            if(this->level != 1) {
                qDebug() << "Level doesn't fit:\n"
                         << "Level now is " << this->level
                         << ", but the pixmap you use is for level 1";
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
