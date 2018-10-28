#include<stdio.h>

typedef struct {
    float x;
    float y;
    float z;
    float m;
} particle;    

int main() {
    float size;
    FILE *f = fopen("lab01-input.txt","r");
    fscanf(f, "%f", &size);
    printf("Size = %f", size, "\n");
    //particle p1[100];
    //load_data(&p1);
    return 0;
}

int load_data(particle *p1) {
    FILE *inFile = fopen("lab01-input.txt", "r");
    while (fscanf(inFile, "%f %f %f %f", p1->x, p1->y, p1->z, p1->m ) == 1) {
        printf("%f %f %f %f", p1->x, p1->y, p1->z, p1->m );
    }
    fclose(inFile);
    return 0;
}
