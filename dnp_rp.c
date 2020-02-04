                        //Restricting No. of Philosophers solution
//Compile:- gcc -pthread dnp_rp.c -o dnprp  (Please ignore warnings in compilation)
//Run:- ./dnprp   
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

sem_t dmutex,footman,forks[5];
int pstatus[5]={0,0,0,0,0},fstatus[5]={0,0,0,0,0};

void output(int *ptr,int x)
{
    printf("[ ");
    for(int i=0; i < 5 ; i++){
       if(ptr[i] == x)
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
   printf("Single Forks = ");
   output(fstatus,1);
   printf("Free Forks = ");
   output(fstatus,0);
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

//getting fork
void get_forks(int i)
{
   sem_wait(&dmutex);
     pstatus[i] = 1;
   sem_post(&dmutex);
   sem_wait(&footman);
   sem_wait(&forks[right(i)]);
   sem_wait(&dmutex);
     fstatus[right(i)] = 1;
   sem_post(&dmutex); 
   sem_wait(&forks[left(i)]);
   sem_wait(&dmutex);
     fstatus[right(i)] = 2;
     fstatus[left(i)] = 2;
     pstatus[i] = 2;
     print(); 
   sem_post(&dmutex);
}

//putting fork
void put_forks(int i)
{
   sem_wait(&dmutex);
   sem_post(&forks[right(i)]);
   fstatus[right(i)] = 0;
   sem_post(&forks[left(i)]);
   fstatus[left(i)] = 0;
   sem_post(&footman);
   pstatus[i] = 0;
   print();
   sem_post(&dmutex);
}

//philosopher
void *philosopher(void *arg)
{
   int x = (int)arg;
   int ct = 1;
   sem_wait(&dmutex);
     print();
   sem_post(&dmutex);
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
   sem_init(&footman,0,4);
   for(int i=0; i < 5; i++)
      sem_init(&forks[i],0,1);
   for(int j=0; j < 5; j++){
       pthread_create(&phil[j],NULL,philosopher,(void *)j);
   }
   for(int k=0; k < 5; k++)
      pthread_join(phil[k],NULL);
 
   return 0;
}
