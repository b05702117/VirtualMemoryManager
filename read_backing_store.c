#include <stdio.h>
#include <stdlib.h>

#define BACKING_STORE_SIZE 65536  // Size of the backing store file in bytes

int main() {
    FILE* backing_store = fopen("BACKING_STORE.bin", "rb");
    if (backing_store == NULL) {
        printf("Failed to open the BACKING_STORE.bin file.\n");
        return 1;
    }

    // Read the content from the backing store
    unsigned char data[BACKING_STORE_SIZE];
    fread(data, sizeof(unsigned char), BACKING_STORE_SIZE, backing_store);

    // Process the data as needed
    for (int i = 0; i < 10; i++) {
        // Access the data at index i, e.g., print it
        printf("%02X ", data[i]);
        if ((i + 1) % 16 == 0) {
            printf("\n");
        }
    }

    // Close the backing store file
    fclose(backing_store);

    return 0;
}
