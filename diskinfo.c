#define OS_OFFSET 3
#define LABEL_POS 43
#define LABEL_SIZE 11
   #define MAX_BUFFER_SIZE 4096

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>  // mmap
#include <fcntl.h>  // open
#include <sys/stat.h> // fstat

char *getthefilename(void *file){
	char* Nameis=(char*)file+OS_OFFSET;
	return Nameis;
}
void getlable( char *mmap,char *label) {
	int sector = 512;
	int i,j,k;
	for (i = 19; i <= 32; i ++) {
		for (j = 0; j < 16; j++) {
			int mask = mmap[(i * sector) + (j * 32) + 11];
			if ((mask & 0x08) == 0x08 && (mask & 0x0F) != 0x0F) {
				for(k = 0; k < 11; k++) {
					label[k] = mmap[(i * sector) + (j * 32) + k];
				}
				return;
			}
		}
	}
	
}
int getSectorCount(FILE* file)
{
  int *tmp1 = malloc(sizeof(int));
  int *tmp2 = malloc(sizeof(int));
  int retVal;
  fseek(file, 19L, SEEK_SET);
  fread(tmp1, 1, 1, file);
  fread(tmp2, 1, 1, file);
  retVal = *tmp1 + ( (*tmp2) << 8 );
  free(tmp1);
  free(tmp2);
  return retVal;
}
int getfatcopynumb(FILE* file)
{
  int count=0;
  fseek(file, 16, SEEK_SET);//Number of FATs is starting at 16
  fread(&count, 1, 1, file);
  return count;
}
int getfreesize(FILE* file)
{
  int counter = 0; //The number of free sectors (of SECTOR_SIZE)
  int n;
  int tmp1 = 0; //byte 1
  int tmp2 = 0; //byte 2
  int result = 0x00;
  //printf("check!!!111");
  for (n = 2; n <= 2848; n++)
  {
    if (n % 2 == 0)
    {
      fseek(file, 512 + 3*n/2, SEEK_SET);
      fread(&tmp1, 1, 1, file);
      fread(&tmp2, 1 ,1, file);
      tmp2 = tmp2 & 0x0F;
      result = (tmp2 << 8) + tmp1;
    }
    else
    {
      fseek(file, 512 + 3*n/2, SEEK_SET);
      fread(&tmp1, 1, 1, file);
      fread(&tmp2, 1 ,1, file);
      tmp1 = tmp1 & 0xF0;
      result = (tmp1 >> 4) + (tmp2 << 4);
    }
//printf("check!!!222");
    if (result == 0x00)
    {
      counter ++;
    }
  }
 // printf("check!!!333");
  return counter;
}
int getrootsfilenum(char *map) {
	int count = 0;
	int i,j;
	i=19;
	//19 is First sector in the floppy disk's root directory 
	for (i; i <= 32; i ++) {
		for (j = 0; j < 16; j++) {
			int Attributes = map[11+(512*i) +(32*j)];
			if (map[(i * 512) + (j * 32)] == 0x00) {
				return count;
			}
			if ((Attributes & 0x0F) != 0x0F && (Attributes & 0x08) != 0x08 && (Attributes & 0x10) != 0x10) {//not Subdirectory, Volume label then the attributes is a file
				count=count+1;
			}
		}
	}
	
	return count;
}
int getfatsector(FILE* file) {
	int count=0;
  fseek(file, 22, SEEK_SET);//Number of FATs is starting at 16
  fread(&count, 1, 1, file);
  return count;
}

int main(int argc, char *argv[]) {
	int of=0;
	
	struct stat file_stats;
	void *map;
	char* image_file_Name = malloc(sizeof(char)*8);
	char* diskLabel = malloc(4096);
	// memset(diskLabel, '\0', MAX_BUFFER_SIZE);
	 if( argc != 2 ){
        fprintf(stderr, "usage: ./diskinfo [disk]\n");
        exit(-1);
    }
	//printf("check!!!2");
	char *image_file = argv[1];

	//FILE* open_file=0;
	of = open(image_file, O_RDONLY);
	FILE* hiahia=0;
	hiahia = fopen(image_file,"r");
	//printf("check!!3!");
	if(of== 0||hiahia==0) {
		printf("Fail to open the image file.\n");
		return 0;
	}
		fstat(of, &file_stats);
		map = mmap(NULL, file_stats.st_size, PROT_READ, MAP_SHARED, of, 0);
		image_file_Name=getthefilename(map);
		printf("OS Name: %s\n", image_file_Name);
		getlable(map,diskLabel);
		printf("Label of the disk: %s\n", diskLabel);
		int totalsectorsize;
		totalsectorsize=getSectorCount(hiahia);
		 printf("Total size of the disk: %d bytes\n", totalsectorsize*512);
		int freesize=0;
		freesize=getfreesize(hiahia);
		printf("Free size on the disk: %d\n", freesize*512);
		printf("==================\n");
		int rootsfilenum=0;
		rootsfilenum=getrootsfilenum(map);
		printf("Number of files in the root directory: %d\n", rootsfilenum);
		printf("==================\n");
		int fatcopy=0;
		fatcopy=getfatcopynumb(hiahia);
		printf("Number of FAT copies: %d\n", fatcopy);
		int fatsector=0;
		fatsector=getfatsector(hiahia);
		printf("Sectors per FAT: %d\n", fatsector);
}

