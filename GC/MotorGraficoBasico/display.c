#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <stdio.h>

#include "definitions.h"

/** EXTERNAL VARIABLES **/
extern GLdouble _window_ratio;
extern object3d *_first_object;
extern object3d *_selected_object;
extern camera *_first_camera;
extern camera *_selected_camera;
extern light luces[GL_MAX_LIGHT];
extern int iluminacion;
extern int _selected_light;
extern int flat_smooth;

/**
 * @brief Function to draw the axes
 */
void draw_axes() {
    /*Draw X axis*/
    glColor3f(KG_COL_X_AXIS_R, KG_COL_X_AXIS_G, KG_COL_X_AXIS_B);
    glBegin(GL_LINES);
    glVertex3d(LONG_AXES, 0, 0);
    glVertex3d(-1 * LONG_AXES, 0, 0);
    glEnd();
    /*Draw Y axis*/
    glColor3f(KG_COL_Y_AXIS_R, KG_COL_Y_AXIS_G, KG_COL_Y_AXIS_B);
    glBegin(GL_LINES);
    glVertex3d(0, LONG_AXES, 0);
    glVertex3d(0, -1 * LONG_AXES, 0);
    glEnd();
    /*Draw Z axis*/
    glColor3f(KG_COL_Z_AXIS_R, KG_COL_Z_AXIS_G, KG_COL_Z_AXIS_B);
    glBegin(GL_LINES);
    glVertex3d(0, 0, LONG_AXES);
    glVertex3d(0, 0, -1 * LONG_AXES);
    glEnd();
}

void copy_mat(GLfloat mat1[], GLfloat mat2[]) {
    int i;

    for (i = 0; i < 16; i++) {
        mat1[i] = mat2[i];
    }
}

void copy_vec_3(GLfloat vec1[], GLfloat vec2[]) {
    int i;

    for (i = 0; i < 3; i++) {
        vec1[i] = vec2[i];
    }
}

/**
 * @brief Callback reshape function. We just store the new proportions of the
 * window
 * @param width New width of the window
 * @param height New height of the window
 */
void reshape(int width, int height) {
    glViewport(0, 0, width, height);
    /*  Take care, the width and height are integer numbers, but the ratio is a
     * GLdouble so, in order to avoid rounding the ratio to integer values we
     * need to cast width and height before computing the ratio */
    _window_ratio = (GLdouble)width / (GLdouble)height;
}

/**
 * @brief Callback display function
 */
void display(void) {
    GLint v_index, v, f, l_index;
    object3d *aux_obj = _first_object;
    GLfloat EJE_X[] = {1, 0, 0, 1};
    GLfloat EJE_Y[] = {0, 1, 0, 1};
    GLfloat EJE_Z[] = {0, 0, 1, 1};

    /* Clear the screen */
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    if (iluminacion == 1)
        glEnable(GL_LIGHTING);
    else
        glDisable(GL_LIGHTING);

    /* Define the projection */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // VISTA CON PERSPECTIVA
    if (_selected_camera->perspectiva == 1)
        glFrustum(_selected_camera->left, _selected_camera->right,
                  _selected_camera->bottom, _selected_camera->top,
                  _selected_camera->near, _selected_camera->far);
    else  // VISTA CON ORTOGONAL
        glOrtho(_selected_camera->left, _selected_camera->right,
                _selected_camera->bottom, _selected_camera->top,
                _selected_camera->near, _selected_camera->far);

    /* Now we start drawing the object */
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    if (_selected_object != 0) {
        glLoadMatrixf(_selected_camera->mcsr->matriz);
        glMultMatrixf(_selected_object->primptr->matriz);
        glMultMatrixf(luces[2].m_luz->matriz);
        glLightfv(GL_LIGHT2, GL_POSITION, luces[2].position);
        glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, luces[2].dir);
        glLightfv(GL_LIGHT2, GL_AMBIENT, luces[2].ambient);
        glLightfv(GL_LIGHT2, GL_DIFFUSE, luces[2].diffuse);
        glLightfv(GL_LIGHT2, GL_SPECULAR, luces[2].specular);
        glLightf(GL_LIGHT2, GL_SPOT_EXPONENT, 5.0);
        glLightf(GL_LIGHT2, GL_SPOT_CUTOFF,
                 luces[2].amplitud);  // FOCO DE ESTE ANGULO
    }

    glLoadIdentity();

    if (_selected_camera != 0) {
        glMultMatrixf(luces[3].m_luz->matriz);
        glLightfv(GL_LIGHT3, GL_POSITION, luces[3].position);
        glLightfv(GL_LIGHT3, GL_SPOT_DIRECTION, luces[3].dir);
        glLightfv(GL_LIGHT3, GL_AMBIENT, luces[3].ambient);
        glLightfv(GL_LIGHT3, GL_DIFFUSE, luces[3].diffuse);
        glLightfv(GL_LIGHT3, GL_SPECULAR, luces[3].specular);
        glLightf(GL_LIGHT3, GL_SPOT_EXPONENT, 5.0);
        glLightf(GL_LIGHT3, GL_SPOT_CUTOFF,
                 luces[3].amplitud);  // FOCO DE ESTE ANGULO
    }

    // Sol
    glLoadMatrixf(_selected_camera->mcsr->matriz);
    glLightfv(GL_LIGHT0, GL_POSITION, luces[0].position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, luces[0].ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, luces[0].diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, luces[0].specular);

    // Bombilla
    glLoadMatrixf(_selected_camera->mcsr->matriz);
    glLightfv(GL_LIGHT1, GL_POSITION, luces[1].position);
    glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, luces[1].dir);
    glLightfv(GL_LIGHT1, GL_AMBIENT, luces[1].ambient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, luces[1].diffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, luces[1].specular);
    glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 1.5);    // atenuacion
    glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.1);      // atenuacion
    glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.05);  // atenuacion

    if (luces[0].encendida == 1)
        glEnable(GL_LIGHT0);
    else
        glDisable(GL_LIGHT0);

    if (luces[1].encendida == 1)
        glEnable(GL_LIGHT1);
    else
        glDisable(GL_LIGHT1);

    if (luces[2].encendida == 1)
        glEnable(GL_LIGHT2);
    else
        glDisable(GL_LIGHT2);

    if (luces[3].encendida == 1)
        glEnable(GL_LIGHT3);
    else
        glDisable(GL_LIGHT3);

    /*First, we draw the axes*/
    glDisable(GL_LIGHTING);
    draw_axes();

    if (iluminacion == 1)
        glEnable(GL_LIGHTING);

    /*Now each of the objects in the list*/
    while (aux_obj != 0) {
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, aux_obj->mater->ambient);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, aux_obj->mater->diffuse);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, aux_obj->mater->specular);
        glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS,
                    aux_obj->mater->shininess * 128.0);

        /* Select the color, depending on whether the current object is the
         * selected one or not */
        if (aux_obj == _selected_object) {
            glColor3f(KG_COL_SELECTED_R, KG_COL_SELECTED_G, KG_COL_SELECTED_B);
        } else {
            glColor3f(KG_COL_NONSELECTED_R, KG_COL_NONSELECTED_G,
                      KG_COL_NONSELECTED_B);
        }

        /* Draw the object; for each face create a new polygon with the
         * corresponding vertices */
        glLoadMatrixf(_selected_camera->mcsr->matriz);
        glMultMatrixf(aux_obj->primptr->matriz);
        for (f = 0; f < aux_obj->num_faces; f++) {
            glBegin(GL_POLYGON);
            if (flat_smooth == 1)
                glNormal3f(aux_obj->face_table[f].vnormal.x,
                           aux_obj->face_table[f].vnormal.y,
                           aux_obj->face_table[f].vnormal.z);
            for (v = 0; v < aux_obj->face_table[f].num_vertices; v++) {
                v_index = aux_obj->face_table[f].vertex_table[v];
                if (flat_smooth == 0)
                    glNormal3f(aux_obj->vertex_table[v_index].vnormal.x,
                               aux_obj->vertex_table[v_index].vnormal.y,
                               aux_obj->vertex_table[v_index].vnormal.z);
                glVertex3d(aux_obj->vertex_table[v_index].coord.x,
                           aux_obj->vertex_table[v_index].coord.y,
                           aux_obj->vertex_table[v_index].coord.z);
            }
            glEnd();
        }
        glBegin(GL_LINES);
        glEnd();
        aux_obj = aux_obj->next;
    }

    glutSwapBuffers();

    /*Do the actual drawing*/
    glFlush();
}
