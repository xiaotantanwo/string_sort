#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>

#define BUFFERSIZE 256
#define STRNUM 5000000

int compare(const void* str1,const void* str2){
    return strcmp(*(char**)str1,*(char**)str2);
}

int loadData(char**data,const char* file){
    int i = 0;
	char* buffer =(char*)malloc(sizeof(char)*BUFFERSIZE);
	FILE* fp = fopen(file, "r");
	if (fp == NULL) {
		printf("can't open file\n");
		return -1;
	}
	while (fscanf(fp,"%s\n",buffer)!= EOF && i < STRNUM) {
        //printf("%d:%s\n", i+1,buffer);
        strcpy_s(*(data+i),BUFFERSIZE,buffer);
		i++;
	}
	//printf("%d\n",i);
	fclose(fp);
    free(buffer);
	return 0;
}

int print2file(char** str, int dataNum,char* fileName) {
	FILE* fp = fopen(fileName, "w");
	if (fp == NULL) {
		printf("Create file failed.\n");
		return -1;
	}
	int i=0;
    for(i=0;i<dataNum;i++){
        fprintf(fp,"%s\n",str[i]);
    }
	fclose(fp);
    printf("file complete\n");
	return 0;
}

int main(){
    const char* file  = "C:\\Users\\86134\\Desktop\\5M_low.txt";
    const int dataNumber = 5000000;
    char** data = (char**)malloc(sizeof(char*)*dataNumber);
    for(int i = 0;i<dataNumber;i++){
        data[i] = (char*)malloc(sizeof(char)*256);
    }
    if (-1==loadData(data, file)) {
		return -1;
	}
    clock_t start = clock();
    qsort(data,dataNumber,sizeof(char*),compare);
    clock_t end = clock();
    printf("the time of sort is: %f\n",(double)(end-start)/1000);
    char* sortFile ="./sorted.txt";
    print2file(data,dataNumber,sortFile);
    for(int i=0;i<5000000;i++){
        free(data[i]);
    }
    free(data);
    printf("free succeed\n");
    return 0;
}