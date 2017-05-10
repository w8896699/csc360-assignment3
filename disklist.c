#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>  // mmap
#include <fcntl.h>  // open
#include <sys/stat.h> // fstat

typedef struct{
	char *type;
	char *filename;
	int size;
	char *date;
	char *time;
	
}IMAinformation;

typedef IMAinformation *IMA;
IMA *filehia;
int getrootsfilenum(char *map) {
	int count = 0;
	int i,j;
	i=19;
	//19 is First sector in the floppy disk's root directory 
	for (i; i <= 32; i ++) {
		for (j = 0; j < 16; j++) {
			int attributes = map[11+(512*i) +(32*j)];
			if (map[(i * 512) + (j * 32)] == 0x00) {
				return count;
			}
			if ((attributes & 0x0F) != 0x0F && (attributes & 0x08) != 0x08 && (attributes & 0x10) != 0x10) {//not Subdirectory, Volume label then the attributes is a file
				count=count+1;
			}
		}
	}
	
	return count;
}
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
void listrootfile(char* map,FILE* file,int rootsfilenum){
	
	int i,j;
	int count = 0;
	int current_file=19*512;		
			int status=0;
			//printf("check!!!I am here (1.5)");
			fseek(file, 19*512, SEEK_SET);
			fread(&status, 1, 1, file);
			while(status != 0x00&&status!=0xE5){
//printf("check!!!I am here (2)");
			int attributesvalue=0;
			fseek(file, current_file + 11, SEEK_SET);
			fread(&attributesvalue, 1, 1, file);
			// printf("wtf is attributesvalue:%d\n",attributesvalue);
			filehia[count]=malloc(sizeof(IMA));
			if ((attributesvalue & 0x0F) != 0x0F && (attributesvalue & 0x08) != 0x08){
				filehia[count]->type=malloc(sizeof(char) * 2);
					filehia[count]->type="F ";
					
				if(attributesvalue & 0x10){
					filehia[count]->type="D ";
				}
				printf("%1s ", filehia[count]->type);
				filehia[count]->size=getsize(file, current_file);//Read in 4 bytes
				printf("%d ", filehia[count]->size);
				filehia[count]->filename=malloc(sizeof(char));
				//memset(filehia[count]->filename, '\0', 4096);
				fseek(file, current_file, SEEK_SET);
				fread(filehia[count]->filename, 1, 8, file);
				//strcat(filehia[count]->filename,".");
				char *file_extension=malloc(sizeof(char) * 3);
				for (j = 0; j < 3; j ++) {
					if (!isspace(map[current_file + 8 + j])) {
					file_extension[j] = map[current_file + 8 + j];
				} else {
					file_extension[j] = '\0';
					break;
				}
	}
				//strcat(filehia[count]->filename,file_extension);
				strcat(filehia[count]->filename,".");
				strcat(filehia[count]->filename,file_extension);
				RemoveSpaces(filehia[count]->filename);
				printf("%10s ", filehia[count]->filename);
				
				filehia[count]->date = malloc(sizeof(char) * 10);
				int tmpdate=getdateandtime(file, current_file,16);//Read in 2 bytes
				int year = ((tmpdate & 0xFE00) >>  0x09) + 1980;
				int month = (tmpdate & 0x01E0) >> 0x05;
				int day = (tmpdate & 0x1F);
				sprintf(filehia[count]->date, "%d/%02d/%02d", day, month, year);
				printf("%s ", filehia[count]->date);
				filehia[count]->time = malloc(sizeof(char));
				int tmptime=getdateandtime(file, current_file,14);
				int hour = ((tmptime & 0xF800) >>  0x0B);
				int min = (tmptime & 0x07E0) >> 0x05;
				int sec = (tmptime & 31)+(tmptime & 31);
				sprintf(filehia[count]->time, "%02d:%02d:%02d", hour, min, sec);
				printf("%s ", filehia[count]->time);
				 count++;
				// printf("wtf is count:%d\n",count);
				}
				current_file=current_file+32;
				 fseek(file, current_file, SEEK_SET);
				fread(&status, 1, 1, file);
				printf("\n");
		}
	}	
	int getdateandtime(FILE* file,int current_file,int pos){
		int offset=current_file+pos;
		fseek(file, offset, SEEK_SET);
		int tmp1 = 0;
		int tmp2 = 0;
		fread(&tmp1, 1, 1, file);
		fread(&tmp2, 1, 1, file);
		int date = tmp1 + (tmp2 << 8);
		return date;
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
	//for (i = 0; i < rootsfilenum; i ++) {
	//	printf("Type is  %s\n", filehia[0]->type);
		//printf("Type is  %s\n", filehia[1]->type);
		
	//	}
int main(int argc, char *argv[]) {
	int of=0;
	int i;
	struct stat file_stats;
	void *map;
	char* image_file_Name = malloc(sizeof(char)*8);
	char* diskLabel = malloc(4096);
	// memset(diskLabel, '\0', MAX_BUFFER_SIZE);
	 if( argc != 2 ){
        fprintf(stderr, "usage: ./diskLIST [disk].IMA\n");
        exit(-1);
    }
	//printf("check!!!2");
	char *image_file = argv[1];
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
		int rootsfilenum=0;
		rootsfilenum=getrootsfilenum(map);
		//printf("Number of files in the root directory: %d\n", rootsfilenum);
		filehia=malloc(sizeof(IMA)*rootsfilenum);
		listrootfile(map,hiahia,rootsfilenum);
	//	for (i = 0; i < rootsfilenum; i ++) {
		//printf("Type is  %s\n", filehia[i]->type);
		//}
		
		
}