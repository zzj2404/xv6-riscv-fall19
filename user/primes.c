#include "kernel/types.h"
#include "user.h"

int func(int* numbers, int size){
    int p[2];
    pipe(p);
    int prime = *numbers;
    if (fork() == 0){
        printf("prime %d\n",prime);
        close(p[0]);//子进程关闭读端
        for (int i = 1;i < size;i++)
            write(p[1],&numbers[i],sizeof(int));
    }
    else{
        close(p[1]);//父进程关闭写端
        int* buf = (int*)malloc(sizeof(int));
        int count = 0;
        while (read(p[0],buf,sizeof(int)))
            if (*buf % prime)
                numbers[count++] = *buf;
        if (count > 0)
            func(numbers, count);
    }
    exit();
}

int main(){
    int numbers[34];
    for (int i= 2;i<=35;i++)
        numbers[i-2] = i;
    func(numbers, 34);
    exit();
}