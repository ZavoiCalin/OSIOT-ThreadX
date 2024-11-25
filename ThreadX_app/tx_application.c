// tx_application.c

#include "tx_api.h"
#include "tx_application.h"

// Define stack sizes and other constants
#define STACK_SIZE 1024
#define NUM_THREADS 5

TX_THREAD Urgent;
TX_THREAD Routine;
TX_BYTE_POOL my_byte_pool;
TX_MUTEX Processor;

// Threads declarations
void Urgent_entry(ULONG thread_input);
void Routine_entry(ULONG thread_input);

void tx_application_define(void* first_unused_memory) {
    CHAR* Urgent_stack_ptr;
    CHAR* Routine_stack_ptr;

    // Create byte pool
    tx_byte_pool_create(&my_byte_pool, "my_byte_pool", first_unused_memory, BYTE_POOL_SIZE);

    // Urgent thread creation
    tx_byte_allocate(&my_byte_pool, (VOID**)&Urgent_stack_ptr, STACK_SIZE, TX_NO_WAIT);
    tx_thread_create(&Urgent, "Urgent", Urgent_entry, 0, Urgent_stack_ptr, STACK_SIZE, 5, 5,
                     TX_NO_TIME_SLICE, TX_AUTO_START);

    // Routine thread creation
    tx_byte_allocate(&my_byte_pool, (VOID**)&Routine_stack_ptr, STACK_SIZE, TX_NO_WAIT);
    tx_thread_create(&Routine, "Routine", Routine_entry, 1, Routine_stack_ptr, STACK_SIZE, 15, 15,
                     TX_NO_TIME_SLICE, TX_AUTO_START);

    // Create mutex for processor
    tx_mutex_create(&Processor, "Processor", TX_NO_INHERIT);
}

// Urgent thread function
void Urgent_entry(ULONG thread_input) {
    while (1) {
        printf("Urgent thread running...\n");
        tx_thread_sleep(100);  // Simulate workload
    }
}

// Routine thread function
void Routine_entry(ULONG thread_input) {
    while (1) {
        printf("Routine thread running...\n");
        tx_thread_sleep(100);  // Simulate workload
    }
}
