#include<stdlib.h>
#include<stdio.h>
#include<string.h>

#define bufferSize 256

typedef struct HeapStruct{
    int capacity;//最大元素数量
    int size;//堆元素数量
    char*** eles;
}PriorityQueue;

PriorityQueue* init_PQ(int maxEleNum){
    PriorityQueue *pq = NULL;
    if(maxEleNum<=0){
        return NULL;
    }
    pq=(PriorityQueue*)malloc(sizeof(PriorityQueue));
    if(NULL==pq){
        printf("malloc failed.");
        return NULL;
    }
    pq->eles = (char***)malloc((maxEleNum+1)*sizeof(char**));
    if(NULL==pq->eles){
        printf("malloc failed\n");
        free(pq);
        return NULL;
    }
    pq->capacity=maxEleNum;
    pq->size=0;
    return pq;
}
//1代表已经满了
int pq_is_full(PriorityQueue *pq){
    if(NULL==pq){
        return 0;
    }
    if(pq->capacity==pq->size){
        return 1;
    }
    else{
        return 0;
    }
}

int pq_is_empty(PriorityQueue*pq){
    if(NULL==pq){
        return 0;
    }
    if(0==pq->size){
        return 1;
    }
    else{
        return 0;
    }
}

int insert_pq(char** str,PriorityQueue*pq){
    int i=0;
    if(pq_is_full(pq)){
        printf("priorityQueue is full\n");
        return 0;
    }
    // printf("insert %s \n",*str);
    for(i=pq->size+1;strcmp(*(pq->eles[i/2]),*str)>0&&i>1;i/=2){
        pq->eles[i]=pq->eles[i/2];
    }
    pq->eles[i]=str;
    pq->size++;
    return 1;
}

int find_min(PriorityQueue* pq,char** str){
    if(pq_is_empty(pq)){
        printf("priorityQueue is empty\n");
        return 0;
    }
    str =pq->eles[1];
    return 1;
}

int delete_min(PriorityQueue* pq,char***min){
    int i=1;
    int minChild =0;
    if(pq_is_empty(pq))
    {
        printf("priorityqueue is empty\n");
        return 0;
    }
    
    *min = pq->eles[1];
    char** last = pq->eles[pq->size];
    pq->size--;
    if(0==pq->size){
        pq->eles[i]=NULL;
        return 1;
    }
    
    for(i=1;i*2<=pq->size;i=minChild){
        minChild=i*2;
        if(minChild!=pq->size&&strcmp(*(pq->eles[minChild+1]),*(pq->eles[minChild]))<0){
            minChild+=1;
        }
        if(strcmp(*last,*(pq->eles[minChild]))>0){
            pq->eles[i]=pq->eles[minChild];
        }
        else{
            break;
        }
    }
    pq->eles[i]=last;
    return 1;
}

int destory_pq(PriorityQueue *pq)
{
    if(NULL == pq)
    {
        return 0;
    }
    free(pq->eles);
    pq->eles = NULL;
    free(pq);
    pq = NULL;
    printf("destory pq success\n");
    return 1;
}

// int main(){
//     PriorityQueue *pq = init_PQ(1);
//     char** a =(char**)malloc(sizeof(char**)*5);
//     for(int i=0;i<5;i++){
//         a[i]=(char*)malloc(sizeof(char)*bufferSize);
//     }
//     a[0]="5NspDIQv1K37Z31483X8Kai4fq74c";
//     a[1]="779Y138lWCNDgjec3h3hkXY7l";
//     a[2]="fakdjfla;dfjadf";
//     a[3]="44kJsO1pXdUKVaLbTOVz6wu";
//     a[4]="fdafadfadfad";
//     char* b[5];
//     b[0]="5NspDIQ31483X8Kai4fq74c";
//     b[1]="779Y13ec3h3hkXY7l";
//     b[3]="44kdUKVaLbTOVz6wu";
//     b[4]="fdafadfadfad";
//     int i=0;
//     insert_pq(a,pq);
//      printf("the arr value is:\n");
//     for(i=0;i < pq->size;i++)
//     {
//         printf(" %s\n",*(pq->eles[i+1]));
//     }
//     printf("\n");
//     printf("pq size is %d\n",pq->size);
//     char** min;
//     int a_number=0;
//     int b_number=0;
//     int size = pq->size;
   
//     /*每次都从堆顶取元素*/
//      while(!pq_is_empty(pq)){
//          //printf("\n1\n");
//         if(1==delete_min(pq,&min)){
//             printf("the min is %s\n",*min);
//             if(a_number<5){
//                 insert_pq(min+1,pq);
//             }
//             a_number++; 
//         }
//     }
//     /*销毁优先队列*/
//     destory_pq(pq);
//     free(min);
//     return 0;
// }

