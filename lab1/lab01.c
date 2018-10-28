#include<stdio.h>

typedef struct {
    float x;
    float y;
    float z;
    float m;
} particle;    

int main() {
    int size;
    FILE *f = fopen("lab01-input.txt","r");
    fscanf(f, "%d", &size);
    // printf("Size = %d", size, "\n");
    
    particle input[size];

    readInfo(input);
    
    /*int i;
    for(i=0; i<size; i++ ){
 	printf("\n");
	printf("x = %f \t y = %f \t z = %f m = %f", input[i].x, input[i].y, input[i].z, input[i].m );
    }*/

    calcCOG(input, size);

    return 0;
}


int calcCOG(particle input[], int size) {
    int i;
    float totalMass;
    float cogX;
    float cogY;
    float cogZ;
    
    for(i = 0; i < size; i++) {
        totalMass += input[i].m;
    }
    
    for(i = 0; i < size; i++) {
        cogX += input[i].m * input[i].x;
        cogY += input[i].m * input[i].y;
        cogZ += input[i].m * input[i].z;
    }

    cogX = cogX/totalMass;
    cogY = cogY/totalMass;
    cogZ = cogZ/totalMass;
    
    printf("%e\t%e\t%e\t%e", cogX, cogY, cogZ, totalMass);
    printf("\n");
    return 0;
}

int
readInfo (particle input[])
{
  FILE *fin;
  char *inputName = "lab01-input.txt";
  int i = 0;

  char buffer[102];
 
  fin = fopen (inputName, "r");

  if (fin == NULL)
    {
      printf ("File cannot be opened\n");
    }/* end if */
  else
    {
      fgets(buffer, 102, fin);

      while (!feof (fin))
	{
 	  fscanf (fin, "%f %f %f %f", &input[i].x,
 	  &input[i].y, &input[i].z, &input[i].m);
	  i++;

	}/* end while */

      fclose (fin);
    }/* end else */

  return i;
}
