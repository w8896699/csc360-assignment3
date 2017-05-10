/*
 * Assignment 3
 *
 * Name: Nick Warwick
 * Student Number: V00773474
 */

#include "functions.h"

int fileSize, numFileBlocks;
struct tm fileModDate, fileCreateDate;
char *fileName;

// get the size of a file
int getFileSize(FILE *file){
	fseek(file, 0, SEEK_END);
	return ftell(file);
}

// Get the total number of blocks for a file
int getNumBlocks(int size){ 
	return (size+DEFAULT_BLOCK_SIZE-1)/DEFAULT_BLOCK_SIZE; // Take the ceiling
}

// Return current date
struct tm getCurrentTime(){
	time_t now = time(NULL);
  	return *localtime(&now);
}

int findFreeRootSpace(FILE *fp){ // Searches for a free entry in root dir, returns its offset if found
	int i;
	int rootStart = getRootDirStart(fp);
	int numBlocks = getRootDirBlocks(fp);
	int numEntries = numBlocks*8; // 8 dir entries per block
	int offset = rootStart*DEFAULT_BLOCK_SIZE;

	for(i=0;i<numEntries;i++){ // for each entry, check if it is free
		if(getEntStatus(fp, offset) == DIRECTORY_ENTRY_FREE){
			//printf("Found free root spot at offset: %d\n", offset);
			return offset;
		}
		offset+=DIRECTORY_ENTRY_SIZE;
	}
	printf("No free space in root directory\n");
	return -1;
}

int findFreeFATEntry(FILE *fp){ // Searches for a free entry in the FAT, returns it if found
	int i;
	int fatStart = getFATStart(fp);
	int offset = fatStart*DEFAULT_BLOCK_SIZE;
	int numFATBlocks = getFATBlocks(fp);
	int numEntries = (FAT_ENTRY_PER_BLOCK * numFATBlocks);

	for(i=0;i<numEntries;i++){
		if(getBlock(fp, offset) == FAT_FREE){
			//printf("Found free FAT entry: %d\n", i);
			return i;
		}
		offset+=FAT_ENTRY_SIZE;
	}
	printf("No free space in FAT table\n");
	return -1;
}

// fread returns the num of bytes read, use that to calc file size 
void copyFile(FILE *fp, FILE *newFile){
	int rootOffset = findFreeRootSpace(fp);
	int fileBlock = findFreeFATEntry(fp); // Set start block as first free entry in FAT table
	
	int fatStart, fatOffset, dataOffset, fileOffset; 
	if(rootOffset == -1){ // Check if there is free space in root directory
		return;
	}

	if(fileBlock == -1){ // Chek if there is free space in the FAT table
		return;
	}

	setEntStatusFile(fp, rootOffset);
	setEntStartBlock(fp, rootOffset, fileBlock); // Set start block val in root dir
	setEntNumBlocks(fp, rootOffset, numFileBlocks);
	setEntFileSize(fp, rootOffset, fileSize);
	setEntCreateDate(fp, rootOffset, fileCreateDate);
	setEntModDate(fp, rootOffset, fileModDate);
	setEntName(fp, rootOffset, fileName);

	fatStart = getFATStart(fp)*DEFAULT_BLOCK_SIZE; // FAT start (in bytes)
	fatOffset = fatStart+(fileBlock*FAT_ENTRY_SIZE); // offset for blocks in the FAT
	dataOffset = fileBlock*DEFAULT_BLOCK_SIZE; // offset for data in the data area
	fileOffset = 0;

	while(fileSize>DEFAULT_BLOCK_SIZE){ // Write one block at a time
		fileSize -= DEFAULT_BLOCK_SIZE;
		//printf("block = %d\n", fileBlock);
		setFATBlock(fp, fatOffset, fileBlock); // Mark fat entry as used (with its own entry num)
		writeFileData(fp, newFile, dataOffset, fileOffset, DEFAULT_BLOCK_SIZE);
		fileBlock = findFreeFATEntry(fp); // Get the next free fat entry
		setFATBlock(fp, fatOffset, fileBlock); // set the next entry as the value at the current offset
		fatOffset = fatStart+(fileBlock*FAT_ENTRY_SIZE); // Update offset to next FAT entry
		dataOffset = fileBlock*DEFAULT_BLOCK_SIZE; // Update offset to next block in Data area
		fileOffset += DEFAULT_BLOCK_SIZE;
	}
	setFATBlock(fp, fatOffset, FAT_EOF); // We're done! Set as end of file
	writeFileData(fp, newFile, dataOffset, fileOffset, fileSize);
}


int main(int argc, char** argv){
	FILE *fp;
	FILE *newFile;

	if((fp=fopen(argv[1], "r+"))){
		if((newFile=fopen(argv[2], "r"))){ // If the file is found, copy it
			fileName = argv[2]; // Get file name
			fileSize = getFileSize(newFile); // Get file size
			numFileBlocks = getNumBlocks(fileSize); // Get number of blocks in file
			fileModDate = getCurrentTime();
			fileCreateDate = getCurrentTime();
			
			copyFile(fp, newFile);

			fclose(newFile);
		}else{
			printf("Cannot find %s\n", argv[2]);
		}

		fclose(fp);
	}else{
		printf("Cannot find %s\n", argv[1]);
	}
	return 0;
}