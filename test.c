#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(){
    char *input = "subscribe";
    char *command = strtok(input, " ");
    printf("%s\n", command);
}