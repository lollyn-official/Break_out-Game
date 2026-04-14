#include "GameObject.h"
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

int lives = 1;
bool gameOver = false;

std::vector<Ball> balls;
std::vector<PowerUp> powerUps;

// 粒子：残影 / 碎片
struct Particle {
    Vector2 pos;
    Vector2 vel;
    Color color;
    float size;
    float alpha;
    float life;
};

std::vector<Particle> particles;

// 函数声明
void spawnBall(Vector2 pos, Vector2 vel);
void applyPowerUp(PowerUpType type, Paddle& paddle);
void resetGame(Paddle& paddle, std::vector<Bricks>& bricks);

// 创建砖块碎裂粒子
void spawnBrickExplosion(Vector2 pos, Color color) {
    for (int i = 0; i < 8; i++) {
        Particle p;
        p.pos = pos;
        p.vel = {
            (float)(rand() % 100 - 50) / 30.0f,
            (float)(rand() % 100 - 50) / 30.0f
        };
        p.color = color;
        p.size = (float)(rand() % 5 + 2);
        p.alpha = 1.0f;
        p.life = 0.5f;
        particles.push_back(p);
    }
}

// 创建小球残影
void spawnBallTrail(Vector2 pos, Color color) {
    Particle p;
    p.pos = pos;
    p.vel = { 0, 0 };
    p.color = color;
    p.size = 4.0f;
    p.alpha = 0.7f;
    p.life = 0.2f;
    particles.push_back(p);
}

// 更新所有粒子
void updateParticles(float dt) {
    for (size_t i = 0; i < particles.size(); ) {
        auto& p = particles[i];
        p.pos.x += p.vel.x;
        p.pos.y += p.vel.y;
        p.life -= dt;
        p.alpha = p.life / 0.5f;

        if (p.life <= 0)
            particles.erase(particles.begin() + i);
        else
            i++;
    }
}

// 绘制粒子
void drawParticles() {
    for (auto& p : particles) {
        Color c = p.color;
        c.a = (unsigned char)(p.alpha * 255);
        DrawCircleV(p.pos, p.size, c);
    }
}

int main() {
    SetTargetFPS(60);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Brick Breaker - Particle FX");
    srand((unsigned int)time(NULL));

    balls.emplace_back(Vector2{ 400, 500 }, Vector2{ 300, -300 }, 10, RED, 10);
    Paddle paddle({ 400, 580 }, Vector2{ 500, 0 }, 100, 20);

    std::vector<Bricks> bricks;
    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 8; col++) {
            bricks.emplace_back(
                Vector2{ (float)(100 + col * 80), (float)(50 + row * 30) },
                70, 20, BLUE
            );
        }
    }

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        Vector2 mouse = GetMousePosition();

        if (!gameOver) {
            if (IsKeyDown(KEY_LEFT)) paddle.moveLeft(dt);
            if (IsKeyDown(KEY_RIGHT)) paddle.moveRight(dt, SCREEN_WIDTH);

            // 球残影
            for (auto& ball : balls)
                spawnBallTrail(ball.getPosition(), ball.getColor());

            // 更新球
            for (auto& ball : balls) {
                ball.updatePosition(dt);
                ball.checkWallCollision(SCREEN_WIDTH, SCREEN_HEIGHT);
            }

            // 球碰挡板
            for (auto& ball : balls) {
                if (CheckCollisionCircleRec(ball.getPosition(), ball.getRadius(), paddle.getRect())) {
                    ball.setVelocity(Vector2{ ball.getVelocity().x, -fabs(ball.getVelocity().y) });
                }
            }

            // 球碰砖块 + 爆炸粒子
            for (auto& brick : bricks) {
                if (!brick.isDestroyed()) {
                    for (auto& ball : balls) {
                        if (CheckCollisionCircleRec(ball.getPosition(), ball.getRadius(), brick.getRect())) {
                            ball.setVelocity(Vector2{ ball.getVelocity().x, -ball.getVelocity().y });
                            brick.setDestroyed(true);

                            // 砖块碎裂效果
                            spawnBrickExplosion(brick.getPosition(), brick.getColor());

                            // 掉落道具
                            if (rand() % 100 < 30) {
                                int r = rand() % 3;
                                PowerUpType t = static_cast<PowerUpType>(r);
                                powerUps.emplace_back(brick.getPosition(), t);
                            }
                        }
                    }
                }
            }

            // 更新道具 & 粒子
            for (auto& p : powerUps) p.update(dt);
            updateParticles(dt);

            // 吃道具
            for (size_t i = 0; i < powerUps.size(); ) {
                if (CheckCollisionCircleRec(powerUps[i].getPosition(), powerUps[i].getRadius(), paddle.getRect())) {
                    applyPowerUp(powerUps[i].getType(), paddle);
                    powerUps.erase(powerUps.begin() + i);
                } else i++;
            }

            // 球掉出屏幕
            for (size_t i = 0; i < balls.size(); ) {
                if (balls[i].getPosition().y > SCREEN_HEIGHT)
                    balls.erase(balls.begin() + i);
                else i++;
            }

            // 死亡判断
            if (balls.empty()) {
                if (lives > 0) {
                    lives--;
                    balls.emplace_back(Vector2{ 400, 500 }, Vector2{ 300, -300 }, 10, RED, 10);
                } else {
                    gameOver = true;
                }
            }
        }
        else {
            // 游戏结束按钮
            bool r = CheckCollisionPointRec(mouse, { 300, 300, 200, 50 });
            bool e = CheckCollisionPointRec(mouse, { 300, 370, 200, 50 });

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                if (r) { resetGame(paddle, bricks); gameOver = false; }
                if (e) { CloseWindow(); return 0; }
            }
        }

        // 绘制
        BeginDrawing();
        ClearBackground(BLACK);

        if (!gameOver) {
            drawParticles();       // 粒子底层

            for (auto& b : bricks) b.draw();
            for (auto& ball : balls) ball.draw();
            for (auto& p : powerUps) p.draw();
            paddle.draw();

            DrawText(TextFormat("Lives: %d", lives), 10, 10, 20, WHITE);
        }
        else {
            DrawText("GAME OVER", 290, 200, 50, RED);
            DrawRectangle(300, 300, 200, 50, DARKGREEN);
            DrawText("RESTART", 350, 315, 25, WHITE);
            DrawRectangle(300, 370, 200, 50, DARKGRAY);
            DrawText("EXIT", 370, 385, 25, WHITE);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}

void spawnBall(Vector2 pos, Vector2 vel) {
    balls.emplace_back(pos, vel, 10, RED, 10);
}

void applyPowerUp(PowerUpType type, Paddle& paddle) {
    if (type == PADDLE_GROW) {
        paddle = Paddle(paddle.getPosition(), Vector2{ 500, 0 }, 160, 20);
    }
    else if (type == BALL_SPLIT) {
        if (!balls.empty()) {
            Vector2 p = balls[0].getPosition();
            spawnBall(p, Vector2{ 200, -300 });
            spawnBall(p, Vector2{ -200, -300 });
        }
    }
    else if (type == EXTRA_LIFE) {
        lives++;
    }
}

void resetGame(Paddle& paddle, std::vector<Bricks>& bricks) {
    lives = 1;
    balls.clear();
    powerUps.clear();
    particles.clear();

    balls.emplace_back(Vector2{ 400, 500 }, Vector2{ 300, -300 }, 10, RED, 10);
    paddle = Paddle(Vector2{ 400, 580 }, Vector2{ 500, 0 }, 100, 20);

    bricks.clear();
    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 8; col++) {
            bricks.emplace_back(
                Vector2{ (float)(100 + col * 80), (float)(50 + row * 30) },
                70, 20, BLUE
            );
        }
    }
}
