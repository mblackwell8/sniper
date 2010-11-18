#import <stdio.h>

void main() {
    char alphabet[] = {'a','b','c','d','e','f','g','h','i'};
    char* spacedPtrs[] = {&alphabet[0], &alphabet[3], &alphabet[6]};

    int i, j;
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) printf("Letter %d is: %c\n", i * 3 + j, *(spacedPtrs[i] + j));
    }
}