#include <GL/glut.h>
#include <stdio.h>

#include "definitions.h"
#include "load_obj.h"

extern object3d *_first_object;
extern object3d *_selected_object;
extern camera *_first_camera;
extern camera *_selected_camera;
extern camera *_last_camera;
extern int flat_smooth;
extern light luces[GL_MAX_LIGHT];
extern int iluminacion;
extern int _selected_light;
int trasl_activa = 1;  // POR DEFECTO trasl_activa EXCLUYENTES ENTRE LOS TRES
int rotac_activa = 0;
int escal_activo = 0;
int objeto = 1;  // POR DEFECTO objeto EXCLUYENTES ENTRE LOS TRES
int camara = 0;
int luz = 0;
int local = 1;  // POR DEFECTO local. 0 es global; 1 es local (excluyentes)
int mensaje_puesto = 0;  // esto es simplemente para que un mensaje de la
                         // consola no se repita todo el rato, sino una vez

/**
 * @brief This function just prints information about the use
 * of the keys
 */
void print_help() {
    printf("\n============= FUNCIONES ==============\n");
    printf("======================================\n\n");
    printf("<?>\t\tVisualizar la ayuda\n");
    printf("<ESC>\t\tFinalizar la ejecución de la aplicación\n");
    printf("<F,f>\t\tCarga de objeto desde un fichero *.obj\n");
    printf("<TAB>\t\tSeleccionar siguiente objeto (de entre los cargados)\n");
    printf("<SUPR>\t\tEliminar objeto seleccinado\n");
    printf("<CTRL + ->\tIncrementar volumen de visualización\n");
    printf("<CTRL + +>\tReducir el volumen de visualización\n");
    printf("<M,m>\t\tActivar traslación (desactiva rotación y escalado)\n");
    printf("<B,b>\t\tActivar rotación (desactiva traslación y escalado)\n");
    printf("<T,t>\t\tActivar escalado (desactiva rotación y traslación)\n");
    printf(
        "<K,k>\t\tActivar modo cámara. Las teclas de transformación "
        "modificarán la cámara y no los objetos\n");
    printf(
        "<A,a>\t\tAplicar transformaciones a las fuentes de luz (siempre que "
        "sea posible)\n");
    printf("\n========== TRANSFORMACIONES ==========\n");
    printf(
        "<G,g>\t\tActivar transformaciones en el sistema de referencia del "
        "mundo (transformaciones globales)\n");
    printf(
        "<L,l>\t\tActivar transformaciones en el sistema de referencia local "
        "del objeto (objeto 3D, cámara o luces)\n");
    printf(
        "<O,o>\t\tAplicar transformaciones al objeto seleccionado (valor por "
        "defecto, desactiva cámara y luz)\n");
    printf(
        "<K,k>\t\tAplicar transformaciones a la cámara actual (desactiva "
        "objeto y luz)\n");
    printf(
        "<A,a>\t\tAplicar transformaciones a la luz selecionada (desactiva "
        "objeto y camara)\n");
    printf("<+>\t\tEscalar + en todos los ejes (solo objetos)\n");
    printf("<->\t\tEscalar - en todos los ejes (solo objetos)\n");
    printf("<CTRL + Z,z>\tDeshacer\n");
    printf("<UP>\t\tTrasladar +Y; Escalar +Y;  Rotar +X\n");
    printf("<DOWN>\t\tTrasladar -Y; Escalar -Y;  Rotar -X\n");
    printf("<RIGHT>\t\tTrasladar +X; Escalar +X;  Rotar +Y\n");
    printf("<LEFT>\t\tTrasladar -X; Escalar -X;  Rotar -Y\n");
    printf("<AVPAG>\t\tTrasladar +Z; Escalar +Z;  Rotar +Z\n");
    printf("<REPAG>\t\tTrasladar -Z; Escalar -Z;  Rotar -Z\n");
    printf("\n============== CAMARAS ===============\n");
    printf("<c>\t\tCambiar de cámara (puede haber más de una cámara...)\n");
    printf(
        "<C>\t\tVisualizar lo que ve el objeto seleccionado (camara del "
        "objeto)\n");
    printf("<n>\t\tAñadir una cámara nueva\n");
    printf(
        "<G,g>\t\tCámara en modo analisis (mirando al objeto seleccionado)\n");
    printf(
        "<L,l>\t\tCámara en modo vuelo. Transformaciones en el sistema local "
        "de la cámara\n");
    printf("<T,t>\t\tCambio del volumen de visión\n");
    printf("<B,b>\t\tRotaciones a la camara\n");
    printf("<M,m>\t\tTraslaciones a la camara\n");
    printf("<P,p>\t\tCambio de tipo de proyección: perspectiva/paralela\n");
    printf("<UP>\t\tTrasladar +Y;  Rotar + en X; Volumen +Y\n");
    printf("<DOWN>\t\tTrasladar -Y;  Rotar - en X; Volumen -Y\n");
    printf("<RIGHT>\t\tTrasladar +X;  Rotar + en Y; Volumen +X\n");
    printf("<LEFT>\t\tTrasladar -X;  Rotar - en  Y; Volumen -X\n");
    printf("<AVPAG>\t\tTrasladar +Z;  Rotar + en Z: Volumen +n,+f\n");
    printf("<REPAG>\t\tTrasladar -Z;  Rotar - en Z; Volumen -n,-f\n");
    printf("<+>\t\tZOOM +\n");
    printf("<->\t\tZOOM -\n");
    printf("\n=============== LUCES ================\n");
    printf("<F1>\t\tEncender/apagar luz del sol\n");
    printf("<F2>\t\tEncender/apagar luz de la bombilla\n");
    printf(
        "<F3>\t\tEncender/apagar luz del foco asociado al objeto "
        "seleccionado\n");
    printf("<F4>\t\tEncender/apagar luz del foco de la camara\n");
    printf("<F9>\t\tActivar/desactivar iluminación (por defecto activado)\n");
    printf("<F12>\t\tCambiar de tipo de iluminación (flat <-> smooth)\n");
    printf("<1-8>\t\tSeleccionar la funte de luz correspondiente\n");
    printf("<0>\t\tAsignar tipo de fuente de luz a la fuente seleccionada\n");
    printf(
        "<+>\t\tSi la fuente de luz seleccionada es de tipo foco incrementar "
        "angulo de apertura\n");
    printf(
        "<->\t\tDecrementar ángulo de apertura del foco (si es que es un "
        "foco)\n");
    printf("\n\n");
}

// CALCULA LA INVERSA DE MCSR
void matriz_inversa_camara(camera *c) {
    matrices *minv;
    minv = malloc(sizeof(matrices));
    minv->matriz[0] = c->mcsr->matriz[0];
    minv->matriz[1] = c->mcsr->matriz[4];
    minv->matriz[2] = c->mcsr->matriz[8];
    minv->matriz[3] = 0;
    minv->matriz[4] = c->mcsr->matriz[1];
    minv->matriz[5] = c->mcsr->matriz[5];
    minv->matriz[6] = c->mcsr->matriz[9];
    minv->matriz[7] = 0;
    minv->matriz[8] = c->mcsr->matriz[2];
    minv->matriz[9] = c->mcsr->matriz[6];
    minv->matriz[10] = c->mcsr->matriz[10];
    minv->matriz[11] = 0;
    minv->matriz[12] = c->coordenadas.x;
    minv->matriz[13] = c->coordenadas.y;
    minv->matriz[14] = c->coordenadas.z;
    minv->matriz[15] = 1;
    minv->sigptr = c->inversa;
    c->inversa = minv;
}

// CALCULA LA INVERSA DE LA INVERSA DE LA MCSR, ES DECIR, LA MCSR
void matriz_camara(camera *c) {
    matrices *mmcsr;
    mmcsr = malloc(sizeof(matrices));
    mmcsr->matriz[0] = c->inversa->matriz[0];
    mmcsr->matriz[1] = c->inversa->matriz[4];
    mmcsr->matriz[2] = c->inversa->matriz[8];
    mmcsr->matriz[3] = 0;
    mmcsr->matriz[4] = c->inversa->matriz[1];
    mmcsr->matriz[5] = c->inversa->matriz[5];
    mmcsr->matriz[6] = c->inversa->matriz[9];
    mmcsr->matriz[7] = 0;
    mmcsr->matriz[8] = c->inversa->matriz[2];
    mmcsr->matriz[9] = c->inversa->matriz[6];
    mmcsr->matriz[10] = c->inversa->matriz[10];
    mmcsr->matriz[11] = 0;
    mmcsr->matriz[12] = -1 * (c->inversa->matriz[12] * c->inversa->matriz[0] +
                              c->inversa->matriz[13] * c->inversa->matriz[1] +
                              c->inversa->matriz[14] * c->inversa->matriz[2]);
    mmcsr->matriz[13] = -1 * (c->inversa->matriz[12] * c->inversa->matriz[4] +
                              c->inversa->matriz[13] * c->inversa->matriz[5] +
                              c->inversa->matriz[14] * c->inversa->matriz[6]);
    mmcsr->matriz[14] = -1 * (c->inversa->matriz[12] * c->inversa->matriz[8] +
                              c->inversa->matriz[13] * c->inversa->matriz[9] +
                              c->inversa->matriz[14] * c->inversa->matriz[10]);
    mmcsr->matriz[15] = 1;
    mmcsr->sigptr = c->mcsr;
    c->mcsr = mmcsr;
}

// libera la memoria de todo el object3d
void liberar_memoria_object3d(object3d *obj) {
    int i;

    for (i = 0; i < obj->num_faces; i++) {
        free(obj->face_table[i].vertex_table);
    }

    free(obj->vertex_table);
    free(obj);
}

// AL ACTIVAR MODO ANALISIS NUESTRO PUNTO DE ATENCION PASA A SER EL CENTRO DEL
// OBJETO
void camb_sist_referencia() {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(_selected_object->primptr->matriz[12] +
                  _selected_camera->inversa->matriz[12],
              _selected_object->primptr->matriz[13] +
                  _selected_camera->inversa->matriz[13],
              _selected_object->primptr->matriz[14] +
                  _selected_camera->inversa->matriz[14],
              _selected_object->primptr->matriz[12],
              _selected_object->primptr->matriz[13],
              _selected_object->primptr->matriz[14], _selected_camera->vup.x,
              _selected_camera->vup.y, _selected_camera->vup.z);
    glGetFloatv(GL_MODELVIEW_MATRIX, _selected_camera->mcsr->matriz);
    _selected_camera->coordenadas.x = _selected_object->primptr->matriz[12] +
                                      _selected_camera->inversa->matriz[12];
    _selected_camera->coordenadas.y = _selected_object->primptr->matriz[13] +
                                      _selected_camera->inversa->matriz[13];
    _selected_camera->coordenadas.z = _selected_object->primptr->matriz[14] +
                                      _selected_camera->inversa->matriz[14];
    matriz_inversa_camara(_selected_camera);
}

/**
 * @brief Callback function to control the basic keys
 * @param key Key that has been pressed
 * @param x X coordinate of the mouse pointer when the key was pressed
 * @param y Y coordinate of the mouse pointer when the key was pressed
 */
void keyboard(unsigned char key, int x, int y) {
    char *fname =
        malloc(sizeof(char) * 128); /* Note that scanf adds a null character at
                                       the end of the vector*/
    int read = 0;
    object3d *auxiliar_object = 0;
    GLdouble wd, he, midx, midy;

    switch (key) {
        case 'f':
        case 'F':
            if (objeto == 1) {
                /*Ask for file*/
                printf("%s", KG_MSSG_SELECT_FILE);
                scanf("%s", fname);
                /*Allocate memory for the structure and read the file*/
                auxiliar_object = (object3d *)malloc(sizeof(object3d));
                read = read_wavefront(fname, auxiliar_object);
                switch (read) {
                    /*Errors in the reading*/
                    case 1:
                        printf("%s: %s\n", fname, KG_MSSG_FILENOTFOUND);
                        break;
                    case 2:
                        printf("%s: %s\n", fname, KG_MSSG_INVALIDFILE);
                        break;
                    case 3:
                        printf("%s: %s\n", fname, KG_MSSG_EMPTYFILE);
                        break;
                    /*Read OK*/
                    case 0:
                        /*Insert the new object in the list*/
                        auxiliar_object->next = _first_object;
                        _first_object = auxiliar_object;
                        _selected_object = _first_object;
                        printf("%s\n", KG_MSSG_FILEREAD);
                        break;
                }
            } else if (camara == 1) {
                printf("Nueva camara\n");
                camera *c;
                c = malloc(sizeof(camera));
                matrices *mmcsr;
                mmcsr = malloc(sizeof(matrices));
                matrices *minv;
                minv = malloc(sizeof(matrices));
                c->coordenadas.x = 5.0;
                c->coordenadas.y = 5.0;
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
                          c->pto_at.x, c->pto_at.y, c->pto_at.z, c->vup.x,
                          c->vup.y, c->vup.z);
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
                minv->matriz[12] = 5;
                minv->matriz[13] = 5;
                minv->matriz[14] = 10;
                minv->matriz[15] = 1;
                minv->sigptr = NULL;
                c->inversa = minv;
                c->tiene_obj = 0;
                c->next = 0;
                _last_camera->next = c;
                c->prev = _last_camera;
                _last_camera = _last_camera->next;
            }
            break;

        case 9: /* <TAB> */
            if (_first_object == NULL) {
                NULL;
            } else {
                _selected_object = _selected_object->next;
                /*The selection is circular, thus if we move out of the list we
                 * go back to the first element*/
                if (_selected_object == 0)
                    _selected_object = _first_object;
                if (camara == 1 && local == 0)
                    camb_sist_referencia();
            }
            break;
        case 127: /* <SUPR> */
            /*Erasing an object depends on whether it is the first one or not*/
            if (_first_object != 0) {
                if (_selected_object == _first_object) {
                    /*To remove the first object we just set the first as the
                     * current's next*/
                    _first_object = _first_object->next;
                    /*Once updated the pointer to the first object it is save to
                     * free the memory*/
                    liberar_memoria_object3d(_selected_object);
                    /*Finally, set the selected to the new first one*/
                    _selected_object = _first_object;
                } else {
                    /*In this case we need to get the previous element to the
                     * one we want to erase*/
                    auxiliar_object = _first_object;
                    while (auxiliar_object->next != _selected_object)
                        auxiliar_object = auxiliar_object->next;
                    /*Now we bypass the element to erase*/
                    auxiliar_object->next = _selected_object->next;
                    /*free the memory*/
                    liberar_memoria_object3d(_selected_object);
                    /*and update the selection*/
                    _selected_object = auxiliar_object;
                }
            } else {
                printf("No existen objetos\n");
            }
            break;

        case '-':
            if (glutGetModifiers() == GLUT_ACTIVE_CTRL) {
                /*Decrease the projection plane; compute the new dimensions*/
                wd = (_selected_camera->right - _selected_camera->left) /
                     KG_STEP_ZOOM;
                he = (_selected_camera->top - _selected_camera->bottom) /
                     KG_STEP_ZOOM;

                /*In order to avoid moving the center of the plane, we get its
                 * coordinates*/
                midx = (_selected_camera->right + _selected_camera->left) / 2;
                midy = (_selected_camera->top + _selected_camera->bottom) / 2;
                /*The the new limits are set, keeping the center of the plane*/
                _selected_camera->right = midx + wd / 2;
                _selected_camera->left = midx - wd / 2;
                _selected_camera->top = midy + he / 2;
                _selected_camera->bottom = midy - he / 2;
            } else {
                if (objeto == 1) {
                    if (escal_activo == 1) {
                        // escalados para reducir en todas las direcciones
                        if (local == 1) {
                            matrices *m;

                            m = malloc(sizeof(matrices));
                            glMatrixMode(GL_MODELVIEW);
                            glLoadMatrixf(_selected_object->primptr->matriz);
                            glScalef(1.0 - 1.0 / 10, 1.0 - 1.0 / 10,
                                     1.0 - 1.0 / 10);
                            glGetFloatv(GL_MODELVIEW_MATRIX, m->matriz);
                            m->sigptr = _selected_object->primptr;
                            _selected_object->primptr = m;
                            matriz_inversa_camara(_selected_object->obj_cam);
                            matriz_camara(_selected_object->obj_cam);
                        } else {
                            matrices *m;
                            matrices *minv;

                            m = malloc(sizeof(matrices));
                            minv = malloc(sizeof(matrices));
                            glMatrixMode(GL_MODELVIEW);
                            glLoadIdentity();
                            glScalef(1.0 - 1.0 / 10, 1.0 - 1.0 / 10,
                                     1.0 - 1.0 / 10);
                            GLfloat aux[16];
                            glGetFloatv(GL_MODELVIEW_MATRIX, aux);
                            glMultMatrixf(_selected_object->primptr->matriz);
                            glGetFloatv(GL_MODELVIEW_MATRIX, m->matriz);
                            glLoadMatrixf(aux);
                            glMultMatrixf(
                                _selected_object->obj_cam->inversa->matriz);
                            glGetFloatv(GL_MODELVIEW_MATRIX, minv->matriz);
                            minv->sigptr = _selected_object->obj_cam->inversa;
                            _selected_object->obj_cam->inversa = minv;
                            matriz_camara(_selected_object->obj_cam);
                            m->sigptr = _selected_object->primptr;
                            _selected_object->primptr = m;
                        }
                    }
                } else if (luz == 1) {
                    if (_selected_light == 2 || _selected_light == 3) {
                        luces[_selected_light].amplitud -= 1.0;
                        if (luces[_selected_light].amplitud < 0)
                            luces[_selected_light].amplitud = 0;
                    }
                }
            }
            break;

        case '+':
            // INPLEMENTA EZAZU CTRL + + KONBINAZIOAREN FUNTZIOANLITATEA
            if (glutGetModifiers() == GLUT_ACTIVE_CTRL) {
                /*Increase the projection plane; compute the new dimensions*/
                wd = (_selected_camera->right - _selected_camera->left) *
                     KG_STEP_ZOOM;
                he = (_selected_camera->top - _selected_camera->bottom) *
                     KG_STEP_ZOOM;

                /*In order to avoid moving the center of the plane, we get its
                 * coordinates*/
                midx = (_selected_camera->right + _selected_camera->left) / 2;
                midy = (_selected_camera->top + _selected_camera->bottom) / 2;
                /*The the new limits are set, keeping the center of the plane*/
                _selected_camera->right = midx + wd / 2;
                _selected_camera->left = midx - wd / 2;
                _selected_camera->top = midy + he / 2;
                _selected_camera->bottom = midy - he / 2;
            } else {
                if (objeto == 1) {
                    if (escal_activo == 1) {
                        // escalados para reducir en todas las direcciones
                        if (local == 1) {
                            matrices *m;

                            m = malloc(sizeof(matrices));
                            glMatrixMode(GL_MODELVIEW);
                            glLoadMatrixf(_selected_object->primptr->matriz);
                            glScalef(1.0 + 1.0 / 10, 1.0 + 1.0 / 10,
                                     1.0 + 1.0 / 10);
                            glGetFloatv(GL_MODELVIEW_MATRIX, m->matriz);
                            m->sigptr = _selected_object->primptr;
                            _selected_object->primptr = m;
                            matriz_inversa_camara(_selected_object->obj_cam);
                            matriz_camara(_selected_object->obj_cam);
                        } else {
                            matrices *m;
                            matrices *minv;

                            m = malloc(sizeof(matrices));
                            minv = malloc(sizeof(matrices));
                            glMatrixMode(GL_MODELVIEW);
                            glLoadIdentity();
                            glScalef(1.0 + 1.0 / 10, 1.0 + 1.0 / 10,
                                     1.0 + 1.0 / 10);
                            GLfloat aux[16];
                            glGetFloatv(GL_MODELVIEW_MATRIX, aux);
                            glMultMatrixf(_selected_object->primptr->matriz);
                            glGetFloatv(GL_MODELVIEW_MATRIX, m->matriz);
                            glLoadMatrixf(aux);
                            glMultMatrixf(
                                _selected_object->obj_cam->inversa->matriz);
                            glGetFloatv(GL_MODELVIEW_MATRIX, minv->matriz);
                            minv->sigptr = _selected_object->obj_cam->inversa;
                            _selected_object->obj_cam->inversa = minv;
                            matriz_camara(_selected_object->obj_cam);
                            m->sigptr = _selected_object->primptr;
                            _selected_object->primptr = m;
                        }
                    }
                } else if (luz == 1) {
                    if (_selected_light == 2 || _selected_light == 3) {
                        luces[_selected_light].amplitud += 1.0;
                        if (luces[_selected_light].amplitud > 180)
                            luces[_selected_light].amplitud = 180;
                    }
                }
            }
            break;

        case '?':
            print_help();
            break;

        case 27: /* <ESC> */
            exit(0);
            break;

        case '1':
            _selected_light = 0;
            printf("Activado transformaciones en el sol\n");
            break;

        case '2':
            _selected_light = 1;
            printf("Activado transformaciones en la bombilla\n");
            break;

        case '3':
            _selected_light = 2;
            printf(
                "Activado transformaciones en el foco del objeto "
                "seleccionado\n");
            break;

        case '4':
            _selected_light = 3;
            printf(
                "Activado transformaciones en el foco de la camara activa\n");
            break;

        // PARA TRANSLACIONES
        case 'M':
        case 'm':
            if (objeto == 1) {
                if (trasl_activa == 0) {
                    trasl_activa = 1;
                    rotac_activa = 0;
                    escal_activo = 0;
                    printf("ACTIVADO TRASLACIONES OBJETO\n");
                }
            } else if (camara == 1) {
                if (trasl_activa == 0) {
                    trasl_activa = 1;
                    rotac_activa = 0;
                    escal_activo = 0;
                    printf("ACTIVADO TRASLACIONES CAMARA\n");
                }
            } else {
                if (trasl_activa == 0) {
                    trasl_activa = 1;
                    rotac_activa = 0;
                    escal_activo = 0;
                    printf("ACTIVADO TRASLACIONES LUCES\n");
                }
            }
            break;

        // PARA ROTACIONES
        case 'B':
        case 'b':
            if (objeto == 1) {
                if (rotac_activa == 0) {
                    trasl_activa = 0;
                    rotac_activa = 1;
                    escal_activo = 0;
                    printf("ACTIVADO ROTACIONES OBJETO\n");
                }
            } else if (camara == 1) {
                if (rotac_activa == 0) {
                    trasl_activa = 0;
                    rotac_activa = 1;
                    escal_activo = 0;
                    printf("ACTIVADO ROTACIONES CAMARA\n");
                }
            } else {
                if (rotac_activa == 0) {
                    trasl_activa = 0;
                    rotac_activa = 1;
                    escal_activo = 0;
                    printf("ACTIVADO ROTACIONES LUCES\n");
                }
            }
            break;

        // PARA ESCALADOS
        case 'T':
        case 't':
            if (objeto == 1) {
                if (escal_activo == 0) {
                    trasl_activa = 0;
                    rotac_activa = 0;
                    escal_activo = 1;
                    printf("ACTIVADO ESCALADOS OBJETO\n");
                }
            } else if (camara == 1) {
                if (escal_activo == 0) {
                    trasl_activa = 0;
                    rotac_activa = 0;
                    escal_activo = 1;
                    printf("ACTIVADO ESCALADOS CAMARA\n");
                }
            }
            break;

        // Cambia de vista con perspectiva a vista ortogonal
        case 'p':
        case 'P':
            if (camara == 1) {
                if (_selected_camera->perspectiva == 0) {
                    _selected_camera->perspectiva = 1;
                    printf("ACTIVADO PROYECCION PERSPECTIVA\n");
                } else {
                    _selected_camera->perspectiva = 0;
                    printf("ACTIVADO PROYECCION PARALELA\n");
                }
            }
            break;

        // ACTIVAR TRANSFORMACIONES EN EL MUNDO EN GENERAL
        case 'G':
        case 'g':
            if (objeto == 1) {
                if (local == 1) {
                    local = 0;
                    printf("ACTIVADO GLOBAL\n");
                }
            } else if (camara == 1) {
                if (local == 1) {
                    local = 0;
                    camb_sist_referencia();
                    printf("ACTIVADO ANALISIS CAMARA\n");
                }
            }
            break;

        // LO MISMO PERO EN EL SISTEMA DE REFERENCIA LOCAL DEL OBJETO O DE LA
        // CAMARA
        case 'L':
        case 'l':
            if (objeto == 1) {
                if (local == 0) {
                    local = 1;
                    printf("ACTIVADO LOCAL\n");
                }
            } else if (camara == 1) {
                if (local == 0) {
                    local = 1;
                    printf("ACTIVADO VUELO CAMARA\n");
                }
            }
            break;

        // Se activa el modo objeto
        case 'O':
        case 'o':
            if (objeto == 0) {
                objeto = 1;
                camara = 0;
                luz = 0;
                trasl_activa = 1;  // Valores por defecto
                rotac_activa = 0;
                escal_activo = 0;
                local = 1;
                printf("ACTIVADO USO DE OBJETOS\n");
            }
            break;

        // Se activa el modo camara
        case 'K':
        case 'k':

            if (mensaje_puesto == 0) {
                printf("Perspectiva por defecto, para cambiar <p, P>\n");
                mensaje_puesto = 1;
            }
            if (camara == 0) {
                objeto = 0;
                camara = 1;
                luz = 0;
                trasl_activa = 1;  // Valores por defecto
                rotac_activa = 0;
                escal_activo = 0;
                local = 1;
                printf("ACTIVADO USO DE CAMARAS\n");
            }
            break;

        // Se activa el modo iluminacion
        case 'A':
        case 'a':
            if (luz == 0) {
                objeto = 0;
                camara = 0;
                luz = 1;
                trasl_activa = 1;  // Valores por defecto
                rotac_activa = 0;
                escal_activo = 0;
                local = 1;
                printf("ACTIVADO USO DE LUZ\n");
            }
            break;

        case 'c':  // CAMBIO DE CAMARA
            if (camara == 1) {
                if (_first_camera != NULL) {
                    _selected_camera = _selected_camera->next;
                    printf("Siguiente camara\n");
                    /*The selection is circular, thus if we move out of the list
                     * we go back to the first element*/
                    if (_selected_camera == 0)
                        _selected_camera = _first_camera;
                }
            }
            break;

        case 'C':  // VISUALIZACION DE LO QUE VE LA CAMARA DEL OBJETO
                   // SELECCIONADO
            if (camara == 1) {
                _selected_camera = _selected_object->obj_cam;
                printf("Visualiza lo que ve el objeto seleccionado\n");
            }
            break;

        case 26:  // CTRL + z
            if (objeto == 1) {
                if (glutGetModifiers() == GLUT_ACTIVE_CTRL) {
                    if (_selected_object != NULL) {
                        if ((_selected_object->primptr->sigptr) != NULL) {
                            _selected_object->primptr =
                                _selected_object->primptr->sigptr;
                            _selected_object->obj_cam->inversa =
                                _selected_object->obj_cam->inversa->sigptr;
                            _selected_object->obj_cam->mcsr =
                                _selected_object->obj_cam->mcsr->sigptr;
                        }
                    }
                }
            }
            break;

        default:
            /*In the default case we just print the code of the key. This is
             * usefull to define new cases*/
            printf("%d %c\n", key, key);
    }

    /*In case we have do any modification affecting the displaying of the
     * object, we redraw them*/
    glutPostRedisplay();
}

// TRANSFORMACIONES DE LAS LUCES
void transformacionLuz(float x, float y, float z, int alpha) {
    // sol
    if (_selected_light == 0) {
        if (rotac_activa == 1) {
            // como solo puede rotar de manera util en global hacemos que
            // rote en global en cualquier caso de las rotaciones
            matrices *m = malloc(sizeof(matrices));
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
            glRotatef(alpha, -y, x, z);
            glMultMatrixf(luces[0].m_luz->matriz);
            glGetFloatv(GL_MODELVIEW_MATRIX, m->matriz);
            m->sigptr = luces[0].m_luz;
            luces[0].m_luz = m;
            luces[0].position[0] = luces[0].m_luz->matriz[12];
            luces[0].position[1] = luces[0].m_luz->matriz[13];
            luces[0].position[2] = luces[0].m_luz->matriz[14];
        }
    }

    // bombilla
    if (_selected_light == 1) {
        if (trasl_activa == 1) {
            if (local == 1) {
                matrices *m = malloc(sizeof(matrices));
                glMatrixMode(GL_MODELVIEW);
                glLoadMatrixf(luces[1].m_luz->matriz);
                glTranslatef(x, y, z);
                glGetFloatv(GL_MODELVIEW_MATRIX, m->matriz);
                m->sigptr = luces[1].m_luz;
                luces[1].m_luz = m;
            } else {
                matrices *m = malloc(sizeof(matrices));
                glMatrixMode(GL_MODELVIEW);
                glLoadIdentity();
                glTranslatef(x, y, z);
                glMultMatrixf(luces[1].m_luz->matriz);
                glGetFloatv(GL_MODELVIEW_MATRIX, m->matriz);
                m->sigptr = luces[1].m_luz;
                luces[1].m_luz = m;
            }
        }

        if (rotac_activa == 1) {
            // como solo puede rotar de manera util en global hacemos que
            // rote en global en cualquier caso de las rotaciones
            matrices *m = malloc(sizeof(matrices));
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
            glRotatef(alpha, -y, x, z);
            glMultMatrixf(luces[1].m_luz->matriz);
            glGetFloatv(GL_MODELVIEW_MATRIX, m->matriz);
            m->sigptr = luces[1].m_luz;
            luces[1].m_luz = m;
        }

        luces[1].position[0] = luces[1].m_luz->matriz[12];
        luces[1].position[1] = luces[1].m_luz->matriz[13];
        luces[1].position[2] = luces[1].m_luz->matriz[14];
    }

    // foco obj o foco cam
    if (_selected_light == 2 || _selected_light == 3) {
        if (rotac_activa == 1) {
            // como solo puede rotar en local hacemos que rote en local en
            // cualquier caso de las rotaciones
            matrices *m = malloc(sizeof(matrices));
            glMatrixMode(GL_MODELVIEW);
            glLoadMatrixf(luces[_selected_light].m_luz->matriz);
            glRotatef(alpha, -y, x, z);
            glGetFloatv(GL_MODELVIEW_MATRIX, m->matriz);
            m->sigptr = luces[_selected_light].m_luz;
            luces[_selected_light].m_luz = m;
        }
    }
}

// TRANSLACIONES EN MODO VUELO
void translacionVuelo(float x, float y, float z) {
    matrices *minv;

    minv = malloc(sizeof(matrices));

    glMatrixMode(GL_MODELVIEW);

    glLoadMatrixf(_selected_camera->inversa->matriz);
    glTranslatef(x, y, z);

    glGetFloatv(GL_MODELVIEW_MATRIX, minv->matriz);
    minv->sigptr = _selected_camera->inversa;
    _selected_camera->inversa = minv;

    matriz_camara(_selected_camera);
    _selected_camera->coordenadas.x += x;
    _selected_camera->coordenadas.y += y;
    _selected_camera->coordenadas.z += z;
}

// ROTACIONES EN MODO VUELO
void rotacionVuelo(float x, float y, float z, int alpha) {
    matrices *minv;
    minv = malloc(sizeof(matrices));
    glMatrixMode(GL_MODELVIEW);

    glLoadMatrixf(_selected_camera->inversa->matriz);
    glRotatef(alpha, x, y, z);
    glGetFloatv(GL_MODELVIEW_MATRIX, minv->matriz);
    minv->sigptr = _selected_camera->inversa;
    _selected_camera->inversa = minv;
    matriz_camara(_selected_camera);
}

// SI LAS TRANSLACIONES ESTAN ACTIVAS LLAMA A LAS TRANSLACIONES
void transformacionCamaraMundoVuelo(float x, float y, float z, int alpha) {
    // NOTA: Mcsr * Mobj (o Mtransf, no se cual era) * Mcsr^⁻1 * Mobj PARA SACAR
    // LAS TRANSFORMACIONES DE LA CAMARA
    if (trasl_activa == 1) {
        translacionVuelo(x, y, z);
        printf("Traslacion camara\n");
    }
    if (rotac_activa == 1) {
        rotacionVuelo(-y, x, z, alpha);
        printf("Rotacion camara\n");
    }
}

// TRANSLACIONES EN MODO ANALISIS
void translacionAnalisis(float x, float y, float z) {
    GLfloat m_at[16] = {1,
                        0,
                        0,
                        0,
                        0,
                        1,
                        0,
                        0,
                        0,
                        0,
                        1,
                        0,
                        _selected_object->primptr->matriz[12],
                        _selected_object->primptr->matriz[13],
                        _selected_object->primptr->matriz[14],
                        1};
    GLfloat m_menos_at[16] = {1,
                              0,
                              0,
                              0,
                              0,
                              1,
                              0,
                              0,
                              0,
                              0,
                              1,
                              0,
                              (-1) * _selected_object->primptr->matriz[12],
                              (-1) * _selected_object->primptr->matriz[13],
                              (-1) * _selected_object->primptr->matriz[14],
                              1};

    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(m_at);
    glTranslatef(x, y, z);
    glMultMatrixf(m_menos_at);

    matriz_inversa_camara(_selected_camera);  // calcula la m inversa mcsr
    glMultMatrixf(_selected_camera->inversa->matriz);
    glGetFloatv(GL_MODELVIEW_MATRIX, _selected_camera->inversa->matriz);
    matriz_camara(_selected_camera);
    _selected_camera->coordenadas.x = _selected_camera->inversa->matriz[12];
    _selected_camera->coordenadas.y = _selected_camera->inversa->matriz[13];
    _selected_camera->coordenadas.z = _selected_camera->inversa->matriz[14];
}

// ROTACIONES EN MODO ANALISIS
void rotacionAnalisis(float x, float y, float z, int alpha) {
    GLfloat m_at[16] = {1,
                        0,
                        0,
                        0,
                        0,
                        1,
                        0,
                        0,
                        0,
                        0,
                        1,
                        0,
                        _selected_object->primptr->matriz[12],
                        _selected_object->primptr->matriz[13],
                        _selected_object->primptr->matriz[14],
                        1};
    GLfloat m_menos_at[16] = {1,
                              0,
                              0,
                              0,
                              0,
                              1,
                              0,
                              0,
                              0,
                              0,
                              1,
                              0,
                              (-1) * _selected_object->primptr->matriz[12],
                              (-1) * _selected_object->primptr->matriz[13],
                              (-1) * _selected_object->primptr->matriz[14],
                              1};

    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(m_at);
    glRotatef(alpha, x, y, z);
    glMultMatrixf(m_menos_at);

    matriz_inversa_camara(_selected_camera);  // calcula la m inversa mcsr
    glMultMatrixf(_selected_camera->inversa->matriz);
    glGetFloatv(GL_MODELVIEW_MATRIX, _selected_camera->inversa->matriz);
    matriz_camara(_selected_camera);
    _selected_camera->coordenadas.x = _selected_camera->inversa->matriz[12];
    _selected_camera->coordenadas.y = _selected_camera->inversa->matriz[13];
    _selected_camera->coordenadas.z = _selected_camera->inversa->matriz[14];
}

// ESTA FUNCION HACE QUE AL ROTAR UN OBJETO LA CAMARA ROTE CON EL
void rotacionCamObj(float x, float y, float z, int alpha) {
    GLfloat m_at[16] = {1,
                        0,
                        0,
                        0,
                        0,
                        1,
                        0,
                        0,
                        0,
                        0,
                        1,
                        0,
                        _selected_object->primptr->matriz[12],
                        _selected_object->primptr->matriz[13],
                        _selected_object->primptr->matriz[14],
                        1};
    GLfloat m_menos_at[16] = {1,
                              0,
                              0,
                              0,
                              0,
                              1,
                              0,
                              0,
                              0,
                              0,
                              1,
                              0,
                              (-1) * _selected_object->primptr->matriz[12],
                              (-1) * _selected_object->primptr->matriz[13],
                              (-1) * _selected_object->primptr->matriz[14],
                              1};

    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(m_at);
    glRotatef(alpha, x, y, z);
    glMultMatrixf(m_menos_at);

    matriz_inversa_camara(
        _selected_object->obj_cam);  // calcula la m inversa mcsr
    glMultMatrixf(_selected_object->obj_cam->inversa->matriz);
    glGetFloatv(GL_MODELVIEW_MATRIX,
                _selected_object->obj_cam->inversa->matriz);
    matriz_camara(_selected_object->obj_cam);
    _selected_object->obj_cam->coordenadas.x =
        _selected_object->obj_cam->inversa->matriz[12];
    _selected_object->obj_cam->coordenadas.y =
        _selected_object->obj_cam->inversa->matriz[13];
    _selected_object->obj_cam->coordenadas.z =
        _selected_object->obj_cam->inversa->matriz[14];
}

// DADOS UNAS X, Y, Z Y UN ANGULO ALFA, SI TOCA HACER TRANSLACIONES, UNICAMENTE
// NOS MOVEMOS X, Y, Z SI TOCA HACER ESCALADOS, ESCALAMOS 1 + X/10, 1 + Y/10, 1
// + Z/10 (AL ESCALAR ESTARIAMOS ESCALANDO, POR EJEMPLO: 1.0, 1.1, 1.0, SI
// ESCALASEMOS LA Y, YA QUE LAS X, Y, Z SON O BIEN 0.0 O 1.0 O -1.0) Y POR
// ULTIMO LAS ROTACIONES USANDO EL ANGULO ALFA DADO Y SIMPLEMENTE LAS X, Y, Z
// QUE SE LE DAN, NADA MAS
void transforLocal(float x, float y, float z, int alpha) {
    // TRANSFORMACION EN EL SISTEMA DE REF DEL OBJETO
    matrices *m;

    m = malloc(sizeof(matrices));
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(_selected_object->primptr->matriz);
    if (trasl_activa == 1) {
        glTranslatef(x, y, z);
        glGetFloatv(GL_MODELVIEW_MATRIX, m->matriz);
        m->sigptr = _selected_object->primptr;
        _selected_object->primptr = m;
        // transformaciones de su camara
        matriz_inversa_camara(_selected_object->obj_cam);
        _selected_object->obj_cam->inversa->matriz[12] += x;
        _selected_object->obj_cam->coordenadas.x += x;
        _selected_object->obj_cam->inversa->matriz[13] += y;
        _selected_object->obj_cam->coordenadas.y += y;
        _selected_object->obj_cam->inversa->matriz[14] += z;
        _selected_object->obj_cam->coordenadas.z += z;
        matriz_camara(_selected_object->obj_cam);
    } else if (escal_activo == 1) {
        glScalef(1.0 + x / 10, 1.0 + y / 10, 1.0 + z / 10);
        glGetFloatv(GL_MODELVIEW_MATRIX, m->matriz);
        m->sigptr = _selected_object->primptr;
        _selected_object->primptr = m;
        matriz_inversa_camara(_selected_object->obj_cam);
        matriz_camara(_selected_object->obj_cam);
    } else {
        glRotatef(alpha, -y, x, z);
        glGetFloatv(GL_MODELVIEW_MATRIX, m->matriz);
        m->sigptr = _selected_object->primptr;
        _selected_object->primptr = m;
        // transformaciones de su camara
        rotacionCamObj(-y, x, z, alpha);
    }
}

// DADOS UNAS X, Y, Z Y UN ANGULO ALFA, SI TOCA HACER TRANSLACIONES, UNICAMENTE
// NOS MOVEMOS X, Y, Z SI TOCA HACER ESCALADOS, ESCALAMOS 1 + X/10, 1 + Y/10, 1
// + Z/10 (AL ESCALAR ESTARIAMOS ESCALANDO, POR EJEMPLO: 1.0, 1.1, 1.0, SI
// ESCALASEMOS LA Y, YA QUE LAS X, Y, Z SON O BIEN 0.0 O 1.0 O -1.0) Y POR
// ULTIMO LAS ROTACIONES USANDO EL ANGULO ALFA DADO Y SIMPLEMENTE LAS X, Y, Z
// QUE SE LE DAN, NADA MAS
void transforGlobal(float x, float y, float z, int alpha) {
    // TRANSFORMACION EN EL SISTEMA DE REF DE LA CAMARA
    matrices *m;
    matrices *minv;

    m = malloc(sizeof(matrices));
    minv = malloc(sizeof(matrices));
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    if (trasl_activa == 1)
        glTranslatef(x, y, z);
    else if (escal_activo == 1)
        glScalef(1.0 + x / 10, 1.0 + y / 10, 1.0 + z / 10);
    else {
        glRotatef(alpha, -y, x, z);
    }
    GLfloat aux[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, aux);
    glMultMatrixf(_selected_object->primptr->matriz);
    glGetFloatv(GL_MODELVIEW_MATRIX, m->matriz);
    glLoadMatrixf(aux);
    glMultMatrixf(_selected_object->obj_cam->inversa->matriz);
    glGetFloatv(GL_MODELVIEW_MATRIX, minv->matriz);
    minv->sigptr = _selected_object->obj_cam->inversa;
    _selected_object->obj_cam->inversa = minv;
    matriz_camara(_selected_object->obj_cam);
    m->sigptr = _selected_object->primptr;
    _selected_object->primptr = m;
}

void specialKeyboard(int key, int x, int y) {
    if (_selected_object != NULL) {
        // AL DETECTAR QUE USAMOS LAS FLECHAS O AVPAG REPAG, LLAMAREMOS A
        // FUNCIONES AUXILIARES QUE SE ENCARGARAN DE HACER LAS TRANSFORMACIONES
        // PERTINENTES EN LOCAL O GLOBAL, SE PUEDE VER QUE PASAMOS EN TODAS
        // ESTAS
        //-1.0 O 1.0 Y 10 EN EL ULTIMO PARAMETRO, PARA ENTENDER MEJOR PORQUE
        // MIRAR LAS FUNCIONES AUXILIARES
        if (key == GLUT_KEY_DOWN) {  // -Y
            if (objeto == 1) {
                if (local == 1) {
                    transforLocal(0.0, -1.0, 0.0, 10);
                } else
                    transforGlobal(0.0, -1.0, 0.0, 10);
            }
            if (camara == 1) {
                if (escal_activo == 1) {
                    _selected_camera->top -= 0.1;
                    _selected_camera->bottom += 0.1;
                }
                if (local == 1)
                    transformacionCamaraMundoVuelo(0.0, -1.0, 0.0, 10);
                else if (rotac_activa == 1)
                    rotacionAnalisis(_selected_camera->mcsr->matriz[0],
                                     _selected_camera->mcsr->matriz[4],
                                     _selected_camera->mcsr->matriz[8], -10);
            }
            if (luz == 1) {
                transformacionLuz(0.0, -1.0, 0.0, 10);
            }
        }

        if (key == GLUT_KEY_LEFT) {  //-X
            if (objeto == 1) {
                if (local == 1)
                    transforLocal(-1.0, 0.0, 0.0, 10);
                else
                    transforGlobal(-1.0, 0.0, 0.0, 10);
            }
            if (camara == 1) {
                if (escal_activo == 1) {
                    _selected_camera->right -= 0.1;
                    _selected_camera->left += 0.1;
                }
                if (local == 1)
                    transformacionCamaraMundoVuelo(-1.0, 0.0, 0.0, 10);
                else if (rotac_activa == 1)
                    rotacionAnalisis(_selected_camera->mcsr->matriz[1],
                                     _selected_camera->mcsr->matriz[5],
                                     _selected_camera->mcsr->matriz[9], -10);
            }
            if (luz == 1) {
                transformacionLuz(-1.0, 0.0, 0.0, 10);
            }
        }

        if (key == GLUT_KEY_RIGHT) {  // +X
            if (objeto == 1) {
                if (local == 1)
                    transforLocal(1.0, 0.0, 0.0, 10);
                else
                    transforGlobal(1.0, 0.0, 0.0, 10);
            }
            if (camara == 1) {
                if (escal_activo == 1) {
                    _selected_camera->right += 0.1;
                    _selected_camera->left -= 0.1;
                }
                if (local == 1)
                    transformacionCamaraMundoVuelo(1.0, 0.0, 0.0, 10);
                else if (rotac_activa == 1)
                    rotacionAnalisis(_selected_camera->mcsr->matriz[1],
                                     _selected_camera->mcsr->matriz[5],
                                     _selected_camera->mcsr->matriz[9], 10);
            }
            if (luz == 1) {
                transformacionLuz(1.0, 0.0, 0.0, 10);
            }
        }

        if (key == GLUT_KEY_UP) {  // +Y
            if (objeto == 1) {
                if (local == 1)
                    transforLocal(0.0, 1.0, 0.0, 10);
                else
                    transforGlobal(0.0, 1.0, 0.0, 10);
            }
            if (camara == 1) {
                if (escal_activo == 1) {
                    _selected_camera->top += 0.1;
                    _selected_camera->bottom -= 0.1;
                }
                if (local == 1)
                    transformacionCamaraMundoVuelo(0.0, 1.0, 0.0, 10);
                else if (rotac_activa == 1)
                    rotacionAnalisis(_selected_camera->mcsr->matriz[0],
                                     _selected_camera->mcsr->matriz[4],
                                     _selected_camera->mcsr->matriz[8], 10);
            }
            if (luz == 1) {
                transformacionLuz(0.0, 1.0, 0.0, 10);
            }
        }

        if (key == GLUT_KEY_PAGE_DOWN) {  // +Z
            if (objeto == 1) {
                if (local == 1)
                    transforLocal(0.0, 0.0, 1.0, 10);
                else
                    transforGlobal(0.0, 0.0, 1.0, 10);
            }
            if (camara == 1) {
                if (escal_activo == 1) {
                    _selected_camera->near += 0.1;
                    _selected_camera->far += 0.1;
                }
                if (local == 1)
                    transformacionCamaraMundoVuelo(0.0, 0.0, 1.0, 10);
                else {
                    if (trasl_activa == 1)
                        translacionAnalisis(_selected_camera->mcsr->matriz[2],
                                            _selected_camera->mcsr->matriz[6],
                                            _selected_camera->mcsr->matriz[10]);
                    else if (rotac_activa == 1)
                        rotacionAnalisis(_selected_camera->mcsr->matriz[2],
                                         _selected_camera->mcsr->matriz[6],
                                         _selected_camera->mcsr->matriz[10],
                                         10);
                }
            }
            if (luz == 1) {
                transformacionLuz(0.0, 0.0, 1.0, 10);
            }
        }

        if (key == GLUT_KEY_PAGE_UP) {  // -Z
            if (objeto == 1) {
                if (local == 1)
                    transforLocal(0.0, 0.0, -1.0, 10);
                else
                    transforGlobal(0.0, 0.0, -1.0, 10);
            }
            if (camara == 1) {
                if (escal_activo == 1) {
                    _selected_camera->near -= 0.1;
                    _selected_camera->far -= 0.1;
                }
                if (local == 1)
                    transformacionCamaraMundoVuelo(0.0, 0.0, -1.0, 10);
                else {
                    if (trasl_activa == 1)
                        translacionAnalisis(
                            -_selected_camera->mcsr->matriz[2],
                            -_selected_camera->mcsr->matriz[6],
                            -_selected_camera->mcsr->matriz[10]);
                    else if (rotac_activa == 1)
                        rotacionAnalisis(_selected_camera->mcsr->matriz[2],
                                         _selected_camera->mcsr->matriz[6],
                                         _selected_camera->mcsr->matriz[10],
                                         -10);
                }
            }
            if (luz == 1) {
                transformacionLuz(0.0, 0.0, -1.0, 10);
            }
        }

        if (key == GLUT_KEY_F1) {
            if (luces[0].encendida == 1) {
                luces[0].encendida = 0;
                printf("Apagada la luz del sol\n");
            } else {
                luces[0].encendida = 1;
                printf("Encendida la luz del sol\n");
            }
        }

        if (key == GLUT_KEY_F2) {
            if (luces[1].encendida == 1) {
                luces[1].encendida = 0;
                printf("Apagada la luz de la bombilla\n");
            } else {
                luces[1].encendida = 1;
                printf("Encendida la luz de la bombilla\n");
            }
        }

        if (key == GLUT_KEY_F3) {
            if (luces[2].encendida == 1) {
                luces[2].encendida = 0;
                printf("Apagada la luz del foco del objeto\n");
            } else {
                luces[2].encendida = 1;
                printf("Encendida la luz del foco del objeto\n");
            }
        }

        if (key == GLUT_KEY_F4) {
            if (luces[3].encendida == 1) {
                luces[3].encendida = 0;
                printf("Apagada la luz del foco de la camara\n");
            } else {
                luces[3].encendida = 1;
                printf("Encendida la luz del foco de la camara\n");
            }
        }

        if (key == GLUT_KEY_F9) {
            if (iluminacion == 1) {
                iluminacion = 0;
                printf("Desactivado iluminacion\n");
            } else {
                iluminacion = 1;
                printf("Activado iluminacion\n");
            }
        }

        if (key == GLUT_KEY_F12) {
            if (flat_smooth == 1) {
                flat_smooth = 0;
                printf("Aplicado suavizado\n");
            } else {
                flat_smooth = 1;
                printf("Quitado suavizado\n");
            }
        }
    }

    glutPostRedisplay();
}
