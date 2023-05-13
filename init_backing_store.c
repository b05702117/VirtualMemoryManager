#include <stdio.h>
#include <stdlib.h>

#define BACKING_STORE_SIZE 65536  // Size of the backing store file in bytes

int main() {
    FILE* backing_store = fopen("BACKING_STORE.bin", "wb");
    if (backing_store == NULL) {
        printf("Failed to open the BACKING_STORE.bin file.\n");
        return 1;
    }

    unsigned char initial_data[BACKING_STORE_SIZE];
    for (int i = 0; i < BACKING_STORE_SIZE; i++) {
        initial_data[i] = i % 256;
    }

    // Write the initial data to the backing store
    fwrite(initial_data, sizeof(unsigned char), BACKING_STORE_SIZE, backing_store);

    fclose(backing_store);

    return 0;
}