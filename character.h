#ifndef CHARACTER_H
#define CHARACTER_H

#include <QString>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <vector>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>

class Character : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    Character(QString img, int img_w, int img_h, int show_w, int show_h, int health, int radius, double x, double y, double xv, double yv, double xa, double ya);
    void setPosition(double x, double y);
    void setSpeed(double xv, double yv);
    void setAcceleration(double xa, double ya);
    double getX() const;
    double getY() const;
    double getRadius() const;
    int getHealth() const;
    bool isDead() const;
    bool isInvulnerable() const;
    bool isAttackable() const;
    inline double angleofsincos(double sin, double cos) const;
    double angleofvector(double x, double y) const;
    virtual Character* testAttackedBy(std::vector<Character*> & attackers);
    virtual Character* testAttackedBy(Character* attacker);
    virtual void attacked();
    void moveTo(double x, double y, double t=125);
    void whiteize(int time=60);
    void setCanBeMirrored(bool canBeMirrored=true);
    void setFaceToLeft(bool face_to_left=true);
    void setImg(QString img);
    void show_img_set();
    void show_img_force_set();
    void setFloating(bool floating=true);
    virtual double imgX() const;
    virtual double imgY() const;
    static short float_timer;
    static const short float_distance=6;
signals:
    void deadSignal();
    void deadSignal(int, int);
    void useSkill(QString);
    void healthChanged(int);
    void killItsBullets();
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
    int radius, health, img_w, img_h, show_w, show_h, img_timer;
    double x, y, xv, yv, xa, ya;
    bool dead, invulnerable, whiteized, face_to_left, canBeMirrored, attackable, floating;
    QString img;
};

#endif // CHARACTER_H
