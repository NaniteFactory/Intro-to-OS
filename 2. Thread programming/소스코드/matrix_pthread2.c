#include "stdio.h"
#include "stdlib.h"
#include "pthread.h"
#include "string.h"

#define MAX_LEN 1000

typedef struct pthread_arg{
	int i;
}pthread_arg;

void finalFusionApproval();
void randomize(int(*a)[MAX_LEN]);
void display(int(*a)[MAX_LEN]);

int d[MAX_LEN][MAX_LEN];
int e[MAX_LEN][MAX_LEN];
int f[MAX_LEN][MAX_LEN];

//pthread_t *finalFusionThreadId[MAX_LEN];
//pthread_arg *finalFusionThreadArguments[MAX_LEN];
pthread_t finalFusionThreadId[MAX_LEN];
pthread_arg finalFusionThreadArguments[MAX_LEN];

int main(){
	int i;
	randomize(d);
	randomize(e);

	finalFusionApproval();	
	
	for(i=0;i<MAX_LEN;i++){
		pthread_join(finalFusionThreadId[i],NULL);
	}
	
	printf("헬 앤 헤븐...! 넌 상대를 잘못 골랐어!! \n");

//	display(f);
//	getchar();
	return 0;
}

void *finalFusionThreadDrive(void *arg){
	pthread_arg *arg1 = (pthread_arg *)arg;
	int tmp = 0;
	int k = 0, j = 0;
	for(j=0;j<MAX_LEN;j++){
		tmp = 0;
		for(k=0;k<MAX_LEN;k++){
			tmp += d[arg1->i][k] * e[k][j];
		}
		f[arg1->i][j] = tmp;
	}
	//
//	free(arg1);
	pthread_exit(NULL);
}

void finalFusionApproval(){
	int i, rc;
	printf("파이널 퓨전을 승인한다	\n");
	for(i=0;i<MAX_LEN;i++){
//			finalFusionThreadId[i] = (pthread_t*)malloc(sizeof(pthread_t));
//			finalFusionThreadArguments[i] = (pthread_arg*)malloc(sizeof(pthread_arg));
//			finalFusionThreadArguments[i]->i = i;
			finalFusionThreadId[i] = i;
			finalFusionThreadArguments[i].i = i;
			rc = pthread_create(&finalFusionThreadId[i],NULL,finalFusionThreadDrive,(void*)&finalFusionThreadArguments[i]);
			if(rc) {printf("error: %d",rc);}
	}
	printf("파이널 퓨전 프로그램 작동 개시! \n");
	
}

void randomize(int(*a)[MAX_LEN]){
	int i,j;
	for(i=0;i<MAX_LEN;i++){
		for(j=0;j<MAX_LEN;j++){
			a[i][j] = rand()%1000;
		}
	}
	printf("end of randomize \n");
}

void display(int(*a)[MAX_LEN]){
	int i,j;
	for(i=0;i<MAX_LEN;i++){
		printf("Row %d :: ", i);
		for(j=0;j<MAX_LEN;j++){
			printf("%d ", a[i][j]);
		}
		printf("\n");
	}
	printf("end of display \n");
}

