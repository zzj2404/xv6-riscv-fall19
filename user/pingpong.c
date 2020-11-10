#include "kernel/types.h"
#include "user.h"

int main(){
    int id;
    int p1[2];
    int p2[2];
    pipe(p1);
    pipe(p2);
    if ((id = fork()) == 0){
        char* buf_child = (char*)malloc(sizeof(char)*5);
        read(p1[0],buf_child,sizeof(char)*5);
        printf("%d: received %s\n",getpid(),buf_child);
        write(p2[1],"pong",sizeof(char)*5);
    }
    else{
        write(p1[1],"ping",sizeof(char)*5);
        char* buf_parent = (char*)malloc(sizeof(char)*5);
        read(p2[0],buf_parent,sizeof(char)*5);
        printf("%d: received %s\n",getpid(),buf_parent);
    }
    exit();
}