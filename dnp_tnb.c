                        //Tanenbaum's solution
//Compile:- gcc -pthread dnp_tnb.c -o dnptnb  (Please ignore warnings in compilation)
//Run:- ./dnptnb   
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

sem_t dmutex,forks[5];
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
   output(fstatus,1);
   printf("Free Forks = ");
   output(fstatus,0);
   printf("\n");
}

//left neighbour
int left(int i){
    return (i+1)%5;
}

//right neighbour
int right(int i){
    return (i+4)%5;
}

//test
void test(int i)
{
   if((pstatus[i] == 1)&&(pstatus[left(i)] != 2) && (pstatus[right(i)] != 2))
   {
      pstatus[i] = 2;
      fstatus[i] = 1;
      fstatus[(i+4)%5] = 1;
      sem_post(&forks[i]);
   }
   print();
}

//getting fork
void get_forks(int i)
{
   sem_wait(&dmutex);
     pstatus[i] = 1;
     test(i);
   sem_post(&dmutex);
   sem_wait(&forks[i]);
}

//putting fork
void put_forks(int i)
{
   sem_wait(&dmutex);
     pstatus[i] = 0;
     fstatus[i] = 0;
     fstatus[(i+4)%5] = 0;     
     test(right(i));
     test(left(i));
   sem_post(&dmutex);
}

//philosopher
void *philosopher(void *arg)
{
   int x = (int)arg;
   int ct = 3;
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
   for(int i=0; i < 5; i++)
      sem_init(&forks[i],0,0);
   for(int j=0; j < 5; j++){
       pthread_create(&phil[j],NULL,philosopher,(void *)j);
   }
   for(int k=0; k < 5; k++)
      pthread_join(phil[k],NULL);
 
   return 0;
}  
