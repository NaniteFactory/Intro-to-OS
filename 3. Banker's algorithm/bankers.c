#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "assert.h"
#include "time.h"

#define __in
#define __out

#define EXTRA_RESOURCES 200
#define M 7 // �� (�ڿ�)
#define N 10 // �� (���μ���; PID)

////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef enum {false, true} bool;

typedef struct listnode {
    int PID; // ���μ��� ���̵�
    struct listnode *next;
} ListNode;

typedef struct list {
    ListNode *front, *back;
    int size;
} List;

typedef struct state {
	int resource[M];
	int available[M];
	int claim[N][M];
	int alloc[N][M];
} state;

////////////////////////////////////////////////////////////////////////////////////////////////////////////

List* LLLCreateList() {
	List* list = (List*) malloc (sizeof(List));
	list->size = 0;
	list->front = list->back = NULL;

	return list;
}

bool LLLIsEmpty(List* list) {
	assert(list);
	return list->size == 0;
}

void LLLRemoveFront(List* list) {
	ListNode* tmp;

	assert(list);

	if(LLLIsEmpty(list)){
		printf("Error: List is Empty.\n");
		return;
	}

	tmp = list->front->next;
	free (list->front);

	if (tmp)    // # of node > 1
		list->front = tmp;
	else        // # of node == 1
		list->front = list->back = tmp;

	list->size--;
}

void LLLRemoveBack(List* list) {
	ListNode* tmp;
	assert(list);

	if(LLLIsEmpty(list)){
		printf("Error: List is Empty.\n");
		return;
	}

	tmp = list->front;

	if (tmp == list->back) { // # of node == 1
		free (list->back);
		list->front = list->back = NULL;
	} else { // # of node > 1
		while(tmp->next != list->back) { // find node to be removed
			tmp=tmp->next;
		}

		free (list->back);
		tmp->next = NULL;
		list->back = tmp;
	}

	list->size--;
}

void LLLRemoveKey (List* list, int key) {
	ListNode *prev = NULL;
	ListNode *cur = NULL;
	assert (list);

	if (LLLIsEmpty(list)){
		printf("Error: List is Empty.\n");
		return;
	}

	cur = list->front;

	// Before remove, # of node == 1
	if (list->size == 1 && cur->PID == key)
	{
		LLLRemoveFront(list);
		return;
	}

	// # of node > 1
	while (cur)
	{
		if (cur->PID == key)
		{
			if (cur == list->front)     // front node
			{
				LLLRemoveFront(list);
				return;
			} else if (cur == list->back) { // back node
				LLLRemoveBack(list);
				return;
			} else {
				prev->next = cur->next; // new link
				free (cur);
				list->size--;
				return;
			}
		}

		prev = cur;
		cur = cur->next;
	}

	// cannot find node
	printf("Error: Cannot find data.\n");
}

void LLLDestroyList(List* list){
	assert(list);

	while (!LLLIsEmpty(list)){
		LLLRemoveFront(list);
	}

	free (list);
	list = NULL;
}

void LLLAddFront(List* list, int _PID) {
	ListNode* n;
	
	assert(list);
	
	n = (ListNode*) malloc (sizeof(ListNode));
	n->PID = _PID;
	n->next = NULL;

	if (LLLIsEmpty(list)) {
		list->front = list->back = n;
	} else {
		n->next = list->front;
		list->front = n;
	}
	list->size++;
}

ListNode* LLLSearchList(const List* list, int key)
{
    ListNode* n;
    assert(list);

	n = list->front;
    while (n)
    {
        if(n->PID == key)  // if find key
            return n;

        n = n->next;
    }

    return NULL;
}

void Swap(int *a, int *b)
{
	int tmp = *a;
	*a = *b;
	*b = tmp;
}

void LLLListSortBubble(List* list)
{
	ListNode *i, *j;

	for (i = list->front; i != NULL; i = i->next)
		for (j = i; j != NULL; j = j->next){
			if (i->PID < j->PID){
				Swap(&i->PID, &j->PID);
			}
		}
}

void LLLCopyList(__in const List* prevList, __out List* newList)
{
	ListNode *i;
	//
	while (!LLLIsEmpty(newList)){
		LLLRemoveFront(newList);
	}
	for (i = prevList->front; i != NULL; i = i->next){
		LLLAddFront(newList, i->PID);
	}
//	LLLListSortBubble(newList);
}

// Process rest�� ����Ʈ�� ����
void GenerateProcesses(List* _list, int amount) {
	int bigPID;
	int i;
	if (LLLIsEmpty(_list)){
		bigPID = 0;
	} else {
		bigPID = _list->front->PID + 1;
	}
	for(i = bigPID; i<(bigPID+amount); i++){
		LLLAddFront(_list, i);
	}
	LLLListSortBubble(_list);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool TheRequestIsBiggerThanTheAvailable(__in const int* request, __in const int* available){
	int i;
	for(i=0;i<M;i++){
		if(request[i] > available[i]){
			return true;
		}
	}
	return false;
}

void FigureOutRequestForSingleProcess(__in const int process, __in const state* S, __out int* request){
	int i;
	for(i=0;i<M;i++){
		request[i] = S->claim[process][i] - S->alloc[process][i];
	}
}

void CopyState(__in const state* prevState, __out state* newState){
	int i, j;
	for(i=0;i<M;i++){
		newState->resource[i] = prevState->resource[i];
		newState->available[i] = prevState->available[i];
	}
	for(j=0;j<N;j++){
		for(i=0;i<M;i++){
			newState->claim[j][i] = prevState->claim[j][i];
			newState->alloc[j][i] = prevState->alloc[j][i];
		}
	}
}

// �ڿ� �Ҵ� ���� ���� ���¸� ����: ���ڷ� �Ѿ�� �� ���μ����� �ڿ��� �Ҵ��� ���� ����� newState�� �ڴ´�.
void MoveOntoTheNextState(
	__in const int process, 
	__in const state* prevState, 
	__in const List* prevProcessList,
	__out state* newState,
	__out List** newProcessList
	){
	int i;
//	int request[M];
	CopyState(prevState, newState);
//	FigureOutRequestForSingleProcess(process, newState, request);
	for(i=0;i<M;i++){
//		newState->alloc[process][i] = newState->alloc[process][i] + request[i];
//		newState->available[i] = newState->available[i] - request[i];
		newState->available[i] = newState->available[i] + newState->alloc[process][i];
		newState->alloc[process][i] = 0;
		newState->claim[process][i] = 0;
	}
	//
	*newProcessList = LLLCreateList();
	LLLCopyList(prevProcessList, *newProcessList);
	LLLRemoveKey(*newProcessList, process);
}

// ���뺤���� ��� ��Һ��� �� ���μ����� ��û ������ ��� ��Ұ� ���� ���ų� �۴�
bool TheProcessIsCheapEnough(__in const state* S, __in const int* currentAvailable, __in const int process){
	int resource;
	for(resource=0; resource<M; resource++){
		if(S->claim[process][resource] - S->alloc[process][resource] > currentAvailable[resource]){
			return false;
		}
	}
	return true;
}

/*
// ���μ����� ��� �䱸�� 0�̴�
bool TheProcessIsDead(__in const state* S, __in const int* currentAvailable, __in const int process){
	int resource;
	for(resource=0; resource<M; resource++){
		if(S->claim[process][resource] != 0){
			return false;
		}
	}
	return true;
}
*/

int FindAProcessTheBankerCanPayBack(__in const state* S, __in const int* currentAvailable, __in const List* rest){
	int process;
	for(process=0; process<N; process++){
		if ( TheProcessIsCheapEnough(S, currentAvailable, process) 
//			&& !TheProcessIsDead(S, currentAvailable, process) 
			&& (LLLSearchList(rest, process) != NULL) ) {
			return process;
		} // if
	} // for
	return -1;
} // func

// state�� ������ �������� Ȯ���Ѵ�
bool IsSafeTheStateTheBankerRecognizes(__in const state* S, __in const List* processList){
	int currentAvailable[M];
	List* rest;
	int i;
	int process;
	//
	for(i=0;i<M;i++){
		currentAvailable[i] = S->available[i];
	}
	rest = LLLCreateList();
	LLLCopyList(processList, rest);
	//
	for(;;){
		process = FindAProcessTheBankerCanPayBack(S, currentAvailable, rest);
		if(process == -1){
			break;
		} else { // ��Ŀ�� ����� �� �� �� �ִ� ���μ����� ã�Ҵ�
			for(i=0;i<M;i++){
				currentAvailable[i] = currentAvailable[i] + S->alloc[process][i];
			}
			LLLRemoveKey(rest, process);
		}
	} // while
	return LLLIsEmpty(rest);
} // func

// ���μ��� �ϳ��� �ڿ� ��û�Ѵ�.
void ResourceAllocationForSingleProcess(__in const int process, __out state* S, __out List* processList){
	int request[M];
	state newState = {{0,}};
	List* newProcessList;
	//
	if(LLLSearchList(processList, process) == NULL){
		printf("Fuck off :: PROCESS %d"
			"\n �׷� ���μ����� �������� �ʽ��ϴ�. \n\n", process);
		return;
	}
	//
	FigureOutRequestForSingleProcess(process, S, request);
	//
	if(TheRequestIsBiggerThanTheAvailable(request, S->available)){
		printf("Fuck off :: PROCESS %d"
			"\n ���ر��� �����մϴ�. ���� ���Ͱ� ��û���� �۽��ϴ�. \n\n", process);
		return;
	} else {
		MoveOntoTheNextState(process, S, processList, &newState, &newProcessList); // �ڿ� �Ҵ� ���¸� ����
		if (IsSafeTheStateTheBankerRecognizes(&newState, newProcessList)){ // ������ ���°� ������ �������� Ȯ��
			CopyState(&newState, S); // ���� ���͸� �����Ѵ�
			LLLRemoveKey(processList, process); // �䱸�ϴ� ���μ����� ������ ����
			printf("�ڿ� �Ҵ� �Ϸ� :: PROCESS %d"
				"\n ������ ���¿��� �ڿ� �Ҵ��� �����մϴ�. \n\n", process);
			return;
		} else {
			printf("�������� ȸ�� :: PROCESS %d"
				"\n �������� ������ �ֽ��ϴ�. �Ҿ����� ���·� �����ϴ� �ڿ� �Ҵ��� �ź��մϴ�. \n\n", process);
			return;
		}
	}
}

void display(__in const state *S, __in const List* processList){
	int i, j;
	ListNode* node;
	//
	puts("<<<<<<<<<<<claim>>>>>>>>>>>");
	for(i=0; i<N; i++){
		for(j=0; j<M; j++){
			printf("%d\t", S->claim[i][j]);
		}
		puts(" ");
	}
	puts("<<<<<<<<<<<alloc>>>>>>>>>>>");
	for(i=0; i<N; i++){
		for(j=0; j<M; j++){
			printf("%d\t", S->alloc[i][j]);
		}
		puts(" ");
	}
	puts("<<<<<<<<<<<request>>>>>>>>>>>");
	for(i=0; i<N; i++){
		for(j=0; j<M; j++){
			printf("%d\t", S->claim[i][j] - S->alloc[i][j]);
		}
		puts(" ");
	}
	puts("<<<<<<<<<<<resource>>>>>>>>>>>");
	for(j=0; j<M; j++){
		printf("%d\t", S->resource[j]);
	}
	puts(" ");
	puts("<<<<<<<<<<<available>>>>>>>>>>>");
	for(j=0; j<M; j++){
		printf("%d\t", S->available[j]);
	}
	puts(" ");
	puts("<<<<<<<<<<<process list>>>>>>>>>>>");
	for(node = processList->front; node != NULL; node = node->next){
		printf("%d\t", node->PID);
	}
	puts(" ");
	puts(" ===================================================================== ");
	puts(" ");
	printf("�ڿ� �Ҵ��� ��û�� ���μ����� ���� >> ");
}

void SetThingsRandom(__out state* AStateOfTrance, List** processList){
	int i, j;
	int sumAlloc[M];
	//
	srand(time(NULL));
	//
	for(i=0; i<N; i++) {
		for(j=0; j<M; j++) {
			AStateOfTrance->claim[i][j] = rand()%255 + 1; // �򰥸��� �ʰ� �ּ� 1�� �ش�
			AStateOfTrance->alloc[i][j] = rand()%(AStateOfTrance->claim[i][j] + 1);
			AStateOfTrance->resource[j] += AStateOfTrance->alloc[i][j]; // ��ü ���ҽ��� �Ҵ��� �ͺ��ٴ� ���� �״� ���Ѽ��� �����Ѵ�
		}
	}
	for(j=0; j<M; j++) {
		sumAlloc[j] = AStateOfTrance->resource[j]; // available�� ���ϱ� ���ؼ� �տ��� ���� �뵵�� �ִ� �� ���� �ؾ� ������ ������ ����Ѵ�
		AStateOfTrance->resource[j] += rand()%EXTRA_RESOURCES; // �ּҰ��� �߰��� ���ҽ� ������ ���Ѵ�
		AStateOfTrance->available[j] = AStateOfTrance->resource[j] - sumAlloc[j];
	}

	//
	*processList = LLLCreateList();
	GenerateProcesses(*processList, N);
}

int main() {
	state AStateOfTrance = {{0,}};
	List* processList;
	int message;

	SetThingsRandom(&AStateOfTrance, &processList);
	
	for(;;){
		display(&AStateOfTrance, processList);
		scanf("%d", &message);
		getchar(); // �Է� ���� ���
		ResourceAllocationForSingleProcess(message, &AStateOfTrance, processList);
		getchar();
	}
	
	getchar();
	return 0;
}








