#ifndef CHARACTER_H
#define CHARACTER_H

#include <QString>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <vector>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <cmath>
#include <algorithm>
#include "game.h"

class Character : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    Character() = delete;
    Character(QString img, int img_w, int img_h, int show_w, int show_h, int health, int radius, double x, double y, double xv, double yv, double xa, double ya);
    void setPosition(double x, double y);
    void setPosition(QPoint pos);
    void setSpeed(double xv, double yv);
    void setAcceleration(double xa, double ya);
    double getX() const;
    double getY() const;
    double getRadius() const;
    int getHealth() const;
    QString getImg() const;
    bool isDead() const;
    bool isInvulnerable() const;
    bool isAttackable() const;
    inline double angleofsincos(double sin, double cos) const;
    double angleofvector(double x, double y) const;
    virtual Character* testAttackedBy(std::vector<Character*> & attackers);
    virtual Character* testAttackedBy(Character* attacker);
    virtual void attacked();
    void moveTo(double x, double y, double t=63);
    void moveTo(QPoint pos, double t=63);
    void moveWith(Character* origin);
    void whiteize(int time=30);
    void setCanBeMirrored(bool canBeMirrored=true);
    void setFaceToLeft(bool face_to_left=true);
    virtual void setImg(QString img);
    void setShowSize(int show_w, int show_h);
    void show_img_set();
    void show_img_force_set();
    virtual void setFloatable(bool floatable=true);
    virtual bool floating() const;
    virtual double imgX() const;
    virtual double imgY() const;
    void bounceAtX();
    void bounceAtY();
    ~Character();
signals:
    void deadSignal();
    void deadSignal(int, int);
    void useSkill(QString);
    void healthChanged(int);
    void killItsBullets();
    void bouncedAtX();
    void bouncedAtY();
    void soundPlay(Game::Sound) const;

public slots:
    void fadein(int time=800);
    void fadeout(int time=500);
    virtual void move();
    virtual void img_move();
    virtual void setVulnerable();
    virtual void setInvulnerable();
    void whiteizedFinish();
    virtual void killItself();
protected:
    static constexpr short float_distance=6;
    short float_timer;
    int radius, health, img_w, img_h, show_w, show_h, img_timer;
    double x, y, xv, yv, xa, ya;
    bool dead, invulnerable, whiteized, face_to_left, canBeMirrored, attackable, floatable;
    QString img;
private:
    static int character_count;
    bool show_img_setting;
    Character* origin;
};

#endif // CHARACTER_H
