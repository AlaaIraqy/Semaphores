#include <pthread.h>
#include <stdio.h>
#include<stdlib.h>
#include <semaphore.h>
#include<iostream>
#include<unistd.h>
#include <cstdint>
using namespace std;
void *mcounter(void* i);
void *mmonitor(void *);
void *mcollector(void *);
int count;
int buffer [10];
int bufferIndex=-1;
sem_t csem,bsem[10];

int main()
{
    pthread_t Nthreads[5];
    pthread_t monthread,collthread;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
    sem_init(&csem,0, 1);
    for(int i=0; i<10; i++)
    {
        sem_init(&bsem[i],0, 1);
    }
    cout<<"main starts"<<endl;
    pthread_create(&monthread,&attr,mmonitor,NULL);
    for (int i = 0; i < 5; i++)
    {
        pthread_create(&Nthreads[i],&attr, mcounter,(void*)i);
        usleep(50);
    }

    pthread_create(&collthread,&attr,mcollector,NULL);
    for (int i = 0; i < 5; i++)
    {
        pthread_join(Nthreads[i], NULL);
    }
    pthread_join(monthread,NULL);
  //pthread_join(collthread,NULL);
    cout<<"main ends"<<endl;
    return 0;
}
void *mcounter(void* i)
{
    int threadno = (intptr_t)i;
    int t=rand()%100;
    usleep(t);
    cout << "counter thread " <<threadno<<": recieved a msg"<<endl;

    cout << "counter thread " <<threadno<<": waiting to write"<<endl;
    sem_wait(&csem);
    count= rand()%100;
    cout << "counter thread " <<threadno<<": writing to the counter value="<<count<<endl;
    sem_post(&csem);
}
void *mmonitor(void*)
{
    for(int i=0; i<3; i++)
    {
        int data;
        int t1=500;
        usleep(t1);
        cout<<"monitor thread: waiting to read counter"<<endl;
        sem_wait(&csem);
        data=count;
        cout<<"monitor thread: reading a count of value "<<data<<endl;
        sem_post(&csem);
        if(bufferIndex==10){
        cout<<"monitor thread: buffer is full"<<endl;
        }
        else{
        cout<<"monitor thread: waiting to write into buffer"<<endl;
        sem_wait(&bsem[bufferIndex]);
        buffer[++bufferIndex]=data;
        cout<<"monitor thread: writing into the buffer the value "<<buffer[bufferIndex]<<endl;
        sem_post(&bsem[bufferIndex]);
    }
    }
}
void *mcollector(void*){
for(int i=0;i<3;i++){
    usleep(300);
    int data;
    if(bufferIndex==-1){
    cout<<"collector thread: buffer is empty"<<endl;
    }else{
    cout<<"collector thread: waiting to collect"<<endl;
    sem_wait(&bsem[bufferIndex]);
    data=buffer[bufferIndex];
    cout<<"collector thread: collecting value "<<data<<" from buffer index "<<bufferIndex<<endl;
    bufferIndex--;
    sem_post(&bsem[bufferIndex]);
}
}
}



