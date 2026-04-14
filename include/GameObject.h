#pragma once
#include <raylib.h>

// 基类：GameObject
class GameObject {
protected:
    Vector2 position;
public:
    GameObject(Vector2 pos) : position(pos) {}
    virtual ~GameObject() {}
    Vector2 getPosition() const { return position; }
    void setPosition(Vector2 pos) { position = pos; }
    virtual void draw() const {}
};

// 物理类
class PhysicalObject : virtual public GameObject {
protected:
    Vector2 velocity;
    float radius;
public:
    PhysicalObject(Vector2 pos, Vector2 vel, float r)
        : GameObject(pos), velocity(vel), radius(r) {}
    virtual ~PhysicalObject() {}
    Vector2 getVelocity() const { return velocity; }
    void setVelocity(Vector2 vel) { velocity = vel; }
    float getRadius() const { return radius; }
    void updatePosition(float dt) {
        position.x += velocity.x * dt;
        position.y += velocity.y * dt;
    }
};

// 视觉类
class VisualObject : virtual public GameObject {
protected:
    Color color;
    bool visible;
public:
    VisualObject(Vector2 pos, Color col, bool vis = true)
        : GameObject(pos), color(col), visible(vis) {}
    virtual ~VisualObject() {}
    Color getColor() const { return color; }
    bool isVisible() const { return visible; }
    void setVisible(bool vis) { visible = vis; }
    void draw() const override {}
};

// 球
class Ball : public PhysicalObject, public VisualObject {
private:
    int scoreValue;
public:
    Ball(Vector2 pos, Vector2 vel, float r, Color col, int score = 10)
        : GameObject(pos),
          PhysicalObject(pos, vel, r),
          VisualObject(pos, col),
          scoreValue(score) {}

    void draw() const override {
        if (isVisible()) {
            DrawCircleV(position, radius, getColor());
        }
    }

    void checkWallCollision(int w, int h) {
        if (position.x - radius <= 0 || position.x + radius >= w) velocity.x *= -1;
        if (position.y - radius <= 0) velocity.y *= -1;
    }

    int getScore() const { return scoreValue; }
};

// 砖块
class Bricks : public VisualObject {
private:
    float width, height;
    bool destroyed;
public:
    Bricks(Vector2 pos, float w, float h, Color col)
        : GameObject(pos),
          VisualObject(pos, col),
          width(w), height(h), destroyed(false) {}

    void draw() const override {
        if (!destroyed) {
            DrawRectangle((int)position.x, (int)position.y, (int)width, (int)height, getColor());
        }
    }

    bool isDestroyed() const { return destroyed; }
    void setDestroyed(bool d) { destroyed = d; setVisible(!d); }

    Rectangle getRect() const {
        return { position.x, position.y, width, height };
    }
};

// 挡板
class Paddle : public PhysicalObject {
private:
    float width, height;
public:
    Paddle(Vector2 pos, Vector2 vel, float w, float h)
        : GameObject(pos),
          PhysicalObject(pos, vel, h / 2),
          width(w), height(h) {}

    void draw() const override {
        DrawRectangle((int)(position.x - width/2), (int)position.y, (int)width, (int)height, WHITE);
    }

    Rectangle getRect() const {
        return { position.x - width/2, position.y, width, height };
    }

    void moveLeft(float dt) {
        position.x -= velocity.x * dt;
    }

    void moveRight(float dt, int screenW) {
        position.x += velocity.x * dt;
        if (position.x + width/2 > screenW) position.x = screenW - width/2;
    }
};

// ==========================
// 🎁 道具类（三种效果）
// ==========================
enum PowerUpType {
    PADDLE_GROW,    // 加长挡板
    BALL_SPLIT,     // 小球分裂
    EXTRA_LIFE      // 额外生命
};

class PowerUp : public VisualObject, public PhysicalObject {
private:
    PowerUpType type;
    float fallSpeed;
public:
    PowerUp(Vector2 pos, PowerUpType t)
        : GameObject(pos),
          VisualObject(pos, YELLOW),
          PhysicalObject(pos, {0, 200}, 12),
          type(t), fallSpeed(200) {}

    void update(float dt) {
        position.y += fallSpeed * dt;
    }

    void draw() const override {
        if (type == PADDLE_GROW) DrawCircleV(position, radius, GREEN);
        if (type == BALL_SPLIT) DrawCircleV(position, radius, ORANGE);
        if (type == EXTRA_LIFE) DrawCircleV(position, radius, PINK);
    }

    PowerUpType getType() const { return type; }
    Rectangle getRect() const {
        return { position.x - radius, position.y - radius, radius*2, radius*2 };
    }
};