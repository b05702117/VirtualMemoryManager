#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define PAGE_BITS 8
#define OFFSET_BITS 8
#define FRAME_BITS 8

#define PAGE_SIZE (1 << OFFSET_BITS) // page size = 2^OFFSET_BITS
#define FRAME_SIZE PAGE_SIZE
#define TLB_SIZE 16

#define NUM_OF_PAGES (1 << PAGE_BITS)
#define NUM_OF_FRAMES (1 << FRAME_BITS)
#define MEMORY_SIZE (NUM_OF_FRAMES * FRAME_SIZE)
#define BACKING_STORE_SIZE MEMORY_SIZE

#define BACKING_STORAGE_FILE "BACKING_STORE.bin"

typedef struct {
    int valid; // Flag to indicate if the page is valid or not
    int frame; // Frame number where the page is soted in physical memory
} PageTableEntry;

typedef struct {
    int page;
    int frame;
} TLBEntry;

// Global variables
unsigned char memory[MEMORY_SIZE];
PageTableEntry page_table[NUM_OF_PAGES];
unsigned int next_avaliable_frame; // simulate a FIFO queue to maintain the free frames
TLBEntry tlb_table[TLB_SIZE];
unsigned int next_avaliable_tlb; // simulate a FIFO queue to maintain the TLB table 
FILE* backing_storage;
FILE* addresses_file;
int total_access, page_fault_cnt, tlb_miss_cnt;

int init(int argc, char* argv[]);
void close_files();
unsigned int get_logical_address(unsigned int value);
unsigned int get_page_number(unsigned int logical_address);
unsigned int get_page_offset(unsigned int logical_address);
unsigned int get_physical_address(unsigned int frame, unsigned int page_offset);
unsigned int translate_address(unsigned int logical_address);
unsigned int select_victim_frame();
void handle_page_fault(unsigned int page_number);
int tlb_lookup(unsigned int page_number, unsigned int* frame_number);
void tlb_update(unsigned int page_number, unsigned int frame_number);
void display_statistic();

int main(int argc, char* argv[]) {
    if (init(argc, argv) != 0) {
        close_files();
        return 0;
    }

    // read line in addresses.txt, and convert the logical address into physical address
    char line[8];
    while (fgets(line, 8, addresses_file)) {
        unsigned int logical_address, physical_address;
        int value; // value read from the memory
        sscanf(line, "%u", &logical_address);
        logical_address = get_logical_address(logical_address); // only reserve the rightmost PAGE_BITS + OFFSET_BITS ass the logical address
        physical_address = translate_address(logical_address);
        value = memory[physical_address]; // access the memory by physical address
        total_access++;
        printf("logical address: %u, physical address: %u, value: %d\n", logical_address, physical_address, value);
    }

    display_statistic();

    close_files();

    return 0;
}

int init(int argc, char* argv[]) {
    // check if the number of arguments is correct
    if (argc < 2) {
        printf("Usage: ./a.out <input file>\n");
        return -1;
    }

    // check if the BACKING_STORAGE_FILE can be opened
    backing_storage = fopen(BACKING_STORAGE_FILE, "rb");
    if (backing_storage == NULL) {
        printf("Unable to open the backing storage file: %s\n", BACKING_STORAGE_FILE);
        return -2;
    }

    // check if the addresses_file can be opened
    addresses_file = fopen(argv[1], "r");
    if (addresses_file == NULL) {
        printf("Unable to open the input file: %s\n", argv[1]);
        return -3;
    }

    // initialize page_table
    for (int i = 0; i < NUM_OF_PAGES; i++) {
        page_table[i].valid = 0;
        page_table[i].frame = -1;
    }

    // initialize tlb_table
    for (int i = 0; i < TLB_SIZE; i++) {
        tlb_table[i].page = -1;
        tlb_table[i].frame = -1;
    }

    return 0;
}

void close_files() {
    if (addresses_file) {
        fclose(addresses_file);
    }
    if (backing_storage) {
        fclose(backing_storage);
    }
}

unsigned int get_logical_address(unsigned int value) {
    return value & ((1 << (PAGE_BITS + OFFSET_BITS)) - 1);
}

unsigned int get_page_number(unsigned int logical_address) {
    return (logical_address & ((1 << (PAGE_BITS + OFFSET_BITS)) - (1 << OFFSET_BITS))) >> OFFSET_BITS;
}

unsigned int get_page_offset(unsigned int logical_address) {
    return logical_address & ((1 << OFFSET_BITS) - 1);
}

unsigned int get_physical_address(unsigned int frame, unsigned int page_offset) {
    return (frame << OFFSET_BITS) | page_offset;
}

unsigned int select_victim_frame() {
    if (next_avaliable_frame < NUM_OF_FRAMES) {
        return next_avaliable_frame++;
    }
    else {
        unsigned int victim_frame = (next_avaliable_frame++) % NUM_OF_FRAMES; // simulate a FIFO queue
        // find the victim page and swap out it
        for (int i = 0; i < NUM_OF_PAGES; i++) {
            if (page_table[i].frame == victim_frame) {
                // assume every frame is unmodified before we swap out the memory
                page_table[i].valid = 0;
            }
        }
        return victim_frame;
    }
}

void handle_page_fault(unsigned int page_number) {
    // assign a frame to the page and validate it 
    page_table[page_number].frame = select_victim_frame();
    page_table[page_number].valid = 1;
    // load the page from the backing store
    fseek(backing_storage, page_number * PAGE_SIZE, SEEK_SET); // seek to the offset in the backing store
    fread(memory + (page_table[page_number].frame * PAGE_SIZE), sizeof(unsigned char), PAGE_SIZE, backing_storage); // read the page from the backing store and load it into the memory 
    page_fault_cnt++;
}

unsigned int translate_address(unsigned int logical_address) {
    unsigned int page_number, page_offset, frame_number;

    page_number = get_page_number(logical_address);
    page_offset = get_page_offset(logical_address);

    if (!tlb_lookup(page_number, &frame_number)) { // TLB miss
        // Check if the page is in physical
        if (!page_table[page_number].valid) {
            // simulate the OS operation after page table send a trap to it
            // printf("handle page fault: %d\n", page_number);
            handle_page_fault(page_number);
        }
        frame_number = page_table[page_number].frame;
        tlb_update(page_number, frame_number);
    }

    return get_physical_address(frame_number, page_offset);
}

int tlb_lookup(unsigned int page_number, unsigned int* frame_number) {
    for (int i = 0; i < TLB_SIZE; i++) {
        if (tlb_table[i].page == page_number) {
            *frame_number = tlb_table[i].frame;
            return 1; // TLB hit
        }
    }
    tlb_miss_cnt++;
    return 0; // TLB miss
}

void tlb_update(unsigned int page_number, unsigned int frame_number) {
    // Replace the least recently used TLB
    int victim = next_avaliable_tlb % TLB_SIZE;
    next_avaliable_tlb = (next_avaliable_tlb + 1) % TLB_SIZE;
    tlb_table[victim].page = page_number;
    tlb_table[victim].frame = frame_number;
}

void display_statistic() {
    float page_fault_rate, tlb_hit_rate;
    
    page_fault_rate = ((float)page_fault_cnt / total_access) * 100;
    tlb_hit_rate = ((float)(total_access - tlb_miss_cnt) / total_access) * 100;

    // printf("total access: %d\n", total_access);
    // printf("page fault cnt: %d\n", page_fault_cnt);
    // printf("TLB miss cnt: %d\n", tlb_miss_cnt);
    printf("Page fault rate: %.2f%%\n", page_fault_rate);
    printf("TLB hit rate: %.2f%%\n", tlb_hit_rate);
}