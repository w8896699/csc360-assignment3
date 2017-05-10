#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include "Constants.h"


// Shared functions
#ifndef FUNCTIONS_H
#define FUNCTIONS_H

void getIdent(FILE *fp, char *ident);
int getBlockSize(FILE *fp);
int getBlockCount(FILE *fp);
int getFATStart(FILE *fp);
int getFATBlocks(FILE *fp);
int getBlock(FILE *fp, int offset);
int getRootDirStart(FILE *fp);
int getRootDirBlocks(FILE *fp);
int getEntStatus(FILE *fp, int offset);
void getFATInfo(FILE *fp);
int getEntStatus(FILE *fp, int offset);
int getEntStartBlock(FILE *fp, int offset);
int getEntFileSize(FILE *fp, int offset);
void getEntName(FILE *fp, int offset, char *entName);
void getEntModDate(FILE *fp, int offset, char *entName);

void setFATBlock(FILE *fp, int fatOffset, int block);
void setEntStatusFile(FILE *fp, int offset);
void setEntStartBlock(FILE *fp, int offset, int block);
void setEntNumBlocks(FILE *fp, int offset, int num);
void setEntFileSize(FILE *fp, int offset, int size);
void setEntCreateDate(FILE *fp, int offset, struct tm date);
void setEntModDate(FILE *fp, int offset, struct tm date);
void setEntName(FILE *fp, int offset, char *entName);

void writeFileData(FILE *fp, FILE *cpyFile, int dataOffset, int fileOffset, int size);
void copyFileData(FILE *fp,  FILE *newFile, int offset, int fileSize);

#endif
#include "functions.h"

//--------------FUNCTIONS FOR FS AND FAT INFO------------------
void getIdent(FILE *fp, char *ident){
	fseek(fp, IDENT_OFFSET, SEEK_SET);
	fread(ident, 1, IDENT_SIZE, fp);
	printf("File system identifier: %s\n", ident);
}

int getBlockSize(FILE *fp){
	int size;

	fseek(fp, BLOCKSIZE_OFFSET, SEEK_SET);
	fread(&size, 1, BLOCKSIZE_SIZE, fp);
	size = htons(size); // Convert from little endian to big endian (using short)

	return size;
}

int getBlockCount(FILE *fp){
	int count;

	fseek(fp, BLOCKCOUNT_OFFSET, SEEK_SET);
	fread(&count, 1, BLOCKCOUNT_SIZE, fp);
	count = htonl(count); // Convert from little endian to big endian (using long)

	return count;
}

int getFATStart(FILE *fp){
	int val;

	fseek(fp, FATSTART_OFFSET, SEEK_SET);
	fread(&val, 1, FATSTART_SIZE, fp);
	val = htonl(val); // Convert from little endian to big endian (using long)
	
	return val;
}

int getFATBlocks(FILE *fp){
	int count;

	fseek(fp, FATBLOCKS_OFFSET, SEEK_SET);
	fread(&count, 1, FATBLOCKS_SIZE, fp);
	count = htonl(count); // Convert from little endian to big endian (using long)
	
	return count;
}

int getRootDirStart(FILE *fp){
	int val;

	fseek(fp, ROOTDIRSTART_OFFSET, SEEK_SET);
	fread(&val, 1, ROOTDIRSTART_SIZE, fp);
	val = htonl(val); // Convert from little endian to big endian (using long)
	
	return val;
}

int getRootDirBlocks(FILE *fp){
	int count;

	fseek(fp, ROOTDIRBLOCKS_OFFSET, SEEK_SET);
	fread(&count, 1, ROOTDIRBLOCKS_SIZE, fp);
	count = htonl(count); // Convert from little endian to big endian (using long)
	
	return count;
}

//--------------FUNCTIONS FOR DIRECTORY ENTRIES------------------
int getEntStatus(FILE *fp, int offset){
	int val = 0; 
	offset+=DIRECTORY_STATUS_OFFSET;

	fseek(fp, offset, SEEK_SET);
	fread(&val, 1, ENT_STATUS_SIZE, fp);
	//val = htons(val); // Convert from little endian to big endian (using long)
	
	return val;
}

void setEntStatusFile(FILE *fp, int offset){
	int status = DIRECTORY_ENTRY_FILE+DIRECTORY_ENTRY_USED;
	offset+=DIRECTORY_STATUS_OFFSET;

	//status = htons(status);
	fseek(fp, offset, SEEK_SET);
	fwrite(&status, 1, ENT_STATUS_SIZE, fp);
}

int getEntStartBlock(FILE *fp, int offset){
	int val =0;
	offset+=DIRECTORY_START_BLOCK_OFFSET;

	fseek(fp, offset, SEEK_SET);
	fread(&val, 1, ENT_START_BLOCK_SIZE, fp);
	val = htonl(val);

	return val;
}

void setEntStartBlock(FILE *fp, int offset, int block){
	offset+=DIRECTORY_START_BLOCK_OFFSET;
	
	block = htonl(block);
	fseek(fp, offset, SEEK_SET);
	fwrite(&block, 1, ENT_START_BLOCK_SIZE, fp);

} 

void setEntNumBlocks(FILE *fp, int offset, int num){
	offset+=DIRECTORY_BLOCK_COUNT_OFFSET;
	
	num = htonl(num);
	fseek(fp, offset, SEEK_SET);
	fwrite(&num, 1, ENT_NUM_BLOCKS_SIZE, fp);
}

int getEntFileSize(FILE *fp, int offset){
	int size;
	offset+=DIRECTORY_FILE_SIZE_OFFSET;

	fseek(fp, offset, SEEK_SET);
	fread(&size, 1, ENT_FILESIZE_SIZE, fp);
	size = htonl(size); // Convert from little endian to big endian (using long)
	
	return size;
}

void setEntFileSize(FILE *fp, int offset, int size){
	offset+=DIRECTORY_FILE_SIZE_OFFSET;

	size = htonl(size);
	fseek(fp, offset, SEEK_SET);
	fwrite(&size, 1, ENT_FILESIZE_SIZE, fp);
}

void getEntName(FILE *fp, int offset, char *entName){
	offset+=DIRECTORY_FILENAME_OFFSET;

	fseek(fp, offset, SEEK_SET);
	fread(entName, 1, ENT_FILE_NAME_SIZE, fp);
}

void setEntName(FILE *fp, int offset, char *entName){
	offset+=DIRECTORY_FILENAME_OFFSET;
	
	fseek(fp, offset, SEEK_SET);
	//fwrite(entName, 1, strlen(entName), fp);
	fwrite(entName, 1, ENT_FILE_NAME_SIZE, fp);
}

void getEntModDate(FILE *fp, int offset, char *entModDate){
	int year=0, day=0, month=0, hour=0, min=0, sec=0;
	offset+=DIRECTORY_MODIFY_OFFSET;

	fseek(fp, offset, SEEK_SET);
	fread(&year, 1, 2, fp);
	fread(&month, 1, 1, fp);
	fread(&day, 1, 1, fp);
	fread(&hour, 1, 1, fp);
	fread(&min, 1, 1, fp);
	fread(&sec, 1, 1, fp);

	struct tm date;
	date.tm_year = htons(year)-1900;
	date.tm_mon = month-1;
	date.tm_mday = day;
	date.tm_hour = hour;
	date.tm_min = min;
	date.tm_sec = sec;

	strftime(entModDate, 31, "%Y/%m/%d %H:%M:%S", &date);
}

void setEntModDate(FILE *fp, int offset, struct tm date){
	int year=0, day=0, month=0, hour=0, min=0, sec=0;
	offset+=DIRECTORY_MODIFY_OFFSET;
	year = htons(date.tm_year+1900);
	month = date.tm_mon+1;
	day = date.tm_mday;
	hour = date.tm_hour;
	min = date.tm_min;
	sec = date.tm_sec;

	fseek(fp, offset, SEEK_SET);
	fwrite(&year, 1, 2, fp); // Write year
	fwrite(&month, 1, 1, fp);
	fwrite(&day, 1, 1, fp);
	fwrite(&hour, 1, 1, fp);
	fwrite(&min, 1, 1, fp);
	fwrite(&sec, 1, 1, fp);
}

void setEntCreateDate(FILE *fp, int offset, struct tm date){
	int year=0, day=0, month=0, hour=0, min=0, sec=0;
	offset+=DIRECTORY_CREATE_OFFSET;
	year = htons(date.tm_year+1900);
	month = date.tm_mon+1;
	day = date.tm_mday;
	hour = date.tm_hour;
	min = date.tm_min;
	sec = date.tm_sec;

	fseek(fp, offset, SEEK_SET);
	fwrite(&year, 1, 2, fp); // Write year
	fwrite(&month, 1, 1, fp);
	fwrite(&day, 1, 1, fp);
	fwrite(&hour, 1, 1, fp);
	fwrite(&min, 1, 1, fp);
	fwrite(&sec, 1, 1, fp);
}

//-------------- FAT FUNCTIONS ------------------

int getBlock(FILE *fp, int offset){ // Return the value of a block in the FAT e.g. RESERVED, EOF..
	int val=0;

	fseek(fp, offset, SEEK_SET);
	fread(&val, 1, FAT_ENTRY_SIZE, fp);
	val = htonl(val);

	return val;
}

void setFATBlock(FILE *fp, int fatOffset, int block){ // Set fat block at current offset to value of block
	fseek(fp, fatOffset, SEEK_SET);

	block = htonl(block);
	fwrite(&block, 1, FAT_ENTRY_SIZE, fp);
}

void writeFileData(FILE *fp, FILE *cpyFile, int dataOffset, int fileOffset, int size){
	char *data=malloc(sizeof(char)*size);
	
	fseek(cpyFile, fileOffset, SEEK_SET); // Read data from file
	fread(data, 1, size, cpyFile);

	fseek(fp, dataOffset, SEEK_SET); // Write data to file system
	fwrite(data, 1, size, fp);
	free(data);
}

//-----------------------------------------------

void copyFileData(FILE *fp,  FILE *newFile, int offset, int size){
	char *data=malloc(sizeof(char)*size); // used to be DEFAULT_BLOCKSIZE

	fseek(fp, offset, SEEK_SET);
	fread(data, 1, size, fp);
	fwrite(data, 1, size, newFile);
	free(data);
}