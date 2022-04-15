#include<stdlib.h>
#include<stdio.h>
#include<string.h>

#define BUFFERSIZE 256
/* 0代表相同，1代表不相同，-1代表文件打开失败*/
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
int main(){
    const char* file1 ="./sort.txt";
    const char* file2 ="./sorted.txt";
    check(file1,file2);
}