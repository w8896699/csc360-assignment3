#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>  // mmap
#include <fcntl.h>  // open
#include <sys/stat.h> // fstat
int fileBlock =0;

void RemoveSpaces(char* source)
{
  char* i = source;
  char* j = source;
  while(*j != 0)
  {
    *i = *j++;
    if(*i != ' ')
      i++;
  }
  *i = 0;
}
void findFile(FILE* file,char* map,char* extractFile){
		
		int i,j;
		int current_file=19*512;		
		int status=0;
		//printf("check!!!I am here (1.5)");
		fseek(file, 19*512, SEEK_SET);
		fread(&status, 1, 1, file);
		int root=0;
		fseek(file, 22, SEEK_SET);
		fread(&root, 1, 4, file);
		root=htonl(root);
		while(status != 0x00&&status!=0xE5){
//printf("check!!!I am here (2)");
			int attributesvalue=0;
			fseek(file, current_file + 11, SEEK_SET);
			fread(&attributesvalue, 1, 1, file);
			// printf("wtf is attributesvalue:%d\n",attributesvalue);
			if ((attributesvalue & 0x0F) != 0x0F && (attributesvalue & 0x08) != 0x08){
				char *fileName = (char *)malloc(sizeof(char));
				fseek(file, current_file, SEEK_SET);
				fread(fileName, 1, 8, file);
				char *file_extension=malloc(sizeof(char) * 3);
				for (j = 0; j < 3; j ++) {
					if (!isspace(map[current_file + 8 + j])) {
					file_extension[j] = map[current_file + 8 + j];
					} else {
					file_extension[j] = '\0';
					break;
					}
				}
				strcat(fileName,".");
				strcat(fileName,file_extension);
				RemoveSpaces(fileName);
				char* newstr=strdup(extractFile);
				char* str=newstr;
				 while(*str){
					*str = toupper( *str );
					str ++;
				}
				//printf("fileName  is :%s!!\n",fileName);
				//printf("searching is :%s!!\n",newstr);
				int seewhatis=strcmp(fileName,newstr);
				//printf("what is it:%d\n",seewhatis);
				if(seewhatis==0){
					printf("File %s found\n", extractFile);
					int size=getsize(file, current_file);
					fseek(file, current_file, SEEK_SET);
					fread(&fileBlock, 1, 4, file);
					fileBlock=htonl(fileBlock);
					getFile(file, extractFile, current_file,size);
					return;
				}
				
			}
			current_file=current_file+32;
				 fseek(file, current_file, SEEK_SET);
				fread(&status, 1, 1, file);
	
	}
	printf("File Not found\n");
}

void getFile(FILE* file,char* filename,int current_file,int size){
	
	FILE* output = fopen(filename, "w");
		if (output == NULL) {
			fprintf(stderr, "Can't open output file %s!\n",filename);
			exit(1);
		}
	int temp;
	fseek(file, 14, SEEK_SET);
	fread(&temp, 1, 4, file);
	temp=htonl(temp);
	int fatStart = temp*512;
	int blockOffset=fatStart+(fileBlock*4);
	int dataOffset = fileBlock*512;
	while(size>512){
		//printf("I am here!123\n");
		size =size- 512;
		char buffer[4096]; // used to be DEFAULT_BLOCKSIZE

		fseek(file, dataOffset, SEEK_SET);
		fread(buffer, 1, 512, file);
		fwrite(buffer, 1, 512, output);
		//free(buffer);
		
		fseek(file, fileBlock, SEEK_SET);
		fread(&temp , 1, 4, file);
		temp=htonl(temp);
		fileBlock =temp;
		//blockOffset =fatStart+(fileBlock*4);
		dataOffset = fileBlock*512;
	}
	printf("I am here!\n");
	char buffer[4096]; // used to be DEFAULT_BLOCKSIZE

		fseek(file, dataOffset, SEEK_SET);
		fread(buffer, 1, 512, file);
		fwrite(buffer, 1, 512, output);
		//free(buffer);
}


int getsize(FILE* file,int current_file){
		int offset=current_file+28;
		fseek(file, offset, SEEK_SET);
		int tmp1 = 0;
		int tmp2 = 0;
		int tmp3 = 0;
		int tmp4 = 0;
		fread(&tmp1, 1, 1, file);
		fread(&tmp2, 1, 1, file);
		fread(&tmp3, 1, 1, file);
		fread(&tmp4, 1, 1, file);
		int size = tmp1 + (tmp2 << 8) + (tmp3 << 16) + (tmp4 << 24);
		return size;
		
	}
int main(int argc, char *argv[]) {
	int of=0;
	int i;
	struct stat file_stats;
	void *map;
	char* image_file_Name = malloc(sizeof(char)*8);
	char* diskLabel = malloc(4096);
	// memset(diskLabel, '\0', MAX_BUFFER_SIZE);
	 if( argc != 3 ){
        fprintf(stderr, "usage: ./diskLIST [disk].IMA selectfile\n");
        exit(-1);
    }
	//printf("check!!!2");
	char *image_file = argv[1];
	char* extractFile = argv[2];
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
	findFile(hiahia,map, extractFile);
	
}