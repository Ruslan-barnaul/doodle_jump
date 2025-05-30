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
    double x, y;
    double width = 0.3;
    double height = 0.05;
};

std::vector<Platform> platforms;

void generatePlatforms()
{
    platforms.push_back({0.0, -0.5});
    for(int i = 0; i < 10; ++i)
    {
        Platform p;
        p.x = (double)(rand() % 180 - 90) / 100.0;
        p.y = (double)i * 0.25;

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
            glColor3d(0.0, 1.0, 0.0);

            glBegin(GL_QUADS);
                glVertex2d(p.x - p.width / 2, p.y - p.height / 2);
                glVertex2d(p.x + p.width / 2, p.y - p.height / 2);
                glVertex2d(p.x + p.width / 2, p.y + p.height / 2);
                glVertex2d(p.x - p.width / 2, p.y + p.height / 2);
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

void generateNewPlatformsIfNeeded()
{
    platforms.erase(std::remove_if(platforms.begin(), platforms.end(), [](const Platform& p) {return p.y < cameraY - 2.0;}), platforms.end());
    while(platforms.empty() || platforms.back().y < cameraY + 2.5)
    {
        Platform p;
        p.x = (double)(rand() % 180 - 90) / 100.0;
        p.y = platforms.empty() ? cameraY + 0.5 : platforms.back().y + 0.25 + (rand() % 50) / 300.0;
        
        platforms.push_back(p);
    }
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
    if(gameOver) exit(0);
    else
    {
        glColor3d(1.0, 1.0, 1.0);
        drawPlayer();
        drawPlatforms();
    }
    glutSwapBuffers();
}

void update(int value)
{
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

void keyboard(unsigned char key, int x, int y)
{
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
