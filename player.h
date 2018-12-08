#ifndef PLAYER_H
#define PLAYER_H
#include "character.h"
#include "bullet.h"
#include <QTimer>

class Player : public Character
{
    Q_OBJECT
public:
    Player(int health, int radius, int shoot_cd, double x=0, double y=0, double xv=0, double yv=0, double xa=0, double ya=0);
    Bullet* shoot();
    bool isShooting() const;
    bool isUsingSkill() const;
    void setShooting(bool shooting);
    Character* testAttackedBy(std::vector<Character*> & attackers);
    void attacked();
    void coolDown();
    void gameEndSetting();
    void setHealthTo999();
    static double speed;
    static constexpr double nonShootingSpeed = 7;
    static constexpr double shootingSpeed = 3;
signals:
    void healthColorChange(QString);
public slots:
    void move();
    void setVulnerable();
    void setInvulnerable();
private:
    bool shooting;
    int shoot_timer, shoot_cd, skill_timer, skill_cd;
    unsigned int vulnerable_func_count;
    QTimer *invulnerable_timer;
};
#endif // PLAYER_H
