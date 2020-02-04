                        //Waiter solution
//Compile:- gcc -pthread dnp_wt.c -o dnpwt  (Please ignore warnings in compilation)
//Run:- ./dnpwt   
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

sem_t dmutex,wmutex;
int pstatus[5]={0,0,0,0,0},fstatus[5]={0,0,0,0,0},forks[5][2];

void output(int *ptr,int x)
{
    printf("[ ");
    for(int i=0; i < 5 ; i++){
       if(ptr[i] == x)
         printf("%d ",i+1);
    }
    printf("]  ");
}

void output1(int *ptr,int x)
{
    printf("[ ");
    for(int i=0; i < 5 ; i++){
       if(ptr[i] != x)
         printf("%d ",i+1);
    }
    printf("]  ");
}

void print()
{
   printf("Thinking = ");
   output(pstatus,0);
   printf("Hungry = ");
   output(pstatus,1);
   printf("Eating = ");
   output(pstatus,2);
   printf("Engaged Forks = ");
   output(fstatus,2);
   printf("Free Forks = ");
   output1(fstatus,2);
   printf("\n");
}

//right fork
int right(int i){
    return (i+4)%5;
}

//left fork
int left(int i){
    return i;
}

//waiter
void waiter(int p)
{ 
   int count = 0;
   while(count != 2){
     sem_wait(&dmutex);
     for(int i=0; i < 5; i++){
        if(fstatus[i] == 0){
          fstatus[i] = 1;
          forks[p][count] = i;
          count++;
          if(count == 2)
            break;
        }
     }
     sem_post(&dmutex);
   }    
} 

//getting fork
void get_forks(int i)
{
   sem_wait(&dmutex);
     pstatus[i] = 1;
   sem_post(&dmutex);

   sem_wait(&wmutex);
     waiter(i);
   sem_wait(&dmutex);
     pstatus[i] = 2;
     fstatus[forks[i][0]] = 2;
     fstatus[forks[i][1]] = 2;
     print();
   sem_post(&dmutex);
   sem_post(&wmutex);
}

//putting fork
void put_forks(int i)
{
   sem_wait(&dmutex);
   fstatus[forks[i][0]] = 0;
   fstatus[forks[i][1]] = 0;
   pstatus[i] = 0;
   print();
   sem_post(&dmutex);
}

//philosopher
void *philosopher(void *arg)
{
   int x = (int)arg;
   int ct = 3;
 //  sem_wait(&dmutex);
   //  print();
   //sem_post(&dmutex);
   while(ct--){
      get_forks(x);
      sleep(1);
      put_forks(x);
      sleep(0);
   }
}   

int main()
{
   pthread_t phil[5];
   sem_init(&dmutex,0,1);
   sem_init(&wmutex,0,1);
   for(int j=0; j < 5; j++){
       pthread_create(&phil[j],NULL,philosopher,(void *)j);
   }
   for(int k=0; k < 5; k++)
      pthread_join(phil[k],NULL);
 
   return 0;
}
