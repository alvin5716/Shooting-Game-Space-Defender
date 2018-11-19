#ifndef ENEMY_H
#define ENEMY_H

#include "character.h"
#include "bullet.h"
#include <vector>
#include <QString>
#include "shield.h"
#include <functional>

typedef std::function<void()> skillFunc;

class Enemy : public Character
{
    Q_OBJECT
public:
    enum shakeLevel{smallShake=4,mediumShake=0,largeShake=-4};
    Enemy(QString img, int img_w, int img_h, int show_w, int show_h, Character* player, int health, int radius, int shoot_cd, int shoot_cd_init, double x=0, double y=0, double xv=0, double yv=0, double xa=0, double ya=0, bool bounceable=false, bool stopable=false);
    virtual std::vector<Bullet*>* shoot(); //return a pointer to vector, whose elements are pointer to new bullets
    virtual std::vector<Bullet*>* shoot2(); //this is only be used for boss
    void outOfFrame();
    bool isBoss() const;
    bool isSecPhase() const;
    Effect* disappear() const;
    void setDisappearTime(int disappearTime);
    Shield* showShield(QString str=":/res/shield2.png",int img_w=70, int img_h=70);
    void setFloatable(bool floatable=true) override final;
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
protected:
    void beABoss(int bossSkillHP);
    void setBossHPToSkill();
    int getBossSkillHP();
    unsigned int point;
    bool bounceable, stopable, already_enter;
    int shoot_cd, shoot_timer, skill_timer;
    Shield* shield;
    Character* player;
    int disappearTime;
    QString death_img;
    void testIfSecPhase(skillFunc initialize, skillFunc secPhaseSkill);
    void testIfSecPhase(skillFunc initialize, skillFunc secPhaseSkill, skillFunc FirPhaseSkill);
private:
    bool boss, secPhase;
    int bossSkillHP;
};

#endif // ENEMY_H
