#include <stdio.h>

void do_print(int num) {
    printf("num: %d\n", num);
}

void main() {
    printf("hello world\n");
    do_print(10);
}