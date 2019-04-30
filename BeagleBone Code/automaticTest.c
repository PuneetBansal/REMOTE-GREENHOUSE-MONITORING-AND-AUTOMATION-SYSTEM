#include <stdio.h>

int main()
{
    FILE * check;
    check = fopen("automatic.txt","w");
    fprintf(check,"Automatic test running\n");
    fclose(check);
    return 0;
}