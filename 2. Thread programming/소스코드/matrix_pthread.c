#include "stdio.h"
#include "stdlib.h"
#include <pthread.h>
#include <string.h>

#define MAX_LEN 1000

typedef struct pthread_arg{
	int i;
	int j;
}pthread_arg;

void finalFusionApproval();
void randomize(int(*a)[MAX_LEN]);
void display(int(*a)[MAX_LEN]);

int d[MAX_LEN][MAX_LEN];
int e[MAX_LEN][MAX_LEN];
int f[MAX_LEN][MAX_LEN];

pthread_t *finalFusionThreadId[MAX_LEN][MAX_LEN];
pthread_arg *finalFusionThreadArguments[MAX_LEN][MAX_LEN];
pthread_attr_t finalFusionThreadAttr;

int main(){
//	int i,j;
	randomize(d);
	randomize(e);
	
	pthread_attr_init(&finalFusionThreadAttr);
	pthread_attr_setdetachstate(&finalFusionThreadAttr, PTHREAD_CREATE_DETACHED);
	
	finalFusionApproval();	
	
	/*
	for(i=0;i<MAX_LEN;i++){
		for(j=0;j<MAX_LEN;j++){
			pthread_join(*finalFusionThreadId[i][j],NULL);
			printf("%d, %d 파이널 퓨전! \n",i,j);
		}
	}
	*/
	
	printf("헬 앤 헤븐...! 넌 상대를 잘못 골랐어!! \n");

//	display(f);
//	getchar();
	return 0;
}

void *finalFusionThreadDrive(void *arg){
	pthread_arg *arg1 = (pthread_arg *)arg;
	int tmp = 0;
	int k = 0;
	for(k=0;k<MAX_LEN;k++){
		tmp += d[arg1->i][k] * e[k][arg1->j];
	}
	f[arg1->i][arg1->j] = tmp;
	//
	free(arg1);
	pthread_exit(NULL);
}

void finalFusionApproval(){
	int i,j,rc = 0;
	printf("파이널 퓨전을 승인한다	\n");
	for(i=0;i<MAX_LEN;i++){
		for(j=0;j<MAX_LEN;j++){
			finalFusionThreadId[i][j] = (pthread_t*)malloc(sizeof(pthread_t));
			finalFusionThreadArguments[i][j] = (pthread_arg*)malloc(sizeof(pthread_arg));
			finalFusionThreadArguments[i][j]->i = i;
			finalFusionThreadArguments[i][j]->j = j;
			do{
				rc = pthread_create(finalFusionThreadId[i][j],&finalFusionThreadAttr,finalFusionThreadDrive,(void*)finalFusionThreadArguments[i][j]);
				if(rc) {printf("thread[%d][%d] error code: %d \n",i,j,rc);}
			}while(rc);
		}
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


