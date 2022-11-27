#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<wait.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/types.h>
#include<signal.h>

#define N 5

{}
void Produce(int *PID_Productor)
{
   int i;
   for(i=0;i<2;i++)
   {
      printf("\nProductor con pid=%d, produciendo=%d\n",*PID_Productor,i);
      sleep(1);	
   }
}

void Consume(int *PID_Consumidor)
{
   int i;
   for(i=0;i<6;i++)
   {
      printf("\nConsumidor con pid=%d, consumiendo=%d\n",*PID_Consumidor,i);
      sleep(1);	
   }
}

void Entra_al_Bufer(int *PID)
{
   int i;
   for(i=0;i<3;i++)
   {
      printf("\nProceso con pid=%d, en el bufer=%d\n",*PID,i);
      sleep(1);
   }
}

int main()
{
   pid_t pid;
   int shmid1;
   int shmid2;
   int shmid3;
   int *PID_Productor;
   int *PID_Consumidor;
   int *cuenta;
   key_t llave1;
   key_t llave2;
   key_t llave3;
   llave1=ftok("Prueba1",'k');
   llave2=ftok("Prueba2",'l');
   llave3=ftok("Prueba3",'m');
   shmid1=shmget(llave1,sizeof(int),IPC_CREAT|0600);
   shmid2=shmget(llave2,sizeof(int),IPC_CREAT|0600);
   shmid3=shmget(llave3,sizeof(int),IPC_CREAT|0600);
   PID_Productor=shmat(shmid1,0,0);
   PID_Consumidor=shmat(shmid2,0,0);
   cuenta=shmat(shmid3,0,0);
   *cuenta=2;
   pid= fork();
   if(pid==-	1)
   {
      perror("\nError al crear el proceso\n");
      exit(-1);
   }
   if(pid==0)
   {
      *PID_Productor=getpid();
      while (1)
      {
         Produce(PID_Productor);
         if(*cuenta==N)
         {
            kill(*PID_Productor,SIGTSTP);
            printf("\nProductor dormido\n");
         }
         Entra_al_Bufer(PID_Productor); 
         *cuenta=*cuenta+1;
         printf("\nCuenta= %d\n",*cuenta); 
         if(*cuenta==1)
         {
            kill(*PID_Consumidor,SIGCONT);
            printf("\nDespertando al consumidor\n");
         }
      }
   }
   else
   {
      *PID_Consumidor=getpid();
      while(1)
      {
         if(*cuenta==0)
         {
            kill(*PID_Consumidor,SIGTSTP);
            printf("\nConsumidor dormido\n");
         }
         Entra_al_Bufer(PID_Consumidor); 
         *cuenta=*cuenta-1;
         printf("\nCuenta= %d\n",*cuenta); 
         if(*cuenta==(N-1))
         {
            kill(*PID_Productor,SIGCONT);
            printf("\nDespertando al productor\n");
         }
         Consume(PID_Consumidor);
      }
   }
   return 0;
}