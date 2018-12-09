#include<stdio.h>
#include<stdlib.h>

const unsigned char LIVE = 1;
const unsigned char DEAD = 0;


void write_ppm(unsigned char* gen, char* filename, size_t side_length)
{

  size_t i;
  FILE *fp = fopen(filename, "wb");
  fprintf(fp, "P6\n%zd %zd\n255\n", side_length, side_length);
  
  const unsigned char _black[] = {0x00, 0x00, 0x00};
  const unsigned char _white[] = {0xff, 0xff, 0xff};

  for(i=0; i<side_length*side_length; ++i)
    if(gen[i] == 1)
      fwrite(_black, 1 , 3 , fp);
    else fwrite(_white, 1 , 3 , fp);

  fclose(fp);

}


unsigned char* read_ppm(char* filename, size_t* side_length)
{

  size_t h = 0, i;
  FILE *fp = fopen(filename, "rb");
  fscanf(fp, "P6\n%zd %zd\n255\n", side_length, &h);

  if (*side_length != h) exit(1);

  unsigned char* gen = malloc( (*side_length) * (*side_length) );

  for(i=0;i<(*side_length)*(*side_length);++i)
  {

    unsigned char buff[3];
    fread(buff, 1 , 3 , fp);


    if(buff[0] == 0 && buff[1] == 0 && buff[2] == 0) gen[i] = LIVE; // black pixel
      else gen[i] = DEAD;
   
  }

  fclose(fp);

  size_t j;
  printf("Side length = %zd", (*side_length));
  for(i=0;i<(*side_length);++i)
  {
    for(j=0;j<(*side_length);++j) 
    {
      printf("%d\t", gen[i+j]);
    }
    printf("\n");
  }
  return gen;

}


int main()
{

  size_t side_length;

  unsigned char * prev = read_ppm("input64.ppm", &side_length);

  write_ppm(prev, "output.ppm", side_length);

 
  free(prev);

  return 0;

} 
