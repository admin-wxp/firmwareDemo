#include<stdio.h>
int main(){
    int reg = 4321;
    reg |= (1<<26);
    printf("%d",reg);
    return 0;
} 