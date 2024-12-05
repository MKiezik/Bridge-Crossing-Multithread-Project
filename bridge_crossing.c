#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<semaphore.h>
#include<unistd.h>
#include<time.h>
#include<stdbool.h>

int number_of_vehicles;
int vehicles_town_A = 0;
int vehicles_town_B = 0;
int vehicles_queue_A = 0;
int vehicles_queue_B = 0;
pthread_mutex_t bridge;


struct thread_data {
    pthread_t thread_id;
    int vehicle_number;
    int town_flag;      //town A - 65; town B - 66
    bool queue_flag;

};

void *function(void *vehicles);
int change_town(char town);

int main (int argc, char *argv[]) {

    if(argv[1] == NULL) {
        printf("Error: Wrong number of arguments! Enter number of //vehicles\n");
        exit(EXIT_FAILURE);
    }
    int number_of_vehicles = atoi(argv[1]);

    printf("Number of vehicles: %d\n\n", number_of_vehicles);
    fflush(stdout);

    
    struct thread_data vehicles[number_of_vehicles];

    int k;
    k = pthread_mutex_init(&bridge,NULL);
    if(k==-1) {
        printf("\n Mutex initialization failed");
        exit(1);
    }

    srand(time(NULL));

    int queue_flag;

    for(int i=0; i<number_of_vehicles;i++) {
        vehicles[i].vehicle_number=i+1;
        vehicles[i].town_flag = (rand() % 2) +65;
        queue_flag = rand() % 2;
        
        if(vehicles[i].town_flag == 65) {
            
            if(queue_flag == 1) {
                vehicles_queue_A++;
                vehicles[i].queue_flag = true;
                printf("Queue A:\n");
                fflush(stdout);
            } else {
                vehicles_town_A++;
                vehicles[i].queue_flag = false;
                printf("Town A:\n");
                fflush(stdout);
            }
            
        } else if(vehicles[i].town_flag == 66) {
            
            if(queue_flag == 1) {
                vehicles_queue_B++;
                vehicles[i].queue_flag = true;
                printf("Queue B:\n");
                fflush(stdout);
            } else {
                vehicles_town_B++;
                vehicles[i].queue_flag = false;
                printf("Town B:\n");
                fflush(stdout);
            }
        }

        k = pthread_create(&(vehicles[i].thread_id), NULL, function, (void *)(vehicles +i));
        
        if(k!=0) {
            printf("\n Thread creation error \n");
            exit(1);
        }
        
    }
    printf("\n\n");
    fflush(stdout);


    for(int i = 0; i <number_of_vehicles; i++) {

       k = pthread_join(vehicles[i].thread_id, NULL);
       printf("k = %d", k);
       if(k!=0) {
            printf("\n Thread join failed \n");
            exit(1);
        }
        
    }

    for(int i=0;i<5;i++) {
        k=pthread_mutex_destroy(&bridge);
        if(k!=0) {
            printf("\n Mutex Destroyed \n");
            exit(1);
        }
    }
    return 0;
}

void *function(void *vehicles) {

    struct thread_data *current_data  = (struct thread_data*)vehicles;
    bool a;

    for(;;) {

        if(current_data->queue_flag==false) {
            usleep(1000000);
            a = !current_data->queue_flag;
            current_data->queue_flag = a;
            if(current_data->town_flag == 65) {
                vehicles_town_A--;
                vehicles_queue_A++;
            } else {
                vehicles_town_B--;
                vehicles_queue_B++;
            }
        }
        
        if(current_data->queue_flag==true) {
            pthread_mutex_lock(&bridge);
            
            if(current_data->town_flag == 65)
                vehicles_queue_A--;
            else 
                vehicles_queue_B--;
            fflush(stdout);
            a = !(current_data->queue_flag);
            current_data->queue_flag = a;
            fflush(stdout);
            usleep(100000);
            if(current_data->town_flag == 65) {
                printf("1. A-%d %d>>> [>> %d >>] <<<%d %d-B\n",vehicles_town_A, vehicles_queue_A, current_data->vehicle_number,vehicles_queue_B, vehicles_town_B);
                fflush(stdout);
            } else {
                printf("2. A-%d %d>>> [<< %d <<] <<<%d %d-B\n",vehicles_town_A, vehicles_queue_A, current_data->vehicle_number,vehicles_queue_B, vehicles_town_B);
                fflush(stdout);
            }
            current_data->town_flag = change_town(current_data->town_flag);
            if(current_data->town_flag == 65)
            vehicles_town_B++;
            else 
            vehicles_town_A++;
                
            pthread_mutex_unlock(&bridge);
                //printf("Queue: %d\n", current_data->queue_flag);
                //fflush(stdout);
            //usleep(10000);
            //fflush(stdout);
        }
    }
}

int change_town(char town) {

    if(town == 65)
        return  66;
    else
        return 65;
}