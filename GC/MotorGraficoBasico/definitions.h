// Daniel Ruskov GC20/21

#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <GL/gl.h>

/** DEFINITIONS **/

#define KG_WINDOW_TITLE "Practica GPO"
#define KG_WINDOW_WIDTH 900
#define KG_WINDOW_HEIGHT 600
#define KG_WINDOW_X 50
#define KG_WINDOW_Y 50

#define KG_MSSG_SELECT_FILE "Idatz ezazu fitxategiaren path-a: "
#define KG_MSSG_FILENOTFOUND "Fitxategi hori ez da existitzen!!"
#define KG_MSSG_INVALIDFILE "Arazoren bat egon da fitxategiarekin ..."
#define KG_MSSG_EMPTYFILE "Fitxategia hutsik dago"
#define KG_MSSG_FILEREAD "Fitxategiaren irakurketa buruta"

#define KG_STEP_MOVE 5.0f
#define KG_STEP_ROTATE 10.0f
#define KG_STEP_ZOOM 0.75
#define KG_STEP_CAMERA_ANGLE 5.0f

#define KG_ORTHO_X_MIN_INIT -5
#define KG_ORTHO_X_MAX_INIT 5
#define KG_ORTHO_Y_MIN_INIT -5
#define KG_ORTHO_Y_MAX_INIT 5
#define KG_ORTHO_Z_MIN_INIT -10000
#define KG_ORTHO_Z_MAX_INIT 10000

#define LEFT -0.1
#define RIGHT 0.1
#define BOTTOM -0.1
#define TOP 0.1
#define NEAR 0.1
#define FAR 1000.0

#define KG_COL_BACK_R 0.30f
#define KG_COL_BACK_G 0.30f
#define KG_COL_BACK_B 0.30f
#define KG_COL_BACK_A 1.00f

#define KG_COL_SELECTED_R 1.00f
#define KG_COL_SELECTED_G 0.75f
#define KG_COL_SELECTED_B 0.00f

#define KG_COL_NONSELECTED_R 1.00f
#define KG_COL_NONSELECTED_G 1.00f
#define KG_COL_NONSELECTED_B 1.00f

#define KG_COL_X_AXIS_R 1.0f
#define KG_COL_X_AXIS_G 0.0f
#define KG_COL_X_AXIS_B 0.0f

#define KG_COL_Y_AXIS_R 0.0f
#define KG_COL_Y_AXIS_G 1.0f
#define KG_COL_Y_AXIS_B 0.0f

#define KG_COL_Z_AXIS_R 0.0f
#define KG_COL_Z_AXIS_G 0.0f
#define KG_COL_Z_AXIS_B 1.0f

#define KG_MAX_DOUBLE 10E25
#define LONG_AXES 5000.0

#define GL_MAX_LIGHT 4

/** STRUCTURES **/

/****************************
 * Structure to store the   *
 * coordinates of 3D points *
 ****************************/
typedef struct {
    GLdouble x, y, z;
} point3;

/*****************************
 * Structure to store the    *
 * coordinates of 3D vectors *
 *****************************/
typedef struct {
    GLdouble x, y, z;
} vector3;

/****************************
 * Structure to store the   *
 * colors in RGB mode       *
 ****************************/
typedef struct {
    GLfloat r, g, b;
} color3;

/****************************
 * Structure to store       *
 * objects' vertices         *
 ****************************/
typedef struct {
    point3 coord;    /* coordinates,x, y, z */
    GLint num_faces; /* number of faces that share this vertex */
    vector3 vnormal; /* vector normal del vertice */
} vertex;

/****************************
 * Structure to store       *
 * objects' faces or        *
 * polygons                 *
 ****************************/
typedef struct {
    GLint num_vertices;  /* number of vertices in the face */
    GLint *vertex_table; /* table with the index of each vertex */
    vector3 vnormal;     /* vector normal de la cara */
} face;

/****************************
 * Structure to store a     *
 * pile of 3D objects       *
 ****************************/
struct object3d {             // VAMOS A TENER QUE AÑADIR OPCION DE DESHACER
    GLint num_vertices;       /* number of vertices in the object*/
    vertex *vertex_table;     /* table of vertices */
    GLint num_faces;          /* number of faces in the object */
    face *face_table;         /* table of faces */
    point3 min;               /* coordinates' lower bounds */
    point3 max;               /* coordinates' bigger bounds */
    struct object3d *next;    /* next element in the pile of objects */
    struct matrices *primptr; /* PRIMER ELEMENTO DE LA LISTA DE MATRICES  */
    struct camera *obj_cam;   /* camara propia del objeto */
    struct material *mater;   /* material del objeto */
};

typedef struct object3d object3d;

typedef struct camera {
    point3 coordenadas; /* coordenadas de la camara */
    point3 pto_at;      /* coordenadas del punto de atencion */
    vector3 vup;        /* coordenadas del vector de verticalidad */
    GLdouble left;      /* parametro left para hallar el volumen de vision */
    GLdouble right;     /* parametro right para hallar el volumen de vision */
    GLdouble bottom;    /* parametro bottom para hallar el volumen de vision */
    GLdouble top;       /* parametro top para hallar el volumen de vision */
    GLdouble near;      /* parametro near para hallar el volumen de vision */
    GLdouble far;       /* parametro far para hallar el volumen de vision */
    int perspectiva; /* modo en el que estamos, si = 1 en perspectiva, si = 0 en
                        ortogonal */
    int tiene_obj;   /* para determinar si la camara tiene objeto asociado, para
                        evitar modo analisis con la camara principal del mundo */
    struct matrices *mcsr; /* matriz de cambio de sistema de referencia */
    struct matrices
        *inversa; /* matriz inversa a la de cambio de sistema de referencia */
    struct camera *next; /* siguiente camara */
    struct camera *prev; /* camara anterior */
} camera;

typedef struct matrices {
    GLfloat matriz[16];
    struct matrices *sigptr;
} matrices;

typedef struct light {
    GLfloat position[4];    /* posicion de la luz */
    GLfloat dir[3];         /* direccion a la que apunta */
    int encendida;          /* indica si la luz esta encendida o no */
    struct matrices *m_luz; /* matriz de la luz */
    GLfloat ambient[4];     /* valores de ambient de la luz */
    GLfloat diffuse[4];     /* valores de diffuse de la luz */
    GLfloat specular[4];    /* valores de specular de la luz */
    GLfloat amplitud;       /* valor de la amplitud (el angulo) */
} light;

typedef struct material {
    GLfloat ambient[4];
    GLfloat diffuse[4];
    GLfloat specular[4];
    GLfloat shininess;
} material;

#endif  // DEFINITIONS_H