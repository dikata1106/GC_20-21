#include <GL/glut.h>
#include <malloc.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "definitions.h"

#define MAXLINE 200

extern camera *_first_camera;
extern camera *_selected_camera;
extern camera *_last_camera;

/*
 * Auxiliar function to process each line of the file
 */
static int sreadint(char *lerroa, int *zenbakiak) {
    char *s = lerroa;
    int i, zbk, kont = 0;

    while (sscanf(s, " %d%n", &zbk, &i) > 0) {
        s += i;
        zenbakiak[kont++] = zbk;
    }

    return (kont);
}

static int sreadint2(char *lerroa, int *zenbakiak) {
    char *s = lerroa;
    int i, zbk, kont = 0;

    while (sscanf(s, " %d%n", &zbk, &i) > 0) {
        s += i;
        while ((*s != ' ') && (*s != '\0'))
            s++;  // jump vector normal information
        zenbakiak[kont++] = zbk;
    }

    printf("%d numbers in the line\n", kont);
    return (kont);
}

void calcVecNormales(object3d *obj) {
    int i;
    int j;
    int index_1, index_2, index_3;
    point3 vertice_1, vertice_2, vertice_3;
    GLfloat Qx, Qy, Qz, Px, Py, Pz, vnormal_X, vnormal_Y, vnormal_Z;
    GLfloat modulo;

    // aqui inicializamos a 0 los vectores normales del vertice
    obj->vertex_table[i].vnormal.x = 0;
    obj->vertex_table[i].vnormal.y = 0;
    obj->vertex_table[i].vnormal.z = 0;

    for (i = 0; i < obj->num_faces; i++) {
        // coger indices de los tres primeros vertices y buscarlos en vertex
        // table de obj
        index_1 = obj->face_table[i].vertex_table[0];
        index_2 = obj->face_table[i].vertex_table[1];
        index_3 = obj->face_table[i].vertex_table[2];
        vertice_1 = obj->vertex_table[index_1].coord;
        vertice_2 = obj->vertex_table[index_2].coord;
        vertice_3 = obj->vertex_table[index_3].coord;
        // vector Q
        Qx = vertice_2.x - vertice_1.x;
        Qy = vertice_2.y - vertice_1.y;
        Qz = vertice_2.z - vertice_1.z;
        // vector P
        Px = vertice_3.x - vertice_1.x;
        Py = vertice_3.y - vertice_1.y;
        Pz = vertice_3.z - vertice_1.z;
        // calculo del vector normal
        vnormal_X = Pz * Qy - Py * Qz;
        vnormal_Y = Px * Qz - Pz * Qx;
        vnormal_Z = Py * Qx - Px * Qy;
        // modulo
        modulo = sqrt((pow(vnormal_X, 2)) + (pow(vnormal_Y, 2)) +
                      (pow(vnormal_Z, 2)));
        obj->face_table[i].vnormal.x = vnormal_X / modulo;
        obj->face_table[i].vnormal.y = vnormal_Y / modulo;
        obj->face_table[i].vnormal.z = vnormal_Z / modulo;
        // sumamos los valores del vector normal para cada vertice de esta cara
        for (j = 0; j < obj->face_table[i].num_vertices; j++) {
            index_1 = obj->face_table[i].vertex_table[j];
            obj->vertex_table[index_1].vnormal.x +=
                obj->face_table[i].vnormal.x;
            obj->vertex_table[index_1].vnormal.y +=
                obj->face_table[i].vnormal.y;
            obj->vertex_table[index_1].vnormal.z +=
                obj->face_table[i].vnormal.z;
        }
    }

    // vector normal para cada vertice
    for (i = 0; i < obj->num_vertices; i++) {
        modulo = sqrt(pow(obj->vertex_table[i].vnormal.x, 2) +
                      pow(obj->vertex_table[i].vnormal.y, 2) +
                      pow(obj->vertex_table[i].vnormal.z, 2));
        obj->vertex_table[i].vnormal.x =
            obj->vertex_table[i].vnormal.x / modulo;
        obj->vertex_table[i].vnormal.y =
            obj->vertex_table[i].vnormal.y / modulo;
        obj->vertex_table[i].vnormal.z =
            obj->vertex_table[i].vnormal.z / modulo;
    }
}

// material del que se componen los objetos (RUBY)
void setMaterial(object3d *obj) {
    obj->mater = malloc(sizeof(material));

    obj->mater->ambient[0] = 0.1745;
    obj->mater->ambient[1] = 0.01175;
    obj->mater->ambient[2] = 0.01175;
    obj->mater->ambient[3] = 1;
    obj->mater->diffuse[0] = 0.61424;
    obj->mater->diffuse[1] = 0.4136;
    obj->mater->diffuse[2] = 0.4136;
    obj->mater->diffuse[3] = 1;
    obj->mater->specular[0] = 0.727811;
    obj->mater->specular[1] = 0.626959;
    obj->mater->specular[2] = 0.626959;
    obj->mater->specular[3] = 1;
    obj->mater->shininess = 0.6;
}

// Inicializacion de la camara asociada al objeto
void initCamObj(object3d *obj) {
    obj->obj_cam = malloc(sizeof(camera));
    matrices *mmcsr;
    mmcsr = malloc(sizeof(matrices));
    matrices *minv;
    minv = malloc(sizeof(matrices));

    obj->obj_cam->coordenadas.x = 0.0;
    obj->obj_cam->coordenadas.y = 0.0;
    obj->obj_cam->coordenadas.z = obj->max.z;
    obj->obj_cam->pto_at.x = 0.0;
    obj->obj_cam->pto_at.y = 0.0;
    obj->obj_cam->pto_at.z = obj->max.z + 1.0;
    obj->obj_cam->vup.x = 0.0;
    obj->obj_cam->vup.y = 1.0;
    obj->obj_cam->vup.z = 0.0;
    obj->obj_cam->next = NULL;
    obj->obj_cam->tiene_obj = 1;
    obj->obj_cam->left = LEFT;
    obj->obj_cam->right = RIGHT;
    obj->obj_cam->bottom = BOTTOM;
    obj->obj_cam->top = TOP;
    obj->obj_cam->near = NEAR;
    obj->obj_cam->far = FAR;
    obj->obj_cam->perspectiva = 1;
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(obj->obj_cam->coordenadas.x, obj->obj_cam->coordenadas.y,
              obj->obj_cam->coordenadas.z, obj->obj_cam->pto_at.x,
              obj->obj_cam->pto_at.y, obj->obj_cam->pto_at.z,
              obj->obj_cam->vup.x, obj->obj_cam->vup.y, obj->obj_cam->vup.z);
    glGetFloatv(GL_MODELVIEW_MATRIX, mmcsr->matriz);
    mmcsr->sigptr = NULL;
    obj->obj_cam->mcsr = mmcsr;
    minv->matriz[0] = 1;
    minv->matriz[1] = 0;
    minv->matriz[2] = 0;
    minv->matriz[3] = 0;
    minv->matriz[4] = 0;
    minv->matriz[5] = 1;
    minv->matriz[6] = 0;
    minv->matriz[7] = 0;
    minv->matriz[8] = 0;
    minv->matriz[9] = 0;
    minv->matriz[10] = 1;
    minv->matriz[11] = 0;
    minv->matriz[12] = 0;
    minv->matriz[13] = 0;
    minv->matriz[14] = obj->max.z;
    minv->matriz[15] = 1;
    minv->sigptr = NULL;
    obj->obj_cam->inversa = minv;
    _last_camera->next = obj->obj_cam;
    obj->obj_cam->prev = _last_camera;
    _last_camera = _last_camera->next;
}

/**
 * @brief Function to read wavefront files (*.obj)
 * @param file_name Path of the file to be read
 * @param object_ptr Pointer of the object3d type structure where the data will
 * be stored
 * @return Result of the reading: 0=Read ok, 1=File not found, 2=Invalid file,
 * 3=Empty file
 */
int read_wavefront(char *file_name, object3d *object_ptr) {
    vertex *vertex_table;
    face *face_table;
    int num_vertices = -1, num_faces = -1, count_vertices = 0, count_faces = 0;
    FILE *obj_file;
    char line[MAXLINE], line_1[MAXLINE], aux[45];
    int k;
    int i, j;
    int values[MAXLINE];

    /*
     * The function reads twice the file. In the first read the number of
     * vertices and faces is obtained. Then, memory is allocated for each
     * of them and in the second read the actual information is read and
     * loaded. Finally, the object structure is created
     */
    if ((obj_file = fopen(file_name, "r")) == NULL)
        return (1);
    while (fscanf(obj_file, "\n%[^\n]", line) > 0) {
        i = 0;
        while (line[i] == ' ')
            i++;
        if ((line[0] == '#') && ((int)strlen(line) > 5)) {
            i += 2;
            j = 0;
            while (line[i] != ' ')
                line_1[j++] = line[i++];
            i++;
            line_1[j] = '\0';
            j = 0;
            while ((line[i] != ' ') && (line[i] != '\0'))
                aux[j++] = line[i++];
            aux[j] = 0;
            if (strcmp(aux, "vertices") == 0) {
                num_vertices = atoi(line_1);
            }
            if (strncmp(aux, "elements", 7) == 0) {
                num_faces = atoi(line_1);
            }
        } else {
            if (strlen(line) > 6) {
                if (line[i] == 'f' && line[i + 1] == ' ')
                    count_faces++;
                else if (line[i] == 'v' && line[i + 1] == ' ')
                    count_vertices++;
            }
        }
    }
    fclose(obj_file);
    printf("1 pasada: num vert = %d (%d), num faces = %d(%d) \n", num_vertices,
           count_vertices, num_faces, count_faces);
    if ((num_vertices != -1 && num_vertices != count_vertices) ||
        (num_faces != -1 && num_faces != count_faces)) {
        printf("WARNING: full file format: (%s)\n", file_name);
        // return (2);
    }
    if (num_vertices == 0 || count_vertices == 0) {
        printf("No vertex found: (%s)\n", file_name);
        return (3);
    }
    if (num_faces == 0 || count_faces == 0) {
        printf("No faces found: (%s)\n", file_name);
        return (3);
    }

    num_vertices = count_vertices;
    num_faces = count_faces;

    vertex_table = (vertex *)malloc(num_vertices * sizeof(vertex));
    face_table = (face *)malloc(num_faces * sizeof(face));

    obj_file = fopen(file_name, "r");
    k = 0;
    j = 0;

    for (i = 0; i < num_vertices; i++)
        vertex_table[i].num_faces = 0;

    while (fscanf(obj_file, "\n%[^\n]", line) > 0) {
        switch (line[0]) {
            case 'v':
                if (line[1] == ' ')  // vn not interested
                {
                    sscanf(line + 2, "%lf%lf%lf", &(vertex_table[k].coord.x),
                           &(vertex_table[k].coord.y),
                           &(vertex_table[k].coord.z));
                    k++;
                }
                break;

            case 'f':
                if (line[1] == ' ')  // fn not interested
                {
                    for (i = 2; i <= (int)strlen(line); i++)
                        line_1[i - 2] = line[i];
                    line_1[i - 2] = '\0';
                    face_table[j].num_vertices = sreadint2(line_1, values);
                    face_table[j].vertex_table =
                        (int *)malloc(face_table[j].num_vertices * sizeof(int));
                    for (i = 0; i < face_table[j].num_vertices; i++) {
                        face_table[j].vertex_table[i] = values[i] - 1;
                        vertex_table[face_table[j].vertex_table[i]].num_faces++;
                    }
                    j++;
                }
                break;
        }
    }

    fclose(obj_file);
    printf("2 pasada\n");
    /*
     * Information read is introduced in the structure */
    object_ptr->vertex_table = vertex_table;
    object_ptr->face_table = face_table;
    object_ptr->num_vertices = num_vertices;
    object_ptr->num_faces = num_faces;

    matrices *m;
    m = malloc(sizeof(matrices));
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glGetFloatv(GL_MODELVIEW_MATRIX, m->matriz);
    m->sigptr = NULL;
    object_ptr->primptr = m;

    /*
     * The maximum and minimum coordinates are obtained **/
    object_ptr->max.x = object_ptr->vertex_table[0].coord.x;
    object_ptr->max.y = object_ptr->vertex_table[0].coord.y;
    object_ptr->max.z = object_ptr->vertex_table[0].coord.z;
    object_ptr->min.x = object_ptr->vertex_table[0].coord.x;
    object_ptr->min.y = object_ptr->vertex_table[0].coord.y;
    object_ptr->min.z = object_ptr->vertex_table[0].coord.z;

    for (i = 1; i < object_ptr->num_vertices; i++) {
        if (object_ptr->vertex_table[i].coord.x < object_ptr->min.x)
            object_ptr->min.x = object_ptr->vertex_table[i].coord.x;

        if (object_ptr->vertex_table[i].coord.y < object_ptr->min.y)
            object_ptr->min.y = object_ptr->vertex_table[i].coord.y;

        if (object_ptr->vertex_table[i].coord.z < object_ptr->min.z)
            object_ptr->min.z = object_ptr->vertex_table[i].coord.z;

        if (object_ptr->vertex_table[i].coord.x > object_ptr->max.x)
            object_ptr->max.x = object_ptr->vertex_table[i].coord.x;

        if (object_ptr->vertex_table[i].coord.y > object_ptr->max.y)
            object_ptr->max.y = object_ptr->vertex_table[i].coord.y;

        if (object_ptr->vertex_table[i].coord.z > object_ptr->max.z)
            object_ptr->max.z = object_ptr->vertex_table[i].coord.z;
    }
    printf("coordenadas calculadas\n");
    initCamObj(object_ptr);
    printf("camera objeto inicializada\n");
    calcVecNormales(object_ptr);
    printf("vectores normales calculados\n");
    setMaterial(object_ptr);
    printf("material definido\n");
    printf("ready to play\n");

    return (0);
}
