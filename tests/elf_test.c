#include <stdio.h>
#include <stdlib.h>

int add(int x, int y)
{
    return x + y;
}

int main(int argc, char *argv[])
{
    int x = 10; 
    int y = 5; 

    printf("%d\n", add(x,y)); 
    return EXIT_SUCCESS;
}

