/* Group Members: Muhammad Hammad Karim (2503820)
                  Abdullah Araiz (2503850)

--------------PF PROJECT FALL 2025----------------
*/

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <sstream>
#include <iomanip>
#include <string>
#include <cmath>
#include <algorithm>
#include <deque>
#include <fstream>
#include <cstdint>

using namespace std;
using namespace sf;

// ==================== CONSTANTS ====================
const int ROWS = 20;
const int COLUMNS = 10;
const int BLOCK_SIZE = 30;
const int SIDEBAR_WIDTH = 180;
const int LEFT_MARGIN = 220;
const int WINDOW_WIDTH = LEFT_MARGIN + COLUMNS * BLOCK_SIZE + SIDEBAR_WIDTH;
const int WINDOW_HEIGHT = ROWS * BLOCK_SIZE + 100;
const float PI = 3.14159265f;
const int MAX_HIGH_SCORES = 10;
const float STRUGGLE_TIME_LIMIT = 300.0f;

// ==================== GAME STATES ====================
enum class GameState
{
    MENU,
    DIFFICULTY_SELECT,
    PLAYING,
    PAUSED,
    GAME_OVER,
    HIGH_SCORES,
    HELP
};

// ==================== DIFFICULTY LEVELS ====================
enum class DifficultyLevel
{
    BEGINNER,
    ADVANCED
};

// ==================== HIGH SCORE ENTRY ====================
struct HighScoreEntry
{
    string name;
    int score;
    
    HighScoreEntry(string n = "---", int s = 0) : name(n), score(s) {}
};

// ==================== JEWEL COLOR STRUCTURE ====================
struct JewelColors
{
    Color base;
    Color light;
    Color dark;
    Color highlight;
    Color sparkle;
    Color glow;
    Color innerGlow;
    string name;
};

// ==================== POINT STRUCTURE ====================
struct Point
{
    int x, y;
    Point(int x = 0, int y = 0) : x(x), y(y) {}
};

// ==================== PARTICLE SYSTEM ====================
struct Particle
{
    Vector2f position;
    Vector2f velocity;
    Color color;
    float lifetime;
    float maxLifetime;
    float size;
    float rotation;
    float rotationSpeed;
    int type;
    float gravity;

    Particle(Vector2f pos, Vector2f vel, Color col, float life, float sz = 4.f, int t = 0, float grav = 200.f)
        : position(pos), velocity(vel), color(col), lifetime(life), maxLifetime(life),
          size(sz), rotation(0), rotationSpeed((rand() % 360 - 180) * 0.1f), type(t), gravity(grav) {}

    bool update(float dt)
    {
        lifetime -= dt;
        position += velocity * dt;
        velocity.y += gravity * dt;
        velocity *= 0.99f;
        rotation += rotationSpeed;
        return lifetime > 0;
    }

    float getAlpha() const
    {
        return (lifetime / maxLifetime) * 255.f;
    }
};

class ParticleSystem
{
private:
    vector<Particle> particles;

public:
    void addParticle(Vector2f pos, Vector2f vel, Color col, float life, float size = 4.f, int type = 0, float gravity = 200.f)
    {
        if (particles.size() < 3000)
            particles.emplace_back(pos, vel, col, life, size, type, gravity);
    }

    void addExplosion(Vector2f pos, Color col, int count = 20)
    {
        for (int i = 0; i < count; i++)
        {
            float angle = (rand() % 360) * PI / 180.f;
            float speed = 50.f + rand() % 200;
            Vector2f vel(cos(angle) * speed, sin(angle) * speed - 100);
            float size = 2.f + (rand() % 40) / 10.f;
            addParticle(pos, vel, col, 0.5f + (rand() % 100) / 100.f, size, rand() % 3);
        }
    }

    void addMagicEffect(Vector2f pos, Color col, int count = 10)
    {
        for (int i = 0; i < count; i++)
        {
            float angle = (rand() % 360) * PI / 180.f;
            float speed = 30.f + rand() % 80;
            Vector2f vel(cos(angle) * speed, sin(angle) * speed - 50);
            addParticle(pos, vel, col, 0.6f + (rand() % 40) / 100.f, 2.f + rand() % 3, 1, 50.f);
        }
    }

    void addShimmer(Vector2f pos, Color col)
    {
        float angle = (rand() % 360) * PI / 180.f;
        float speed = 20.f + rand() % 40;
        Vector2f vel(cos(angle) * speed, sin(angle) * speed - 50);
        addParticle(pos, vel, Color(255, 255, 255, 200), 0.4f + (rand() % 20) / 100.f, 1.5f + rand() % 2, 1, 30.f);
    }

    void addFireEffect(Vector2f pos, float intensity = 1.0f)
    {
        Color fireColors[] = {Color(255, 200, 50), Color(255, 150, 30), Color(255, 100, 20), Color(255, 80, 10)};
        for (int i = 0; i < 2; i++)
        {
            float angle = (-90 + rand() % 40 - 20) * PI / 180.f;
            float speed = (25.f + rand() % 35) * intensity;
            Vector2f vel(cos(angle) * speed * 0.4f, sin(angle) * speed);
            addParticle(pos, vel, fireColors[rand() % 4], 0.3f + (rand() % 25) / 100.f,
                        (3.f + rand() % 4) * intensity, 1, -80.f);
        }
    }

    void addLineClearEffect(int row, int startX, int startY)
    {
        for (int j = 0; j < COLUMNS; j++)
        {
            Vector2f pos(startX + j * BLOCK_SIZE + BLOCK_SIZE / 2,
                         startY + row * BLOCK_SIZE + BLOCK_SIZE / 2);
            for (int k = 0; k < 15; k++)
            {
                float angle = (rand() % 360) * PI / 180.f;
                float speed = 150.f + rand() % 250;
                Vector2f vel(cos(angle) * speed, sin(angle) * speed - 200);
                Color colors[] = {
                    Color(255, 255, 255), Color(100, 200, 255), Color(255, 200, 100),
                    Color(200, 150, 255), Color(255, 215, 0), Color(100, 255, 200),
                    Color(255, 100, 150)};
                addParticle(pos, vel, colors[rand() % 7], 1.0f + (rand() % 50) / 100.f,
                            3.f + rand() % 6, rand() % 3);
            }
        }
    }

    void addSparkle(Vector2f pos, Color col)
    {
        for (int i = 0; i < 8; i++)
        {
            float angle = (rand() % 360) * PI / 180.f;
            float speed = 40.f + rand() % 60;
            Vector2f vel(cos(angle) * speed, sin(angle) * speed - 80);
            addParticle(pos, vel, Color(255, 255, 255), 0.5f + (rand() % 30) / 100.f, 2.f + rand() % 2, 1, 100.f);
        }
    }

    void addJewelSparkle(Vector2f pos, Color col)
    {
        float angle = (rand() % 360) * PI / 180.f;
        float speed = 10.f + rand() % 20;
        Vector2f vel(cos(angle) * speed, sin(angle) * speed - 30);
        addParticle(pos, vel, Color(255, 255, 255, 230), 0.4f, 2.0f, 1, 50.f);
    }

    void addChasmMist(float x, float baseY, float width)
    {
        if (rand() % 4 == 0)
        {
            float px = x + rand() % (int)width;
            Vector2f pos(px, baseY + rand() % 30);
            Vector2f vel((rand() % 30 - 15) * 0.1f, -8.f - rand() % 15);
            addParticle(pos, vel, Color(80, 60, 100, 40), 4.f + (rand() % 200) / 100.f,
                        20.f + rand() % 25, 4, -3.f);
        }
    }

    void addMenuParticle(float width, float height)
    {
        if (rand() % 3 == 0)
        {
            float x = rand() % (int)width;
            float y = height + 20;
            Vector2f vel((rand() % 40 - 20) * 0.5f, -30.f - rand() % 50);
            Color colors[] = {
                Color(255, 100, 100, 150), Color(100, 255, 100, 150), 
                Color(100, 100, 255, 150), Color(255, 255, 100, 150),
                Color(255, 100, 255, 150), Color(100, 255, 255, 150)};
            addParticle(Vector2f(x, y), vel, colors[rand() % 6], 3.f + (rand() % 200) / 100.f,
                        3.f + rand() % 4, 1, -15.f);
        }
    }

    void clear() { particles.clear(); }

    void update(float dt)
    {
        particles.erase(
            remove_if(particles.begin(), particles.end(),
                      [dt](Particle &p)
                      { return !p.update(dt); }),
            particles.end());
    }

    void draw(RenderWindow &window)
    {
        for (auto &p : particles)
        {
            Color col = p.color;
            col.a = static_cast<uint8_t>(min(255.f, p.getAlpha() * (p.color.a / 255.f)));

            if (p.type == 1)
            {
                CircleShape circle(p.size);
                circle.setOrigin(Vector2f(p.size, p.size));
                circle.setPosition(p.position);
                circle.setFillColor(col);
                window.draw(circle);

                if (p.size > 2)
                {
                    RectangleShape h(Vector2f(p.size * 2.5f, 1));
                    h.setOrigin(Vector2f(p.size * 1.25f, 0.5f));
                    h.setPosition(p.position);
                    h.setFillColor(Color(255, 255, 255, col.a / 3));
                    window.draw(h);

                    RectangleShape v(Vector2f(1, p.size * 2.5f));
                    v.setOrigin(Vector2f(0.5f, p.size * 1.25f));
                    v.setPosition(p.position);
                    v.setFillColor(Color(255, 255, 255, col.a / 3));
                    window.draw(v);
                }
            }
            else if (p.type == 4)
            {
                CircleShape mist(p.size);
                mist.setOrigin(Vector2f(p.size, p.size));
                mist.setPosition(p.position);
                mist.setFillColor(col);
                window.draw(mist);
            }
            else
            {
                ConvexShape gem;
                gem.setPointCount(6);
                gem.setPoint(0, Vector2f(0, -p.size));
                gem.setPoint(1, Vector2f(p.size * 0.7f, -p.size * 0.3f));
                gem.setPoint(2, Vector2f(p.size * 0.7f, p.size * 0.3f));
                gem.setPoint(3, Vector2f(0, p.size));
                gem.setPoint(4, Vector2f(-p.size * 0.7f, p.size * 0.3f));
                gem.setPoint(5, Vector2f(-p.size * 0.7f, -p.size * 0.3f));
                gem.setPosition(p.position);
                gem.setRotation(degrees(p.rotation));
                gem.setFillColor(col);
                window.draw(gem);
            }
        }
    }

    int getCount() const { return particles.size(); }
};

// ==================== STAR BACKGROUND ====================
class StarField
{
private:
    struct Star
    {
        Vector2f position;
        float size;
        float twinkle;
        float twinkleSpeed;
        Color color;
        float depth;
    };
    vector<Star> stars;

public:
    StarField(float w, float h, int count = 150)
    {
        for (int i = 0; i < count; i++)
        {
            Star s;
            s.position = Vector2f(rand() % (int)w, rand() % (int)h);
            s.size = 0.5f + (rand() % 25) / 10.f;
            s.twinkle = (rand() % 100) / 100.f * 2 * PI;
            s.twinkleSpeed = 1.f + (rand() % 50) / 10.f;
            s.depth = 0.3f + (rand() % 70) / 100.f;
            int colorVar = rand() % 5;
            if (colorVar == 0)
                s.color = Color(255, 255, 255);
            else if (colorVar == 1)
                s.color = Color(200, 220, 255);
            else if (colorVar == 2)
                s.color = Color(255, 240, 220);
            else if (colorVar == 3)
                s.color = Color(220, 200, 255);
            else
                s.color = Color(255, 220, 240);
            stars.push_back(s);
        }
    }

    void update(float dt)
    {
        for (auto &s : stars)
            s.twinkle += s.twinkleSpeed * dt;
    }

    void draw(RenderWindow &window)
    {
        for (auto &s : stars)
        {
            float alpha = (sin(s.twinkle) + 1.f) / 2.f * 180 + 75;
            alpha *= s.depth;
            
            CircleShape star(s.size * s.depth);
            star.setPosition(s.position);
            Color col = s.color;
            col.a = static_cast<uint8_t>(alpha);
            star.setFillColor(col);
            window.draw(star);

            if (s.size > 1.5f && s.depth > 0.6f)
            {
                CircleShape glow(s.size * 3.0f * s.depth);
                glow.setOrigin(Vector2f(s.size * 2.0f * s.depth, s.size * 2.0f * s.depth));
                glow.setPosition(s.position);
                glow.setFillColor(Color(s.color.r, s.color.g, s.color.b, static_cast<uint8_t>(alpha * 0.12f)));
                window.draw(glow);
            }
        }
    }
};

// ==================== FLOATING TEXT ====================
struct FloatingText
{
    string text;
    Vector2f position;
    Color color;
    float lifetime;
    float maxLifetime;
    int size;
    float scale;
    bool growing;

    FloatingText(string t, Vector2f pos, Color col, float life = 1.f, int sz = 24)
        : text(t), position(pos), color(col), lifetime(life), maxLifetime(life),
          size(sz), scale(0.5f), growing(true) {}

    bool update(float dt)
    {
        lifetime -= dt;
        position.y -= 35.f * dt;
        if (growing)
        {
            scale += dt * 4;
            if (scale >= 1.2f)
            {
                scale = 1.2f;
                growing = false;
            }
        }
        else if (scale > 1.0f)
        {
            scale -= dt * 2;
            if (scale < 1.0f)
                scale = 1.0f;
        }
        return lifetime > 0;
    }

    float getAlpha() const
    {
        if (lifetime > maxLifetime * 0.7f)
            return 1.0f;
        return (lifetime / (maxLifetime * 0.7f));
    }
};

// ==================== SCREEN TRANSITION ====================
class ScreenTransition
{
private:
    float progress;
    float duration;
    bool active;
    bool fadingOut;

public:
    ScreenTransition() : progress(0), duration(0.4f), active(false), fadingOut(true) {}

    void start(bool fadeOut, float dur = 0.4f)
    {
        active = true;
        fadingOut = fadeOut;
        progress = fadeOut ? 0.f : 1.f;
        duration = dur;
    }

    bool update(float dt)
    {
        if (!active)
            return false;
        if (fadingOut)
        {
            progress += dt / duration;
            if (progress >= 1.f)
            {
                progress = 1.f;
                active = false;
                return true;
            }
        }
        else
        {
            progress -= dt / duration;
            if (progress <= 0.f)
            {
                progress = 0.f;
                active = false;
            }
        }
        return false;
    }

    void draw(RenderWindow &window)
    {
        if (progress > 0)
        {
            RectangleShape overlay(Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));
            overlay.setFillColor(Color(0, 0, 0, static_cast<uint8_t>(progress * 255)));
            window.draw(overlay);
        }
    }

    bool isActive() const { return active; }
    float getProgress() const { return progress; }
};

// ==================== TETRIS GAME LOGIC ====================
class Tetris
{
private:
    Point actPosition[4], tempPosition[4];
    Point nextPieces[3][4];
    int nextColors[3];
    Point holdPiece[4];
    int holdColor;
    bool canHold;
    int currentColor;
    int score;
    int level;
    int linesCleared;
    int combo;
    bool gameEnd;
    int frozenRows;
    DifficultyLevel difficulty;

    int beginnerPieces[4] = {0, 6, 3, 4};
    int numAvailablePieces;

    int figures[7][4] = {
        {1, 3, 5, 7}, // I
        {2, 4, 5, 7}, // Z
        {3, 5, 4, 6}, // S
        {3, 5, 4, 7}, // T
        {2, 3, 5, 7}, // L
        {3, 5, 7, 6}, // J
        {2, 3, 4, 5}  // O
    };

    int field[ROWS][COLUMNS] = {0};
    bool frozenField[ROWS][COLUMNS] = {false};

public:
    Tetris()
    {
        reset(DifficultyLevel::BEGINNER);
    }

    void reset(DifficultyLevel diff)
    {
        difficulty = diff;
        numAvailablePieces = (diff == DifficultyLevel::BEGINNER) ? 4 : 7;
        
        for (int i = 0; i < ROWS; i++)
        {
            for (int j = 0; j < COLUMNS; j++)
            {
                field[i][j] = 0;
                frozenField[i][j] = false;
            }
        }

        score = 0;
        level = (diff == DifficultyLevel::BEGINNER) ? 1 : 2;
        linesCleared = 0;
        combo = 0;
        gameEnd = false;
        holdColor = 0;
        canHold = true;
        frozenRows = 0;

        for (int i = 0; i < 3; i++)
            generatePiece(nextPieces[i], nextColors[i]);
        spawnNewTetrimino();
    }

    void generatePiece(Point piece[4], int &color)
    {
        int n;
        if (difficulty == DifficultyLevel::BEGINNER)
        {
            n = beginnerPieces[rand() % 4];
        }
        else
        {
            n = rand() % 7;
        }
        
        color = n + 1;
        for (int i = 0; i < 4; i++)
        {
            piece[i].x = figures[n][i] % 2;
            piece[i].y = figures[n][i] / 2;
        }
    }

    void spawnNewTetrimino()
    {
        currentColor = nextColors[0];
        for (int i = 0; i < 4; i++)
        {
            actPosition[i].x = nextPieces[0][i].x + (COLUMNS / 2) - 1;
            actPosition[i].y = nextPieces[0][i].y - 1;
        }
        for (int i = 0; i < 2; i++)
        {
            nextColors[i] = nextColors[i + 1];
            for (int j = 0; j < 4; j++)
                nextPieces[i][j] = nextPieces[i + 1][j];
        }
        generatePiece(nextPieces[2], nextColors[2]);
        canHold = true;
    }

    bool checkCollision()
    {
        for (int i = 0; i < 4; i++)
        {
            if (actPosition[i].x < 0 || actPosition[i].x >= COLUMNS || actPosition[i].y >= ROWS)
                return false;
            if (actPosition[i].y >= 0 && field[actPosition[i].y][actPosition[i].x])
                return false;
        }
        return true;
    }

    bool checkCollisionAt(Point pos[4])
    {
        for (int i = 0; i < 4; i++)
        {
            if (pos[i].x < 0 || pos[i].x >= COLUMNS || pos[i].y >= ROWS)
                return false;
            if (pos[i].y >= 0 && field[pos[i].y][pos[i].x])
                return false;
        }
        return true;
    }

    void move(int dx)
    {
        for (int i = 0; i < 4; i++)
        {
            tempPosition[i] = actPosition[i];
            actPosition[i].x += dx;
        }
        if (!checkCollision())
            for (int i = 0; i < 4; i++)
                actPosition[i] = tempPosition[i];
    }

    bool rotateTetrimino()
    {
        if (currentColor == 7)
            return false;

        for (int i = 0; i < 4; i++)
            tempPosition[i] = actPosition[i];

        Point p = actPosition[1];
        for (int i = 0; i < 4; i++)
        {
            int x = actPosition[i].y - p.y;
            int y = actPosition[i].x - p.x;
            actPosition[i].x = p.x - x;
            actPosition[i].y = p.y + y;
        }

        if (!checkCollision())
        {
            for (int i = 0; i < 4; i++)
                actPosition[i].x -= 1;
            if (!checkCollision())
            {
                for (int i = 0; i < 4; i++)
                    actPosition[i].x += 2;
                if (!checkCollision())
                {
                    for (int i = 0; i < 4; i++)
                        actPosition[i] = tempPosition[i];
                    return false;
                }
            }
        }
        return true;
    }

    bool timeMove()
    {
        for (int i = 0; i < 4; i++)
        {
            tempPosition[i] = actPosition[i];
            actPosition[i].y += 1;
        }

        if (!checkCollision())
        {
            for (int i = 0; i < 4; i++)
            {
                if (tempPosition[i].y >= 0)
                    field[tempPosition[i].y][tempPosition[i].x] = currentColor;
                if (tempPosition[i].y <= 0)
                {
                    gameEnd = true;
                }
            }
            if (!gameEnd)
                spawnNewTetrimino();
            return true;
        }
        return false;
    }

    void hardDrop()
    {
        while (!timeMove())
            ;
    }

    bool holdCurrentPiece()
    {
        if (!canHold)
            return false;

        if (holdColor == 0)
        {
            holdColor = currentColor;
            for (int i = 0; i < 4; i++)
            {
                holdPiece[i].x = figures[currentColor - 1][i] % 2;
                holdPiece[i].y = figures[currentColor - 1][i] / 2;
            }
            spawnNewTetrimino();
        }
        else
        {
            int tempColor = holdColor;
            Point tempPiece[4];
            for (int i = 0; i < 4; i++)
                tempPiece[i] = holdPiece[i];

            holdColor = currentColor;
            for (int i = 0; i < 4; i++)
            {
                holdPiece[i].x = figures[currentColor - 1][i] % 2;
                holdPiece[i].y = figures[currentColor - 1][i] / 2;
            }

            currentColor = tempColor;
            for (int i = 0; i < 4; i++)
            {
                actPosition[i].x = tempPiece[i].x + (COLUMNS / 2) - 1;
                actPosition[i].y = tempPiece[i].y - 1;
            }
        }
        canHold = false;
        return true;
    }

    void getGhostPosition(Point ghost[4])
    {
        for (int i = 0; i < 4; i++)
            ghost[i] = actPosition[i];

        bool canMove = true;
        while (canMove)
        {
            for (int i = 0; i < 4; i++)
                ghost[i].y += 1;
            if (!checkCollisionAt(ghost))
            {
                for (int i = 0; i < 4; i++)
                    ghost[i].y -= 1;
                canMove = false;
            }
        }
    }

    int findFullLines(vector<int> &fullRows)
    {
        fullRows.clear();
        for (int i = 0; i < ROWS - frozenRows; i++)
        {
            int count = 0;
            for (int j = 0; j < COLUMNS; j++)
                if (field[i][j])
                    count++;
            if (count == COLUMNS)
                fullRows.push_back(i);
        }
        return fullRows.size();
    }

    void clearFullLines(vector<int> &rowsToClear)
    {
        if (rowsToClear.empty())
            return;

        int cleared = rowsToClear.size();
        sort(rowsToClear.begin(), rowsToClear.end(), greater<int>());

        for (int row : rowsToClear)
        {
            for (int i = row; i > 0; i--)
            {
                for (int j = 0; j < COLUMNS; j++)
                {
                    field[i][j] = field[i - 1][j];
                    frozenField[i][j] = frozenField[i - 1][j];
                }
            }
            for (int j = 0; j < COLUMNS; j++)
            {
                field[0][j] = 0;
                frozenField[0][j] = false;
            }
        }

        if (cleared > 0)
        {
            combo++;
            addScore(cleared);
        }
        else
        {
            combo = 0;
        }
    }

    void resetCombo() { combo = 0; }

    void addScore(int lines)
    {
        linesCleared += lines;
        
        int baseScore[] = {0, 10, 30, 60, 100};
        int points = baseScore[lines] * level;
        
        if (combo > 1)
            points += 5 * combo * level;
        
        score += points;
    }

    void freezeBottomRow()
    {
        if (frozenRows >= ROWS - 4)
            return;
            
        int rowToFreeze = ROWS - 1 - frozenRows;
        
        for (int j = 0; j < COLUMNS; j++)
        {
            if (field[rowToFreeze][j] == 0)
            {
                field[rowToFreeze][j] = (rand() % 7) + 1;
            }
            frozenField[rowToFreeze][j] = true;
        }
        frozenRows++;
    }

    int getHighestBlock() const
    {
        for (int i = 0; i < ROWS; i++)
            for (int j = 0; j < COLUMNS; j++)
                if (field[i][j])
                    return ROWS - i;
        return 0;
    }

    float getDangerLevel() const { return (float)getHighestBlock() / ROWS; }
    
    bool getGameEnd() { return gameEnd; }
    Point *getActivePositions() { return actPosition; }
    int getCurrentColor() { return currentColor; }
    int getScore() { return score; }
    int getLevel() { return level; }
    int getLines() { return linesCleared; }
    int getCombo() { return combo; }
    int getField(int i, int j) { return field[i][j]; }
    bool isFrozen(int i, int j) { return frozenField[i][j]; }
    int getNextColor(int idx) { return nextColors[idx]; }
    Point *getNextPiece(int idx) { return nextPieces[idx]; }
    int getHoldColor() { return holdColor; }
    Point *getHoldPiece() { return holdPiece; }
    bool canHoldPiece() { return canHold; }
    int getFrozenRows() { return frozenRows; }
    DifficultyLevel getDifficulty() { return difficulty; }
};

// ==================== MAIN GAME CLASS ====================
class Game
{
private:
    RenderWindow window;
    Music menuMusic;           //Music for menu
    Music gameMusic;           //Music for gameplay
    bool musicEnabled;      
    bool musicLoaded;
    bool gameMusicLoaded;             
    Tetris tetris;
    GameState state;
    GameState previousState;
    Font font;
    bool fontLoaded;

    bool isFullscreen;
    Vector2u windowedSize;

    // Sound effect variables
    SoundBuffer arrowBuffer, spaceBuffer, gameOverBuffer;
    Sound* arrowSound;
    Sound* spaceSound;
    Sound* gameOverSound;
    bool soundEffectsEnabled;
    bool soundEffectsLoaded;

    Clock gameClock;
    float timer;
    float baseDelay;
    float delay;
    float lineClearTimer;
    bool clearingLines;
    vector<int> linesToClear;
    float gameTime;
    float totalPlayTime;
    float timeSinceLastClear;
    int lastLineCount;

    ParticleSystem particles;
    StarField stars;
    vector<FloatingText> floatingTexts;
    ScreenTransition transition;

    float screenShake;
    float screenShakeIntensity;
    float menuAnimation;
    float titleGlow;
    float difficultySelectAnimation;

    HighScoreEntry highScores[MAX_HIGH_SCORES];
    bool enteringName;
    string playerName;
    int newScoreIndex;

    int menuSelection;
    int pauseMenuSelection;
    int difficultySelection;

    float boardX, boardY;
    float boardWidth, boardHeight;

    //Jewel palette with more vibrant colors
    JewelColors jewelPalette[8] = {
        {Color(30, 30, 50), Color(40, 40, 60), Color(20, 20, 30),
         Color(50, 50, 70), Color(60, 60, 80), Color(30, 30, 50, 0), Color(40, 40, 60), "None"},
        //Cyan/Aquamarine - I piece
        {Color(40, 200, 230), Color(130, 240, 255), Color(20, 140, 170),
         Color(200, 255, 255), Color(255, 255, 255), Color(40, 200, 230, 120), Color(100, 230, 255, 150), "Aquamarine"},
        // Red/Ruby - Z piece
        {Color(220, 40, 70), Color(255, 100, 120), Color(160, 20, 50),
         Color(255, 180, 190), Color(255, 220, 230), Color(220, 40, 70, 120), Color(255, 80, 100, 150), "Ruby"},
        // Green/Emerald - S piece
        {Color(50, 200, 90), Color(120, 255, 160), Color(30, 140, 60),
         Color(200, 255, 220), Color(230, 255, 240), Color(50, 200, 90, 120), Color(100, 255, 140, 150), "Emerald"},
        //Purple/Amethyst - T piece
        {Color(170, 70, 220), Color(210, 140, 255), Color(120, 40, 160),
         Color(240, 200, 255), Color(250, 230, 255), Color(170, 70, 220, 120), Color(200, 120, 255, 150), "Amethyst"},
        // Orange/Topaz - L piece
        {Color(255, 170, 50), Color(255, 210, 120), Color(210, 130, 30),
         Color(255, 240, 200), Color(255, 250, 230), Color(255, 170, 50, 120), Color(255, 200, 100, 150), "Topaz"},
        //Blue/Sapphire - J piece
        {Color(50, 80, 220), Color(100, 140, 255), Color(30, 50, 160),
         Color(170, 200, 255), Color(210, 230, 255), Color(50, 80, 220, 120), Color(80, 120, 255, 150), "Sapphire"},
        // Yellow/Citrine - O piece
{Color(255, 240, 100), Color(255, 250, 180), Color(220, 200, 60),
 Color(255, 255, 220), Color(255, 255, 180), Color(255, 240, 100, 120), Color(255, 245, 150, 180), "Citrine"}
    };

public:
    Game() : window(VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT}), "GEMSTONE GUARDIAN - Tetris",
                    Style::Titlebar | Style::Close | Style::Resize),
             stars(WINDOW_WIDTH, WINDOW_HEIGHT, 250),
             state(GameState::MENU),
             previousState(GameState::MENU),
             isFullscreen(false),
             windowedSize(WINDOW_WIDTH, WINDOW_HEIGHT),
             timer(0), baseDelay(0.5f), delay(0.5f), lineClearTimer(0), clearingLines(false),
             gameTime(0), totalPlayTime(0), timeSinceLastClear(0), lastLineCount(0),
             screenShake(0), screenShakeIntensity(0), menuAnimation(0), titleGlow(0),
             difficultySelectAnimation(0),
             enteringName(false), playerName(""), newScoreIndex(-1),
             menuSelection(0), pauseMenuSelection(0), difficultySelection(0),
             musicEnabled(true), musicLoaded(false), gameMusicLoaded(false),
            soundEffectsEnabled(true), soundEffectsLoaded(false),
            arrowSound(nullptr), spaceSound(nullptr), gameOverSound(nullptr)
    
        
    
            {
        srand(static_cast<unsigned int>(time(0)));
        window.setFramerateLimit(60);

        boardX = LEFT_MARGIN;
        boardY = 50;
        boardWidth = COLUMNS * BLOCK_SIZE;
        boardHeight = ROWS * BLOCK_SIZE;

        fontLoaded = font.openFromFile("arial.ttf") ||
                     font.openFromFile("/System/Library/Fonts/Arial.ttf") ||
                     font.openFromFile("/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf") ||
                     font.openFromFile("arial.ttf");

        loadHighScores();
        loadMusic();
    }

    ~Game()
    {
        delete arrowSound;
        delete spaceSound;
        delete gameOverSound;
    }
    void loadMusic()
{
    // Load menu music
    musicLoaded = menuMusic.openFromFile("menu_music.ogg") ||
                  menuMusic.openFromFile("menu_music.wav") ||
                  menuMusic.openFromFile("menu.ogg");
    
    if (musicLoaded)
    {
        menuMusic.setLooping(true);
        menuMusic.setVolume(40.f);
    }
    
    // Load gameplay music
    gameMusicLoaded = gameMusic.openFromFile("game_music.ogg") ||
                      gameMusic.openFromFile("game_music.wav") ||
                      gameMusic.openFromFile("game.ogg");
    
    if (gameMusicLoaded)
    {
        gameMusic.setLooping(true);
        gameMusic.setVolume(40.f);
    }
    
    // Start menu music if enabled
    if (musicEnabled && musicLoaded)
        menuMusic.play();
        // Load sound effects
    soundEffectsLoaded = arrowBuffer.loadFromFile("arrows_music.ogg") &&
                         spaceBuffer.loadFromFile("spacebar_music.ogg") &&
                         gameOverBuffer.loadFromFile("gameover_music.ogg");
    
    if (soundEffectsLoaded)
    {
        arrowSound = new Sound(arrowBuffer);
        spaceSound = new Sound(spaceBuffer);
        gameOverSound = new Sound(gameOverBuffer);
        
        arrowSound->setVolume(50.f);
        spaceSound->setVolume(50.f);
        gameOverSound->setVolume(60.f);
    }
}

void toggleMusic()
{
    if (!musicLoaded && !gameMusicLoaded)
        return;
        
    musicEnabled = !musicEnabled;
    
    if (musicEnabled)
    {
        //Play appropriate music based on game state
        if (state == GameState::PLAYING)
        {
            if (gameMusicLoaded)
                gameMusic.play();
        }
        else
        {
            if (musicLoaded)
                menuMusic.play();
        }
    }
    else
    {
        menuMusic.pause();
        gameMusic.pause();
    }
}

void switchToGameMusic()
{
    if (!musicEnabled)
        return;
        
    if (musicLoaded)
        menuMusic.pause();
    
    if (gameMusicLoaded)
        gameMusic.play();
}

void switchToMenuMusic()
{
    if (!musicEnabled)
        return;
        
    if (gameMusicLoaded)
        gameMusic.pause();
    
    if (musicLoaded)
        menuMusic.play();
}
 void toggleSoundEffects()
{
    if (!soundEffectsLoaded)
        return;
        
    soundEffectsEnabled = !soundEffectsEnabled;
}

    void toggleFullscreen()
    {
        if (isFullscreen)
        {
            window.create(VideoMode(windowedSize), "GEMSTONE GUARDIAN - Tetris",
                          Style::Titlebar | Style::Close | Style::Resize);
            isFullscreen = false;
        }
        else
        {
            windowedSize = window.getSize();
            window.create(VideoMode::getDesktopMode(), "GEMSTONE GUARDIAN - Tetris", State::Fullscreen);
            isFullscreen = true;
        }
        window.setFramerateLimit(60);
    }

    View getScaledView()
    {
        View view(FloatRect(Vector2f(0, 0), Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT)));

        Vector2u windowSize = window.getSize();
        float windowRatio = (float)windowSize.x / (float)windowSize.y;
        float viewRatio = (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT;

        float sizeX = 1.f, sizeY = 1.f, posX = 0.f, posY = 0.f;

        if (windowRatio > viewRatio)
        {
            sizeX = viewRatio / windowRatio;
            posX = (1.f - sizeX) / 2.f;
        }
        else
        {
            sizeY = windowRatio / viewRatio;
            posY = (1.f - sizeY) / 2.f;
        }

        view.setViewport(FloatRect(Vector2f(posX, posY), Vector2f(sizeX, sizeY)));
        return view;
    }

    void loadHighScores()
    {
        ifstream file("highscores.txt");
        if (file.is_open())
        {
            for (int i = 0; i < MAX_HIGH_SCORES; i++)
            {
                if (file >> highScores[i].name >> highScores[i].score)
                {
                }
                else
                {
                    highScores[i] = HighScoreEntry("---", 0);
                }
            }
            file.close();
        }
        else
        {
            for (int i = 0; i < MAX_HIGH_SCORES; i++)
            {
                highScores[i] = HighScoreEntry("---", 0);
            }
        }
    }

    void saveHighScores()
    {
        ofstream file("highscores.txt");
        if (file.is_open())
        {
            for (int i = 0; i < MAX_HIGH_SCORES; i++)
            {
                file << highScores[i].name << " " << highScores[i].score << "\n";
            }
            file.close();
        }
    }

    int checkHighScore(int score)
    {
        for (int i = 0; i < MAX_HIGH_SCORES; i++)
        {
            if (score > highScores[i].score)
            {
                return i;
            }
        }
        return -1;
    }

    void insertHighScore(int index, const string &name, int score)
    {
        for (int i = MAX_HIGH_SCORES - 1; i > index; i--)
        {
            highScores[i] = highScores[i - 1];
        }
        highScores[index] = HighScoreEntry(name, score);
        saveHighScores();
    }

    void run()
    {
        while (window.isOpen())
        {
            float dt = gameClock.restart().asSeconds();
            processEvents();
            update(dt);
            render();
        }
    }

private:
    void processEvents()
    {
        while (optional<Event> event = window.pollEvent())
        {
            if (event->is<Event::Closed>())
                window.close();

            if (const auto *keyPressed = event->getIf<Event::KeyPressed>())
            {
                if (keyPressed->code == Keyboard::Key::F11 || keyPressed->code == Keyboard::Key::F)
                {
                    toggleFullscreen();
                }
                else if (enteringName)
                {
                    handleNameInput(keyPressed->code);
                }
                else
                {
                    handleKeyPress(keyPressed->code);
                }
            }

            if (enteringName)
            {
                if (const auto *textEntered = event->getIf<Event::TextEntered>())
                {
                    if (textEntered->unicode >= 32 && textEntered->unicode < 127 && playerName.length() < 10)
                    {
                        playerName += static_cast<char>(textEntered->unicode);
                    }
                }
            }
        }
    }

    void handleNameInput(Keyboard::Key key)
    {
        if (key == Keyboard::Key::Backspace && !playerName.empty())
        {
            playerName.pop_back();
        }
        else if (key == Keyboard::Key::Enter && !playerName.empty())
        {
            insertHighScore(newScoreIndex, playerName, tetris.getScore());
            enteringName = false;
            playerName = "";
            newScoreIndex = -1;
        }
        else if (key == Keyboard::Key::Escape)
        {
            enteringName = false;
            playerName = "";
            newScoreIndex = -1;
        }
    }

    void handleKeyPress(Keyboard::Key key)
    {
        if (key == Keyboard::Key::M)
    {
        toggleMusic();
        return;
    }
    if (key == Keyboard::Key::N)
    {
        toggleSoundEffects();
        return;
    }
        if (transition.isActive())
            return;

        switch (state)
        {
        case GameState::MENU:
            handleMenuInput(key);
            break;

        case GameState::DIFFICULTY_SELECT:
            handleDifficultyInput(key);
            break;

        case GameState::PLAYING:
            handlePlayingInput(key);
            break;

        case GameState::PAUSED:
            handlePauseInput(key);
            break;

        case GameState::GAME_OVER:
            if (!enteringName)
            {
                if (key == Keyboard::Key::Enter || key == Keyboard::Key::Escape)
                switchToMenuMusic();
                state = GameState::MENU;
            }
            break;

        case GameState::HIGH_SCORES:
        case GameState::HELP:
            if (key == Keyboard::Key::Escape || key == Keyboard::Key::Enter)
                state = previousState;
            break;
        }
    }

    void handleMenuInput(Keyboard::Key key)
    {
        if (key == Keyboard::Key::Up)
        {
            menuSelection = (menuSelection - 1 + 4) % 4;
        }
        else if (key == Keyboard::Key::Down)
        {
            menuSelection = (menuSelection + 1) % 4;
        }
        else if (key == Keyboard::Key::Num1)
        {
            menuSelection = 0;
            executeMenuOption();
        }
        else if (key == Keyboard::Key::Num2)
        {
            menuSelection = 1;
            executeMenuOption();
        }
        else if (key == Keyboard::Key::Num3)
        {
            menuSelection = 2;
            executeMenuOption();
        }
        else if (key == Keyboard::Key::Num4)
        {
            menuSelection = 3;
            executeMenuOption();
        }
        else if (key == Keyboard::Key::Enter || key == Keyboard::Key::Space)
        {
            executeMenuOption();
        }
    }

    void executeMenuOption()
    {
        switch (menuSelection)
        {
        case 0:
            state = GameState::DIFFICULTY_SELECT;
            difficultySelection = 0;
            difficultySelectAnimation = 0;
            break;
        case 1:
            previousState = GameState::MENU;
            state = GameState::HIGH_SCORES;
            break;
        case 2:
            previousState = GameState::MENU;
            state = GameState::HELP;
            break;
        case 3:
            window.close();
            break;
        }
    }

    void handleDifficultyInput(Keyboard::Key key)
    {
        if (key == Keyboard::Key::Up || key == Keyboard::Key::Left)
        {
            difficultySelection = (difficultySelection - 1 + 2) % 2;
        }
        else if (key == Keyboard::Key::Down || key == Keyboard::Key::Right)
        {
            difficultySelection = (difficultySelection + 1) % 2;
        }
        else if (key == Keyboard::Key::Num1 || key == Keyboard::Key::B)
        {
            difficultySelection = 0;
            startGame(DifficultyLevel::BEGINNER);
        }
        else if (key == Keyboard::Key::Num2 || key == Keyboard::Key::A)
        {
            difficultySelection = 1;
            startGame(DifficultyLevel::ADVANCED);
        }
        else if (key == Keyboard::Key::Enter || key == Keyboard::Key::Space)
        {
            startGame(difficultySelection == 0 ? DifficultyLevel::BEGINNER : DifficultyLevel::ADVANCED);
        }
        else if (key == Keyboard::Key::Escape)
        {
            state = GameState::MENU;
        }
    }

    void handlePlayingInput(Keyboard::Key key)
    {
        if (clearingLines)
            return;

        if (key == Keyboard::Key::Left)
        {
            tetris.move(-1);
            if (soundEffectsEnabled && soundEffectsLoaded && arrowSound)
                arrowSound->play();
        }
        else if (key == Keyboard::Key::Right)
        {
            tetris.move(1);
            if (soundEffectsEnabled && soundEffectsLoaded && arrowSound)
                arrowSound->play();
        }
        else if (key == Keyboard::Key::Up)
        {
            if (soundEffectsEnabled && soundEffectsLoaded && arrowSound)
                arrowSound->play();
        
            if (tetris.rotateTetrimino())
            {
                Point *pos = tetris.getActivePositions();
                particles.addSparkle(
                    Vector2f(boardX + pos[1].x * BLOCK_SIZE + BLOCK_SIZE / 2,
                             boardY + pos[1].y * BLOCK_SIZE + BLOCK_SIZE / 2),
                    jewelPalette[tetris.getCurrentColor()].sparkle);
            }
        }
        else if (key == Keyboard::Key::Space)
        {
            if (soundEffectsEnabled && soundEffectsLoaded && spaceSound)
                spaceSound->play();
            
            tetris.hardDrop();
            checkForLines();
            screenShake = 0.15f;
            screenShakeIntensity = 6.f;
        }
        else if (key == Keyboard::Key::C)
        {
            tetris.holdCurrentPiece();
        }
        else if (key == Keyboard::Key::P || key == Keyboard::Key::Escape)
        {
            state = GameState::PAUSED;
            pauseMenuSelection = 4;
        }
    }

    void handlePauseInput(Keyboard::Key key)
    {
        if (key == Keyboard::Key::Up)
        {
            pauseMenuSelection = (pauseMenuSelection - 1 + 5) % 5;
        }
        else if (key == Keyboard::Key::Down)
        {
            pauseMenuSelection = (pauseMenuSelection + 1) % 5;
        }
        else if (key == Keyboard::Key::Num1)
        {
            state = GameState::DIFFICULTY_SELECT;
            difficultySelection = 0;
        }
        else if (key == Keyboard::Key::Num2)
        {
            previousState = GameState::PAUSED;
            state = GameState::HIGH_SCORES;
        }
        else if (key == Keyboard::Key::Num3)
        {
            previousState = GameState::PAUSED;
            state = GameState::HELP;
        }
        else if (key == Keyboard::Key::Num4)
        {
            switchToMenuMusic();
            state = GameState::MENU;
        }
        else if (key == Keyboard::Key::Num5 || key == Keyboard::Key::P)
        {
            state = GameState::PLAYING;
        }
        else if (key == Keyboard::Key::Enter || key == Keyboard::Key::Space)
        {
            executePauseOption();
        }
        else if (key == Keyboard::Key::Escape)
        {
            state = GameState::PLAYING;
        }
    }

    void executePauseOption()
    {
        switch (pauseMenuSelection)
        {
        case 0:
            state = GameState::DIFFICULTY_SELECT;
            difficultySelection = 0;
            break;
        case 1:
            previousState = GameState::PAUSED;
            state = GameState::HIGH_SCORES;
            break;
        case 2:
            previousState = GameState::PAUSED;
            state = GameState::HELP;
            break;
        case 3:
            switchToMenuMusic();
            state = GameState::MENU;
            break;
        case 4:
            state = GameState::PLAYING;
            break;
        }
    }

    void startGame(DifficultyLevel diff)
    {
        switchToGameMusic(); 
        state = GameState::PLAYING;
        tetris.reset(diff);
        
        if (diff == DifficultyLevel::BEGINNER)
        {
            baseDelay = 0.5f;
        }
        else
        {
            baseDelay = 0.35f;
        }
        
        delay = baseDelay;
        particles.clear();
        floatingTexts.clear();
        totalPlayTime = 0;
        timeSinceLastClear = 0;
        lastLineCount = 0;
    }

    void checkForLines()
    {
        vector<int> fullRows;
        int cleared = tetris.findFullLines(fullRows);

        if (cleared > 0)
        {
            clearingLines = true;
            lineClearTimer = 0.45f;
            linesToClear = fullRows;

            for (int row : fullRows)
                particles.addLineClearEffect(row, boardX, boardY);

            screenShake = 0.3f;
            screenShakeIntensity = 5.f * cleared;

            timeSinceLastClear = 0;
            lastLineCount = tetris.getLines();

            string msg;
            Color col;
            if (cleared == 4)
            {
                msg = "TETRIS!";
                col = Color(255, 215, 0);
                screenShake = 0.5f;
                screenShakeIntensity = 12.f;
            }
            else if (cleared == 3)
            {
                msg = "TRIPLE!";
                col = Color(255, 100, 255);
            }
            else if (cleared == 2)
            {
                msg = "DOUBLE!";
                col = Color(100, 255, 255);
            }
            else
            {
                msg = "SINGLE!";
                col = Color(100, 255, 100);
            }

            int baseScore[] = {0, 10, 30, 60, 100};
            int points = baseScore[cleared] * tetris.getLevel();
            
            floatingTexts.emplace_back(msg + " +" + to_string(points),
                                       Vector2f(boardX + boardWidth / 2 - 80, WINDOW_HEIGHT / 2 - 60),
                                       col, 1.5f, 32);

            if (tetris.getCombo() > 1)
            {
                floatingTexts.emplace_back(
                    "COMBO x" + to_string(tetris.getCombo()),
                    Vector2f(boardX + boardWidth / 2 - 60, WINDOW_HEIGHT / 2 - 10),
                    Color(255, 200, 50), 1.2f, 24);
            }
        }
        else
        {
            tetris.resetCombo();
        }
    }

    void update(float dt)
    {
        stars.update(dt);
        particles.update(dt);
        menuAnimation += dt;
        titleGlow += dt * 2.5f;
        gameTime += dt;
        difficultySelectAnimation += dt;

        if (transition.update(dt))
        {
        }

        floatingTexts.erase(
            remove_if(floatingTexts.begin(), floatingTexts.end(),
                      [dt](FloatingText &ft)
                      { return !ft.update(dt); }),
            floatingTexts.end());

        if (screenShake > 0)
            screenShake -= dt;

        //menu particles
        if (state == GameState::MENU || state == GameState::DIFFICULTY_SELECT)
        {
            particles.addMenuParticle(WINDOW_WIDTH, WINDOW_HEIGHT);
        }

        if (state == GameState::PLAYING)
        {
            updateGame(dt);
        }
    }

    void updateGame(float dt)
    {
        totalPlayTime += dt;
        timeSinceLastClear += dt;

        if (timeSinceLastClear >= STRUGGLE_TIME_LIMIT)
        {
            tetris.freezeBottomRow();
            timeSinceLastClear = 0;
            
            floatingTexts.emplace_back("ROW FROZEN!",
                                       Vector2f(boardX + boardWidth / 2 - 70, WINDOW_HEIGHT / 2),
                                       Color(255, 100, 100), 2.0f, 28);
            
            screenShake = 0.4f;
            screenShakeIntensity = 8.f;
        }

        particles.addChasmMist(boardX, boardY + boardHeight, boardWidth);

        //Random sparkles on blocks - more frequent for shinier effect
        if (rand() % 30 == 0)
        {
            int randRow = rand() % ROWS;
            int randCol = rand() % COLUMNS;
            int colorIdx = tetris.getField(randRow, randCol);
            if (colorIdx > 0 && !tetris.isFrozen(randRow, randCol))
            {
                Vector2f pos(boardX + randCol * BLOCK_SIZE + BLOCK_SIZE / 2,
                             boardY + randRow * BLOCK_SIZE + BLOCK_SIZE / 2);
                particles.addJewelSparkle(pos, jewelPalette[colorIdx].sparkle);
            }
        }
        
        if (rand() % 20 == 0)
        {
            Point *pos = tetris.getActivePositions();
            int idx = rand() % 4;
            if (pos[idx].y >= 0)
            {
                particles.addShimmer(
                    Vector2f(boardX + pos[idx].x * BLOCK_SIZE + BLOCK_SIZE / 2,
                             boardY + pos[idx].y * BLOCK_SIZE + BLOCK_SIZE / 2),
                    jewelPalette[tetris.getCurrentColor()].sparkle);
            }
        }

        if (clearingLines)
        {
            lineClearTimer -= dt;
            if (lineClearTimer <= 0)
            {
                tetris.clearFullLines(linesToClear);
                linesToClear.clear();
                clearingLines = false;
            }
            return;
        }

        if (Keyboard::isKeyPressed(Keyboard::Key::Down))
            delay = 0.04f;
        else
        {
            delay = baseDelay;
        }

        timer += dt;
        if (timer > delay)
        {
            if (tetris.timeMove())
                checkForLines();
            timer = 0;
        }

        if (tetris.getGameEnd())
        {
            //Stop game music and play gameover sound
            if (gameMusicLoaded)
                gameMusic.stop();
            
            if (soundEffectsEnabled && soundEffectsLoaded && gameOverSound)
                gameOverSound->play();
            
            state = GameState::GAME_OVER;
            screenShake = 0.6f;
            screenShakeIntensity = 18.f;
            
            newScoreIndex = checkHighScore(tetris.getScore());
            if (newScoreIndex >= 0)
            {
                enteringName = true;
                playerName = "";
            }
            
            for (int i = 0; i < 10; i++)
            {
                particles.addExplosion(
                    Vector2f(boardX + rand() % (int)boardWidth, boardY + rand() % (int)(boardHeight / 3)),
                    Color(255, 50, 50), 30);
            }
        }
    }

    void render()
    {
        window.clear(Color::Black);

        View view = getScaledView();

        if (screenShake > 0)
        {
            float intensity = screenShakeIntensity * min(1.f, screenShake / 0.3f);
            float offsetX = (rand() % 100 - 50) / 50.f * intensity;
            float offsetY = (rand() % 100 - 50) / 50.f * intensity;
            view.move(Vector2f(offsetX, offsetY));
        }

        window.setView(view);

        drawSkyBackground();
        stars.draw(window);

        switch (state)
        {
        case GameState::MENU:
            drawMenuScreen();
            break;
        case GameState::DIFFICULTY_SELECT:
            drawDifficultyScreen();
            break;
        case GameState::PLAYING:
        case GameState::PAUSED:
            drawGameScene();
            if (state == GameState::PAUSED)
                drawPauseOverlay();
            break;
        case GameState::GAME_OVER:
            drawGameScene();
            drawGameOver();
            break;
        case GameState::HIGH_SCORES:
            drawHighScoresScreen();
            break;
        case GameState::HELP:
            drawHelpScreen();
            break;
        }

        particles.draw(window);
        drawFloatingTexts();
        transition.draw(window);

        if (fontLoaded)
        {
            Text fsHint(font);
            fsHint.setString("F11: Fullscreen");
            Text musicHint(font);
            Text soundHint(font);
            string soundStatus = soundEffectsEnabled ? "SFX: ON [N]" : "SFX: OFF [N]";
            soundHint.setString(soundStatus);
            soundHint.setCharacterSize(10);
            soundHint.setPosition(Vector2f(WINDOW_WIDTH - 180, WINDOW_HEIGHT - 15));
            soundHint.setFillColor(soundEffectsEnabled ? Color(100, 200, 100, 150) : Color(150, 80, 80, 120));
            window.draw(soundHint);
            string musicStatus = musicEnabled ? "Music: ON [M]" : "Music: OFF [M]";
            musicHint.setString(musicStatus);
            musicHint.setCharacterSize(10);
            musicHint.setPosition(Vector2f(WINDOW_WIDTH - 90, WINDOW_HEIGHT - 15));
            musicHint.setFillColor(musicEnabled ? Color(100, 200, 100, 150) : Color(150, 80, 80, 120));
            window.draw(musicHint);
            fsHint.setCharacterSize(10);
            fsHint.setPosition(Vector2f(5, WINDOW_HEIGHT - 15));
            fsHint.setFillColor(Color(80, 80, 100, 120));
            window.draw(fsHint);
        }

        window.display();
    }

    void drawSkyBackground()
    {
        //Gradient sky
        for (int i = 0; i < 10; i++)
        {
            float t = i / 10.f;
            RectangleShape strip(Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT / 10.f + 1));
            strip.setPosition(Vector2f(0, t * WINDOW_HEIGHT));
            int r = static_cast<int>(5 + t * 15);
            int g = static_cast<int>(3 + t * 10);
            int b = static_cast<int>(15 + t * 20);
            strip.setFillColor(Color(r, g, b));
            window.draw(strip);
        }

        drawDistantMountains();
    }

    void drawDistantMountains()
    {
        Color farColor(25, 20, 40, 180);
        for (int i = 0; i < 5; i++)
        {
            ConvexShape mountain;
            mountain.setPointCount(3);
            float x = i * 180 - 80;
            float h = 70 + (i % 3) * 35;
            mountain.setPoint(0, Vector2f(x, WINDOW_HEIGHT * 0.72f));
            mountain.setPoint(1, Vector2f(x + 100, WINDOW_HEIGHT * 0.72f - h));
            mountain.setPoint(2, Vector2f(x + 200, WINDOW_HEIGHT * 0.72f));
            mountain.setFillColor(farColor);
            window.draw(mountain);
        }

        Color midColor(35, 28, 50, 200);
        for (int i = 0; i < 4; i++)
        {
            ConvexShape mountain;
            mountain.setPointCount(3);
            float x = i * 220 + 30;
            float h = 90 + (i % 2) * 45;
            mountain.setPoint(0, Vector2f(x, WINDOW_HEIGHT * 0.78f));
            mountain.setPoint(1, Vector2f(x + 130, WINDOW_HEIGHT * 0.78f - h));
            mountain.setPoint(2, Vector2f(x + 260, WINDOW_HEIGHT * 0.78f));
            mountain.setFillColor(midColor);
            window.draw(mountain);
        }
    }

    void drawMenuScreen()
    {
        //Animated gradient overlay
        float wave = sin(menuAnimation * 0.5f) * 0.1f + 0.9f;
        RectangleShape overlay(Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));
        overlay.setFillColor(Color(0, 0, 20, static_cast<uint8_t>(140 * wave)));
        window.draw(overlay);

        if (!fontLoaded)
            return;

        float bounce = sin(menuAnimation * 2.5f) * 8;
        float glow = (sin(titleGlow) + 1) / 2;

        //Floating gems around title - larger and more animated
        for (int i = 0; i < 7; i++)
        {
            float angle = menuAnimation * 0.4f + i * PI * 2 / 7;
            float radius = 190 + sin(menuAnimation * 1.5f + i) * 20;
            float gx = WINDOW_WIDTH / 2 + cos(angle) * radius;
            float gy = 135 + sin(angle) * 40;
            float gs = 28 + sin(menuAnimation * 2.5f + i * 0.5f) * 6;
            float rotation = menuAnimation * 30 + i * 45;
            drawJewelBlock(gx - gs / 2, gy - gs / 2, gs, i + 1, false, false, 0, rotation);
        }

        //Title glow effec
        for (int g = 3; g >= 0; g--)
        {
            Text titleGlowText(font);
            titleGlowText.setString("GEMSTONE");
            titleGlowText.setCharacterSize(58 + g * 2);
            titleGlowText.setPosition(Vector2f(WINDOW_WIDTH / 2 - 180 - g, 55 + bounce - g));
            titleGlowText.setFillColor(Color(255, 200, 100, static_cast<uint8_t>(30 - g * 7)));
            titleGlowText.setStyle(Text::Bold);
            window.draw(titleGlowText);
        }

        //Main title
        Text shadow(font);
        shadow.setString("GEMSTONE");
        shadow.setCharacterSize(58);
        shadow.setPosition(Vector2f(WINDOW_WIDTH / 2 - 176, 58 + bounce));
        shadow.setFillColor(Color(0, 0, 0, 180));
        shadow.setStyle(Text::Bold);
        window.draw(shadow);

        Text title(font);
        title.setString("GEMSTONE");
        title.setCharacterSize(58);
        title.setPosition(Vector2f(WINDOW_WIDTH / 2 - 180, 55 + bounce));
        title.setFillColor(Color(255, static_cast<uint8_t>(200 + glow * 55), static_cast<uint8_t>(100 + glow * 50)));
        title.setStyle(Text::Bold);
        window.draw(title);

        //Subtitle with glow
        Text shadow2(font);
        shadow2.setString("Tetris");
        shadow2.setCharacterSize(48);
        shadow2.setPosition(Vector2f(WINDOW_WIDTH / 2 - 68, 118 + bounce));
        shadow2.setFillColor(Color(0, 0, 0, 180));
        shadow2.setStyle(Text::Bold);
        window.draw(shadow2);

        Text title2(font);
        title2.setString("Tetris");
        title2.setCharacterSize(48);
        title2.setPosition(Vector2f(WINDOW_WIDTH / 2 - 70, 115 + bounce));
        title2.setFillColor(Color(100, static_cast<uint8_t>(190 + glow * 55), 255));
        title2.setStyle(Text::Bold);
        window.draw(title2);

        //Decorative line
        float lineWidth = 300 + sin(menuAnimation * 2) * 20;
        RectangleShape decoLine(Vector2f(lineWidth, 3));
        decoLine.setPosition(Vector2f(WINDOW_WIDTH / 2 - lineWidth / 2, 180));
        decoLine.setFillColor(Color(255, 215, 0, 150));
        window.draw(decoLine);

        //Menu panel
        float panelW = 340;
        float panelH = 280;
        float panelX = (WINDOW_WIDTH - panelW) / 2;
        float panelY = 210;

        RectangleShape menuPanel(Vector2f(panelW, panelH));
        menuPanel.setPosition(Vector2f(panelX, panelY));
        menuPanel.setFillColor(Color(10, 8, 25, 220));
        menuPanel.setOutlineThickness(2);
        menuPanel.setOutlineColor(Color(80, 60, 120));
        window.draw(menuPanel);

        drawCornerDeco(panelX, panelY, panelW, panelH);

        //Menu options
        string menuItems[] = {"START GAME", "HIGH SCORES", "HOW TO PLAY", "EXIT"};
        string menuKeys[] = {"1", "2", "3", "4"};
        
        for (int i = 0; i < 4; i++)
        {
            bool selected = (i == menuSelection);
            float itemY = panelY + 25 + i * 62;
            float pulse = selected ? (sin(menuAnimation * 6) * 0.15f + 1.0f) : 1.0f;
            float itemWidth = 300 * pulse;
            
            //Item background
            RectangleShape itemBg(Vector2f(itemWidth, 50));
            itemBg.setPosition(Vector2f(WINDOW_WIDTH / 2 - itemWidth / 2, itemY));
            
            if (selected)
            {
                // Gradient like effect for selected item
                itemBg.setFillColor(Color(50, 35, 80, 230));
                itemBg.setOutlineThickness(2);
                itemBg.setOutlineColor(Color(255, 215, 0, static_cast<uint8_t>(180 + glow * 75)));
            }
            else
            {
                itemBg.setFillColor(Color(25, 18, 45, 180));
                itemBg.setOutlineThickness(1);
                itemBg.setOutlineColor(Color(60, 50, 90));
            }
            window.draw(itemBg);

            // Key number
            Text keyText(font);
            keyText.setString("[" + menuKeys[i] + "]");
            keyText.setCharacterSize(14);
            keyText.setPosition(Vector2f(WINDOW_WIDTH / 2 - itemWidth / 2 + 15, itemY + 17));
            keyText.setFillColor(selected ? Color(255, 215, 0) : Color(120, 100, 150));
            window.draw(keyText);

            // Menu text
            Text item(font);
            item.setString(menuItems[i]);
            item.setCharacterSize(22);
            item.setPosition(Vector2f(WINDOW_WIDTH / 2 - 60, itemY + 13));
            item.setFillColor(selected ? Color(255, 255, 255) : Color(160, 160, 190));
            if (selected)
                item.setStyle(Text::Bold);
            window.draw(item);

            // Selection arrow
            if (selected)
            {
                float arrowPulse = sin(menuAnimation * 8) * 5;
                ConvexShape arrow;
                arrow.setPointCount(3);
                float ax = WINDOW_WIDTH / 2 - itemWidth / 2 - 20 + arrowPulse;
                float ay = itemY + 25;
                arrow.setPoint(0, Vector2f(ax, ay - 12));
                arrow.setPoint(1, Vector2f(ax + 18, ay));
                arrow.setPoint(2, Vector2f(ax, ay + 12));
                arrow.setFillColor(Color(255, 215, 0));
                window.draw(arrow);

                // Right arrow too
                ConvexShape arrow2;
                arrow2.setPointCount(3);
                float ax2 = WINDOW_WIDTH / 2 + itemWidth / 2 + 5 - arrowPulse;
                arrow2.setPoint(0, Vector2f(ax2, ay - 12));
                arrow2.setPoint(1, Vector2f(ax2 - 18, ay));
                arrow2.setPoint(2, Vector2f(ax2, ay + 12));
                arrow2.setFillColor(Color(255, 215, 0));
                window.draw(arrow2);
            }
        }

        // Instructions
        Text hint(font);
        hint.setString("Arrow Keys + Enter  |  Press 1-4");
        hint.setCharacterSize(13);
        hint.setPosition(Vector2f(WINDOW_WIDTH / 2 - 120, WINDOW_HEIGHT - 35));
        hint.setFillColor(Color(100, 100, 140));
        window.draw(hint);
    }

    void drawDifficultyScreen()
    {
        // Animated background
        RectangleShape overlay(Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));
        overlay.setFillColor(Color(0, 0, 25, 200));
        window.draw(overlay);

        if (!fontLoaded)
            return;

        float bounce = sin(difficultySelectAnimation * 3) * 5;

        // Title
        Text title(font);
        title.setString("SELECT DIFFICULTY");
        title.setCharacterSize(42);
        title.setPosition(Vector2f(WINDOW_WIDTH / 2 - 190, 60 + bounce));
        title.setFillColor(Color(255, 215, 0));
        title.setStyle(Text::Bold);
        window.draw(title);

        // Decorative gems on sides
        for (int side = 0; side < 2; side++)
        {
            for (int i = 0; i < 4; i++)
            {
                float x = side == 0 ? 40 : WINDOW_WIDTH - 70;
                float y = 150 + i * 120;
                float wobble = sin(difficultySelectAnimation * 2 + i) * 8;
                drawJewelBlock(x + wobble * (side == 0 ? 1 : -1), y, 30, (i % 7) + 1, false, false);
            }
        }

        // Difficulty options
        float cardWidth = 280;
        float cardHeight = 320;
        float spacing = 40;
        float startX = (WINDOW_WIDTH - cardWidth * 2 - spacing) / 2;
        float cardY = 140;

        for (int i = 0; i < 2; i++)
        {
            bool selected = (i == difficultySelection);
            float cardX = startX + i * (cardWidth + spacing);
            float pulse = selected ? sin(difficultySelectAnimation * 5) * 0.05f + 1.0f : 1.0f;
            float actualWidth = cardWidth * pulse;
            float actualHeight = cardHeight * pulse;
            float offsetX = (cardWidth - actualWidth) / 2;
            float offsetY = (cardHeight - actualHeight) / 2;

            //Card background
            RectangleShape card(Vector2f(actualWidth, actualHeight));
            card.setPosition(Vector2f(cardX + offsetX, cardY + offsetY));
            
            if (selected)
            {
                card.setFillColor(Color(30, 25, 55, 240));
                card.setOutlineThickness(4);
                Color outlineColor = i == 0 ? Color(100, 255, 100) : Color(255, 100, 100);
                card.setOutlineColor(outlineColor);
            }
            else
            {
                card.setFillColor(Color(15, 12, 35, 200));
                card.setOutlineThickness(2);
                card.setOutlineColor(Color(60, 50, 90));
            }
            window.draw(card);

            if (selected)
                drawCornerDeco(cardX + offsetX, cardY + offsetY, actualWidth, actualHeight);

            // Difficulty icon (gem cluster)
            float iconX = cardX + cardWidth / 2;
            float iconY = cardY + 60;
            int numGems = i == 0 ? 4 : 7;
            float gemSize = 24;
            
            for (int g = 0; g < numGems; g++)
            {
                float angle = g * 2 * PI / numGems - PI / 2;
                float radius = 35;
                float gx = iconX + cos(angle) * radius - gemSize / 2;
                float gy = iconY + sin(angle) * radius - gemSize / 2;
                int colorIdx = i == 0 ? (g % 4 == 0 ? 1 : g % 4 == 1 ? 7 : g % 4 == 2 ? 4 : 5) : (g % 7) + 1;
                drawJewelBlock(gx, gy, gemSize, colorIdx, false, false, 0, difficultySelectAnimation * 20 + g * 20);
            }

            // Difficulty name
            Text diffName(font);
            diffName.setString(i == 0 ? "BEGINNER" : "ADVANCED");
            diffName.setCharacterSize(28);
            diffName.setPosition(Vector2f(cardX + cardWidth / 2 - (i == 0 ? 75 : 80), cardY + 115));
            diffName.setFillColor(i == 0 ? Color(100, 255, 100) : Color(255, 100, 100));
            diffName.setStyle(Text::Bold);
            window.draw(diffName);

            // Key hint
            Text keyHint(font);
            keyHint.setString(i == 0 ? "[1] or [B]" : "[2] or [A]");
            keyHint.setCharacterSize(14);
            keyHint.setPosition(Vector2f(cardX + cardWidth / 2 - 40, cardY + 150));
            keyHint.setFillColor(Color(140, 140, 170));
            window.draw(keyHint);

            // Description
            vector<string> desc;
            if (i == 0)
            {
                desc = {"4 Block Types:", "I, O, T, L pieces", "", "Slower Speed", "", "Perfect for", "learning!"};
            }
            else
            {
                desc = {"All 7 Block Types:", "Full Tetris experience", "", "Faster Speed", "", "For experienced", "players!"};
            }

            float descY = cardY + 180;
            for (const string& line : desc)
            {
                Text descText(font);
                descText.setString(line);
                descText.setCharacterSize(13);
                descText.setPosition(Vector2f(cardX + 30, descY));
                descText.setFillColor(line.empty() ? Color::Transparent : 
                                     (line.find(':') != string::npos ? Color(200, 180, 255) : Color(180, 180, 200)));
                window.draw(descText);
                descY += 18;
            }

            // Selection indicator
            if (selected)
            {
                float indicatorY = cardY + cardHeight + 15;
                ConvexShape indicator;
                indicator.setPointCount(3);
                indicator.setPoint(0, Vector2f(cardX + cardWidth / 2 - 15, indicatorY));
                indicator.setPoint(1, Vector2f(cardX + cardWidth / 2 + 15, indicatorY));
                indicator.setPoint(2, Vector2f(cardX + cardWidth / 2, indicatorY - 15));
                indicator.setFillColor(i == 0 ? Color(100, 255, 100) : Color(255, 100, 100));
                window.draw(indicator);
            }
        }

        // Instructions
        Text hint(font);
        hint.setString("Arrow Keys to select  |  Enter to start  |  ESC to go back");
        hint.setCharacterSize(14);
        hint.setPosition(Vector2f(WINDOW_WIDTH / 2 - 200, WINDOW_HEIGHT - 40));
        hint.setFillColor(Color(120, 120, 160));
        window.draw(hint);
    }

    void drawHighScoresScreen()
    {
        RectangleShape overlay(Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));
        overlay.setFillColor(Color(0, 0, 20, 235));
        window.draw(overlay);

        if (!fontLoaded)
            return;

        // Animated gems on sides
        for (int i = 0; i < 5; i++)
        {
            float wobble = sin(gameTime * 2 + i) * 10;
            drawJewelBlock(30 + wobble, 100 + i * 110, 25, (i % 7) + 1, false, false);
            drawJewelBlock(WINDOW_WIDTH - 55 - wobble, 100 + i * 110, 25, ((i + 3) % 7) + 1, false, false);
        }

        Text title(font);
        title.setString("HIGH SCORES");
        title.setCharacterSize(46);
        title.setPosition(Vector2f(WINDOW_WIDTH / 2 - 155, 40));
        title.setFillColor(Color(255, 215, 0));
        title.setStyle(Text::Bold);
        window.draw(title);

        //Trophy icon
        CircleShape trophy(25);
        trophy.setPosition(Vector2f(WINDOW_WIDTH / 2 - 25, 95));
        trophy.setFillColor(Color(255, 215, 0, 100));
        window.draw(trophy);

        float tableX = WINDOW_WIDTH / 2 - 200;
        float tableY = 145;
        
        RectangleShape tableBg(Vector2f(400, 400));
        tableBg.setPosition(Vector2f(tableX - 20, tableY - 10));
        tableBg.setFillColor(Color(15, 10, 30, 245));
        tableBg.setOutlineThickness(2);
        tableBg.setOutlineColor(Color(80, 60, 120));
        window.draw(tableBg);

        drawCornerDeco(tableX - 20, tableY - 10, 400, 400);

        // Header
        Text rankHeader(font);
        rankHeader.setString("RANK");
        rankHeader.setCharacterSize(16);
        rankHeader.setPosition(Vector2f(tableX, tableY));
        rankHeader.setFillColor(Color(150, 130, 180));
        rankHeader.setStyle(Text::Bold);
        window.draw(rankHeader);

        Text nameHeader(font);
        nameHeader.setString("NAME");
        nameHeader.setCharacterSize(16);
        nameHeader.setPosition(Vector2f(tableX + 90, tableY));
        nameHeader.setFillColor(Color(150, 130, 180));
        nameHeader.setStyle(Text::Bold);
        window.draw(nameHeader);

        Text scoreHeader(font);
        scoreHeader.setString("SCORE");
        scoreHeader.setCharacterSize(16);
        scoreHeader.setPosition(Vector2f(tableX + 260, tableY));
        scoreHeader.setFillColor(Color(150, 130, 180));
        scoreHeader.setStyle(Text::Bold);
        window.draw(scoreHeader);

        for (int i = 0; i < MAX_HIGH_SCORES; i++)
        {
            float rowY = tableY + 38 + i * 35;
            
            //Row highlight for top 3
            if (i < 3)
            {
                RectangleShape rowBg(Vector2f(380, 30));
                rowBg.setPosition(Vector2f(tableX - 10, rowY - 5));
                Color bgColor = (i == 0) ? Color(255, 215, 0, 30) : 
                               (i == 1) ? Color(192, 192, 192, 25) : Color(205, 127, 50, 20);
                rowBg.setFillColor(bgColor);
                window.draw(rowBg);
            }

            Color rowColor = (i == 0) ? Color(255, 215, 0) : 
                            (i == 1) ? Color(200, 200, 220) : 
                            (i == 2) ? Color(205, 140, 80) : Color(180, 180, 200);
            
            //Medal for top 3
            if (i < 3)
            {
                CircleShape medal(11);
                medal.setPosition(Vector2f(tableX + 10, rowY));
                Color medalColor = (i == 0) ? Color(255, 215, 0) : 
                                   (i == 1) ? Color(192, 192, 192) : Color(205, 127, 50);
                medal.setFillColor(medalColor);
                medal.setOutlineThickness(1);
                medal.setOutlineColor(Color(medalColor.r / 2, medalColor.g / 2, medalColor.b / 2));
                window.draw(medal);

                Text medalNum(font);
                medalNum.setString(to_string(i + 1));
                medalNum.setCharacterSize(12);
                medalNum.setPosition(Vector2f(tableX + 16, rowY + 2));
                medalNum.setFillColor(Color(40, 30, 20));
                medalNum.setStyle(Text::Bold);
                window.draw(medalNum);
            }
            else
            {
                Text rank(font);
                rank.setString(to_string(i + 1) + ".");
                rank.setCharacterSize(18);
                rank.setPosition(Vector2f(tableX + 10, rowY));
                rank.setFillColor(rowColor);
                window.draw(rank);
            }

            Text name(font);
            name.setString(highScores[i].name);
            name.setCharacterSize(18);
            name.setPosition(Vector2f(tableX + 90, rowY));
            name.setFillColor(rowColor);
            window.draw(name);

            Text score(font);
            score.setString(to_string(highScores[i].score));
            score.setCharacterSize(18);
            score.setPosition(Vector2f(tableX + 260, rowY));
            score.setFillColor(rowColor);
            score.setStyle(i < 3 ? Text::Bold : Text::Regular);
            window.draw(score);
        }

        Text back(font);
        back.setString("Press ESC or ENTER to go back");
        back.setCharacterSize(14);
        back.setPosition(Vector2f(WINDOW_WIDTH / 2 - 130, WINDOW_HEIGHT - 45));
        back.setFillColor(Color(140, 140, 170));
        window.draw(back);
    }

    void drawHelpScreen()
    {
        RectangleShape overlay(Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));
        overlay.setFillColor(Color(0, 0, 20, 235));
        window.draw(overlay);

        if (!fontLoaded)
            return;

        Text title(font);
        title.setString("HOW TO PLAY");
        title.setCharacterSize(42);
        title.setPosition(Vector2f(WINDOW_WIDTH / 2 - 130, 25));
        title.setFillColor(Color(255, 215, 0));
        title.setStyle(Text::Bold);
        window.draw(title);

        float panelX = 40;
        float panelY = 85;
        float panelW = WINDOW_WIDTH - 80;
        float panelH = WINDOW_HEIGHT - 130;

        RectangleShape panel(Vector2f(panelW, panelH));
        panel.setPosition(Vector2f(panelX, panelY));
        panel.setFillColor(Color(15, 10, 30, 245));
        panel.setOutlineThickness(2);
        panel.setOutlineColor(Color(80, 60, 120));
        window.draw(panel);

        drawCornerDeco(panelX, panelY, panelW, panelH);

        // Controls section
        Text controlsTitle(font);
        controlsTitle.setString("CONTROLS");
        controlsTitle.setCharacterSize(20);
        controlsTitle.setPosition(Vector2f(panelX + 25, panelY + 15));
        controlsTitle.setFillColor(Color(100, 200, 255));
        controlsTitle.setStyle(Text::Bold);
        window.draw(controlsTitle);

        string controls[] = {
            "LEFT / RIGHT    Move piece",
            "UP              Rotate piece",
            "DOWN            Soft drop",
            "SPACE           Hard drop",
            "C               Hold piece",
            "P / ESC         Pause",
            "M               MUSIC ON/OFF",
            "N               SFX ON/OFF"
        };

        for (int i = 0; i < 8; i++)
        {
            Text control(font);
            control.setString(controls[i]);
            control.setCharacterSize(13);
            control.setPosition(Vector2f(panelX + 40, panelY + 45 + i * 22));
            control.setFillColor(Color(200, 200, 220));
            window.draw(control);
        }

        // Scoring section
        Text scoringTitle(font);
        scoringTitle.setString("SCORING");
        scoringTitle.setCharacterSize(20);
        scoringTitle.setPosition(Vector2f(panelX + 25, panelY + 250));
        scoringTitle.setFillColor(Color(100, 200, 255));
        scoringTitle.setStyle(Text::Bold);
        window.draw(scoringTitle);

        string scoring[] = {
            "1 Line  =  10 x Level",
            "2 Lines =  30 x Level",
            "3 Lines =  60 x Level",
            "4 Lines = 100 x Level (TETRIS!)"
        };

        for (int i = 0; i < 4; i++)
        {
            Text score(font);
            score.setString(scoring[i]);
            score.setCharacterSize(13);
            score.setPosition(Vector2f(panelX + 40, panelY + 280 + i * 22));
            score.setFillColor(Color(200, 200, 220));
            window.draw(score);
        }

        // Difficulty section (right side)
        Text diffTitle(font);
        diffTitle.setString("DIFFICULTY MODES");
        diffTitle.setCharacterSize(20);
        diffTitle.setPosition(Vector2f(panelX + 320, panelY + 15));
        diffTitle.setFillColor(Color(100, 200, 255));
        diffTitle.setStyle(Text::Bold);
        window.draw(diffTitle);

        // Beginner
        RectangleShape beginnerBox(Vector2f(250, 90));
        beginnerBox.setPosition(Vector2f(panelX + 320, panelY + 45));
        beginnerBox.setFillColor(Color(20, 40, 20, 180));
        beginnerBox.setOutlineThickness(1);
        beginnerBox.setOutlineColor(Color(100, 200, 100));
        window.draw(beginnerBox);

        Text beginnerTitle(font);
        beginnerTitle.setString("BEGINNER");
        beginnerTitle.setCharacterSize(16);
        beginnerTitle.setPosition(Vector2f(panelX + 335, panelY + 50));
        beginnerTitle.setFillColor(Color(100, 255, 100));
        beginnerTitle.setStyle(Text::Bold);
        window.draw(beginnerTitle);

        Text beginnerDesc(font);
        beginnerDesc.setString("4 block types (I, O, T, L)\nSlower falling speed\nGreat for learning!");
        beginnerDesc.setCharacterSize(11);
        beginnerDesc.setPosition(Vector2f(panelX + 335, panelY + 75));
        beginnerDesc.setFillColor(Color(180, 220, 180));
        window.draw(beginnerDesc);

        // Advanced
        RectangleShape advancedBox(Vector2f(250, 90));
        advancedBox.setPosition(Vector2f(panelX + 320, panelY + 145));
        advancedBox.setFillColor(Color(40, 20, 20, 180));
        advancedBox.setOutlineThickness(1);
        advancedBox.setOutlineColor(Color(200, 100, 100));
        window.draw(advancedBox);

        Text advancedTitle(font);
        advancedTitle.setString("ADVANCED");
        advancedTitle.setCharacterSize(16);
        advancedTitle.setPosition(Vector2f(panelX + 335, panelY + 150));
        advancedTitle.setFillColor(Color(255, 100, 100));
        advancedTitle.setStyle(Text::Bold);
        window.draw(advancedTitle);

        Text advancedDesc(font);
        advancedDesc.setString("All 7 block types\nFaster falling speed\nClassic Tetris challenge!");
        advancedDesc.setCharacterSize(11);
        advancedDesc.setPosition(Vector2f(panelX + 335, panelY + 175));
        advancedDesc.setFillColor(Color(220, 180, 180));
        window.draw(advancedDesc);

        // Struggle rule
        Text struggleTitle(font);
        struggleTitle.setString("STRUGGLE RULE");
        struggleTitle.setCharacterSize(20);
        struggleTitle.setPosition(Vector2f(panelX + 320, panelY + 255));
        struggleTitle.setFillColor(Color(255, 150, 100));
        struggleTitle.setStyle(Text::Bold);
        window.draw(struggleTitle);

        Text struggle(font);
        struggle.setString("If no lines cleared for 5 minutes,\nbottom row freezes permanently!\nKeep clearing to stay alive!");
        struggle.setCharacterSize(12);
        struggle.setPosition(Vector2f(panelX + 335, panelY + 285));
        struggle.setFillColor(Color(255, 200, 180));
         window.draw(struggle);

        // Game over info
        Text gameOverTitle(font);
        gameOverTitle.setString("GAME OVER");
        gameOverTitle.setCharacterSize(18);
        gameOverTitle.setPosition(Vector2f(panelX + 25, panelY + 385));
        gameOverTitle.setFillColor(Color(255, 100, 100));
        gameOverTitle.setStyle(Text::Bold);
        window.draw(gameOverTitle);

        Text gameOver(font);
        gameOver.setString("Game ends when blocks reach the top!");
        gameOver.setCharacterSize(12);
        gameOver.setPosition(Vector2f(panelX + 40, panelY + 410));
        gameOver.setFillColor(Color(200, 200, 220));
        window.draw(gameOver);

        // Gem display
        Text gemsTitle(font);
        gemsTitle.setString("GEM TYPES");
        gemsTitle.setCharacterSize(18);
        gemsTitle.setPosition(Vector2f(panelX + 25, panelY + 445));
        gemsTitle.setFillColor(Color(100, 200, 255));
        gemsTitle.setStyle(Text::Bold);
        window.draw(gemsTitle);

        string gemNames[] = {"Aquamarine", "Ruby", "Emerald", "Amethyst", "Topaz", "Sapphire", "Citrine"};
        for (int i = 0; i < 7; i++)
        {
            float gx = panelX + 40 + (i % 4) * 140;
            float gy = panelY + 475 + (i / 4) * 50;
            drawJewelBlock(gx, gy, 28, i + 1, false, false);
            
            Text gemName(font);
            gemName.setString(gemNames[i]);
            gemName.setCharacterSize(10);
            gemName.setPosition(Vector2f(gx + 32, gy + 8));
            gemName.setFillColor(jewelPalette[i + 1].light);
            window.draw(gemName);
        }

        Text back(font);
        back.setString("Press ESC or ENTER to go back");
        back.setCharacterSize(14);
        back.setPosition(Vector2f(WINDOW_WIDTH / 2 - 130, WINDOW_HEIGHT - 35));
        back.setFillColor(Color(140, 140, 170));
        window.draw(back);
    }

    void drawGameScene()
    {
        drawGameCliff(0, true);
        drawGameCliff(WINDOW_WIDTH - LEFT_MARGIN + SIDEBAR_WIDTH, false);
        drawGameChasm();
        drawGameBoard();
        drawHoldPanel();
        drawStatsPanel();
        drawNextPanel();
    }

    void drawGameCliff(float x, bool isEnemy)
    {
        float cliffWidth = isEnemy ? LEFT_MARGIN : (WINDOW_WIDTH - boardX - boardWidth);

        Color baseColor = isEnemy ? Color(40, 30, 45) : Color(45, 50, 60);
        Color darkColor = isEnemy ? Color(28, 20, 32) : Color(32, 38, 48);

        RectangleShape cliff(Vector2f(cliffWidth, WINDOW_HEIGHT));
        cliff.setPosition(Vector2f(x, 0));
        cliff.setFillColor(baseColor);
        window.draw(cliff);

        for (int i = 0; i < 12; i++)
        {
            float lineY = 30 + i * 55;
            float indent = (i % 2) * 8;
            RectangleShape layer(Vector2f(cliffWidth - 20 - indent, 3));
            layer.setPosition(Vector2f(x + 10 + indent / 2, lineY));
            layer.setFillColor(darkColor);
            window.draw(layer);
        }

        float edgeX = isEnemy ? x + cliffWidth - 8 : x;
        RectangleShape edge(Vector2f(8, WINDOW_HEIGHT));
        edge.setPosition(Vector2f(edgeX, 0));
        edge.setFillColor(Color(baseColor.r + 15, baseColor.g + 15, baseColor.b + 15));
        window.draw(edge);

        if (isEnemy)
            drawSmallTorch(x + cliffWidth - 40, 150);
        else
            drawSmallTorch(x + 25, 150);
    }

    void drawSmallTorch(float x, float y)
    {
        RectangleShape bracket(Vector2f(4, 15));
        bracket.setPosition(Vector2f(x, y));
        bracket.setFillColor(Color(55, 45, 35));
        window.draw(bracket);

        float flicker = sin(gameTime * 10 + x * 0.1f) * 0.3f + 0.7f;
        CircleShape glow(12 * flicker);
        glow.setOrigin(Vector2f(12 * flicker, 12 * flicker));
        glow.setPosition(Vector2f(x + 2, y - 8));
        glow.setFillColor(Color(255, 150, 50, 50));
        window.draw(glow);

        ConvexShape flame;
        flame.setPointCount(4);
        float fh = 14 * flicker;
        flame.setPoint(0, Vector2f(x + 2, y - 5 - fh));
        flame.setPoint(1, Vector2f(x + 8, y - 5));
        flame.setPoint(2, Vector2f(x + 2, y));
        flame.setPoint(3, Vector2f(x - 4, y - 5));
        flame.setFillColor(Color(255, 180, 60));
        window.draw(flame);

        if (rand() % 6 == 0)
            particles.addFireEffect(Vector2f(x + 2, y - 10), 0.4f);
    }

    void drawGameChasm()
    {
        RectangleShape chasm(Vector2f(boardWidth + 20, WINDOW_HEIGHT));
        chasm.setPosition(Vector2f(boardX - 10, 0));
        chasm.setFillColor(Color(2, 0, 6));
        window.draw(chasm);

        for (int i = 0; i < 4; i++)
        {
            RectangleShape depth(Vector2f(boardWidth + 20 - i * 10, 40));
            depth.setPosition(Vector2f(boardX - 10 + i * 5, WINDOW_HEIGHT - 60 + i * 15));
            depth.setFillColor(Color(8 + i * 4, 2 + i * 2, 15 + i * 4, 120));
            window.draw(depth);
        }

        particles.addChasmMist(boardX, WINDOW_HEIGHT - 20, boardWidth);
    }

    void drawGameBoard()
    {
        // Outer glow
        RectangleShape glow(Vector2f(boardWidth + 20, boardHeight + 20));
        glow.setPosition(Vector2f(boardX - 10, boardY - 10));
        glow.setFillColor(Color(50, 30, 80, 40));
        window.draw(glow);

        RectangleShape board(Vector2f(boardWidth, boardHeight));
        board.setPosition(Vector2f(boardX, boardY));
        board.setFillColor(Color(4, 2, 10, 245));
        board.setOutlineThickness(3);
        board.setOutlineColor(Color(70, 55, 100));
        window.draw(board);

        // Grid lines
        for (int i = 0; i <= ROWS; i++)
        {
            uint8_t alpha = static_cast<uint8_t>(20 + (float)i / ROWS * 25);
            RectangleShape line(Vector2f(boardWidth, 1));
            line.setPosition(Vector2f(boardX, boardY + i * BLOCK_SIZE));
            line.setFillColor(Color(60, 45, 85, alpha));
            window.draw(line);
        }
        for (int j = 0; j <= COLUMNS; j++)
        {
            RectangleShape line(Vector2f(1, boardHeight));
            line.setPosition(Vector2f(boardX + j * BLOCK_SIZE, boardY));
            line.setFillColor(Color(60, 45, 85, 25));
            window.draw(line);
        }

        // Danger zone indicator
        float danger = tetris.getDangerLevel();
        if (danger > 0.3f)
        {
            float intensity = (danger - 0.3f) / 0.7f;
            float pulse = (sin(gameTime * 4 * intensity) + 1) / 2;
            int dangerRows = 2 + (int)(danger * 4);
            RectangleShape dangerZone(Vector2f(boardWidth, dangerRows * BLOCK_SIZE));
            dangerZone.setPosition(Vector2f(boardX, boardY));
            dangerZone.setFillColor(Color(150, 0, 0, static_cast<uint8_t>((15 + pulse * 35) * intensity)));
            window.draw(dangerZone);
        }

        // Frozen rows indicator
        int frozenRows = tetris.getFrozenRows();
        if (frozenRows > 0)
        {
            RectangleShape frozenZone(Vector2f(boardWidth, frozenRows * BLOCK_SIZE));
            frozenZone.setPosition(Vector2f(boardX, boardY + (ROWS - frozenRows) * BLOCK_SIZE));
            frozenZone.setFillColor(Color(100, 130, 180, 40));
            window.draw(frozenZone);
        }

        // Draw placed blocks
        for (int i = 0; i < ROWS; i++)
        {
            for (int j = 0; j < COLUMNS; j++)
            {
                int colorIdx = tetris.getField(i, j);
                if (colorIdx == 0)
                    continue;

                bool isClearing = false;
                if (clearingLines)
                {
                    for (int row : linesToClear)
                        if (row == i)
                        {
                            isClearing = true;
                            break;
                        }
                }

                bool isFrozen = tetris.isFrozen(i, j);

                if (isClearing)
                {
                    float flash = sin(lineClearTimer * 50) * 0.5f + 0.5f;
                    float scale = 1.0f + flash * 0.15f;
                    float offset = (scale - 1.0f) * BLOCK_SIZE / 2;
                    drawJewelBlock(boardX + j * BLOCK_SIZE - offset,
                                   boardY + i * BLOCK_SIZE - offset,
                                   BLOCK_SIZE * scale, colorIdx, false, false, flash);
                }
                else
                {
                    drawJewelBlock(boardX + j * BLOCK_SIZE, boardY + i * BLOCK_SIZE,
                                   BLOCK_SIZE, colorIdx, false, isFrozen);
                }
            }
        }

        //Draw ghost piece
        if (state == GameState::PLAYING && !clearingLines)
        {
            Point ghost[4];
            tetris.getGhostPosition(ghost);
            for (int i = 0; i < 4; i++)
            {
                if (ghost[i].y >= 0)
                {
                    drawJewelBlock(boardX + ghost[i].x * BLOCK_SIZE,
                                   boardY + ghost[i].y * BLOCK_SIZE,
                                   BLOCK_SIZE, tetris.getCurrentColor(), true, false);
                }
            }

            // Draw active piece with glow
            Point *pos = tetris.getActivePositions();
            for (int i = 0; i < 4; i++)
            {
                if (pos[i].y >= 0)
                {
                    JewelColors &jc = jewelPalette[tetris.getCurrentColor()];
                    
                    //Animated glow
                    float glowPulse = (sin(gameTime * 4) + 1) / 2 * 0.4f + 0.6f;
                    RectangleShape pieceGlow(Vector2f(BLOCK_SIZE + 10, BLOCK_SIZE + 10));
                    pieceGlow.setPosition(Vector2f(boardX + pos[i].x * BLOCK_SIZE - 5,
                                                   boardY + pos[i].y * BLOCK_SIZE - 5));
                    pieceGlow.setFillColor(Color(jc.glow.r, jc.glow.g, jc.glow.b, 
                                                 static_cast<uint8_t>(jc.glow.a * glowPulse)));
                    window.draw(pieceGlow);

                    drawJewelBlock(boardX + pos[i].x * BLOCK_SIZE,
                                   boardY + pos[i].y * BLOCK_SIZE,
                                   BLOCK_SIZE, tetris.getCurrentColor(), false, false);
                }
            }
        }
    }

    void drawJewelBlock(float x, float y, float size, int colorIdx, bool ghost = false, 
                        bool frozen = false, float flashAmount = 0.f, float rotation = 0.f)
    {
        if (colorIdx < 1 || colorIdx > 7)
            return;

        JewelColors jc = jewelPalette[colorIdx];

        if (frozen)
        {
            auto freezify = [](Color c) -> Color
            {
                int avg = (c.r + c.g + c.b) / 3;
                return Color(
                    static_cast<uint8_t>((c.r + avg * 2) / 3),
                    static_cast<uint8_t>((c.g + avg * 2) / 3),
                    static_cast<uint8_t>(min(255, (c.b + avg * 2) / 3 + 40)),
                    c.a);
            };
            jc.base = freezify(jc.base);
            jc.light = freezify(jc.light);
            jc.dark = freezify(jc.dark);
            jc.innerGlow = freezify(jc.innerGlow);
        }

        if (flashAmount > 0)
        {
            auto flashify = [flashAmount](Color c) -> Color
            {
                return Color(
                    static_cast<uint8_t>(min(255.f, c.r + (255 - c.r) * flashAmount)),
                    static_cast<uint8_t>(min(255.f, c.g + (255 - c.g) * flashAmount)),
                    static_cast<uint8_t>(min(255.f, c.b + (255 - c.b) * flashAmount)),
                    c.a);
            };
            jc.base = flashify(jc.base);
            jc.light = flashify(jc.light);
            jc.dark = flashify(jc.dark);
            jc.innerGlow = flashify(jc.innerGlow);
        }

        float margin = 1;
        float innerSize = size - margin * 2;
        float cx = x + size / 2;
        float cy = y + size / 2;

        if (ghost)
        {
            RectangleShape outline(Vector2f(innerSize, innerSize));
            outline.setPosition(Vector2f(x + margin, y + margin));
            outline.setFillColor(Color(jc.base.r, jc.base.g, jc.base.b, 25));
            outline.setOutlineThickness(2);
            outline.setOutlineColor(Color(jc.light.r, jc.light.g, jc.light.b, 80));
            window.draw(outline);

            //Ghost diamond
            float r = innerSize * 0.2f;
            ConvexShape diamond;
            diamond.setPointCount(4);
            diamond.setPoint(0, Vector2f(cx, cy - r));
            diamond.setPoint(1, Vector2f(cx + r, cy));
            diamond.setPoint(2, Vector2f(cx, cy + r));
            diamond.setPoint(3, Vector2f(cx - r, cy));
            diamond.setFillColor(Color(255, 255, 255, 25));
            window.draw(diamond);
            return;
        }

        //Outer glow for shiny effect
        RectangleShape outerGlow(Vector2f(innerSize + 4, innerSize + 4));
        outerGlow.setPosition(Vector2f(x + margin - 2, y + margin - 2));
        outerGlow.setFillColor(Color(jc.glow.r, jc.glow.g, jc.glow.b, 30));
        window.draw(outerGlow);

        //Main gem body - octagonal shape
        ConvexShape gemBody;
        gemBody.setPointCount(8);
        float cut = innerSize * 0.18f;
        gemBody.setPoint(0, Vector2f(x + margin + cut, y + margin));
        gemBody.setPoint(1, Vector2f(x + margin + innerSize - cut, y + margin));
        gemBody.setPoint(2, Vector2f(x + margin + innerSize, y + margin + cut));
        gemBody.setPoint(3, Vector2f(x + margin + innerSize, y + margin + innerSize - cut));
        gemBody.setPoint(4, Vector2f(x + margin + innerSize - cut, y + margin + innerSize));
        gemBody.setPoint(5, Vector2f(x + margin + cut, y + margin + innerSize));
        gemBody.setPoint(6, Vector2f(x + margin, y + margin + innerSize - cut));
        gemBody.setPoint(7, Vector2f(x + margin, y + margin + cut));
        gemBody.setFillColor(jc.base);
        gemBody.setOutlineThickness(1);
        gemBody.setOutlineColor(jc.dark);
        window.draw(gemBody);

        float facetDepth = innerSize * 0.28f;

        // Top facet (lightest - main highlight)
        ConvexShape topFacet;
        topFacet.setPointCount(4);
        topFacet.setPoint(0, Vector2f(x + margin + cut, y + margin));
        topFacet.setPoint(1, Vector2f(x + margin + innerSize - cut, y + margin));
        topFacet.setPoint(2, Vector2f(x + margin + innerSize - cut - facetDepth * 0.5f, y + margin + facetDepth));
        topFacet.setPoint(3, Vector2f(x + margin + cut + facetDepth * 0.5f, y + margin + facetDepth));
        topFacet.setFillColor(jc.light);
        window.draw(topFacet);

        // Left facet
        ConvexShape leftFacet;
        leftFacet.setPointCount(4);
        leftFacet.setPoint(0, Vector2f(x + margin, y + margin + cut));
        leftFacet.setPoint(1, Vector2f(x + margin + facetDepth, y + margin + cut + facetDepth * 0.5f));
        leftFacet.setPoint(2, Vector2f(x + margin + facetDepth, y + margin + innerSize - cut - facetDepth * 0.5f));
        leftFacet.setPoint(3, Vector2f(x + margin, y + margin + innerSize - cut));
        leftFacet.setFillColor(Color((jc.base.r + jc.light.r) / 2, 
                                     (jc.base.g + jc.light.g) / 2, 
                                     (jc.base.b + jc.light.b) / 2));
        window.draw(leftFacet);

        // Right facet (darker)
        ConvexShape rightFacet;
        rightFacet.setPointCount(4);
        rightFacet.setPoint(0, Vector2f(x + margin + innerSize, y + margin + cut));
        rightFacet.setPoint(1, Vector2f(x + margin + innerSize, y + margin + innerSize - cut));
        rightFacet.setPoint(2, Vector2f(x + margin + innerSize - facetDepth, y + margin + innerSize - cut - facetDepth * 0.5f));
        rightFacet.setPoint(3, Vector2f(x + margin + innerSize - facetDepth, y + margin + cut + facetDepth * 0.5f));
        rightFacet.setFillColor(jc.dark);
        window.draw(rightFacet);

        // Bottom facet (darkest)
        ConvexShape bottomFacet;
        bottomFacet.setPointCount(4);
        bottomFacet.setPoint(0, Vector2f(x + margin + cut, y + margin + innerSize));
        bottomFacet.setPoint(1, Vector2f(x + margin + innerSize - cut, y + margin + innerSize));
        bottomFacet.setPoint(2, Vector2f(x + margin + innerSize - cut - facetDepth * 0.5f, y + margin + innerSize - facetDepth));
        bottomFacet.setPoint(3, Vector2f(x + margin + cut + facetDepth * 0.5f, y + margin + innerSize - facetDepth));
        bottomFacet.setFillColor(Color(jc.dark.r * 0.65f, jc.dark.g * 0.65f, jc.dark.b * 0.65f));
        window.draw(bottomFacet);

        // Inner glow (center shine)
        float innerGlowSize = innerSize * 0.35f;
        CircleShape innerGlow(innerGlowSize);
        innerGlow.setOrigin(Vector2f(innerGlowSize, innerGlowSize));
        innerGlow.setPosition(Vector2f(cx, cy));
        innerGlow.setFillColor(Color(jc.innerGlow.r, jc.innerGlow.g, jc.innerGlow.b, 60));
        window.draw(innerGlow);

        // Center diamond highlight
        ConvexShape diamond;
        diamond.setPointCount(4);
        float dr = innerSize * 0.18f;
        diamond.setPoint(0, Vector2f(cx, cy - dr));
        diamond.setPoint(1, Vector2f(cx + dr, cy));
        diamond.setPoint(2, Vector2f(cx, cy + dr));
        diamond.setPoint(3, Vector2f(cx - dr, cy));
        diamond.setFillColor(Color(jc.light.r, jc.light.g, jc.light.b, 90));
        window.draw(diamond);

        // Primary sparkle (top-left)
        float spSize = innerSize * 0.11f;
        CircleShape sp1(spSize);
        sp1.setPosition(Vector2f(x + margin + innerSize * 0.18f, y + margin + innerSize * 0.12f));
        sp1.setFillColor(Color(255, 255, 255, 240));
        window.draw(sp1);

        // Secondary sparkle
        CircleShape sp2(spSize * 0.55f);
        sp2.setPosition(Vector2f(x + margin + innerSize * 0.35f, y + margin + innerSize * 0.24f));
        sp2.setFillColor(Color(255, 255, 255, 180));
        window.draw(sp2);

        //Tertiary sparkle (animated twinkle)
        float twinkle = (sin(gameTime * 8 + x * 0.1f + y * 0.1f) + 1) / 2;
        if (twinkle > 0.7f)
        {
            CircleShape sp3(spSize * 0.4f * twinkle);
            sp3.setPosition(Vector2f(x + margin + innerSize * 0.6f, y + margin + innerSize * 0.15f));
            sp3.setFillColor(Color(255, 255, 255, static_cast<uint8_t>(200 * twinkle)));
            window.draw(sp3);
        }

        //Star sparkle effect on some blocks
        if (!frozen && (int)(x + y) % 100 < 30)
        {
            float starTwinkle = (sin(gameTime * 6 + x * 0.2f) + 1) / 2;
            if (starTwinkle > 0.6f)
            {
                float starX = x + margin + innerSize * 0.25f;
                float starY = y + margin + innerSize * 0.2f;
                float starSize = 4 * starTwinkle;
                
                RectangleShape starH(Vector2f(starSize * 2, 1));
                starH.setOrigin(Vector2f(starSize, 0.5f));
                starH.setPosition(Vector2f(starX, starY));
                starH.setFillColor(Color(255, 255, 255, static_cast<uint8_t>(200 * starTwinkle)));
                window.draw(starH);
                
                RectangleShape starV(Vector2f(1, starSize * 2));
                starV.setOrigin(Vector2f(0.5f, starSize));
                starV.setPosition(Vector2f(starX, starY));
                starV.setFillColor(Color(255, 255, 255, static_cast<uint8_t>(200 * starTwinkle)));
                window.draw(starV);
            }
        }

        // Frozen ice overlay
        if (frozen)
        {
            RectangleShape iceOverlay(Vector2f(innerSize, innerSize));
            iceOverlay.setPosition(Vector2f(x + margin, y + margin));
            iceOverlay.setFillColor(Color(180, 220, 255, 50));
            window.draw(iceOverlay);

            // Ice crystals
            for (int i = 0; i < 4; i++)
            {
                float ix = x + margin + 5 + (rand() % (int)(innerSize - 10));
                float iy = y + margin + 5 + (rand() % (int)(innerSize - 10));
                CircleShape ice(1.5f);
                ice.setPosition(Vector2f(ix, iy));
                ice.setFillColor(Color(220, 240, 255, 120));
                window.draw(ice);
            }

            // Ice crack lines
            RectangleShape crack(Vector2f(innerSize * 0.5f, 1));
            crack.setPosition(Vector2f(x + margin + innerSize * 0.2f, y + margin + innerSize * 0.4f));
            crack.setRotation(degrees(25));
            crack.setFillColor(Color(200, 230, 255, 80));
            window.draw(crack);
        }
    }

    void drawHoldPanel()
    {
        float panelX = 10;
        float panelY = boardY;
        float panelW = LEFT_MARGIN - 25;
        float panelH = 130;

        RectangleShape panel(Vector2f(panelW, panelH));
        panel.setPosition(Vector2f(panelX, panelY));
        panel.setFillColor(Color(10, 6, 22, 240));
        panel.setOutlineThickness(2);
        panel.setOutlineColor(Color(70, 55, 100));
        window.draw(panel);

        drawCornerDeco(panelX, panelY, panelW, panelH);

        if (!fontLoaded)
            return;

        Text title(font);
        title.setString("HOLD [C]");
        title.setCharacterSize(14);
        title.setPosition(Vector2f(panelX + panelW / 2 - 38, panelY + 6));
        title.setFillColor(Color(170, 150, 210));
        title.setStyle(Text::Bold);
        window.draw(title);

        int holdColor = tetris.getHoldColor();
        if (holdColor > 0)
        {
            Point *holdPiece = tetris.getHoldPiece();
            float blockSz = BLOCK_SIZE * 0.6f;
            float offX = panelX + panelW / 2 - blockSz;
            float offY = panelY + 50;

            for (int i = 0; i < 4; i++)
            {
                drawJewelBlock(offX + holdPiece[i].x * blockSz,
                               offY + holdPiece[i].y * blockSz,
                               blockSz, holdColor, false, !tetris.canHoldPiece());
            }
        }
        else
        {
            Text empty(font);
            empty.setString("Empty");
            empty.setCharacterSize(12);
            empty.setPosition(Vector2f(panelX + panelW / 2 - 22, panelY + 65));
            empty.setFillColor(Color(70, 60, 90));
            window.draw(empty);
        }
    }

    void drawStatsPanel()
    {
        float panelX = 10;
        float panelY = boardY + 145;
        float panelW = LEFT_MARGIN - 25;
        float panelH = 230;

        RectangleShape panel(Vector2f(panelW, panelH));
        panel.setPosition(Vector2f(panelX, panelY));
        panel.setFillColor(Color(10, 6, 22, 240));
        panel.setOutlineThickness(2);
        panel.setOutlineColor(Color(70, 55, 100));
        window.draw(panel);

        drawCornerDeco(panelX, panelY, panelW, panelH);

        if (!fontLoaded)
            return;

        // Difficulty label
        string diffName = (tetris.getDifficulty() == DifficultyLevel::BEGINNER) ? "BEGINNER" : "ADVANCED";
        Color diffColor = (tetris.getDifficulty() == DifficultyLevel::BEGINNER) ? Color(100, 255, 100) : Color(255, 100, 100);
        
        Text diffLabel(font);
        diffLabel.setString(diffName);
        diffLabel.setCharacterSize(12);
        diffLabel.setPosition(Vector2f(panelX + panelW / 2 - 35, panelY + 6));
        diffLabel.setFillColor(diffColor);
        diffLabel.setStyle(Text::Bold);
        window.draw(diffLabel);

        drawStatLabel(panelX + 8, panelY + 28, "SCORE");
        drawStatValue(panelX + 8, panelY + 44, formatScore(tetris.getScore()), 20);

        drawStatLabel(panelX + 8, panelY + 78, "LEVEL");
        drawStatValue(panelX + 8, panelY + 94, to_string(tetris.getLevel()), 24);

        drawStatLabel(panelX + 8, panelY + 128, "LINES");
        drawStatValue(panelX + 8, panelY + 144, to_string(tetris.getLines()), 20, Color(100, 200, 255));

        drawStatLabel(panelX + 8, panelY + 175, "TIME");
        int minutes = (int)(totalPlayTime / 60);
        int seconds = (int)totalPlayTime % 60;
        stringstream timeStr;
        timeStr << setfill('0') << setw(2) << minutes << ":" << setw(2) << seconds;
        drawStatValue(panelX + 8, panelY + 191, timeStr.str(), 18, Color(200, 200, 220));

        // Struggle warning
        float timeLeft = STRUGGLE_TIME_LIMIT - timeSinceLastClear;
        if (timeLeft < 60 && timeLeft > 0)
        {
            float pulse = (sin(gameTime * 6) + 1) / 2;
            Text warning(font);
            warning.setString("FREEZE: " + to_string((int)timeLeft) + "s");
            warning.setCharacterSize(11);
            warning.setPosition(Vector2f(panelX + 8, panelY + 212));
            warning.setFillColor(Color(255, static_cast<uint8_t>(80 + pulse * 120), 80));
            warning.setStyle(Text::Bold);
            window.draw(warning);
        }
    }

    void drawNextPanel()
    {
        float panelX = boardX + boardWidth + 10;
        float panelY = boardY;
        float panelW = WINDOW_WIDTH - panelX - 10;
        float panelH = 240;

        RectangleShape panel(Vector2f(panelW, panelH));
        panel.setPosition(Vector2f(panelX, panelY));
        panel.setFillColor(Color(10, 6, 22, 240));
        panel.setOutlineThickness(2);
        panel.setOutlineColor(Color(70, 55, 100));
        window.draw(panel);

        drawCornerDeco(panelX, panelY, panelW, panelH);

        if (!fontLoaded)
            return;

        Text title(font);
        title.setString("NEXT");
        title.setCharacterSize(14);
        title.setPosition(Vector2f(panelX + panelW / 2 - 22, panelY + 6));
        title.setFillColor(Color(255, 120, 120));
        title.setStyle(Text::Bold);
        window.draw(title);

        for (int n = 0; n < 3; n++)
        {
            Point *piece = tetris.getNextPiece(n);
            int color = tetris.getNextColor(n);
            float pieceY = panelY + 40 + n * 68;
            float blockSz = BLOCK_SIZE * 0.55f;
            float pieceX = panelX + panelW / 2 - blockSz;

            if (n > 0)
            {
                RectangleShape sep(Vector2f(panelW - 20, 1));
                sep.setPosition(Vector2f(panelX + 10, pieceY - 8));
                sep.setFillColor(Color(50, 40, 70, 100));
                window.draw(sep);
            }

            for (int i = 0; i < 4; i++)
            {
                drawJewelBlock(pieceX + piece[i].x * blockSz,
                               pieceY + piece[i].y * blockSz,
                               blockSz, color, false, false);
            }
        }
    }
    void drawStatLabel(float x, float y, const string &text)
    {
        Text label(font);
        label.setString(text);
        label.setCharacterSize(10);
        label.setPosition(Vector2f(x, y));
        label.setFillColor(Color(120, 105, 160));
        window.draw(label);
    }

    void drawStatValue(float x, float y, const string &text, int size, Color color = Color(255, 215, 0))
    {
        Text value(font);
        value.setString(text);
        value.setCharacterSize(size);
        value.setPosition(Vector2f(x, y));
        value.setFillColor(color);
        value.setStyle(Text::Bold);
        window.draw(value);
    }

    string formatScore(int score)
    {
        stringstream ss;
        ss << setw(7) << setfill('0') << score;
        return ss.str();
    }

    void drawCornerDeco(float x, float y, float w, float h)
    {
        float cs = 10;
        Color cc(255, 215, 0, 180);

        RectangleShape tl1(Vector2f(cs, 2));
        tl1.setPosition(Vector2f(x, y));
        tl1.setFillColor(cc);
        window.draw(tl1);
        RectangleShape tl2(Vector2f(2, cs));
        tl2.setPosition(Vector2f(x, y));
        tl2.setFillColor(cc);
        window.draw(tl2);

        RectangleShape tr1(Vector2f(cs, 2));
        tr1.setPosition(Vector2f(x + w - cs, y));
        tr1.setFillColor(cc);
        window.draw(tr1);
        RectangleShape tr2(Vector2f(2, cs));
        tr2.setPosition(Vector2f(x + w - 2, y));
        tr2.setFillColor(cc);
        window.draw(tr2);

        RectangleShape bl1(Vector2f(cs, 2));
        bl1.setPosition(Vector2f(x, y + h - 2));
        bl1.setFillColor(cc);
        window.draw(bl1);
        RectangleShape bl2(Vector2f(2, cs));
        bl2.setPosition(Vector2f(x, y + h - cs));
        bl2.setFillColor(cc);
        window.draw(bl2);

        RectangleShape br1(Vector2f(cs, 2));
        br1.setPosition(Vector2f(x + w - cs, y + h - 2));
        br1.setFillColor(cc);
        window.draw(br1);
        RectangleShape br2(Vector2f(2, cs));
        br2.setPosition(Vector2f(x + w - 2, y + h - cs));
        br2.setFillColor(cc);
        window.draw(br2);
    }

    void drawPauseOverlay()
    {
        RectangleShape overlay(Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));
        overlay.setFillColor(Color(0, 0, 20, 190));
        window.draw(overlay);

        if (!fontLoaded)
            return;

        float panelW = 400;
        float panelH = 340;
        float panelX = (WINDOW_WIDTH - panelW) / 2;
        float panelY = (WINDOW_HEIGHT - panelH) / 2;

        RectangleShape panel(Vector2f(panelW, panelH));
        panel.setPosition(Vector2f(panelX, panelY));
        panel.setFillColor(Color(12, 8, 28, 250));
        panel.setOutlineThickness(3);
        panel.setOutlineColor(Color(100, 80, 140));
        window.draw(panel);

        drawCornerDeco(panelX, panelY, panelW, panelH);

        // Pause icon
        for (int i = 0; i < 2; i++)
        {
            RectangleShape pauseBar(Vector2f(15, 45));
            pauseBar.setPosition(Vector2f(panelX + panelW / 2 - 22 + i * 28, panelY + 18));
            pauseBar.setFillColor(Color(255, 215, 0));
            window.draw(pauseBar);
        }

        Text title(font);
        title.setString("GAME PAUSED");
        title.setCharacterSize(32);
        title.setPosition(Vector2f(panelX + panelW / 2 - 115, panelY + 70));
        title.setFillColor(Color(255, 255, 255));
        title.setStyle(Text::Bold);
        window.draw(title);

        string menuItems[] = {"New Game", "High Scores", "Help", "Exit to Menu", "Continue"};
        string menuKeys[] = {"1", "2", "3", "4", "5"};
        
        for (int i = 0; i < 5; i++)
        {
            bool selected = (i == pauseMenuSelection);
            float itemY = panelY + 125 + i * 40;
            
            if (selected)
            {
                RectangleShape highlight(Vector2f(panelW - 50, 35));
                highlight.setPosition(Vector2f(panelX + 25, itemY - 5));
                highlight.setFillColor(Color(50, 35, 75, 200));
                highlight.setOutlineThickness(2);
                highlight.setOutlineColor(Color(255, 215, 0));
                window.draw(highlight);

                float arrowPulse = sin(gameTime * 8) * 4;
                ConvexShape arrow;
                arrow.setPointCount(3);
                float ax = panelX + 35 + arrowPulse;
                float ay = itemY + 12;
                arrow.setPoint(0, Vector2f(ax, ay - 10));
                arrow.setPoint(1, Vector2f(ax + 14, ay));
                arrow.setPoint(2, Vector2f(ax, ay + 10));
                arrow.setFillColor(Color(255, 215, 0));
                window.draw(arrow);
            }

            Text keyText(font);
            keyText.setString("[" + menuKeys[i] + "]");
            keyText.setCharacterSize(14);
            keyText.setPosition(Vector2f(panelX + 60, itemY + 2));
            keyText.setFillColor(selected ? Color(255, 215, 0) : Color(120, 100, 150));
            window.draw(keyText);

            Text item(font);
            item.setString(menuItems[i]);
            item.setCharacterSize(18);
            item.setPosition(Vector2f(panelX + 110, itemY));
            item.setFillColor(selected ? Color(255, 255, 255) : Color(180, 180, 200));
            if (selected)
                item.setStyle(Text::Bold);
            window.draw(item);
        }

        Text hint(font);
        hint.setString("Press P or ESC to resume");
        hint.setCharacterSize(12);
        hint.setPosition(Vector2f(panelX + panelW / 2 - 85, panelY + panelH - 30));
        hint.setFillColor(Color(120, 120, 150));
        window.draw(hint);
    }

    void drawGameOver()
    {
        RectangleShape overlay(Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));
        overlay.setFillColor(Color(40, 0, 0, 210));
        window.draw(overlay);

        if (!fontLoaded)
            return;

        float panelW = 480;
        float panelH = enteringName ? 440 : 370;
        float panelX = (WINDOW_WIDTH - panelW) / 2;
        float panelY = (WINDOW_HEIGHT - panelH) / 2;

        RectangleShape panel(Vector2f(panelW, panelH));
        panel.setPosition(Vector2f(panelX, panelY));
        panel.setFillColor(Color(25, 10, 15, 250));
        panel.setOutlineThickness(4);
        panel.setOutlineColor(Color(160, 50, 50));
        window.draw(panel);

        drawCornerDeco(panelX, panelY, panelW, panelH);

        // X symbol
        CircleShape symbolBg(45);
        symbolBg.setPosition(Vector2f(panelX + panelW / 2 - 45, panelY + 20));
        symbolBg.setFillColor(Color(60, 20, 25));
        symbolBg.setOutlineThickness(3);
        symbolBg.setOutlineColor(Color(160, 50, 50));
        window.draw(symbolBg);

        for (int i = 0; i < 2; i++)
        {
            RectangleShape xMark(Vector2f(60, 8));
            xMark.setOrigin(Vector2f(30, 4));
            xMark.setPosition(Vector2f(panelX + panelW / 2, panelY + 65));
            xMark.setRotation(degrees(45 + i * 90));
            xMark.setFillColor(Color(200, 60, 60));
            window.draw(xMark);
        }

        Text title(font);
        title.setString("GAME OVER");
        title.setCharacterSize(40);
        title.setPosition(Vector2f(panelX + panelW / 2 - 120, panelY + 120));
        title.setFillColor(Color(255, 80, 80));
        title.setStyle(Text::Bold);
        window.draw(title);

        // Stats box
        RectangleShape statsBg(Vector2f(panelW - 60, 80));
        statsBg.setPosition(Vector2f(panelX + 30, panelY + 175));
        statsBg.setFillColor(Color(40, 20, 25, 200));
        statsBg.setOutlineThickness(1);
        statsBg.setOutlineColor(Color(100, 50, 60));
        window.draw(statsBg);

        Text scoreLabel(font);
        scoreLabel.setString("Final Score");
        scoreLabel.setCharacterSize(14);
        scoreLabel.setPosition(Vector2f(panelX + 45, panelY + 183));
        scoreLabel.setFillColor(Color(150, 130, 140));
        window.draw(scoreLabel);

        Text scoreVal(font);
        scoreVal.setString(formatScore(tetris.getScore()));
        scoreVal.setCharacterSize(32);
        scoreVal.setPosition(Vector2f(panelX + 45, panelY + 203));
        scoreVal.setFillColor(Color(255, 200, 150));
        scoreVal.setStyle(Text::Bold);
        window.draw(scoreVal);

        Text stats(font);
        stats.setString("Level " + to_string(tetris.getLevel()) + "  |  " + 
                       to_string(tetris.getLines()) + " lines");
        stats.setCharacterSize(16);
        stats.setPosition(Vector2f(panelX + 260, panelY + 215));
        stats.setFillColor(Color(180, 180, 200));
        window.draw(stats);

        if (enteringName)
        {
            // High score entry
            float hsY = panelY + 275;
            
            Text hsTitle(font);
            hsTitle.setString("NEW HIGH SCORE!");
            hsTitle.setCharacterSize(26);
            hsTitle.setPosition(Vector2f(panelX + panelW / 2 - 120, hsY));
            hsTitle.setFillColor(Color(255, 215, 0));
            hsTitle.setStyle(Text::Bold);
            window.draw(hsTitle);

            Text namePrompt(font);
            namePrompt.setString("Enter your name:");
            namePrompt.setCharacterSize(16);
            namePrompt.setPosition(Vector2f(panelX + panelW / 2 - 70, hsY + 40));
            namePrompt.setFillColor(Color(200, 200, 220));
            window.draw(namePrompt);

            RectangleShape nameBox(Vector2f(220, 40));
            nameBox.setPosition(Vector2f(panelX + panelW / 2 - 110, hsY + 70));
            nameBox.setFillColor(Color(50, 30, 40));
            nameBox.setOutlineThickness(2);
            nameBox.setOutlineColor(Color(255, 215, 0));
            window.draw(nameBox);

            string displayName = playerName;
            if (fmod(gameTime, 1.0f) < 0.5f)
                displayName += "_";

            Text nameText(font);
            nameText.setString(displayName);
            nameText.setCharacterSize(22);
            nameText.setPosition(Vector2f(panelX + panelW / 2 - 100, hsY + 77));
            nameText.setFillColor(Color::White);
            window.draw(nameText);

            Text enterHint(font);
            enterHint.setString("Press ENTER to confirm");
            enterHint.setCharacterSize(12);
            enterHint.setPosition(Vector2f(panelX + panelW / 2 - 80, hsY + 120));
            enterHint.setFillColor(Color(140, 140, 160));
            window.draw(enterHint);
        }
        else
        {
            Text hint(font);
            hint.setString("Press ENTER or ESC to return to menu");
            hint.setCharacterSize(15);
            hint.setPosition(Vector2f(panelX + panelW / 2 - 150, panelY + panelH - 45));
            hint.setFillColor(Color(150, 150, 170));
            window.draw(hint);
        }
    }

    void drawFloatingTexts()
    {
        if (!fontLoaded)
            return;

        for (auto &ft : floatingTexts)
        {
            float alpha = ft.getAlpha();

            // Shadow
            Text shadow(font);
            shadow.setString(ft.text);
            shadow.setCharacterSize(static_cast<unsigned int>(ft.size * ft.scale));
            shadow.setPosition(Vector2f(ft.position.x + 3, ft.position.y + 3));
            shadow.setFillColor(Color(0, 0, 0, static_cast<uint8_t>(alpha * 180)));
            shadow.setStyle(Text::Bold);
            window.draw(shadow);

            // Main text
            Text text(font);
            text.setString(ft.text);
            text.setCharacterSize(static_cast<unsigned int>(ft.size * ft.scale));
            text.setPosition(ft.position);
            Color col = ft.color;
            col.a = static_cast<uint8_t>(alpha * 255);
            text.setFillColor(col);
            text.setStyle(Text::Bold);
            text.setOutlineThickness(2);
            text.setOutlineColor(Color(0, 0, 0, static_cast<uint8_t>(alpha * 200)));
            window.draw(text);
        }
    }
};

// ==================== MAIN ====================
int main()
{
    Game game;
    game.run();
    return 0;
}