                          //Chandy Misra solution
//Compile:- gcc -pthread dnp_cm.c -o dnpcm  (Please ignore warnings in compilation)
//Run:- ./dnpcm    
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

sem_t dmutex;
int pstatus[5]={0,0,0,0,0},fstatus[5]={0,0,0,0,0},fown[5]={0,1,2,3,0};
int fst[5]={0,0,0,0,0},req[5]={0,0,0,0,0};  //for engaged forks,free forks,single forks

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
   output(fst,2);
   printf("Requested Forks = ");
   output(req,1);
   printf("Single Forks = ");
   output(fst,1);
   printf("Free Forks = ");
   output(fst,0);
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

//Request Fork
void request(int id,int fk)
{
   sem_wait(&dmutex);
     req[fk]++; 
     if(fown[fk]==id){
       req[fk]--; 
       fst[fk] = 1;
     }
   sem_post(&dmutex);

   while(fown[fk]!=id){
      sem_wait(&dmutex);
      if((pstatus[fown[fk]]!=2)&&(fstatus[fk]==0)){
          fstatus[fk] = 1; //clean fork
          fst[fk] = 1;  //single fork
          req[fk]--;
          fown[fk] = id;  //handing over fork
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
   request(i,right(i));
   request(i,left(i));
   sem_wait(&dmutex);
     pstatus[i]=2;
     fstatus[right(i)]=0;  //dirty fork
     fstatus[left(i)]=0;   //dirty fork
     fst[right(i)]=2;  //engaged fork
     fst[left(i)]=2;   //engaged fork
     print();
   sem_post(&dmutex);
}

//putting fork
void put_forks(int i)
{
   sem_wait(&dmutex);
     pstatus[i] = 0;  
     fst[right(i)]=0;  //free fork
     fst[left(i)]=0;   //free fork   
     print();
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
   for(int j=0; j < 5; j++){
       pthread_create(&phil[j],NULL,philosopher,(void *)j);
   }
   for(int k=0; k < 5; k++)
      pthread_join(phil[k],NULL);
 
   return 0;
}  
