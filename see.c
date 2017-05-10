#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>  // mmap
#include <fcntl.h>  // open
#include <sys/stat.h> // fstat
#include <string.h> // strcat
#include <ctype.h> // isspace
#ifndef DISKLIST_H_INCLUDED
#define DISKLIST_H_INCLUDED

int get_bytes_per_sector(char *mmap);
int get_number_files_in_root(char *mmap);
void get_files_in_root(char *mmap, int num_files_in_root);
void get_file_type(char *mmap, char *file_type, int offset);
void get_file_name(char *mmap, char *file_name, int offset);
int get_file_size(char *mmap, int offset);
void get_file_creation_date(char *mmap, char *file_creation_date, int offset);
void get_file_creation_time(char *mmap, char *file_creation_date, int offset);
int get_two_byte_value(char *mmap, int offset);
int get_four_byte_value(char *mmap, int offset);

#endif

typedef struct {
	char *file_type;
	char *file_name;
	int file_size;
	char *file_creation_date;
	char *file_creation_time;
	
} file_struct;

typedef file_struct * file_struct_pointer;

file_struct_pointer *root_files;
	
int main(int argc, char *argv[])
{
	if(argc != 2)
	{
		fprintf(stderr, "Usage: disklist <file system image>\n");
		return -1;
	}
	
	char *file_system_image = argv[1];
	
	int fd;
	struct stat file_stats;
	char *map;
	
	if ((fd = open(file_system_image, O_RDONLY))) {
		// Return information about the file and store it in file_stats
		fstat(fd, &file_stats);
		
		// void * mmap(void * addr, size_t length, int prot, int flags, int fd, off_t offset);
		map = mmap(NULL, file_stats.st_size, PROT_READ, MAP_SHARED, fd, 0);
		
		// get the total number of files in the root directory
		int number_files_in_root = get_number_files_in_root(map);
		
		root_files = malloc(number_files_in_root * sizeof(file_struct_pointer));
		
		get_files_in_root(map, number_files_in_root);
		
		int i;
		for (i = 0; i < number_files_in_root; i ++) {
			printf("%1s %10d %20s %10s %5s\n", root_files[i]->file_type, root_files[i]->file_size, root_files[i]->file_name, root_files[i]->file_creation_date, root_files[i]->file_creation_time);
		}
		
	} else {
		perror("Error opening file for reading");
		exit(EXIT_FAILURE);
	}
	return 0;
}

int get_bytes_per_sector(char *mmap) {
	return get_two_byte_value(mmap, 11);
}

int get_number_files_in_root(char *mmap) {
	int bytes_per_sector = get_bytes_per_sector(mmap);
	int files = 0;
	int i;
	for (i = 19; i <= 32; i ++) {
		// Directory entries are 32 bytes long
		int j = 0;
		for (j = 0; j < 16; j++) {
			int attributeValue = mmap[(i * bytes_per_sector) + (j * 32) + 11];
			
			// If the first byte of the Filename field is 0x00, then this directory entry is free and all the
			// remaining directory entries in this directory are also free.
			if (mmap[(i * bytes_per_sector) + (j * 32)] == 0x00) {
				return files;
			}
			
			if ((attributeValue & 0x0F) != 0x0F && (attributeValue & 0x08) != 0x08 && (attributeValue & 0x10) != 0x10) {
				files ++;
			}
		}
	}
	
	return files;
}

void get_files_in_root(char *mmap, int num_files_in_root) {
	int bytes_per_sector = get_bytes_per_sector(mmap);
	int i;
	int index = 0;
	for (i = 19; i <= 32; i ++) {
		// Directory entries are 32 bytes long
		int j = 0;
		for (j = 0; j < 16; j++) {
			int offset = (i * bytes_per_sector) + (j * 32);
			int attributeValue = mmap[offset + 11];
			
			// If the first byte of the Filename field is 0x00, then this directory entry is free and all the
			// remaining directory entries in this directory are also free.
			if (mmap[(i * bytes_per_sector) + (j * 32)] == 0x00) {
				return;
			}
			
			if ((attributeValue & 0x0F) != 0x0F && (attributeValue & 0x08) != 0x08 && (attributeValue & 0x10) != 0x10) {
				root_files[index] = malloc(sizeof(file_struct));
				
				root_files[index]->file_name = malloc(sizeof(char) * 12);
				get_file_name(mmap, root_files[index]->file_name, offset);
				
				root_files[index]->file_type = malloc(sizeof(char) * 2);
				get_file_type(mmap, root_files[index]->file_type, offset);
				
				root_files[index]->file_size = get_file_size(mmap, offset);
				
				root_files[index]->file_creation_date = malloc(sizeof(char) * 10);
				get_file_creation_date(mmap, root_files[index]->file_creation_date, offset);
				
				root_files[index]->file_creation_time = malloc(sizeof(char) * 5);
				get_file_creation_time(mmap, root_files[index]->file_creation_time, offset);
				
				index ++;
			}
		}
	}
}

void get_file_type(char *mmap, char *file_type, int offset) {
	int attributeValue = mmap[offset + 11];
	
	//file_type = malloc(sizeof(char));
	file_type[0] = 'F';
	file_type[1] = '\0';
	if (attributeValue == 0x10) {
		file_type[0] = 'D';
	}
}

void get_file_name(char *mmap, char *file_name, int offset) {
	int i;
	char *temp_file_name = malloc(sizeof(char) * 8);
	for(i = 0; i < 8; i ++) {
		if (!isspace(mmap[offset + i])) {
			temp_file_name[i] = mmap[offset + i];
		} else {
			temp_file_name[i] = '\0';
			break;
		}
	}
	
	int j;
	char *temp_file_extension = malloc(sizeof(char) * 3);
	for (j = 0; j < 3; j ++) {
		if (!isspace(mmap[offset + 8 + j])) {
			temp_file_extension[j] = mmap[offset + 8 + j];
		} else {
			temp_file_extension[j] = '\0';
			break;
		}
	}
	
	strcpy(file_name, temp_file_name);
	strcat(file_name, ".");
	strcat(file_name, temp_file_extension);
	free(temp_file_name);
	free(temp_file_extension);
}

int get_file_size(char *mmap, int offset) {
	return get_four_byte_value(mmap, offset + 28);
}

void get_file_creation_date(char *mmap, char *file_creation_date, int offset) {
	int date = get_two_byte_value(mmap, offset + 16);
	printf("File creation date: %x\n", date);
	
	// day is the first five bits: 11111 binary = 31 decimal
	int day = date & 31;
	
	// month is the middle 4 bits. Shift them right until they are the low order bits. 1111 binary = 15 decimal.
	int month = (date >> 5) & 15;
	
	// year is the last 7 bits. Shift them right until they are the low order bits. 1111111 binary = 127
	// Since the year is based at 1980, we must also add 1980 to it.
	int year = ((date >> 9) & 127) + 1980;
	
	sprintf(file_creation_date, "%d-%02d-%02d", year, month, day);
}

void get_file_creation_time(char *mmap, char *file_creation_time, int offset) {
	int time = get_two_byte_value(mmap, offset + 14);
	
	// seconds is the first five bits: 11111 binary = 31 decimal. They are counted in two second intervals so we must multiply by 2.
	int seconds = (time & 31) * 2;
	
	// minutes is the middle 6 bits. Shift them right until they are the low order bits. 111111 binary = 63 decimal.
	int minutes = (time >> 5) & 63;
	
	// hours is the last 5 bits. Shift them right until they are the low order bits. 11111 binary = 31
	// Since the year is based at 1980, we must also add 1980 to it.
	int hours = (time >> 11) & 31;
	
	sprintf(file_creation_time, "%02d:%02d:%02d", hours, minutes, seconds);
}

int get_two_byte_value(char *mmap, int offset) {
	int *tmp1 = malloc(sizeof(int));
	int *tmp2 = malloc(sizeof(int));
	int retVal;
	
	* tmp1 = (unsigned char) mmap[offset];
	* tmp2 = (unsigned char) mmap[offset + 1];
	
	// Switch to Big Endian format
	retVal = *tmp1 + ((*tmp2) << 8);
	
	free(tmp1);
	free(tmp2);
	
	return retVal;
}

int get_four_byte_value(char *mmap, int offset) {
	int *tmp1 = malloc(sizeof(int));
	int *tmp2 = malloc(sizeof(int));
	int *tmp3 = malloc(sizeof(int));
	int *tmp4 = malloc(sizeof(int));
	int retVal;
	
	* tmp1 = (unsigned char) mmap[offset];
	* tmp2 = (unsigned char) mmap[offset + 1];
	* tmp3 = (unsigned char) mmap[offset + 2];
	* tmp4 = (unsigned char) mmap[offset + 3];
	
	// Switch to Big Endian format
	retVal = *tmp1 + ((*tmp4) << 16) + ((*tmp3) << 12) + ((*tmp2) << 8);
	
	free(tmp1);
	free(tmp2);
	free(tmp3);
	free(tmp4);
	
	return retVal;
}