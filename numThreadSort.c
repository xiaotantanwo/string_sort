#include<stdio.h>
#include<pthread.h>
#include<limits.h>
#include<sys/time.h>
#include<stdlib.h>
#include<string.h>

#define NUM 80000000L

long nums[NUM];
long sNums[NUM];
pthread_barrier_t b;
pthread_mutex_t mutex;

int compare(const void* num1,const void* num2){
    long l1=*(long*)num1;
    long l2=*(long*)num2;
    return l1-l2;
}

typedef struct SortInfo_t{
    long startIndex;//数据起使下标
    long num;//要排序的数量
}SortInfo;

void* workThread(void* arg){
    pthread_mutex_lock(&mutex);
    SortInfo* sortInfo =(SortInfo*)arg;
    long index = sortInfo->startIndex;
    long num = sortInfo->num;
    qsort(&nums[index],num,sizeof(long),compare);
    pthread_mutex_unlock(&mutex);
    pthread_barrier_wait(&b);
    return (void*)0;
}

//threadNum是说明有多少个数组需要合并
void merge(SortInfo* sortInfos,size_t threadNum){
    long index[threadNum];
    memset(index,0,threadNum);
    long i,minIndex,sIndex,num;
    //记录索引信息
    for(i=0;i<threadNum;i++){
        index[i]=sortInfos[i].startIndex;
    }
    //遍历各个数据，已经比较过后，比较下标增加
    for(sIndex=0;sIndex<NUM;sIndex++){
        num=LONG_MAX;
        for(i=0;i<threadNum;i++){
            if(index[i]<(sortInfos[i].startIndex+sortInfos[i].num)&&(nums[index[i]]<num)){
                num=nums[index[i]];
                minIndex=i;
            }
        }
        sNums[sIndex] = nums[index[minIndex]];
        index[minIndex]++;
    }
}

int main(int argc,char* argv[]){
    unsigned long i;

    //记录耗费时间
    struct timeval start,end;
    long long startusec,endusec;
    double elapsed;

    int err;
    pthread_t tid;/*线程id*/

    long perThreadNum;
    long lastThreadNum;

    /*获取线程数量，默认为1*/
    size_t threadNum = 0;
    if(argc>1){
        threadNum = atoi(argv[1]);
    }
    if(0==threadNum){
        threadNum=1;
    }
    printf("thread num: %zu\n",threadNum);
    SortInfo *sortInfos = (SortInfo*)malloc(sizeof(SortInfo)*threadNum);
    memset(sortInfos,0,threadNum*sizeof(SortInfo));


    /*生成随机数组*/
    srand(time(NULL));
    for(i=0;i< NUM;i++){
        nums[i]=rand();
    }

    /*如果不能整除，调整最后一个线程处理的数据量*/
    long PER_THREAD_NUM = NUM/threadNum;
    if(0!=NUM%threadNum){
        perThreadNum=NUM/(threadNum-1);
        lastThreadNum = PER_THREAD_NUM%(threadNum-1);
    }else{
        perThreadNum = PER_THREAD_NUM;
        lastThreadNum  = PER_THREAD_NUM;
    }

    gettimeofday(&start,NULL);
    pthread_barrier_init(&b,NULL,threadNum+1);
    /*创建线程，并进行排序，传入要排序的部分*/
    for(i=0;i<threadNum;i++){
        sortInfos[i].startIndex = i*perThreadNum;
        sortInfos[i].num = perThreadNum;
        if(i==threadNum-1){
            sortInfos[i].num = lastThreadNum;
        }
        err = pthread_create(&tid,NULL,workThread,(void*)(&sortInfos[i]));
        if(0!=err){
            printf("create failed\n");
            free(sortInfos);
            return -1;
        }
    }
    pthread_barrier_wait(&b);
    pthread_barrier_destroy(&b);
    /*合并*/
    merge(&sortInfos[0],threadNum);
    gettimeofday(&end,NULL);
    startusec = start.tv_sec*1000000+ start.tv_usec;
    endusec = end.tv_sec*1000000+end.tv_usec;
    elapsed =(double)(endusec-startusec)/1000000.0;
    printf("time %f\n",elapsed);
    for(i=0;i<NUM;i++){
        //printf("%ld\n",sNums[i]);
    }
    free(sortInfos);
    return 0;
}