// Condition variable usage example.
// Compile with: gcc -O2 -Wall -pthread cv-example.c -o cv-example

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h> // tha xrhsimpopoihsw boolean times gia na organwsw tiw ergasies moy
#include <time.h> //tha th xrhsimopoihsw gia na blaw random times ston pinka

#define MESSAGES 20
#define N 10000 // arithmos stoixeiwn pinaka
#define THREADS 4 //arithmos twn  Threads
#define CUTOFF 10 

typedef struct{
    double *a;
    int n;
}message_type; // ta eidh twn mhymatwn aforoyn thn eidopoihsh twn threads sxetika me thn ergasia poy exoy na kanoyn

void* work (void*); //synarthsh pou tha xrhsimopoihthei gia threading

int partition(double *a, int n) //syarthsh poy diaxwrizei ton pinaka se ypopinakes

void inssort(double *a,int n) //otan oi ypopinakews poy dhmioyrgoyntai apo th partition ftasoyn na exoyn 10 stoixeia kai katw(<=CUTOFF) tot kaloume th inssort

int job_crt = 0; // plhthos ergsiwn pou dhmiourghthikan

int job_cmp = 0; // plhthos ergasiwn pou oloklhrwthikan

bool  job_to_be_done(void); //epistrefei true an yparxei ergasia se ekremothta



message_type global_buffer[N]; //sfairikh oyra ergasiwn

void  add_job(double *, int); //prosthikh ergasias sthn oyra

message_type* completeJob(void); // ektelesh ergasia sthn oyra



pthread_cond_t msg_in = PTHREAD_COND_INITIALIZER;


pthread_cond_t msg_out = PTHREAD_COND_INITIALIZER;

// sygxronizei ta threads dioti moirazontai thn idia synarthsh
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;




int main() {
	
	
	
	double *a = (double *)malloc(N * sizeof(double));//dhmioyrgia pinaka me tyxaioys arithmoys
	srand(time(NULL));
	
	for (int i = 0; i < N; i++){
		a[i] = rand() 
	}

  
  pthread_t * thread_array = malloc(THREADS * sizeof(pthread_t)); // dhmioyrgia twn threads
	for(int i = 0; i < THREADS; i++)
    	pthread_create(&thread_array[i], NULL, work, (void*)((long)i));
  

	
	addJob(a, N); // prosthikh ergasiwn sth sfairikh oyra ergasiwn
	pthread_cond_signal(&msg_in); // eidopoihsh twn threads gia thn ergasia
  
  
	for (int i = 0; i < (N - 1); i++){// tsekaroyme an pragmatopoihthiike h taksinomhsh
		if (a[i] > a[i + 1]){
			printf("taksinomisi ok\n");
			break;
		}else{
			printf("h taksinomhsh apetyxe\n");
		}
	}



  
    for (int i = 0; i < THREADS; i++){// kanoume join ta threads
        pthread_join(thread_array[i], NULL);
	}




	free(thread_array);
	free(a);
	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&msg_out);
  	pthread_cond_destroy(&msg_in);

  return 0;
}


int partition(double *a, int n){
 
  first = 0;
  middle = n-1;
  last = n/2;  
  

  if (a[middle]<a[first]) { t = a[middle]; a[middle] = a[first]; a[first] = t; }
  if (a[last]<a[middle]) { t = a[last]; a[last] = a[middle]; a[middle] = t; }
  if (a[middle]<a[first]) { t = a[middle]; a[middle] = a[first]; a[first] = t; }
  
 p = a[middle]; // stoixeio me basg to opoio xwrizoyme ton pinaka se 2 merh : sto aristero meros ta mikrotera kai sto deksi ta megalytera stoixeia
  for (i=1,j=n-2;;i++,j--) {
    while (a[i]<p) i++;
    while (p<a[j]) j--;
    if (i>=j) break;

    t = a[i]; a[i] = a[j]; a[j] = t;      
  }
}

message_type* completeJob(void){//diabazei th epomenh ergasia apo ton buffer kai epistrefei deikth ths domhs message type

    
    job_cmp++;
    return &global_buffer[job_cmp-1];
}

bool job_to_be_done(void){
	
    if( job_cmp < job_crt ){
	
        return true;
    }else
		return false;
}


void addJob(double* add_a, int add_n){
	
 global_buffer[job_crt].a = add_a;
    
global_buffer[job_crt].n = add_n;
    
	job_crt++;
}

void inssort(double *a,int n)
{
	int i, j;
	double t;

	for (i = 1; i < n; i++) {
		j = i;
		while ((j > 0) && (a[j - 1] > a[j])) {
			t = a[j - 1];
			a[j - 1] = a[j];
			a[j] = t;
			j--;
		}
	}
}


void *work(void* which)
{
	message_type *p; // deikths poy deixnei stiw metablhtes ths domhs
	while(1){
		
		pthread_mutex_lock(&mutex); //kleidwnoyme to mutex etsi wste na exei mono ena thread prosbash
		while (!job_to_be_done()){
		
		
			pthread_cond_wait(&msg_in); // anamonh mexri thn eyresh ergsias
		}
	
		p = completeJob();
		pthread_mutex_unlock(&mutex); // ksekleidwma Mutex 


	
		if ( (*p).n <= CUTOFF) {
			pthread_mutex_lock(&mutex);
		
		

			inssort((*p).a, (*p).n);  // ektelesh thsinssort
			
			pthread_mutex_unlock(&mutex);
			
		}
		else{
			

			int i = partition((*p).a, (*p).n); // ektelesh ths partition

		
			pthread_mutex_lock(&mutex);
			
			addJob((*p).a, i); //prosthikh neas ergasia(opws tha anakalousame thn quicksort) 
			
			addJob((*p).a + i, (*p).n - i); 
			
			pthread_mutex_unlock(&mutex);
			pthread_cond_broadcast(&msg_in); // ksipnima twn threads gia nea ergasia
		}
		pthread_cond_signal(&msg_out);// eidopoihsh oti h ergasia teleiwse (eite h inssort eite h partition)
	}
	pthread_mutex_unlock(&mutex);
	pthread_exit(NULL);
}
