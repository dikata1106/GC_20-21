#include <GL/glut.h>
#include <math.h>
#include <stdio.h>

#include "cargar-triangulos.h"

extern void load_ppm(char *file, unsigned char **bufferptr, int *dimxptr,
                     int *dimyptr);

unsigned char *bufferra;        // puntero a los pixeles rgb de la imagen
int dimx, dimy;                 // dimensiones x e y de la imagen
int num_triangles, indice = 0;  // numero de triangulos e indice
hiruki *triangulosptr;          // puntero a la informacion de los puntos de los
                                // diferentes triangulos

/** Funcion que comprueba si un triangulo cumple que x, y = {0, 499}
 * z = {0} u, v = {0.0 0.999} y en caso de que alguna coordenada pase
 * los limites, hace el modulo correspondiente
 */
void comprobar_triangulos_modulo_signo(void) {
    int i;

    for (i = 0; i < num_triangles; i++) {
        if (triangulosptr[i].p1.x >= 500 || triangulosptr[i].p1.x <= -500)
            triangulosptr[i].p1.x = fmod(triangulosptr[i].p1.x, 500);
        if (triangulosptr[i].p1.x < 0)
            triangulosptr[i].p1.x += 500;
        if (triangulosptr[i].p1.y >= 500 || triangulosptr[i].p1.y <= -500)
            triangulosptr[i].p1.y = fmod(triangulosptr[i].p1.y, 500);
        if (triangulosptr[i].p1.y < 0)
            triangulosptr[i].p1.y += 500;
        if (triangulosptr[i].p1.z != 0)
            triangulosptr[i].p1.z = 0;
        if (triangulosptr[i].p1.u >= 1 || triangulosptr[i].p1.u <= -1)
            triangulosptr[i].p1.u = fmod(triangulosptr[i].p1.u, 1);
        if (triangulosptr[i].p1.u < 0)
            triangulosptr[i].p1.u += 1;
        if (triangulosptr[i].p1.v >= 1 || triangulosptr[i].p1.v <= -1)
            triangulosptr[i].p1.v = fmod(triangulosptr[i].p1.v, 1);
        if (triangulosptr[i].p1.v < 0)
            triangulosptr[i].p1.v += 1;

        if (triangulosptr[i].p2.x >= 500 || triangulosptr[i].p2.x <= -500)
            triangulosptr[i].p2.x = fmod(triangulosptr[i].p2.x, 500);
        if (triangulosptr[i].p2.x < 0)
            triangulosptr[i].p2.x += 500;
        if (triangulosptr[i].p2.y >= 500 || triangulosptr[i].p2.y <= -500)
            triangulosptr[i].p2.y = fmod(triangulosptr[i].p2.y, 500);
        if (triangulosptr[i].p2.y < 0)
            triangulosptr[i].p2.y += 500;
        if (triangulosptr[i].p2.z != 0)
            triangulosptr[i].p2.z = 0;
        if (triangulosptr[i].p2.u >= 1 || triangulosptr[i].p2.u <= -1)
            triangulosptr[i].p2.u = fmod(triangulosptr[i].p2.u, 1);
        if (triangulosptr[i].p2.u < 0)
            triangulosptr[i].p2.u += 1;
        if (triangulosptr[i].p2.v >= 1 || triangulosptr[i].p2.v <= -1)
            triangulosptr[i].p2.v = fmod(triangulosptr[i].p2.v, 1);
        if (triangulosptr[i].p2.v < 0)
            triangulosptr[i].p2.v += 1;

        if (triangulosptr[i].p3.x >= 500 || triangulosptr[i].p3.x <= -500)
            triangulosptr[i].p3.x = fmod(triangulosptr[i].p3.x, 500);
        if (triangulosptr[i].p3.x < 0)
            triangulosptr[i].p3.x += 500;
        if (triangulosptr[i].p3.y >= 500 || triangulosptr[i].p3.y <= -500)
            triangulosptr[i].p3.y = fmod(triangulosptr[i].p3.y, 500);
        if (triangulosptr[i].p3.y < 0)
            triangulosptr[i].p3.y += 500;
        if (triangulosptr[i].p3.z != 0)
            triangulosptr[i].p3.z = 0;
        if (triangulosptr[i].p3.u >= 1 || triangulosptr[i].p3.u <= -1)
            triangulosptr[i].p3.u = fmod(triangulosptr[i].p3.u, 1);
        if (triangulosptr[i].p3.u < 0)
            triangulosptr[i].p3.u += 1;
        if (triangulosptr[i].p3.v >= 1 || triangulosptr[i].p3.v <= -1)
            triangulosptr[i].p3.v = fmod(triangulosptr[i].p3.v, 1);
        if (triangulosptr[i].p3.v < 0)
            triangulosptr[i].p3.v += 1;
    }
}

/** Funcion que dadas las coordenadas u y v de la foto ppm,
 * devuelve puntero a la la posicion del pixel de esas coordenadas
 * en el buffer que almacena la informacion de cada uno de los
 * pixeles de la foto (color r, g, b).
 */
unsigned char *color_textura(float u, float v) {
    return (bufferra + 3 * dimx * (int)((1.0 - v) * (dimy - 1)) +
            3 * (int)(u * (dimx - 1)));
}

/** Funcion que, dadas las coordenadas i, j y la informacion
 * de color r, g, b, dibuja el pixel i, j con el color RGB
 */
void dibujar_pixel(int i, int j, int r, int g, int b) {
    glBegin(GL_POINTS);
    glColor3ub(r, g, b);
    glVertex3f(i, j, 0);
    glEnd();
}

/** Funcion que dibuja un segmento horizontal entre los puntos
 * inters1 e inters2, recogiendo desde buferra los pixeles del
 * triangulo original de forma proporcional
 */
void dibujar_seg(punto inters1, punto inters2) {
    unsigned char *colorv;
    int x;
    float U, deltaU, V, deltaV;

    U = inters1.u;
    V = inters1.v;

    if (inters2.x - inters1.x != 0) {
        deltaU = (inters2.u - inters1.u) / (inters2.x - inters1.x);
        deltaV = (inters2.v - inters1.v) / (inters2.x - inters1.x);
    } else {
        deltaU = 0;
        deltaV = 0;
    }

    for (x = inters1.x; x <= inters2.x; x++) {
        colorv = color_textura(U, V);
        U += deltaU;
        V += deltaV;
        dibujar_pixel(x, inters1.y, colorv[0], colorv[1], colorv[2]);
    }
}

/** Funcion que establece el orden de los tres puntos de un triangulo
 * en el eje vertical como superior, inferior y mediano, haciendo que
 * cada puntero apunte al vertice correspondiente en orden
 */
void establecer_orden(punto **sptrptr, punto **iptrptr, punto **mptrptr) {
    if (triangulosptr[indice].p1.y >= triangulosptr[indice].p2.y) {
        *sptrptr = &(triangulosptr[indice].p1);
        *iptrptr = &(triangulosptr[indice].p2);
    } else {
        *sptrptr = &(triangulosptr[indice].p2);
        *iptrptr = &(triangulosptr[indice].p1);
    }

    if (triangulosptr[indice].p3.y > (*sptrptr)->y) {
        *mptrptr = *sptrptr;
        *sptrptr = &(triangulosptr[indice].p3);
    } else if (triangulosptr[indice].p3.y < (*iptrptr)->y) {
        *mptrptr = *iptrptr;
        *iptrptr = &(triangulosptr[indice].p3);
    } else {
        *mptrptr = &(triangulosptr[indice].p3);
    }
}

/** Funcion que calcula punto de interseccion entre un eje y = h
 * y uno de los lados del triangulo con los que hace corte mediante
 * regla de tres (proporcional)
 */
void calcular_interseccion(punto *sptr, punto *iptr, int y, punto *inters) {
    if (sptr->y - iptr->y != 0) {
        inters->x =
            (sptr->x - iptr->x) / (sptr->y - iptr->y) * (y - iptr->y) + iptr->x;
        inters->u =
            (sptr->u - iptr->u) / (sptr->y - iptr->y) * (y - iptr->y) + iptr->u;
        inters->v =
            (sptr->v - iptr->v) / (sptr->y - iptr->y) * (y - iptr->y) + iptr->v;
    } else {
        inters->x = iptr->x;
        inters->u = iptr->u;
        inters->v = iptr->v;
    }

    inters->y = y;
    inters->z = 0;
}

/** Funcion que dubuja el triangulo correspondiente al indice en funcion
 * de linea en linea horizontalmente, y llevandolo a cabo de forma proporcional
 */
void dibujar_triangulo(void) {
    punto *sptr, *iptr, *mptr, inters1, inters2;
    int y;

    establecer_orden(&sptr, &iptr, &mptr);

    if (sptr->y == iptr->y) {
        if (sptr->x >= mptr->x && mptr->x >= iptr->x)
            dibujar_seg(*iptr, *sptr);
        else if (sptr->x >= mptr->x && iptr->x >= mptr->x)
            dibujar_seg(*mptr, *sptr);
        else if (mptr->x >= sptr->x && sptr->x >= iptr->x)
            dibujar_seg(*iptr, *mptr);
        else if (mptr->x >= sptr->x && iptr->x >= sptr->x)
            dibujar_seg(*sptr, *mptr);
        else if (mptr->x >= sptr->x)
            dibujar_seg(*sptr, *iptr);
        else
            dibujar_seg(*mptr, *iptr);
    } else if (sptr->y == mptr->y) {
        if (sptr->x > mptr->x)
            dibujar_seg(*mptr, *sptr);
        else
            dibujar_seg(*sptr, *mptr);
        for (y = sptr->y - 1; y >= iptr->y; y--) {
            calcular_interseccion(mptr, iptr, y, &inters1);
            calcular_interseccion(sptr, iptr, y, &inters2);
            if (inters1.x > inters2.x)
                dibujar_seg(inters2, inters1);
            else
                dibujar_seg(inters1, inters2);
        }
    } else if (mptr->y == iptr->y) {
        for (y = sptr->y; y > mptr->y; y--) {
            calcular_interseccion(sptr, mptr, y, &inters1);
            calcular_interseccion(sptr, iptr, y, &inters2);
            if (inters1.x > inters2.x)
                dibujar_seg(inters2, inters1);
            else
                dibujar_seg(inters1, inters2);
        }
        if (iptr->x > mptr->x)
            dibujar_seg(*mptr, *iptr);
        else
            dibujar_seg(*iptr, *mptr);
    } else {
        for (y = sptr->y; y >= mptr->y; y--) {
            calcular_interseccion(sptr, mptr, y, &inters1);
            calcular_interseccion(sptr, iptr, y, &inters2);
            if (inters1.x > inters2.x)
                dibujar_seg(inters2, inters1);
            else
                dibujar_seg(inters1, inters2);
        }
        for (; y >= iptr->y; y--) {
            calcular_interseccion(mptr, iptr, y, &inters1);
            calcular_interseccion(sptr, iptr, y, &inters2);
            if (inters1.x > inters2.x)
                dibujar_seg(inters2, inters1);
            else
                dibujar_seg(inters1, inters2);
        }
    }
}

/** Funcion que configura entorno de salida del programa mediante OpenGL
 * y dibuja en el el triangulo correspondiente al indice actual con la
 * foto ppm. Inicialmente hace comprobacion  de que todos los vertices
 * pertenezcan al plano representado y los ajusta si sobrepasan alguna
 * dimension
 */
static void marraztu(void) {
    float u, v;
    float i, j;
    unsigned char *colorv;
    unsigned char r, g, b;

    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, 500.0, 0.0, 500.0, -250.0, 250.0);
    dibujar_triangulo();
    glFlush();
}

/** Funcion que sera llamada cada vez que se pulsa una tecla.
 * Dependiendo de la tecla pulsada incrementara el indice
 * correspondiente al triangulo a dibujar <ENTER> o finalizara
 * el la ejecucion del programa <ESC>. Imprimira el valor de
 * la tecla pulsada e.o.c
 */
static void teklatua(unsigned char key, int x, int y) {
    switch (key) {
        case 13:  // <ENTER>
            if (indice < num_triangles)
                indice++;
            else
                indice = 0;
            printf(
                "ENTER: que hay que dibujar el siguiente triángulo - triangulo "
                "numero %d (1..num_triangulos)\n",
                indice + 1);
            break;
        case 27:  // <ESC>
            printf("ESC: finalizar programa\n");
            exit(0);
            break;
        default:
            printf("%d: N/A %c\n", key, key);
            break;
    }

    // The screen must be drawn to show the new triangle
    glutPostRedisplay();
}

int main(int argc, char **argv) {
    printf("This program draws points in the viewport \n");
    printf("Press <ESC> to finish\n");
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB);
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("GL_POINTS");
    glutDisplayFunc(marraztu);
    glutKeyboardFunc(teklatua);
    load_ppm("foto.ppm", &bufferra, &dimx, &dimy);
    cargar_triangulos(&num_triangles, &triangulosptr);
    comprobar_triangulos_modulo_signo();
    glClearColor(0.0f, 0.0f, 0.7f, 1.0f);
    glutMainLoop();

    return 0;
}
