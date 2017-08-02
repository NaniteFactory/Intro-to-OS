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

pthread_t finalFusionThreadId;
pthread_arg finalFusionThreadArguments;

void *finalFusionThreadDrive(void*arg){
	int tmp = 0;
	int k = 0, j = 0, i = 0;
	for(i=0;i<MAX_LEN;i++){
		for(j=0;j<MAX_LEN;j++){
			tmp = 0;
			for(k=0;k<MAX_LEN;k++){
				tmp += d[i][k] * e[k][j];
			}
			f[i][j] = tmp;
		}
	}
	pthread_exit(NULL);
}

int main(){
	randomize(d);
	randomize(e);
	
	pthread_create(&finalFusionThreadId,NULL,finalFusionThreadDrive,(void*)&finalFusionThreadArguments);
	pthread_join(finalFusionThreadId,NULL);
	
	printf("헬 앤 헤븐...! 넌 상대를 잘못 골랐어!! \n");

//	display(f);
//	getchar();
	return 0;
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
