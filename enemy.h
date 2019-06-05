#ifndef ENEMY_H
#define ENEMY_H

#include "character.h"
#include "bullet.h"
#include <vector>
#include <QString>
#include "shield.h"
#include <functional>
#include "player.h"

typedef std::function<void()> skillFunc;

class PrepEffectInfo {
public:
    friend class Enemy;
    PrepEffectInfo(const QString &img, int img_w, int img_h);
    PrepEffectInfo& setTime(int time = 70);
    PrepEffectInfo& setScale(int scale = 2.8);
    PrepEffectInfo& setSound(bool withSound = true);
private:
    QString img;
    int img_w, img_h;
    int time, scale;
    bool withSound;
};

class Enemy : public Character
{
    Q_OBJECT
public:
    enum shakeLevel{smallShake=4,mediumShake=0,largeShake=-4};
    Enemy(int pixmap, int img_w, int img_h, int show_w, int show_h, Player *player, int health, int radius, int shoot_cd, int shoot_cd_init, double x=0, double y=0, double xv=0, double yv=0, double xa=0, double ya=0, bool bounceable=false, bool stopable=false);
    Enemy(const QString &img, int img_w, int img_h, int show_w, int show_h, Player *player, int health, int radius, int shoot_cd, int shoot_cd_init, double x=0, double y=0, double xv=0, double yv=0, double xa=0, double ya=0, bool bounceable=false, bool stopable=false);
    virtual std::vector<Bullet*>* shoot(); //return a pointer to vector, whose elements are pointer to new bullets
    virtual std::vector<Bullet*>* shoot2(); //this is only be used for boss
    bool isBoss() const;
    bool isSecPhase() const;
    virtual Effect* disappear();
    void setDisappearTime(int disappearTime);
    Shield* showShield(QString str=":/res/effect/shield2.png",int img_w=70, int img_h=70);
    void setFloatable(bool floatable=true) override final;
    void noPoint();
    void attacked() override;
    void prepEffect(PrepEffectInfo PrepInfo);
    void prepEffect(const QString &img, int img_w, int img_h);
    ~Enemy();
public slots:
    virtual void move();
    virtual void skill();
    void diedFromPlayer();
signals:
    void summonEffect(Effect*);
    void summonEnemy(Enemy*);
    void pointGive(int);
    void shakeScreen(short);
    void shakeScreenVertical(short);
    void killAllBullets();
    void killOtherEnemies(Enemy*);
    void dialogueStart(int x=0);
protected:
    void beABoss(int bossSkillHP);
    void setBossHPToSkill();
    int getBossSkillHP();
    unsigned int point;
    bool bounceable, stopable, already_enter;
    int shoot_cd, shoot_timer, skill_timer;
    Shield* shield;
    Player* player;
    int disappearTime;
    QString death_img;
    int death_pixmap;
    bool death_usePixmap;
    int death_img_w, death_img_h;
    void setDeathImg(const QString &death_img);
    void setDeathImg(int death_pixmap);
    void setDeathImg(const QString &death_img, int death_img_w, int death_img_h);
    void setDeathImg(int death_pixmap, int death_img_w, int death_img_h);
    void setFreezeWhenDied(bool freezeWhenDied = true);
    void testIfSecPhase(skillFunc initialize, skillFunc secPhaseSkill);
    void testIfSecPhase(skillFunc initialize, skillFunc secPhaseSkill, skillFunc FirPhaseSkill);
private:
    bool boss, secPhase, freezeWhenDied;
    int bossSkillHP;
};

#endif // ENEMY_H
