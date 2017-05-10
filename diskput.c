#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <assert.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <arpa/inet.h>
void InsertFile(FILE* file,FILE* insert,int FileSize,char* inputFile,struct stat sf_file ){
		int i,j;
		int current_file=19*512;		
		int status=0;
		//printf("check!!!I am here (1.5)");
		fseek(file, 19*512, SEEK_SET);
		fread(&status, 1, 1, file);
		//printf("chekckk!!!!1111222");
		while(status != 0x00&&status!=0xE5){
			current_file=current_file+32;
				 fseek(file, current_file, SEEK_SET);
				fread(&status, 1, 1, file);
		}
		char content[4096];
		char *filename;
		char *extendname;
		char* dot = strtok (inputFile,".");
		filename=dot;
		dot = strtok (NULL, ".");
		extendname=dot;
		char* newstr=strdup(filename);
				char* str=newstr;
				 while(*str){
					*str = toupper( *str );
					str ++;
				}
		filename=newstr;
		char* newstr2=strdup(extendname);
				char* str2=newstr2;
				 while(*str2){
					*str2 = toupper( *str2 );
					str2 ++;
				}
		extendname=newstr2;
	time_t modify_t = sf_file.st_mtime; // modify time
	time_t create_t = sf_file.st_ctime; // Create time
	struct tm *tm_modify;
	struct tm *tm_create;
	char lastmodifyTime[50]; 
	char lastcreateTime[50]; 
	// Get modify time in local time
	tm_modify = localtime(&modify_t);
	strftime(lastmodifyTime, sizeof(lastmodifyTime), "%Y%m%d%H%M%S", tm_modify);

	// Get create time in local time
	tm_create = localtime(&create_t);
	strftime(lastcreateTime, sizeof(lastcreateTime), "%Y%m%d%H%M%S", tm_create);
	printf("modify time: %s\n", lastmodifyTime);
	printf("create time: %s\n", lastcreateTime);
	 sprintf(content, "%-8s%-3s%c%c%c", filename,extendname, '\0', '\0', '\0');
	fwrite(content, sizeof(char), 30, file);	 
	sprintf(content, "%s", tm_create);
	 fwrite(content, sizeof(char), 14, file);
	//printf("I am here");
	int FAT = GetFreeFAT(file, -1);
	 unsigned int res = 0;
  res = (FAT >> 8 ) | (FAT << 8);
	 unsigned int cfat = res;
	  char cfata = (char) (cfat >> 8);
	  char cfatb = (char) cfat;


  res = ((FileSize >> 24) & 0x000000ff) | // move byte 3 to byte 0
        ((FileSize << 8)  & 0x00ff0000) | // move byte 1 to byte 2
        ((FileSize >> 8)  & 0x0000ff00) | // move byte 2 to byte 1
        ((FileSize << 24) & 0xff000000); // byte 0 to byte 3
	  unsigned int csize = res;
	  char fsa = (char) (csize >> 24);
	  char fsb = (char) (csize >> 16);
	  char fsc = (char) (csize >> 8);
	  char fsd = (char) csize;

	  sprintf(content, "%c%c%c%c%c%c",
		cfata, cfatb,
		fsa, fsb, fsc, fsd);
	  fwrite(content, sizeof(char), 6, file);
//==========================================
			int size = 0;
		  int nextFAT = FAT;
		  while(1)
		  {
			FAT = nextFAT;
			int readSize;
			if(FileSize - size<512){
				 readSize = FileSize - size; //[0, 512]
			}else{
				readSize=512;
			}
			char* buffer = malloc(4096);
			//memset(buffer, '\0', 4096);
			fread(buffer, 1, readSize, insert); //Read in X bytes from input file into the buffer
			
			int sectorPos=(FAT+33-2)*512;
			 fseek(file, sectorPos, SEEK_SET);
			fwrite(buffer, sizeof(char), size, file);
			//writeDataFromFAT(buffer, readSize, file, FAT); //Write to data section.
			free(buffer);

			size += readSize;
			if(size >= FileSize)
			{
			  nextFAT = 0xFFF;
			}
			else
			{
			  nextFAT = GetFreeFAT(file, FAT); //Get the next fat, not 0
			}
			writeNextFat(file, FAT, nextFAT);
		//printf("I am hererer");
		  if(size >= readSize){
			break;
			}
		  }
}




int GetFreeFAT(FILE* file, int notFAT)
{
  int n = 2;
  int base = 512;
  int tmp1 = 0; //byte 1
  int tmp2 = 0; //byte 2
  int result = 0x00;

  while(1)
  {
    if (n % 2 == 0)
    {
      fseek(file, base + 3*n/2, SEEK_SET);
      fread(&tmp1, 1, 1, file);
      fread(&tmp2, 1 ,1, file);
      tmp2 = tmp2 & 0x0F; //Get the low 4 bits
      result = (tmp2 << 8) + tmp1;
    }
    else
    {
      fseek(file, base + 3*n/2, SEEK_SET);
      fread(&tmp1, 1, 1, file);
      fread(&tmp2, 1 ,1, file);
      tmp1 = tmp1 & 0xF0; //high 4 bits
      result = (tmp1 >> 4) + (tmp2 << 4);
    }

    if (result == 0x00)
    {
      if(n != notFAT)
      {
        return n;
      }
    }
    ++n;
  }
}

int main(int argc, char *argv[]) {
	int of,od;
	int i;
	struct stat file_stats,file_disk;
	void *map;
	FILE* hiahia=0;
	FILE* *disk=0;
	char* image_file_Name = malloc(sizeof(char)*8);
	char* diskLabel = malloc(4096);
	// memset(diskLabel, '\0', MAX_BUFFER_SIZE);
	 if( argc != 3 ){
        fprintf(stderr, "usage: ./diskLIST [disk].IMA selectfile\n");
        exit(-1);
    }
	char *image_file = argv[1];
	char* inputFile = argv[2];
	if((od=open(image_file, O_RDONLY))){
		if((of=open(inputFile, O_RDONLY))){
			fstat(of, &file_stats);
			fstat(od, &file_disk);
			map = mmap(NULL, file_disk.st_size, PROT_READ, MAP_SHARED, od, 0);
			hiahia = fopen(inputFile,"r");
			int FileSize =0;
			 fseek(hiahia, 0L, SEEK_END);
			 FileSize = ftell(hiahia);
			printf("what is its' size %d\n",FileSize);
			
			if(hiahia){
				disk=fopen(argv[1],"r+");
				//printf("chekckk!!!!1111");
				InsertFile(hiahia, disk, FileSize, inputFile,file_stats);
		}else{
			printf("File not found.");
				exit(-1);
			}
		}
	}
	
	
}