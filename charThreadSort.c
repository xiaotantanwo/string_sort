#include<stdio.h>
#include<pthread.h>
#include<limits.h>
#include<sys/time.h>
#include<stdlib.h>
#include<string.h>


#define STRNUM 5000000L
#define BUFFERSIZE 256

pthread_barrier_t b;
pthread_mutex_t mutex;
char** data = NULL;
char** sData = NULL;
const char* file  = "C:\\Users\\86134\\Desktop\\5M_low.txt";

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

int compare(const void* elem1,const void* elem2){
    return strcmp(*(char**)elem1,*(char**)elem2);
}

typedef struct SortInfo_t{
    long startIndex;//数据起使下标
    long num;//要排序的数量
}SortInfo;

void* workThread(void* arg){
    //pthread_mutex_lock(&mutex);
    SortInfo* sortInfo = (SortInfo*)arg;
    long index =sortInfo->startIndex;
    long num = sortInfo->num;
    qsort(data+index,num,sizeof(char*),compare);
    //printf("last str is: %s\n",data[index+sortInfo->num-1]);
    //pthread_mutex_unlock(&mutex);
    pthread_barrier_wait(&b);
    return (void*)0;
}

//threadNum是说明有多少个数组要合并
void merge(SortInfo* sortInfos,size_t threadNum){
    long index[threadNum];
    memset(index,0,threadNum*sizeof(long));
    long i,minIndex,sIndex;
    char* max =(char*)malloc(sizeof(char)*BUFFERSIZE);
    for(i=0;i<BUFFERSIZE-1;i++){
        max[i]='z';
    }
    max[BUFFERSIZE-1]='\0';
    char* min = (char*)malloc(sizeof(char)*BUFFERSIZE);
    //记录索引信息
    for(i=0;i<threadNum;i++){
        index[i]=sortInfos[i].startIndex;
        //printf("index[%d]: %d\n",i,index[i]);
    }
    //遍历各个数据，已经比较过后，比较下标增加
    for(sIndex=0;sIndex<STRNUM;sIndex++){
        memcpy(min,max,BUFFERSIZE*sizeof(char));
        for(i=0;i<threadNum;i++){
            if(index[i]<(sortInfos[i].startIndex+sortInfos[i].num)&&(strcmp(min,data[index[i]])>0)){
                memcpy(min,data[index[i]],sizeof(char)*BUFFERSIZE);
                minIndex = i;
            }
        }
        memcpy(sData[sIndex],min,sizeof(char)*BUFFERSIZE);
        index[minIndex]++; 
    }
    free(max);
    free(min);
}

int print2file(char** str, int dataNum,const char* fileName) {
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

void check(const char* file1,const char* file2){
    FILE* fp1 = fopen(file1,"r");
    FILE* fp2 = fopen(file2,"r");
    if(NULL==fp1){
        printf("%s 打开失败",file1);
        return;
    }
    if(NULL==fp2){
        printf("%s 打开失败",file2);
        return;
    }
    char elem1[BUFFERSIZE];
    char elem2[BUFFERSIZE];
    while (fscanf(fp1,"%s\n",elem1)!= EOF &&fscanf(fp2,"%s\n",elem2)!=EOF){
        if(strcmp(elem1,elem2)!=0){
            printf("排序结果错误\n");
            fclose(fp1);
            fclose(fp2);
            return;
        }
    }
    printf("排序结果正确\n");
    fclose(fp1);
    fclose(fp2);
    return;
}

int main(int argc,char* argv[]){
    unsigned long i;
    data =(char**)malloc(sizeof(char*)*STRNUM);
    sData =(char**)malloc(sizeof(char*)*STRNUM);
    for(i=0;i<STRNUM;i++){
        *(data+i) = (char*)malloc(sizeof(char)*BUFFERSIZE);
        *(sData+i) = (char*)malloc(sizeof(char)*BUFFERSIZE);
    }
    /*加载数据*/
     if (-1==loadData(data, file)) {
		return -1;
	}
    //记录耗费时间
    struct timeval start,end;
    long long startusec,endusec;
    double elapsed;

    int err;
    pthread_t tid;/*线程id*/

    long perThreadNum;
    long lastThreadNum;

    size_t threadNum = 0;
    if(argc>1){
        threadNum = atoi(argv[1]);
    }
    if(0==threadNum){
        threadNum = 1;
    }
    printf("thread num: %zu\n",threadNum);
    SortInfo* sortInfos = (SortInfo*)malloc(sizeof(SortInfo)*threadNum);
    memset(sortInfos,0,threadNum*sizeof(SortInfo));


    /*如果不能整除，调整最后一个线程处理的数据量*/
    long PER_THREAD_NUM = STRNUM/threadNum;
    if(0!=STRNUM%threadNum){
        perThreadNum = PER_THREAD_NUM;
        lastThreadNum = STRNUM - (threadNum-1)*PER_THREAD_NUM;
    }
    else{
        perThreadNum = PER_THREAD_NUM;
        lastThreadNum = PER_THREAD_NUM;
    }

    gettimeofday(&start,NULL);
    pthread_barrier_init(&b,NULL,threadNum+1);
    /*创建线程，并进行排序，传入要排序的部分*/
    for(i=0;i<threadNum;i++){
        sortInfos[i].startIndex = i*perThreadNum;
        sortInfos[i].num = perThreadNum;
        if(i==threadNum-1){
            sortInfos[i].num =lastThreadNum;
        }

        //printf("sortInfos[%ld].startIndex: %ld\n",i,sortInfos[i].startIndex);
        //printf("sortInfos[%ld].num: %ld\n",i,sortInfos[i].num);

        err = pthread_create(&tid,NULL,workThread,(void*)(&sortInfos[i]));
        if(0!=err){
            printf("create failed\n");
            free(sortInfos);
            for(i=0;i<STRNUM;i++){
                free(*(data+i));
                free(*(sData+i));
            }
            free(data);
            free(sData);
            return -1;
        }
    }
    pthread_barrier_wait(&b);
    pthread_barrier_destroy(&b);
    /*合并*/
    merge(sortInfos,threadNum);
    gettimeofday(&end,NULL);
    startusec = start.tv_sec*1000000+ start.tv_usec;
    endusec = end.tv_sec*1000000+end.tv_usec;
    elapsed =(double)(endusec-startusec)/1000000.0;
    printf("time %f\n",elapsed);
    // for(i=0;i<STRNUM;i++){
    //     printf("%s\n",sData[i]);
    // }
    printf("%s\n",sData[STRNUM-1]);
    free(sortInfos);
    const char* sFile = "./threadSort.txt";
    print2file(sData,STRNUM,sFile);
    check(sFile,"./sort.txt");
    // for(i=0;i<STRNUM;i++){
    //     free(*(data+i));
    //     free(*(sData+i));
    // }
    // free(data);
    // free(sData);
    return 0;
}
