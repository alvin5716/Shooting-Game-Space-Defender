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
    void loadLevelInit(int level);
    int level;
    std::vector<QPixmap*> pixmaps;
public:
    static const int global_count = 6;
    static const int level1_index = global_count;
    static const int level1_count = 18;
    static const int level2_index = level1_index+level1_count;
    static const int level2_count = 0;
    static const int level3_index = level2_index+level2_count;
    static const int level3_count = 0;
    static const int level4_index = level3_index+level3_count;
    static const int level4_count = 0;
    static const int level5_index = level4_index+level4_count;
    static const int level5_count = 14;
    static const int all_count = level5_index+level5_count;
    enum Global {
        bg_normal = 0,
        effect_dot,
        player,
        bullet_1_white,
        effect_shield1,
        effect_shield2
    };
    enum Level1 {
        bg_boss = level1_index,
        bullet_1_black,
        bullet_1_blue,
        bullet_1_green,
        bullet_1_pink,
        bullet_1_purple,
        bullet_1_red,
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
    enum Level5 {
        bg_boss_5 = level5_index,
        enemy_4_blue,
        bullet_4_blue,
        bullet_5_fire,
        bullet_5_water,
        bullet_5_seed,
        bullet_5_plant,
        bullet_5_white_circle,
        bullet_5_white_melt,
        bullet_5_white_reflect,
        bullet_5_white_seed,
        bullet_5_white_leaf,
        enemy_5_environment_1,
        enemy_5_environment_2
    };
};


#endif // IMGPIXMAP_H
