//İLayda Özel 260201037
#include <stdio.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>

//define variables of the circle
#define CENTER_X 0
#define CENTER_Y 0
#define RADIUS 1

float inside_circle_num;
float outside_circle_num;
sem_t mutex;

typedef struct toss{
    int toss_id;
    float x;
    float y;
}toss;


int check_inside_circle(float x, float y){
    int distance = (x- CENTER_X)*(x-CENTER_X) + (y-CENTER_Y)*(y-CENTER_Y); //calculate the distance between the center of the circle and the point
    if(distance < (RADIUS*RADIUS)){  //use pysagor theorem(x^2= a^2+b^2) checking if the distance is less than the radius, if less then it is inside the circle
        return 1;
    }
    else{
        return 0;
    }

}
void * generate_toss(void * toss_amount){

    int toss_num= *((int *) toss_amount); //cast void pointer to integer
    struct toss toss_arr[toss_num]; //create struct array

    for(int i =0; i < toss_num ; i++){
        float scale_x =  (float)rand() / (float) RAND_MAX;
        float scale_y =  (float)rand() / (float) RAND_MAX;
        //creating points and putting them in toss array
        toss_arr[i].toss_id=i;
        toss_arr[i].x= -1 + scale_x * ( 2.0 );  //generate random floats between -1 and 1
        toss_arr[i].y= -1 + scale_y * ( 2.0 );

        sem_wait(&mutex);  //lock the process of incrementing, since incrementing the same value with two or more threads simultaneously can result in wrong counts
        if(check_inside_circle(toss_arr[i].x, toss_arr[i].y)){ //checking if the point is inside the circle or not with a sub function
            inside_circle_num += 1.0; //increment if inside
        }
        else{
            outside_circle_num += 1.0; //increment if outside
        }
        sem_post(&mutex); //stop the semaphore
    }

}

float calculate_pi(float x, float y){ // calculate pi number with the formula 4*(amount of points inside circle/amount of all points)
    float num= x/ y;
    float pi = 4.0 * num;
    printf("Pi value: %f\n", pi);
    return pi;
}

int main(){
    srand((unsigned int)time(NULL));   // initialize random number generating function

    sem_init(&mutex, 0, 1); //initialize the semaphore
    int N;
    printf("Enter toss amount: "); //getting input of the number of tosses from user
    scanf("%d", &N);
    if (N>100000){
        printf("Enter a number less than 100000: \n" ); //if user enters a value greater than 100000, ask user to enter again
        scanf("%d", &N);
    }


    int thread_num;
    printf("Enter thread amount: ");  //getting input of the number of threads from user
    scanf("%d", &thread_num);
    if (thread_num > 100000){
        printf("Enter a number less than 100000: \n" );//if user enters a value greater than 100000, ask user to enter again
        scanf("%d", &thread_num);
    }
    int remainder= N%thread_num; //calculate remainder of division between toss amount and thread amount

    int toss_amount= N/thread_num; //calculate the value of toss amount that every thread should create

    pthread_t tid[thread_num]; //create thread array

    for (int i=0; i< thread_num; i++){
       if (remainder != 0){ //if remainder is not equal to 0, the first threads should generate 1 point more, until remainder is 0
            int rem_amount = toss_amount+1;
            pthread_create(&tid[i], NULL, generate_toss,  &rem_amount);//create point threads
            remainder --;
        }
        else{ //if remainder is 0, threads should generate as much as the divison of the all tosses and all threads
            pthread_create(&tid[i], NULL, generate_toss,  &toss_amount);// create point threads
        }
        pthread_join(tid[i], NULL);

    }

    printf("Points inside circle num %f\n", inside_circle_num);
    printf("Points outside circle amount %f\n", outside_circle_num);
    calculate_pi(inside_circle_num, (float)N);
}

