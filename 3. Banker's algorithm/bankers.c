#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "assert.h"
#include "time.h"

#define __in
#define __out

#define EXTRA_RESOURCES 200
#define M 7 // 열 (자원)
#define N 10 // 행 (프로세스; PID)

////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef enum {false, true} bool;

typedef struct listnode {
    int PID; // 프로세스 아이디
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

// Process rest를 리스트로 구현
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

// 자원 할당 수행 뒤의 상태를 상정: 인자로 넘어온 그 프로세스에 자원을 할당한 뒤의 행렬을 newState에 박는다.
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

// 가용벡터의 모든 요소보다 그 프로세스의 요청 벡터의 모든 요소가 전부 같거나 작다
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
// 프로세스의 모든 요구가 0이다
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

// state가 안전한 상태인지 확인한다
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
		} else { // 뱅커가 충분히 돈 줄 수 있는 프로세스를 찾았다
			for(i=0;i<M;i++){
				currentAvailable[i] = currentAvailable[i] + S->alloc[process][i];
			}
			LLLRemoveKey(rest, process);
		}
	} // while
	return LLLIsEmpty(rest);
} // func

// 프로세스 하나가 자원 요청한다.
void ResourceAllocationForSingleProcess(__in const int process, __out state* S, __out List* processList){
	int request[M];
	state newState = {{0,}};
	List* newProcessList;
	//
	if(LLLSearchList(processList, process) == NULL){
		printf("Fuck off :: PROCESS %d"
			"\n 그런 프로세스는 존재하지 않습니다. \n\n", process);
		return;
	}
	//
	FigureOutRequestForSingleProcess(process, S, request);
	//
	if(TheRequestIsBiggerThanTheAvailable(request, S->available)){
		printf("Fuck off :: PROCESS %d"
			"\n 지준금이 부족합니다. 가용 벡터가 요청보다 작습니다. \n\n", process);
		return;
	} else {
		MoveOntoTheNextState(process, S, processList, &newState, &newProcessList); // 자원 할당 상태를 가정
		if (IsSafeTheStateTheBankerRecognizes(&newState, newProcessList)){ // 상정한 상태가 안전한 상태인지 확인
			CopyState(&newState, S); // 상태 벡터를 수정한다
			LLLRemoveKey(processList, process); // 요구하는 프로세스를 실제로 뺀다
			printf("자원 할당 완료 :: PROCESS %d"
				"\n 안전한 상태에서 자원 할당을 수행합니다. \n\n", process);
			return;
		} else {
			printf("교착상태 회피 :: PROCESS %d"
				"\n 교착상태 위험이 있습니다. 불안전한 상태로 전이하는 자원 할당을 거부합니다. \n\n", process);
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
	printf("자원 할당을 요청할 프로세스를 선택 >> ");
}

void SetThingsRandom(__out state* AStateOfTrance, List** processList){
	int i, j;
	int sumAlloc[M];
	//
	srand(time(NULL));
	//
	for(i=0; i<N; i++) {
		for(j=0; j<M; j++) {
			AStateOfTrance->claim[i][j] = rand()%255 + 1; // 헷갈리지 않게 최소 1을 준다
			AStateOfTrance->alloc[i][j] = rand()%(AStateOfTrance->claim[i][j] + 1);
			AStateOfTrance->resource[j] += AStateOfTrance->alloc[i][j]; // 전체 리소스가 할당한 것보다는 많을 테니 하한선을 설정한다
		}
	}
	for(j=0; j<M; j++) {
		sumAlloc[j] = AStateOfTrance->resource[j]; // available을 구하기 위해서 합에서 빼는 용도가 있는 이 값을 잊어 버리기 직전에 기억한다
		AStateOfTrance->resource[j] += rand()%EXTRA_RESOURCES; // 최소값에 추가로 리소스 여분을 더한다
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
		getchar(); // 입력 버퍼 비움
		ResourceAllocationForSingleProcess(message, &AStateOfTrance, processList);
		getchar();
	}
	
	getchar();
	return 0;
}








