all: createDiskInfo createDiskList createDiskGet createDiskPut


createDiskInfo:
	gcc -Wall -g diskinfo.c -o diskinfo

createDiskList:
	gcc -Wall -g disklist.c -o disklist

createDiskGet:
	gcc -Wall -g diskget.c -o diskget

createDiskPut:
	gcc -Wall -g diskput.c -o diskput	