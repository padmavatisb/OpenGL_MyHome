
#include <GL/glut.h> 
#include <cmath>
#include <vector>
#include <ctime>
#include <cstdlib>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

float angleX = 20.0f, angleY = -45.0f;
float cameraZ = -20.0f;

bool rainEnabled = true;
float rainSpeed = 0.05f;
bool isDay = true;

float carX = 0.0f, carZ = 4.0f;
float gateAngle = 0.0f;
bool gateOpen = false;

struct Drop {
    float x, y, z;
};
std::vector<Drop> rain;

void updateLighting() {
    GLfloat light_pos[] = { 2.0f, 5.0f, 5.0f, 1.0f };
    GLfloat light_amb_day[] = { 0.4f, 0.4f, 0.4f, 1.0f };
    GLfloat light_diff_day[] = { 0.9f, 0.9f, 0.9f, 1.0f };

    GLfloat light_amb_night[] = { 0.1f, 0.1f, 0.2f, 1.0f };
    GLfloat light_diff_night[] = { 0.3f, 0.3f, 0.5f, 1.0f };

    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
    if (isDay) {
        glLightfv(GL_LIGHT0, GL_AMBIENT, light_amb_day);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diff_day);
        glClearColor(0.6f, 0.8f, 1.0f, 1.0f); // blue sky
    }
    else {
        glLightfv(GL_LIGHT0, GL_AMBIENT, light_amb_night);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diff_night);
        glClearColor(0.05f, 0.05f, 0.15f, 1.0f); // dark sky
    }
}

void drawCube(float x, float y, float z, float w, float h, float d, float r, float g, float b) {
    glPushMatrix();
    glTranslatef(x, y, z);
    glScalef(w, h, d);
    glColor3f(r, g, b);
    glutSolidCube(1.0);
    glPopMatrix();
}

void drawGround() {
    glColor3f(0.3f, 0.8f, 0.3f);
    glBegin(GL_QUADS);
    glVertex3f(-20.0f, 0.0f, -20.0f);
    glVertex3f(20.0f, 0.0f, -20.0f);
    glVertex3f(20.0f, 0.0f, 20.0f);
    glVertex3f(-20.0f, 0.0f, 20.0f);
    glEnd();
}

void drawRoad() {
    glColor3f(0.1f, 0.1f, 0.1f);
    glBegin(GL_QUADS);
    glVertex3f(-20.0f, 0.01f, 3.0f);
    glVertex3f(20.0f, 0.01f, 3.0f);
    glVertex3f(20.0f, 0.01f, 5.0f);
    glVertex3f(-20.0f, 0.01f, 5.0f);
    glEnd();

    glColor3f(1.0f, 1.0f, 0.0f);
    for (float i = -20.0f; i < 20.0f; i += 2.0f) {
        glBegin(GL_QUADS);
        glVertex3f(i, 0.02f, 3.95f);
        glVertex3f(i + 1.0f, 0.02f, 3.95f);
        glVertex3f(i + 1.0f, 0.02f, 4.05f);
        glVertex3f(i, 0.02f, 4.05f);
        glEnd();
    }
}

void drawHouse() {
    drawCube(0, 0.5f, 0, 2.0f, 1.0f, 2.0f, 0.95f, 0.9f, 0.8f);

    glPushMatrix();
    glTranslatef(0.0f, 1.01f, 0.0f);
    glRotatef(-90, 1, 0, 0);
    glColor3f(0.8f, 0.2f, 0.2f);
    glutSolidCone(2.05f, 1.0f, 20, 20);
    glPopMatrix();

    // door
    drawCube(0.0f, 0.25f, 1.01f, 0.4f, 0.5f, 0.05f, 0.4f, 0.2f, 0.0f);
    // windows
    drawCube(-0.7f, 0.6f, 1.01f, 0.3f, 0.3f, 0.05f, 0.2f, 0.2f, 0.8f);
    drawCube(0.7f, 0.6f, 1.01f, 0.3f, 0.3f, 0.05f, 0.2f, 0.2f, 0.8f);
}

void drawCompoundWall() {
    float wallH = 0.3f, wallT = 0.1f;
    drawCube(-1.75f, wallH / 2, 2.5f, 2.5f, wallH, wallT, 1, 0.95, 0.85);
    drawCube(1.75f, wallH / 2, 2.5f, 2.5f, wallH, wallT, 1, 0.95, 0.85);
    drawCube(0.0f, wallH / 2, -2.5f, 5.0f, wallH, wallT, 1, 0.95, 0.85);
    drawCube(-2.5f, wallH / 2, 0.0f, wallT, wallH, 5.0f, 1, 0.95, 0.85);
    drawCube(2.5f, wallH / 2, 0.0f, wallT, wallH, 5.0f, 1, 0.95, 0.85);
}

void drawGate() {
    glPushMatrix();
    glTranslatef(-0.6f, 0.15f, 2.55f);
    glRotatef(gateAngle, 0, 1, 0);
    glTranslatef(0.6f, 0.0f, 0.0f);

    glColor3f(1.0f, 0.8f, 0.0f);
    for (float i = -0.5f; i <= 0.5f; i += 0.1f) {
        glPushMatrix();
        glTranslatef(i, 0.0f, 0.0f);
        glScalef(0.05f, 0.5f, 0.05f);
        glutSolidCube(1.0);
        glPopMatrix();
    }
    glPopMatrix();
}

void drawCar() {
    glPushMatrix();
    glTranslatef(carX, 0.1f, carZ);
    glColor3f(0.8f, 0.0f, 0.0f);
    glPushMatrix(); glScalef(1.5f, 0.3f, 0.6f); glutSolidCube(1.0); glPopMatrix();
    glPushMatrix(); glTranslatef(0.0f, 0.2f, 0.0f); glScalef(1.0f, 0.3f, 0.6f); glutSolidCube(1.0); glPopMatrix();
    glColor3f(0.1f, 0.1f, 0.1f);
    for (float xoff = -0.6f; xoff <= 0.6f; xoff += 1.2f)
        for (float zoff = -0.3f; zoff <= 0.3f; zoff += 0.6f) {
            glPushMatrix(); glTranslatef(xoff, -0.05f, zoff);
            glutSolidTorus(0.02, 0.07, 8, 8); glPopMatrix();
        }
    glPopMatrix();

}
/*
void drawStreetLight(float x, float z, int lightID) {
    // Pole
    drawCube(x, 1.0f, z, 0.05f, 2.0f, 0.05f, 0.3f, 0.3f, 0.3f);

    // Bulb
    glPushMatrix();
    glTranslatef(x, 2.05f, z);
    if (isDay)
        glColor3f(0.9f, 0.9f, 0.6f); // looks off
    else
        glColor3f(1.0f, 0.6f, 0.1f); // warm orange glow
    glutSolidSphere(0.1, 10, 10);
    glPopMatrix();
    *
    // Spotlight effect at night
    if (!isDay) {
        GLfloat light_pos[] = { x, 2.05f, z, 1.0f };
        GLfloat light_diff[] = { 1.0f, 0.6f, 0.1f, 1.0f }; // orange
        GLfloat light_dir[] = { 0.0f, -1.0f, 0.0f };

        glEnable(GL_LIGHT0 + lightID);
        glLightfv(GL_LIGHT0 + lightID, GL_POSITION, light_pos);
        glLightfv(GL_LIGHT0 + lightID, GL_DIFFUSE, light_diff);
        glLightf(GL_LIGHT0 + lightID, GL_SPOT_CUTOFF, 45.0f);
        glLightfv(GL_LIGHT0 + lightID, GL_SPOT_DIRECTION, light_dir);
        glLightf(GL_LIGHT0 + lightID, GL_SPOT_EXPONENT, 10.0f);
        glLightf(GL_LIGHT0 + lightID, GL_CONSTANT_ATTENUATION, 0.5f);
        glLightf(GL_LIGHT0 + lightID, GL_LINEAR_ATTENUATION, 0.1f);
    }
    else {
        glDisable(GL_LIGHT0 + lightID);
    }
}

*/
void drawCloud(float x, float y, float z) {
    glPushMatrix();
    glTranslatef(x, y, z);
    glColor3f(1.0f, 1.0f, 1.0f);
    glutSolidSphere(0.3, 16, 16);
    glPushMatrix(); glTranslatef(0.3, 0.1, 0); glutSolidSphere(0.25, 16, 16); glPopMatrix();
    glPushMatrix(); glTranslatef(-0.3, 0.1, 0); glutSolidSphere(0.25, 16, 16); glPopMatrix();
    glPushMatrix(); glTranslatef(0.15, 0.2, 0.1); glutSolidSphere(0.2, 16, 16); glPopMatrix();
    glPushMatrix(); glTranslatef(-0.15, 0.2, -0.1); glutSolidSphere(0.2, 16, 16); glPopMatrix();
    glPopMatrix();
}

void drawTree(float x, float z, float trunkH = 0.5f, float crownR = 0.4f) {
    drawCube(x, trunkH / 2, z, 0.1f, trunkH, 0.1f, 0.6f, 0.4f, 0.2f);
    glPushMatrix();
    glTranslatef(x, trunkH + crownR, z);
    glColor3f(0.2f, 0.6f, 0.2f);
    glutSolidSphere(crownR, 16, 16);
    glPopMatrix();
}


void drawRain() {
    if (!rainEnabled) return;
    glColor3f(0.5f, 0.5f, 1.0f);
    glBegin(GL_LINES);
    for (auto& d : rain) {
        glVertex3f(d.x, d.y, d.z);
        glVertex3f(d.x, d.y - 0.1f, d.z);
    }
    glEnd();
}

void updateRain(int) {
    if (rainEnabled)
        for (auto& d : rain) {
            d.y -= rainSpeed;
            if (d.y < 0) { d.y = 4.0f; d.x = (rand() % 200 - 100) / 10.0f; d.z = (rand() % 200 - 100) / 10.0f; }
        }
    glutPostRedisplay();
    glutTimerFunc(30, updateRain, 0);
}

void initRain() {
    for (int i = 0; i < 300; ++i) {
        Drop d; d.x = (rand() % 100 - 50) / 10.0f; d.y = (rand() % 100) / 10.0f + 1.0f; d.z = (rand() % 100 - 50) / 10.0f;
        rain.push_back(d);
    }
}

void display() {
    updateLighting();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glTranslatef(0.0f, -1.0f, cameraZ);
    glRotatef(angleX, 1, 0, 0);
    glRotatef(angleY, 0, 1, 0);

    // Draw ground
    drawGround();

    // Draw road
    drawRoad();


    // Draw compound wall and gate
    drawCompoundWall();
    drawGate();

    // Draw house
    drawHouse();

    // Trees
    drawTree(-3.0f, -2.0f);
    drawTree(2.5f, -3.0f, 0.4f, 0.3f);
    drawTree(-4.0f, 3.0f, 0.6f, 0.5f);
    drawTree(3.8f, 3.0f, 0.5f, 0.4f);

    // Clouds
    drawCloud(1.5f, 2.5f, -2.0f);
    drawCloud(-1.5f, 2.7f, 1.5f);
    drawCloud(0.0f, 2.8f, 0.0f);

    // Car
    drawCar();

    // Rain
    drawRain();

    glutSwapBuffers();
}


void reshape(int w, int h) {
    if (h == 0) h = 1;
    float asp = (float)w / h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, asp, 1.0f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
}

void keyboard(unsigned char key, int, int) {
    if (key == 'z') cameraZ += 0.5f;
    if (key == 'x') cameraZ -= 0.5f;
    if (key == 'w') carZ -= 0.3f;
    if (key == 's') carZ += 0.3f;
    if (key == 'a') carX -= 0.3f;
    if (key == 'd') carX += 0.3f;

    if (key == 'g') { gateOpen = !gateOpen; gateAngle = gateOpen ? 45.0f : 0.0f; }
    if (key == 'n') { isDay = !isDay; }
    if (key == 'r') rainEnabled = !rainEnabled;
    if (key == 'f') rainSpeed = std::min(0.2f, rainSpeed + 0.01f);
    if (key == 'v') rainSpeed = std::max(0.005f, rainSpeed - 0.01f);
    glutPostRedisplay();
}

void specialKeys(int key, int, int) {
    if (key == GLUT_KEY_UP) angleX -= 5;
    if (key == GLUT_KEY_DOWN) angleX += 5;
    if (key == GLUT_KEY_LEFT) angleY -= 5;
    if (key == GLUT_KEY_RIGHT) angleY += 5;
    glutPostRedisplay();
}

void init() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    srand((unsigned)time(0));
    initRain();
    glutTimerFunc(0, updateRain, 0);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1000, 600);
    glutCreateWindow("House, Car, Road, Gate, Rain, Day/Night Toggle");
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);
    init();
    glutMainLoop();
    return 0;
}
