#ifndef IMGPIXMAP_H
#define IMGPIXMAP_H

#include <QPixmap>
#include <vector>

class ImgPixmap {
public:
    ImgPixmap(int level);
    void loadLevel(int level);
    QPixmap* getPixmap(int pixmap) const;
    void deletePixmaps();
    ~ImgPixmap();
private:
    int level;
    std::vector<QPixmap*> pixmaps;
public:
    static const int global_count = 4;
    enum Global {
        bg_normal = 0,
        effect_dot = 1,
        effect_shield1 = 2,
        effect_shield2 = 3
    };
    static const int level1_count = 21;
    enum Level1 {
        bg_boss = 4,
        bullet_1_black,
        bullet_1_blue,
        bullet_1_green,
        bullet_1_pink,
        bullet_1_purple,
        bullet_1_red,
        bullet_1_white,
        bullet_1_yellow,
        bullet_laser_blue,
        bullet_laser_black,
        effect_lightball,
        enemy_1_blue,
        enemy_1_blue_2,
        enemy_1_blue_3,
        enemy_1_green,
        enemy_1_pink,
        enemy_1_red,
        enemy_1_yellow
    };
    static const int all_count = global_count + level1_count;
};


#endif // IMGPIXMAP_H
