#include "tx_api.h"
#include "tx_trace.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>  // Include the header for getcwd
#include <string.h>

//#include "azureiotsdk/azure-iot-sdk-c/umqtt/deps/c-utility/inc/azure_c_shared_utility/platform.h"
//#include "azure_c_shared_utility/strings.h"
//#include "azureiotsdk/azure-iot-sdk-c/umqtt/deps/c-utility/inc/azure_c_shared_utility/platform.h"
//#include "azureiotsdk/azure-iot-sdk-c/umqtt/deps/c-utility/inc/azure_c_shared_utility/threadapi.h"
//#include "azureiotsdk/azure-iot-sdk-c/umqtt/deps/c-utility/inc/azure_c_shared_utility/strings.h"

//#include "/workspaces/azure-rtos-learn-samples/courses/threadx/ProjectHelloWorld/mqtt/paho.mqtt.c/src/MQTTAsync.h"
//#include "/workspaces/azure-rtos-learn-samples/courses/threadx/ProjectHelloWorld/mqtt/paho.mqtt.c/src/MQTTClient.h"

#define STACK_SIZE          1024
#define BYTE_POOL_SIZE      9120
#define NUM_THREADS         5
#define TX_TRACE_H

void common_task_logic(ULONG thread_id, ULONG task_id);
 
TX_THREAD Urgent;
TX_THREAD Routine;
TX_THREAD High;
TX_MUTEX Processor;
int status;
TX_BYTE_POOL my_byte_pool;
 
TX_THREAD threads[NUM_THREADS];
UCHAR thread_stacks[NUM_THREADS][STACK_SIZE];
 
 
TX_QUEUE task_queue;
UCHAR task_queue_buffer[10 * sizeof(ULONG)];
 
void Urgent_entry(ULONG thread_input);
void Routine_entry(ULONG thread_input);
void high_priority_thread_entry(ULONG param);
void medium_priority_thread_entry(ULONG param);
void low_priority_thread_entry(ULONG param);
void other_threads_entry(ULONG param);
 
void tx_application_define(void* first_unused_memory);
FILE* trace_file; 

#define MAX_LINE_LENGTH 256

typedef struct {
    int id;
    int time;
    char priority[32];
} LogEntry;

LogEntry* parse_output_txt(const char* input_file, int* num_entries) {
    FILE* file = fopen(input_file, "r");
    if (!file) {
        perror("Error opening file");
        return NULL;
    }
 
    LogEntry* entries = NULL;
    int count = 0;
 
    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file) != NULL) {
        LogEntry entry;
 
        // Assuming a simple format: id 2,time 12,priority Urgent
        if (sscanf(line, "id %d,time %d,priority %s", &entry.id, &entry.time, entry.priority) == 3) {
            entries = realloc(entries, (count + 1) * sizeof(LogEntry));
            entries[count++] = entry;
        }
    }
 
    fclose(file);
 
    *num_entries = count;
    return entries;
}
 
void generate_tracex_format(const LogEntry* entries, int num_entries, const char* output_file) {
    FILE* file = fopen(output_file, "w");
    if (!file) {
        perror("Error opening file");
        return;
    }
 
    for (int i = 0; i < num_entries; ++i) {
        // Generate TraceX-compatible output
        fprintf(file, "id %d,time %d,priority %s\n", entries[i].id, entries[i].time, entries[i].priority);
    }
 
    fclose(file);
}

int main(void)
{
    // Print the current working directory
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        fprintf(stdout, "Current working directory: %s\n", cwd);
    } else {
        perror("getcwd() error");
    }

   // if (platform_init() != 0) {            }
//MQTTClient client;
//MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
//conn_opts.username = "HubZavBob/MyDevice/2018-06-30";
//conn_opts.password = "Test";

//MQTTClient_create(&client, "tcp://HubZavBob.azure-devices.net:1883", "MyDevice", MQTTCLIENT_PERSISTENCE_NONE, NULL);
//MQTTClient_connect(client, &conn_opts);

   // TX_TRACE_BUFFER_ENTRY trace_entry;
    VOID* trace_buffer_start;
    ULONG trace_buffer_size = 4096;  // Replace with the actual size of your trace buffer
    ULONG registry_entries = 128;   // Replace with the actual number of registry entries

    trace_buffer_start = malloc(trace_buffer_size);
    if (trace_buffer_start == NULL) {
        printf("Error: Unable to allocate memory for the trace buffer\n");
        return 1;  // Exit with an error code
    }

     // Open a file for writing TraceX events
    trace_file = fopen("/workspaces/azure-rtos-learn-samples/courses/threadx/ProjectHelloWorld/trace_output.txt", "w");
if (trace_file == NULL) {
   printf("Error opening trace output file");
    free(trace_buffer_start);
    return 1;
}

    fprintf(trace_file, "File opened successfully\n");


    
    // Set up TraceX to log to the file
//tx_trace_event_filter_set(TX_TRACE_ID_EVENTS, TX_TRUE);
//tx_trace_event_filter_set(TX_TRACE_ID_OBJECTS, TX_TRUE);

 //Enable TraceX
tx_trace_enable(trace_buffer_start, trace_buffer_size, registry_entries);

//TX_TRACE_IN_LINE_INSERT(TX_TRACE_EVENT_ID_STRING, "Thread created: HighPriorityThread",
//                        TX_POINTER_TO_ULONG_CONVERT(&threads[0]), 0, 0, TX_TRACE_ID_EVENTS);

// Retrieve and log the trace entry
//tx_trace_event_get(&trace_entry);
//fprintf(trace_file, "%s\n", trace_entry.tx_trace_buffer_entry_info);


    fflush(trace_file);  // Flush the buffer
   // fclose(trace_file);


    tx_kernel_enter();
 
    printf("ThreadX initialized\n");

    tx_trace_enable(trace_buffer_start, trace_buffer_size, registry_entries);  // Enable TraceX
 
    tx_application_define(NULL);
 
    printf("Application defined\n");

     tx_trace_user_event_insert(0, 1, 0, 0, 0);
 
    tx_thread_create(&threads[0], "HighPriorityThread", high_priority_thread_entry, 0,
                     thread_stacks[0], STACK_SIZE,
                     20, 20, TX_NO_TIME_SLICE,
                     TX_AUTO_START);


  TX_TRACE_IN_LINE_INSERT(TX_TRACE_EVENT_ID_STRING, "Thread created: HighPriorityThread",
                            TX_POINTER_TO_ULONG_CONVERT(&threads[0]), 0, 0, TX_TRACE_ID_EVENTS);

    tx_thread_create(&threads[1], "MediumPriorityThread", medium_priority_thread_entry, 0,
                     thread_stacks[1], STACK_SIZE,
                     18, 18, TX_NO_TIME_SLICE,
                     TX_AUTO_START);

      TX_TRACE_IN_LINE_INSERT(TX_TRACE_EVENT_ID_STRING, "Thread created: MediumPriorityThread",
                            TX_POINTER_TO_ULONG_CONVERT(&threads[1]), 0, 0, TX_TRACE_ID_EVENTS);                 
 
    tx_thread_create(&threads[2], "LowPriorityThread", low_priority_thread_entry, 0,
                     thread_stacks[2], STACK_SIZE,
                     16, 16, TX_NO_TIME_SLICE,
                     TX_AUTO_START);
 
    tx_thread_create(&threads[3], "OtherThread1", other_threads_entry, 0,
                     thread_stacks[3], STACK_SIZE,
                     14, 14, TX_NO_TIME_SLICE,
                     TX_AUTO_START);
 
    tx_thread_create(&threads[4], "OtherThread2", other_threads_entry, 0,
                     thread_stacks[4], STACK_SIZE,
                     12, 12, TX_NO_TIME_SLICE,
                     TX_AUTO_START);
 
    printf("Threads created\n");
 
   // tx_kernel_enter();

   // fclose(trace_file);

    free(trace_buffer_start);
      
    return 0;
}
 
void tx_application_define(void* first_unused_memory)
{
    
    CHAR* Urgent_stack_ptr;
    CHAR* Routine_stack_ptr;
    CHAR* High_stack_ptr;  // Change High to CHAR*

   TX_TRACE_INITIALIZE;

    tx_byte_pool_create(&my_byte_pool, "my_byte_pool",
                        first_unused_memory, BYTE_POOL_SIZE);

    tx_byte_allocate(&my_byte_pool, (VOID**)&Urgent_stack_ptr,
                     STACK_SIZE, TX_NO_WAIT);

    tx_thread_create(&Urgent, "Urgent",
                     Urgent_entry, 0,
                     Urgent_stack_ptr, STACK_SIZE, 5, 5,
                     TX_NO_TIME_SLICE, TX_AUTO_START);


    tx_byte_allocate(&my_byte_pool, (VOID**)&Routine_stack_ptr,
                     STACK_SIZE, TX_NO_WAIT);

    tx_thread_create(&Routine, "Routine",
                     Routine_entry, 1, Routine_stack_ptr,
                     STACK_SIZE, 15, 15,
                     TX_NO_TIME_SLICE, TX_AUTO_START);

    tx_mutex_create(&Processor, "Processor", TX_NO_INHERIT);

    // Allocate memory for the High thread stack
    tx_byte_allocate(&my_byte_pool, (VOID**)&High_stack_ptr,
                     STACK_SIZE, TX_NO_WAIT);

    // Create the High thread
    status = tx_thread_create(&High, "High",
                     high_priority_thread_entry, 0,
                     High_stack_ptr, STACK_SIZE, 22, 22,
                     TX_NO_TIME_SLICE, TX_AUTO_START);

                      printf("ThreadX initialized\n");

    if (status == 0)
    { printf("Thread created successfully!\n"); }
    else{
             printf("Thread creation failed! Error code: %d/n", status); }                  
 
 
    printf("Application defined\n");

    TX_TRACE_IN_LINE_INSERT(TX_TRACE_EVENT_ID_STRING, "Application initialized",
                            TX_NULL, 0, 0, TX_TRACE_ID_EVENTS);
 
    tx_thread_create(&threads[0], "HighPriorityThread", high_priority_thread_entry, 0,
                     thread_stacks[0], STACK_SIZE,
                     20, 20, TX_NO_TIME_SLICE,
                     TX_AUTO_START);
 
    tx_thread_create(&threads[1], "MediumPriorityThread", medium_priority_thread_entry, 0,
                     thread_stacks[1], STACK_SIZE,
                     18, 18, TX_NO_TIME_SLICE,
                     TX_AUTO_START);

    TX_TRACE_IN_LINE_INSERT(TX_TRACE_EVENT_ID_STRING, "Thread created: MediumPriorityThread",
                        TX_POINTER_TO_ULONG_CONVERT(&threads[1]), 0, 0, TX_TRACE_ID_EVENTS);                 
 
    tx_thread_create(&threads[2], "LowPriorityThread", low_priority_thread_entry, 0,
                     thread_stacks[2], STACK_SIZE,
                     16, 16, TX_NO_TIME_SLICE,
                     TX_AUTO_START);
 
    tx_thread_create(&threads[3], "OtherThread1", other_threads_entry, 0,
                     thread_stacks[3], STACK_SIZE,
                     14, 14, TX_NO_TIME_SLICE,
                     TX_AUTO_START);
 
    tx_thread_create(&threads[4], "OtherThread2", other_threads_entry, 0,
                     thread_stacks[4], STACK_SIZE,
                     12, 12, TX_NO_TIME_SLICE,
                     TX_AUTO_START);
 
    printf("Threads created\n");
 

    
}

 
void Urgent_entry(ULONG thread_input)
{
    ULONG task_id = 1; 
    ULONG current_time;
    ULONG start_time;
    ULONG end_time;
    ULONG period = 141;
 
    while (1)
    {
         start_time = tx_time_get();
        tx_thread_sleep(4);
        common_task_logic(1, task_id);
 
        tx_mutex_get(&Processor, TX_WAIT_FOREVER);
        tx_thread_sleep(8);
        tx_mutex_put(&Processor);
 
        current_time = tx_time_get();

        printf("id %lu,time %lu,priority Urgent\n", task_id, 
               current_time);

        task_id++;       

    TX_TRACE_IN_LINE_INSERT(TX_TRACE_EVENT_ID_STRING, "Urgent Thread: Some important event",task_id, current_time,"Urgent", TX_TRACE_ID_EVENTS);


         // Open a file for writing TraceX events
    trace_file = fopen("/workspaces/azure-rtos-learn-samples/courses/threadx/ProjectHelloWorld/trace_output.txt", "a+");
if (trace_file == NULL) {
   printf("Error opening trace output file");
   // return 1;
}
    fprintf(trace_file, "id %lu,time %lu,priority Urgent\n", task_id,
               current_time);
    //fprintf(trace_file, "Printed2");
    fflush(trace_file);  // Flush the buffer                               

 
        // Trace additional activity
 
        tx_thread_sleep(8);

 
        tx_thread_sleep(5);



        end_time = tx_time_get();

    // Calculate jitter (variation in execution time)
        ULONG execution_time = end_time - start_time;
        ULONG jitter = execution_time - period;
       // jitter/=10000000;
 
        // Print or log the jitter value
        printf("Urgent Priority-Jitter: %lu microseconds\n ", jitter);
 
         // Your thread sleep or wait logic goes here
        tx_thread_sleep(100); // Adjust the sleep time as needed
    }
}
 
void Routine_entry(ULONG thread_input)
{
    ULONG task_id = 1; 
    ULONG current_time;
     ULONG start_time;
    ULONG end_time;
    ULONG period = 128;

    while (1)
    {   
        start_time = tx_time_get();
        tx_thread_sleep(4);

        common_task_logic(3, task_id);
        
 
        // Additional activities for Routine thread
     //   printf("Routine Thread: Additional activity\n");
 
        current_time = tx_time_get();
         TX_TRACE_IN_LINE_INSERT(TX_TRACE_EVENT_ID_STRING, "Routine Thread: Some important event",task_id, current_time,"Routine", TX_TRACE_ID_EVENTS);

        printf("id %lu,time %lu,priority Routine\n", task_id,
               current_time);

        task_id++;       

        // Open a file for writing TraceX events
    trace_file = fopen("/workspaces/azure-rtos-learn-samples/courses/threadx/ProjectHelloWorld/trace_output.txt", "a+");
if (trace_file == NULL) {
   printf("Error opening trace output file");
   // return 1;
   
}
    fprintf(trace_file, "id %lu,time %lu,priority Routine\n", task_id, current_time);
    //fprintf(trace_file, "Printed2");
    fflush(trace_file);  // Flush the buffer       
   
   end_time = tx_time_get();

    // Calculate jitter (variation in execution time)
        ULONG execution_time = end_time - start_time;
        ULONG jitter = execution_time - period;
        //jitter/=10000000;
 
        // Print or log the jitter value
        printf("Routine Priority-Jitter: %lu microseconds\n ", jitter);
 
         // Your thread sleep or wait logic goes here
        tx_thread_sleep(100); // Adjust the sleep time as needed
          
    }
}
 
void high_priority_thread_entry(ULONG param)
{
    ULONG task_id = 1;
    ULONG current_time;
    ULONG start_time;
    ULONG end_time;
    ULONG period = 136;
 
   while (task_id <= 10)  // Stop at 10
    {
         start_time = tx_time_get();

        if (task_id % 2 != 0)
        {
            common_task_logic(2, task_id);
            tx_queue_send(&task_queue, &task_id, sizeof(ULONG));
        }
 
        task_id++;
 
        tx_thread_sleep(6);
         current_time = tx_time_get();

        // Additional activities for High Priority thread
TX_TRACE_IN_LINE_INSERT(TX_TRACE_EVENT_ID_STRING, "High Priority Thread: Additional activity", task_id, current_time,"High",TX_TRACE_ID_EVENTS);
        printf("id %lu,time %lu,priority High\n", task_id, current_time);
        tx_thread_sleep(8);


         // Open a file for writing TraceX events
    trace_file = fopen("/workspaces/azure-rtos-learn-samples/courses/threadx/ProjectHelloWorld/trace_output.txt", "a+");
if (trace_file == NULL) {
   printf("Error opening trace output file");
   // return 1;
}
    fprintf(trace_file, "id %lu,time %lu,priority High\n", task_id, current_time);
    //fprintf(trace_file, "Printed2");
    fflush(trace_file);  // Flush the buffer   


    end_time = tx_time_get();

    // Calculate jitter (variation in execution time)
        ULONG execution_time = end_time - start_time;
        ULONG jitter = execution_time - period;
 
        // Print or log the jitter value
        printf("High Priority-Jitter: %lu microseconds\n ", jitter);
 
         // Your thread sleep or wait logic goes here
        tx_thread_sleep(100); // Adjust the sleep time as needed

    }

}
 
void medium_priority_thread_entry(ULONG param)
{
    ULONG task_id = 2;  // Start with an even task_id for medium priority
    ULONG current_time;
    ULONG start_time;
    ULONG end_time;
    ULONG period = 120;

     while (task_id <= 8)  // Stop at 10
    {
        start_time = tx_time_get();

        common_task_logic(4, task_id);
        tx_queue_send(&task_queue, &task_id, sizeof(ULONG));

        task_id += 2;  // Increment by 2 for the next even task_id
        current_time = tx_time_get();
        // Additional activities for Medium Priority thread after reaching 10000

TX_TRACE_IN_LINE_INSERT(TX_TRACE_EVENT_ID_STRING, "Medium Priority Thread: Additional activity",task_id, current_time,"Medium",TX_TRACE_ID_EVENTS);

    printf("id %lu,time %lu,priority Medium\n", task_id, current_time);
        tx_thread_sleep(9);


            // Open a file for writing TraceX events
    trace_file = fopen("/workspaces/azure-rtos-learn-samples/courses/threadx/ProjectHelloWorld/trace_output.txt", "a+");
if (trace_file == NULL) {
   printf("Error opening trace output file");
   // return 1;
}
    fprintf(trace_file, "id %lu,time %lu,priority Medium\n", task_id, current_time);
    //fprintf(trace_file, "Printed2");
    fflush(trace_file);  // Flush the buffer     
      
    end_time = tx_time_get();

    // Calculate jitter (variation in execution time)
        ULONG execution_time = end_time - start_time;
        ULONG jitter = execution_time - period;
 
        // Print or log the jitter value
        printf("Medium Priority-Jitter: %lu microseconds\n ", jitter);
 
         // Your thread sleep or wait logic goes here
        tx_thread_sleep(100); // Adjust the sleep time as needed

    }
    
}

void low_priority_thread_entry(ULONG param)
{
   // ULONG received_task_id;
    ULONG task_id = 3;
    ULONG current_time;
     ULONG start_time;
    ULONG end_time;
     ULONG period = 103;

     while (task_id <= 10)  // Stop at 10
    {
        start_time = tx_time_get();

        tx_queue_receive(&task_queue, &task_id, TX_WAIT_FOREVER);

        common_task_logic(6, task_id);

        tx_thread_sleep(12);

        task_id++;  
        current_time = tx_time_get();

 TX_TRACE_IN_LINE_INSERT(TX_TRACE_EVENT_ID_STRING, "Low Priority Thread: Additional activity",task_id, current_time,"Low", TX_TRACE_ID_EVENTS);

        // Additional activities for Low Priority thread
        printf("id %lu,time %lu,priority Low\n", task_id, current_time);
        tx_thread_sleep(6);


        // Open a file for writing TraceX events
    trace_file = fopen("/workspaces/azure-rtos-learn-samples/courses/threadx/ProjectHelloWorld/trace_output.txt", "a+");
if (trace_file == NULL) {
   printf("Error opening trace output file");
   // return 1;
}
    fprintf(trace_file, "id %lu,time %lu,priority Low\n", task_id, current_time);
    //fprintf(trace_file, "Printed2");
    fflush(trace_file);  // Flush the buffer 

end_time = tx_time_get();

    // Calculate jitter (variation in execution time)
        ULONG execution_time = end_time - start_time;
        ULONG jitter = execution_time - period;
 
        // Print or log the jitter value
        printf("Low Priority-Jitter: %lu microseconds\n ", jitter);
 
         // Your thread sleep or wait logic goes here
        tx_thread_sleep(100); // Adjust the sleep time as needed

    }
}


 
void other_threads_entry(ULONG param)
{
    ULONG task_id = 4;
    ULONG current_time;
    ULONG start_time;
    ULONG end_time;
    ULONG period = 114;


     while (task_id <= 10)  // Stop at 10
    {
    start_time = tx_time_get();
        common_task_logic(5, task_id);
 
        tx_thread_sleep(18);
 
        task_id++;
        current_time = tx_time_get();

        TX_TRACE_IN_LINE_INSERT(TX_TRACE_EVENT_ID_STRING, "Other Threads: Additional activity",task_id, current_time,"Other", TX_TRACE_ID_EVENTS);
 
        // Additional activities for Other Threads
        printf("id %lu,time %lu,priority Medium\n", task_id, current_time);
        tx_thread_sleep(5);

    // Open a file for writing TraceX events
    trace_file = fopen("/workspaces/azure-rtos-learn-samples/courses/threadx/ProjectHelloWorld/trace_output.txt", "a+");
if (trace_file == NULL) {
   printf("Error opening trace output file");
   // return 1;
}
    fprintf(trace_file, "id %lu,time %lu,priority Medium\n", task_id, current_time);
    //fprintf(trace_file, "Printed2");
    fflush(trace_file);  // Flush the buffer 

       end_time = tx_time_get();

    // Calculate jitter (variation in execution time)
        ULONG execution_time = end_time - start_time;
        ULONG jitter = execution_time - period;
 
        // Print or log the jitter value
        printf("Other Priority-Jitter: %lu microseconds\n ", jitter);
 
         // Your thread sleep or wait logic goes here
        tx_thread_sleep(100); // Adjust the sleep time as needed 
    }
}
 
void common_task_logic(ULONG thread_id, ULONG task_id)
{
    tx_thread_sleep(500);
}