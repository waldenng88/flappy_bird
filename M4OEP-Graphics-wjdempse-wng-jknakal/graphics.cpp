#include "graphics.h"
#include "circle.h"
#include "rect.h"
#include <iostream>
#include <memory>
#include <vector>
#include <ctime>
#include <chrono>

using namespace std;
using namespace chrono;

GLdouble width, height;
int wd;
const color skyBlue(113/255.0, 197/255.0, 207/255.0);
const color white(1, 1, 1);
const color brickRed(201/255.0, 20/255.0, 20/255.0);
const color darkBlue(1/255.0, 110/255.0, 214/255.0);
const color pipeGreen(115,191,45);
const color purple(119/255.0, 11/255.0, 224/255.0);
const color black(0, 0, 0);
const color magenta(1, 0, 1);
const color orange(1, 163/255.0, 22/255.0);
const color cyan (0, 1, 1);
const color opaque (0,0,0,0);
const color darkGray(0.5,0.5,0.5);

Rect bottom;
vector<Rect> bottomBuildings;
vector<Rect> topBuildings;
vector<Rect> pointBuildings;
Rect user;
int jumps;
int points = 0;
int buildingsPassed = 0;
bool overlapped = false;
bool reset = false;
bool alive = true;

void initBottom() {
    bottom.setCenter(375, 800);
    bottom.setSize(width + 100, 100);
    bottom.setColor(darkGray);
}

void initBuildings() {
    // building variables
    int totalBuildingWidth = 0;
    dimensions buildingSize;

    // topBuildings
    totalBuildingWidth = 0;
    while (totalBuildingWidth < width + 200) {
        // Building height
        buildingSize.height = rand() % 150 + 190;
        // Building width
        buildingSize.width = 150;
        topBuildings.push_back(Rect(purple,
                totalBuildingWidth+(buildingSize.width/2)+5,
                height-((buildingSize.height/2)+50),
                buildingSize));
        totalBuildingWidth += buildingSize.width + 5; // Placeholder line to avoid infinite loop
    }

    // bottomBuildings
    totalBuildingWidth = 0;
    while (totalBuildingWidth < width + 100) {
        // Building height
        buildingSize.height = rand() % 150 + 190;
        // Building width
        buildingSize.width = 150;
        bottomBuildings.push_back(Rect(darkBlue,
                                       totalBuildingWidth+(buildingSize.width/2)+5,
                                       height-((buildingSize.height/2)+50),
                                       buildingSize));
        totalBuildingWidth += buildingSize.width + 5; // Placeholder line to avoid infinite loop
    }

    // pointBuildings
    totalBuildingWidth = 0;
    while (totalBuildingWidth < width + 100) {
        // Building height
        buildingSize.height = 1000;
        // Building width
        buildingSize.width = 150;
        pointBuildings.push_back(Rect(skyBlue,
                                       totalBuildingWidth+(buildingSize.width/2)+5,
                                       height-((buildingSize.height/2)+50),
                                       buildingSize));
        totalBuildingWidth += buildingSize.width + 5; // Placeholder line to avoid infinite loop
    }
}

void initUser() {
    user.setCenter(375,375);
    user.setSize(30, 30);
    user.setColor(white);
}

void init() {
    width = 800;
    height = 800;
    srand(time(0));
    initBottom();
    initBuildings();
    initUser();
}

/* Initialize OpenGL Graphics */
void initGL() {
    // Set "clearing" or background color
    glClearColor(skyBlue.red, skyBlue.green, skyBlue.blue, 1.0f);
}

void display() {
    // Tell OpenGL to use the whole window for drawing
    glViewport(0, 0, width, height); // DO NOT CHANGE THIS LINE (unless you are running Catalina on Mac)
    
    // Do an orthographic parallel projection with the coordinate
    // system set to first quadrant, limited by screen/window size
    glMatrixMode(GL_PROJECTION); // DO NOT CHANGE THIS LINE
    glLoadIdentity(); // DO NOT CHANGE THIS LINE
    glOrtho(0.0, width, height, 0.0, -1.f, 1.f); // DO NOT CHANGE THIS LINE

    // Clear the color buffer with current clearing color
    glClear(GL_COLOR_BUFFER_BIT); // DO NOT CHANGE THIS LINE
    
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // DO NOT CHANGE THIS LINE

    //Drawing the "buildings" used as flags to track the points, as well as keeping track of points
    /* Note, the values in place here by using buildingsPassed, checking if divisible by 6, and subtracting
     * points is to get around the limitation of the vectors being size 6
     */
    for (Rect &r : pointBuildings) {
        if (r.isOverlapping(user)) {
            if (r.getTouched() == false) {
                // Resets all buildings status so the count can continue
                if ((buildingsPassed % 6) == 0) {
                    for (Rect &r: pointBuildings) {
                        r.setTouched(false);
                    }
                    buildingsPassed++;
                    // To account for when the player overlaps two pointBuildings
                    points--;
                }
                else {
                    r.setTouched(true);
                    points++;
                    buildingsPassed++;
                }
            }
        }
        r.draw();
    }
    user.draw();
    bottom.draw();

    //draw purple buildings first because they are in the back
    for (Rect &r : topBuildings) {
        if (r.isOverlapping(user)) {
            r.setColor(brickRed);
            alive = false;
        } else {
            r.setColor(purple);
        }
        r.draw();
    }

    //draw blue buildings before because they are in the background
    for (Rect &r : bottomBuildings) {
        if (r.isOverlapped(user)) {
            r.setColor(brickRed);
            alive = false;
        } else {
            r.setColor(darkBlue);
        }
        r.draw();
    }

    //prints this message for the user if they haven't started yet
    string welcome = "Welcome to Walmart Flappy Bird. Press the jump key (Spacebar) to begin.";
    string welcome2 =  "Pressing esc at anytime will kill the program.";
    if (jumps == 0) {
        glColor3f(1, 1, 1);
        glRasterPos2i(50, height-550);
        for (char letter : welcome) {
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, letter);
        }
        glRasterPos2i(50, height-500);
        for (char letter : welcome2) {
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, letter);
        }
        //Keep points at 0 until user starts jumping
        points = 0;
    }

    //Score counter to be constantly displayed bottom left
    string displayScore = "Score: " + to_string(points);
    glColor3f(1,1,1);
    glRasterPos2i(80, height - 13);
    for (char letter: displayScore) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, letter);
    }

    //exit message for when the bird dies
    string score = to_string(points);
    string dead = "Thank you for playing, your score is " + score;
    string finish = "Please hit the esc key to close the window";
    if (!alive) {
        glColor3f(1,1,1);
        glRasterPos2i(80, height-500);
        for (char letter : dead) {
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, letter);
        }
        glRasterPos2i(80, height-450);
        for (char letter : finish) {
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, letter);
        }
    }
    
    glFlush();  // Render now
}

// http://www.theasciicode.com.ar/ascii-control-characters/escape-ascii-code-27.html
void kbd(unsigned char key, int x, int y) {

    //if space is pressed, jump
    if (key == 32) {
        //only jump if user is alive
        if (alive) {
            jumps++;
            user.moveY(-40);
        }
    }

    // escape
    if (key == 27) {
        glutDestroyWindow(wd);
        exit(0);
    }

    glutPostRedisplay();
}

void buildingTimer(int dummy) {
    // Make the vectors of buildings move

    //move topBuildings
    for (int i = 0; i < topBuildings.size(); ++i) {
        // Move all the buildings to the left
        topBuildings[i].setCenterY((topBuildings[i].getHeight()/2));
        topBuildings[i].moveX(-5);
        // If a shape has moved off the screen
        if (topBuildings[i].getCenterX() < -(topBuildings[i].getWidth()/2)) {
            // Set it to the right of the screen so that it passes through again
            int buildingOnLeft = (i == 0) ? topBuildings.size()-1 : i - 1;
            topBuildings[i].setCenterX(topBuildings[buildingOnLeft].getCenterX() + topBuildings[buildingOnLeft].getWidth()/2 + topBuildings[i].getWidth()/2 + 5);
        }
    }

    //move buildings on bottom
    for (int i = 0; i < bottomBuildings.size(); ++i) {
        // Move all the buildings to the left
        bottomBuildings[i].moveX(-5);
        // If a shape has moved off the screen
        if (bottomBuildings[i].getCenterX() < -(bottomBuildings[i].getWidth()/2)) {
            // Set it to the right of the screen so that it passes through again
            int buildingOnLeft = (i == 0) ? bottomBuildings.size()-1 : i - 1;
            bottomBuildings[i].setCenterX(bottomBuildings[buildingOnLeft].getCenterX() + bottomBuildings[buildingOnLeft].getWidth()/2 + bottomBuildings[i].getWidth()/2 + 5);
        }
    }

    //move point building
    for (int i = 0; i < pointBuildings.size(); ++i) {
        // Move all the buildings to the left
        pointBuildings[i].moveX(-5);
        // If a shape has moved off the screen
        if (pointBuildings[i].getCenterX() < -(pointBuildings[i].getWidth()/2)) {
            // Set it to the right of the screen so that it passes through again
            int buildingOnLeft = (i == 0) ? pointBuildings.size()-1 : i - 1;
            pointBuildings[i].setCenterX(pointBuildings[buildingOnLeft].getCenterX() + pointBuildings[buildingOnLeft].getWidth()/2 + pointBuildings[i].getWidth()/2 + 5);
        }
    }

    //this makes it so the user does not start dropping until space is clicked once
    if (jumps > 0) {
        user.moveY(2);
    }

    if (alive) {
        glutPostRedisplay();
        glutTimerFunc(30, buildingTimer, dummy);
    }
}

/* Main function: GLUT runs as a console application starting at main()  */
int main(int argc, char** argv) {
    
    init();
    
    glutInit(&argc, argv);          // Initialize GLUT
    
    glutInitDisplayMode(GLUT_RGBA);
    
    glutInitWindowSize((int)width, (int)height);
    glutInitWindowPosition(100, 200); // Position the window's initial top-left corner
    /* create the window and store the handle to it */
    wd = glutCreateWindow("Walmart Flappy Bird" /* title */ );

    // Register callback handler for window re-paint event
    glutDisplayFunc(display);
    
    // Our own OpenGL initialization
    initGL();

    // register keyboard press event processing function
    // works for numbers, letters, spacebar, etc.
    glutKeyboardFunc(kbd);

    glutTimerFunc(0, buildingTimer, 0);


    // Enter the event-processing loop
    glutMainLoop();

    return 0;
}
