#include <malloc.h>
#include <stdio.h>

#include "cargar-triangulos.h"

#define MAXLINE 200

void cargar_triangulos(int *hkopptr, hiruki **hptrptr) {
    FILE *obj_file;
    char line[MAXLINE];
    int i, num_triangles;

    if ((obj_file = fopen("triangles.txt", "r")) == NULL) {
        *hkopptr = 0;
        return;
    }

    num_triangles = 0;
    while (fscanf(obj_file, "\n%[^\n]", line) > 0) {
        if (line[0] == 't') {
            num_triangles++;
        }
    }

    fclose(obj_file);
    *hkopptr = num_triangles;
    *hptrptr = (hiruki *)malloc(num_triangles * sizeof(hiruki));
    obj_file = fopen("triangles.txt", "r");

    i = 0;
    while (fscanf(obj_file, "\n%[^\n]", line) > 0) {
        if (line[0] == 't') {
            sscanf(line + 2, "%f%f%f%f%f%f%f%f%f%f%f%f%f%f%f",
                   &((*hptrptr)[i].p1.x), &((*hptrptr)[i].p1.y),
                   &((*hptrptr)[i].p1.z), &((*hptrptr)[i].p1.u),
                   &((*hptrptr)[i].p1.v), &((*hptrptr)[i].p2.x),
                   &((*hptrptr)[i].p2.y), &((*hptrptr)[i].p2.z),
                   &((*hptrptr)[i].p2.u), &((*hptrptr)[i].p2.v),
                   &((*hptrptr)[i].p3.x), &((*hptrptr)[i].p3.y),
                   &((*hptrptr)[i].p3.z), &((*hptrptr)[i].p3.u),
                   &((*hptrptr)[i].p3.v));
            i++;
        }
    }

    fclose(obj_file);
}
