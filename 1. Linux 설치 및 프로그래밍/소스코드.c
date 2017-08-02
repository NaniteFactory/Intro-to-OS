#include <stdio.h>
#include <stdlib.h>
#define MAX_LEN 1000

void multiply(int(* a)[MAX_LEN], int(* b)[MAX_LEN], int(* c)[MAX_LEN]);
void randomize(int (*a)[MAX_LEN]);
void display(int (*a)[MAX_LEN]);

int d[MAX_LEN][MAX_LEN];
int e[MAX_LEN][MAX_LEN];
int f[MAX_LEN][MAX_LEN];

void main(){
	randomize(d);
	randomize(e);
	multiply(d,e,f);
	display(f);
	
	getchar();
	return;
}

void multiply(int(* a)[MAX_LEN], int(* b)[MAX_LEN], int(* c)[MAX_LEN]){
	int i,j, k, tmp;
	for(i=0;i<MAX_LEN;i++){
		for(j=0;j<MAX_LEN;j++){
			tmp = 0;
			for(k=0;k<MAX_LEN;k++){
				tmp += a[i][k] * b[k][j];
			}
			c[i][j] = tmp;
		}
	}
	printf("end of multiply \n");
}

void randomize(int (*a)[MAX_LEN]){
	int i,j;
	for(i=0;i<MAX_LEN;i++){
		for(j=0;j<MAX_LEN;j++){
		//	a[i][j] = rand()%(1+ rand()%1000);
			a[i][j] = rand()%1000;
		}
	}
	printf("end of radomize \n");
}

void display(int (*a)[MAX_LEN]){
	int i,j;
	for(i=0;i<MAX_LEN;i++){
		printf("Row %d :: ",i);
		for(j=0;j<MAX_LEN;j++){
			printf("%d ", a[i][j]);
		}
		printf("\n");
	}
	printf("end of display \n");
}







