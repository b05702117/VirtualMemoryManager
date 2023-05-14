# Virtual Memory Simulation

This program simulates virtual memory using a page-based memory management system. It translates logical addresses to physical addresses using a page table and a Translation Look-aside Buffer (TLB). The program reads logical addresses from an input file, converts them to physical addresses, and retrieves the corresponding values from memory.

## Description

The program simulates a virtual memory system with the following components:

Page-based memory management: The virtual address space is divided into fixed-size pages, and physical memory is divided into frames of the same size. Each page is mapped to a frame using a page table.

* Page-based memory management: The virtual address space is divided into fixed-size pages, and physical memory is divided into frames of the same size. Each page is mapped to a frame using a page table.

* TLB: The TLB is a cache that stores recently accessed page-table entries to accelerate address translation.

* Backing Store: The backing store represents the secondary storage where the page data is stored when it is not present in physical memory.

The program reads logical addresses from an input file and performs the following steps for each address:

1. Extract the page number and page offset from the logical address.
2. Check if the TLB contains the page number. If found (TLB hit), retrieve the corresponding frame number.
3. If the TLB does not contain the page number (TLB miss), check if the page is present in physical memory using the page table.
    * If the page is not present (page fault), load the page from the backing store into a free frame in physical memory and update the page table.
    * If the page is present, retrieve the corresponding frame number from the page table.
4. Update the TLB with the page number and frame number.
5. Generate the physical address by combining the frame number and page offset.
6. Retrieve the value from memory using the physical address.

After processing all addresses, the program displays statistics including the page fault rate and TLB hit rate.

## Usage 

To run the program, use the following command:
```bash
./vm_manager addresses.txt
logical address: 32363, physical address: 107, value: 107
logical address: 1754, physical address: 474, value: 218
logical address: 50484, physical address: 564, value: 52
...
logical address: 56458, physical address: 33418, value: 138
logical address: 48325, physical address: 43973, value: 197
logical address: 15064, physical address: 6616, value: 216
Page fault rate: 25.30%
TLB hit rate: 7.00%
```

## Implementation Details

The program is implemented in C and consists of the following functions:

* `init`: Initializes the program by opening the backing storage file, input file, and initializing data structures.
* `close_files`: Closes the opened files.
* `get_logical_address`: Extracts the logical address from a value by masking the irrelevant bits.
* `get_page_number`: Extracts the page number from a logical address.
* `get_page_offset`: Extracts the page offset from a logical address.
* `get_physical_address`: Generates the physical address by combining the frame number and page offset.
* `select_victim_frame`: Selects a victim frame for replacement in case of a page fault.
* `handle_page_fault`: Handles a page fault by loading the page from the backing store and updating the page table.
* `translate_address`: Translates a logical address to a physical address using the TLB and page table.
* `tlb_lookup`: Looks up a page number in the TLB and retrieves the corresponding frame number.
* `tlb_update`: Updates the TLB with a page number and frame number.
* `display_statistic`: Displays the page fault rate and TLB hit rate.

The program also defines constants for page size, TLB size, memory size, and file names.

