/******************************************************************************

                            Online C Compiler.
                Code, Compile, Run and Debug C program online.
Write your code in this editor and press "Run" button to compile and execute it.

*******************************************************************************/
#include <stdio.h>      
#include <stdlib.h>     
#include <pthread.h>    
#include <semaphore.h>   
#include <unistd.h>     
#include <time.h>       

#define MAX_WAITING 3 // Max
#define NUM_STUDENTS 5 // Number of students

sem_t sem_ta; // Semaphore for waking the TA
pthread_mutex_t mutex; // Mutex for protecting shared data
int waiting_students = 0; // Count of waiting students

//Student: This function is of a student who needs help from the TA.
void *student(void *id) {
    int student_id = *((int *)id);
    
    while (1) {
        // Simulate programming time
        sleep(rand() % 3 + 1);
        printf("Student %d needs help.\n", student_id);

        pthread_mutex_lock(&mutex); // Lock mutex
        if (waiting_students < MAX_WAITING) { // Check if room to wait
            waiting_students++;
            printf("Student %d is waiting. (Waiting students: %d)\n", student_id, waiting_students);
            pthread_mutex_unlock(&mutex); // Unlock mutex
            
            // Student waits for help
            sem_wait(&sem_ta); // Wait TA to be free
            
            // Student receives help
            printf("Student %d is receiving help.\n", student_id);
            sleep(rand() % 3 + 1); // Simulate help time
            
            pthread_mutex_lock(&mutex);
            waiting_students--; // Decrease wait count
            pthread_mutex_unlock(&mutex);
        } else {
            pthread_mutex_unlock(&mutex); // Unlock if no room to wait
            printf("Student %d found no chairs, going back to studying.\n", student_id);
        }
    }
}

//TA
void *teaching_assistant() {
    while (1) {
        printf("TA is napping.\n");
        sleep(rand() % 5 + 1); // Simulate napping time

        while (1) { // Inner loop for helping students
            pthread_mutex_lock(&mutex); // Lock to check waiting students
            if (waiting_students > 0) {
                printf("TA wakes up to help.\n");
                sem_post(&sem_ta); // Signal one waiting student
                waiting_students--; // Decrease count of waiting students
                pthread_mutex_unlock(&mutex); // Unlock before helping

                // Simulate helping time
                sleep(rand() % 2 + 1);
            } else {
                pthread_mutex_unlock(&mutex); // Unlock if no waiting students
                break; // No students waiting, exit inner loop and return to napping
            }
        }
    }
}

int main() {
    pthread_t ta_thread;
    pthread_t students[NUM_STUDENTS];
    int student_ids[NUM_STUDENTS];
    
    // Initialize semaphore and mutex
    sem_init(&sem_ta, 0, 0);
    pthread_mutex_init(&mutex, NULL);
    
    // Create the TA thread
    pthread_create(&ta_thread, NULL, teaching_assistant, NULL);
    
    // Create student threads
    for (int i = 0; i < NUM_STUDENTS; i++) {
        student_ids[i] = i + 1; // Assign student IDs
        pthread_create(&students[i], NULL, student, &student_ids[i]);
    }
    
    // Join threads
    pthread_join(ta_thread, NULL);
    for (int i = 0; i < NUM_STUDENTS; i++) {
        pthread_join(students[i], NULL);
    }
    
    // Cleanup
    sem_destroy(&sem_ta);
    pthread_mutex_destroy(&mutex);
    
    return 0;
}

