#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <stdio.h>

#include "definitions.h"
#include "display.h"
#include "io.h"

/** GLOBAL VARIABLES **/
GLdouble _window_ratio;  // Control of window's proportions
/*
GLdouble left,right;                        //Variables for the control of the
orthographic projection GLdouble bottom,top;                        //Variables
for the control of the orthographic projection GLdouble near,far; //Variables
for the control of the orthographic projection
*/
object3d *_first_object = 0;    /*List of objects*/
object3d *_selected_object = 0; /*Object currently selected*/
camera *_first_camera = 0;      /*List of cameras*/
camera *_selected_camera = 0;   /*Camera currently selected*/
camera *_last_camera =
    0; /*Last of the list of cameras (just used for charging new cameras)*/
int flat_smooth = 1;       /* Indica si se aplica suavizado o no */
light luces[GL_MAX_LIGHT]; /* Lista de luces */
int iluminacion = 1;       /* Inidica si la iluminacion esta activa o no */
int _selected_light;       /* Number of light currently selected */

// INICIALIZA LAS LUCES DEL MUNDO
void init_luces() {
    // SOL
    luces[0].position[0] = 1;
    luces[0].position[1] = 1;
    luces[0].position[2] = 0;
    luces[0].position[3] = 0;
    luces[0].dir[0] = 0.0;
    luces[0].dir[1] = 0.0;
    luces[0].dir[2] = 0.0;
    luces[0].ambient[0] = 0;
    luces[0].ambient[1] = 0;
    luces[0].ambient[2] = 0;
    luces[0].ambient[3] = 1;
    luces[0].diffuse[0] = 1;
    luces[0].diffuse[1] = 1;
    luces[0].diffuse[2] = 1;
    luces[0].diffuse[3] = 1;
    luces[0].specular[0] = 1;
    luces[0].specular[1] = 1;
    luces[0].specular[2] = 1;
    luces[0].specular[3] = 1;
    luces[0].encendida = 1;  // 0 es que esta apagada
    luces[0].m_luz = malloc(sizeof(matrices));
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(luces[0].position[0], luces[0].position[1],
                 luces[0].position[2]);
    glGetFloatv(GL_MODELVIEW_MATRIX, luces[0].m_luz->matriz);
    luces[0].m_luz->sigptr = NULL;
    _selected_light = 0;

    // BOMBILLA
    luces[1].position[0] = 0.0;
    luces[1].position[1] = 4.0;
    luces[1].position[2] = 0.0;
    luces[1].position[3] = 1;
    luces[1].dir[0] = 0.0;
    luces[1].dir[1] = 0.0;
    luces[1].dir[2] = 0.0;
    luces[1].ambient[0] = 0;
    luces[1].ambient[1] = 0;
    luces[1].ambient[2] = 0;
    luces[1].ambient[3] = 1;
    luces[1].diffuse[0] = 1;
    luces[1].diffuse[1] = 1;
    luces[1].diffuse[2] = 1;
    luces[1].diffuse[3] = 1;
    luces[1].specular[0] = 1;
    luces[1].specular[1] = 1;
    luces[1].specular[2] = 1;
    luces[1].specular[3] = 1;
    luces[1].encendida = 0;  // 0 es que esta apagada
    luces[1].m_luz = malloc(sizeof(matrices));
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(luces[1].position[0], luces[1].position[1],
                 luces[1].position[2]);
    glGetFloatv(GL_MODELVIEW_MATRIX, luces[1].m_luz->matriz);
    luces[1].m_luz->sigptr = NULL;

    // Foco_obj
    luces[2].position[0] = 0.0;
    luces[2].position[1] = 0.0;
    luces[2].position[2] = 0.0;
    luces[2].position[3] = 1;
    luces[2].dir[0] = 0.0;
    luces[2].dir[1] = 0.0;
    luces[2].dir[2] = 1.0;
    luces[2].ambient[0] = 0.0;
    luces[2].ambient[1] = 0.0;
    luces[2].ambient[2] = 0.0;
    luces[2].ambient[3] = 1;
    luces[2].diffuse[0] = 0.5;
    luces[2].diffuse[1] = 0.5;
    luces[2].diffuse[2] = 0.5;
    luces[2].diffuse[3] = 1;
    luces[2].specular[0] = 0.5;
    luces[2].specular[1] = 0.5;
    luces[2].specular[2] = 0.5;
    luces[2].specular[3] = 1;
    luces[2].encendida = 0;  // 0 es que esta apagada
    luces[2].amplitud = 45.0;
    luces[2].m_luz = malloc(sizeof(matrices));
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(luces[2].position[0], luces[2].position[1],
                 luces[2].position[2]);
    glGetFloatv(GL_MODELVIEW_MATRIX, luces[2].m_luz->matriz);
    luces[2].m_luz->sigptr = NULL;

    // Foco_cam
    luces[3].position[0] = 0.0;
    luces[3].position[1] = 0.0;
    luces[3].position[2] = 0.0;
    luces[3].position[3] = 1;
    luces[3].dir[0] = 0.0;
    luces[3].dir[1] = 0.0;
    luces[3].dir[2] = -1.0;
    luces[3].ambient[0] = 0.5;
    luces[3].ambient[1] = 0.5;
    luces[3].ambient[2] = 0.5;
    luces[3].ambient[3] = 1;
    luces[3].diffuse[0] = 0.75;
    luces[3].diffuse[1] = 0.75;
    luces[3].diffuse[2] = 0.75;
    luces[3].diffuse[3] = 1;
    luces[3].specular[0] = 0.5;
    luces[3].specular[1] = 0.5;
    luces[3].specular[2] = 0.5;
    luces[3].specular[3] = 1;
    luces[3].encendida = 0;  // 0 es que esta apagada
    luces[3].amplitud = 45.0;
    luces[3].m_luz = malloc(sizeof(matrices));
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(luces[3].position[0], luces[3].position[1],
                 luces[3].position[2]);
    glGetFloatv(GL_MODELVIEW_MATRIX, luces[3].m_luz->matriz);
    luces[3].m_luz->sigptr = NULL;
}

/** GENERAL INITIALIZATION **/
void initialization() {
    /*Initialization of all the variables with the default values*/
    //_window_ratio = (GLdouble) KG_WINDOW_WIDTH / (GLdouble) KG_WINDOW_HEIGHT;
    /*Definition of the background color*/
    glClearColor(KG_COL_BACK_R, KG_COL_BACK_G, KG_COL_BACK_B, KG_COL_BACK_A);
    /*Definition of the method to draw the objects*/
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_DEPTH_TEST);  // activar el test de profundidad (Z-buffer)
    glEnable(GL_LIGHTING);
    glEnable(GL_NORMALIZE);
    init_luces();

    if (flat_smooth == 1)
        glShadeModel(
            GL_SMOOTH);  // hacen falta los vectores normales de cada vertice
    else
        glShadeModel(GL_FLAT);  // basta con vector normal del poligono

    // PRIMERA CAMARA DEL MUNDO POR DEFECTO
    if (_first_camera == 0) {
        camera *c;
        c = malloc(sizeof(camera));
        matrices *mmcsr;
        mmcsr = malloc(sizeof(matrices));
        matrices *minv;
        minv = malloc(sizeof(matrices));
        c->coordenadas.x = 0.0;
        c->coordenadas.y = 0.0;
        c->coordenadas.z = 10.0;
        c->pto_at.x = 0.0;
        c->pto_at.y = 0.0;
        c->pto_at.z = 0.0;
        c->vup.x = 0.0;
        c->vup.y = 1.0;
        c->vup.z = 0.0;
        c->left = LEFT;
        c->right = RIGHT;
        c->bottom = BOTTOM;
        c->top = TOP;
        c->near = NEAR;
        c->far = FAR;
        c->perspectiva = 1;
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        gluLookAt(c->coordenadas.x, c->coordenadas.y, c->coordenadas.z,
                  c->pto_at.x, c->pto_at.y, c->pto_at.z, c->vup.x, c->vup.y,
                  c->vup.z);
        glGetFloatv(GL_MODELVIEW_MATRIX, mmcsr->matriz);
        mmcsr->sigptr = NULL;
        c->mcsr = mmcsr;
        minv->matriz[0] = mmcsr->matriz[0];
        minv->matriz[1] = mmcsr->matriz[4];
        minv->matriz[2] = mmcsr->matriz[8];
        minv->matriz[3] = 0;
        minv->matriz[4] = mmcsr->matriz[1];
        minv->matriz[5] = mmcsr->matriz[5];
        minv->matriz[6] = mmcsr->matriz[9];
        minv->matriz[7] = 0;
        minv->matriz[8] = mmcsr->matriz[2];
        minv->matriz[9] = mmcsr->matriz[6];
        minv->matriz[10] = mmcsr->matriz[10];
        minv->matriz[11] = 0;
        minv->matriz[12] = 0;
        minv->matriz[13] = 0;
        minv->matriz[14] = 10;
        minv->matriz[15] = 1;
        minv->sigptr = NULL;
        c->inversa = minv;
        c->tiene_obj = 0;  // se supone que es la camara del mundo
        c->next = 0;
        c->prev = 0;
        _first_camera = c;
        _selected_camera = _first_camera;
        _last_camera = _first_camera;
    }
}

/** MAIN FUNCTION **/
int main(int argc, char **argv) {
    /*First of all, print the help information*/
    print_help();
    /* glut initializations */
    glutInit(&argc, argv);
    // glutInitDisplayMode(GLUT_RGB);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowSize(KG_WINDOW_WIDTH, KG_WINDOW_HEIGHT);
    glutInitWindowPosition(KG_WINDOW_X, KG_WINDOW_Y);
    glutCreateWindow(KG_WINDOW_TITLE);
    /* set the callback functions */
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeyboard);
    /* this initialization has to be AFTER the creation of the window */
    initialization();
    /* start the main loop */
    glutMainLoop();

    return 0;
}
