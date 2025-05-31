#include <vector>
#include <string>
#include <GL/glut.h>
#include <algorithm>
#include <time.h>
#include <cmath>

double playerX = 0.0, playerY = 0.0;
double velocity = 0.0;
double cameraY = 0.0;
const double gravity = -0.001;
const double jumpForce = 0.05;
const double maxJumpHeight = 0.3;
const double deathThreshold = 2.0;
const double MaxFallSpeed = -0.15;
bool gameOver = false;

struct Platform
{
    int type = 1;
    double x, y;
    double width = 0.3;
    double height = 0.05;
    double speed = 0.0;
    double moveRange = 0.0;
    double startX;
};

struct Enemy
{
    double x, y;
    double width = 0.2;
    double height = 0.3;
    bool alive = true;
};

std::vector<Platform> platforms;
std::vector<Enemy> enemies;

void generatePlatforms()
{
    platforms.push_back({1, 0.0, -0.5});
    for(int i = 0; i < 10; ++i)
    {
        Platform p;
        p.type = (rand() % 5 == 0) ? 2 : 1;
        p.x = (double)(rand() % 180 - 90) / 100.0;
        p.y = (double)i * 0.25;

        if(p.type == 2)
        {
            p.speed = (rand() % 50 + 20) / 7500.0;
            p.moveRange = (rand() % 50 + 30) / 100.0;
            p.startX = p.x;
        }

        if(i > 0) p.y += (rand() % 50) / 250.0;
        platforms.push_back(p);
    }
}

void drawPlayer()
{
    glColor3d(1.0, 1.0, 1.0);
    glBegin(GL_QUADS);
        glVertex2d(playerX - 0.1, playerY - 0.1);
        glVertex2d(playerX + 0.1, playerY - 0.1);
        glVertex2d(playerX + 0.1, playerY + 0.1);
        glVertex2d(playerX - 0.1, playerY + 0.1);
    glEnd();
}

void drawPlatforms()
{
    for(const auto& p : platforms)
    {
        if(p.y >= cameraY - 2.0 && p.y <= cameraY + 2.0)
        {
            if(p.type == 2)
            {
                glColor3d(0.0, 0.5, 1.0);
            }
            else
            {
                glColor3d(0.0, 1.0, 0.0);
            }

            glBegin(GL_QUADS);
                glVertex2d(p.x - p.width / 2, p.y - p.height / 2);
                glVertex2d(p.x + p.width / 2, p.y - p.height / 2);
                glVertex2d(p.x + p.width / 2, p.y + p.height / 2);
                glVertex2d(p.x - p.width / 2, p.y + p.height / 2);
            glEnd();
        }
    }
}

void drawEnemies()
{
    glColor3d(1.0, 0.0, 0.0);
    for(const auto& e : enemies) 
    {
        if(e.alive && e.y >= cameraY - 2.0 && e.y <= cameraY + 2.0) 
        {
            glBegin(GL_QUADS);
                glVertex2d(e.x - e.width/2, e.y - e.height/2);
                glVertex2d(e.x + e.width/2, e.y - e.height/2);
                glVertex2d(e.x + e.width/2, e.y + e.height/2);
                glVertex2d(e.x - e.width/2, e.y + e.height/2);
            glEnd();
        }
    }
}

bool checkCollision(double px, double py, const Platform& p)
{
    return 
        (px + 0.1 > p.x - p.width / 2) &&
        (px - 0.1 < p.x + p.width / 2) &&
        (py - 0.1 <= p.y + p.height / 2) &&
        (py - 0.1 >= p.y - p.height / 2) && 
        (velocity <= 0);
            
}

bool checkEnemyCollision(double px, double py, const Enemy& e) 
{
    return 
        (px + 0.1 > e.x - e.width/2) &&
        (px - 0.1 < e.x + e.width/2) &&
        (py - 0.1 <= e.y + e.height/2) &&
        (py - 0.1 >= e.y - e.height/2) &&
        (velocity <= 0);
}

void generateNewPlatformsIfNeeded()
{
    platforms.erase(std::remove_if(platforms.begin(), platforms.end(), [](const Platform& p) {return p.y < cameraY - 2.0;}), platforms.end());
    enemies.erase(std::remove_if(enemies.begin(), enemies.end(), [](const Enemy& e) { return e.y < cameraY - 2.0; }), enemies.end());
    while(platforms.empty() || platforms.back().y < cameraY + 2.5)
    {
        if(rand() % 100 < 5)
        {
            Enemy e;
            e.x = (double)(rand() % 160 - 80) / 100.0;
            e.y = platforms.empty() ? cameraY + 0.7 : platforms.back().y + 0.25 + (rand() % 40) / 100.0;
            enemies.push_back(e);
        }
        else
        {
            Platform p;
            p.type = (rand() % 5 == 0) ? 2 : 1;
            p.x = (double)(rand() % 180 - 90) / 100.0;
            p.y = platforms.empty() ? cameraY + 0.5 : platforms.back().y + 0.25 + (rand() % 50) / 300.0;
            if(p.type == 2)
            {
                p.speed = (rand() % 50 + 20) / 7500.0;
                p.moveRange = (rand() % 50 + 30) / 100.0;
                p.startX = p.x;
            }
            platforms.push_back(p);
        }
    }
}

void drawGameOver()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1.0, 1.0, -1.5, 1.5);
    glMatrixMode(GL_MODELVIEW);
    glColor3d(1.0, 0.0, 0.0);

    glBegin(GL_QUADS);
        glVertex2d(-0.5, -0.2);
        glVertex2d(0.5, -0.2);
        glVertex2d(0.5, 0.2);
        glVertex2d(-0.5, 0.2);
    glEnd();

    glColor3d(1.0, 1.0, 1.0);
    glRasterPos2d(-0.4, -0.05);
    std::string text = "Game over! Press R to restart";
    for(char c : text) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
}

void init()
{
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1, 1, -1.5, 1.5);
    glMatrixMode(GL_MODELVIEW);
    cameraY = playerY - 0.75;
    generatePlatforms();
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1.0, 1.0, cameraY - 1.5, cameraY + 1.5);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    if(gameOver) drawGameOver();
    else
    {
        glColor3d(1.0, 1.0, 1.0);
        drawPlayer();
        drawPlatforms();
        drawEnemies();
    }
    glutSwapBuffers();
}

void update(int value)
{
    if(gameOver)
    {
        glutPostRedisplay();
        return;
    } 

    for(auto& p : platforms)
    {
        if(p.type == 2)
        {
            p.x += p.speed;
            if(fabs(p.x - p.startX) > p.moveRange) 
            {
                p.speed *= -1;
            }
        }
    }

    for(auto& e : enemies) {
        if(!e.alive) continue;

        bool topCollision = checkEnemyCollision(playerX, playerY, e);
    
        bool generalCollision = (playerX + 0.05 > e.x - e.width/2 && playerX - 0.05 < e.x + e.width/2 && playerY + 0.05 > e.y - e.height/2 && playerY - 0.05 < e.y + e.height/2);

        if(topCollision) 
        {
            e.alive = false;
            velocity = jumpForce;
        }
        else if(generalCollision) gameOver = true;
    }

    velocity += gravity;
    if(velocity < MaxFallSpeed) velocity = MaxFallSpeed;
    playerY += velocity;

    bool onPlatform = false;
    for(const auto& p : platforms)
    {
        if(checkCollision(playerX, playerY, p))
        {
            velocity = jumpForce;
            onPlatform = true;
            break;
        }
    }

    if(!onPlatform && velocity < 0) velocity += gravity * 0.3;

    if(playerY < cameraY - deathThreshold) gameOver = true;
    
    if(playerY > cameraY + 0.5) cameraY = playerY - 0.5;

    generateNewPlatformsIfNeeded();

    if(playerX > 1.1) playerX = -1.1;
    if(playerX < -1.1) playerX = 1.1;

    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

void restartGame()
{
    gameOver = false;
    playerX = 0.0;
    playerY = 0.0;
    velocity = 0.0;
    cameraY = -0.5;
    platforms.clear();
    enemies.clear();
    generatePlatforms();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1.0, 1.0, -1.5, 1.5);
    glMatrixMode(GL_MODELVIEW);
    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

void keyboard(unsigned char key, int x, int y)
{
    if(gameOver && (key == 'r' || key == 'R'))
    {
        restartGame();
    }
    if(key == 27)
    {
        exit(0);
    }
    if(key == 'a' || key == 'A')
    {
        playerX -= 0.15;
    }
    if(key == 'd' || key == 'D')
    {
        playerX += 0.15;
    }
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 800);
    glutCreateWindow("Doodle Jump");
    srand(time(0));

    init();

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(0, update, 0);

    glutMainLoop();
    return 0;
}

