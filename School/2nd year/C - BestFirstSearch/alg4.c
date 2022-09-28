#include <stdio.h>
#include <stdlib.h>

typedef struct Node{
	int E;
	int Xlast;
	int Ylast;
	int X;
	int Y;
	int iter;
	struct Node* next;
} Node;

typedef struct Queue{
	Node* first;
	Node* last;
	int n;
} Queue;

Queue* createQueue(){
	Queue* queue = (Queue*)malloc(sizeof(Queue));
	queue->first = NULL;
	queue->last = NULL;
	queue->n = 0;
	return queue;
}

void push(Queue* queue, int E, int X, int Y, int Xlast, int Ylast, int iter){
	Node* node = (Node*)malloc(sizeof(Node));
	node->E = E;
	node->X = X;
	node->Y = Y;
	node->Xlast = Xlast;
	node->Ylast = Ylast;
	node->iter = iter;

	node->next = NULL;

	if(queue->n == 0){
		queue->first = node;
	}else{
		queue->last->next = node;
	}queue->last = node;
	queue->n++;
}

Node* pop(Queue* queue){
	if(queue->n == 0){
		//printf("noPOP\n");
		return NULL;}
	Node* ret = queue->first;
	queue->n--;
	if(queue->n != 0){
	queue->first = ret->next;
	}else{queue->first = NULL;}	
	return ret;
}

int calculate1(int I1, int I2, int Dmov, int E){
if(I1 < I2) return E+Dmov;
if(I1 == I2) return E;
if(E >= Dmov) return E-Dmov;
return 0;
}

int calculate2(int I2, int Denv, int E){
if(E >= Denv + I2) return E - Denv;
if(E > I2) return I2;
return E;
}

int calculate(int I1, int I2, int Dmov, int Denv, int E){
E = calculate1(I1, I2, Dmov, E);
E = calculate2(I2, Denv, E);
return E;
}

int main(int argc, char* argv[]){
int M, N, Einit, Dmov, Denv;
int lastVisited = 0;
scanf("%d %d %d %d %d", &M, &N, &Einit, &Dmov, &Denv);

int** enviroment = (int**)malloc(sizeof(int*)*M);
for(int i = 0; i<M; i++){
	enviroment[i] = (int*)malloc(sizeof(int)*N);
}
int** bestPower = (int**)malloc(sizeof(int*)*M);
for(int i = 0; i<M; i++){
	bestPower[i] = (int*)malloc(sizeof(int)*N);
}
for(int i = 0; i<M; i++){
	for(int j = 0; j<N; j++){
		bestPower[i][j] = -1;
	}
}
for(int i = 0; i<M; i++){
	for(int j = 0; j<N; j++){
		scanf("%d", &enviroment[i][j]);
	}
}

Queue* queue = createQueue();
push(queue, Einit, 0, 1, 0, 0, 1);
push(queue, Einit, 1, 0, 0, 0, 1);
bestPower[0][0] = Einit;
int iteration = 0;
while(1){
	Node* node = pop(queue);
	//printf("%d\n", node->E);
	node->E = calculate(enviroment[node->Xlast][node->Ylast], enviroment[node->X][node->Y], Dmov, Denv, node->E);
	//printf("%d\n", node->E);
	if(node->X == M-1 && node->Y == N-1){
		iteration = node->iter;
		break;
	}
	if(node->E <= 0){
		free(node);
		//printf("EnergyDown\n");
		continue;
	}
	if(bestPower[node->X][node->Y] < node->E){
		bestPower[node->X][node->Y] = node->E;
	}else{
		//printf("betterPowerBefore\n");
		free(node);
		continue;
	}
	//printf("%d   %d\n", node->E, node->iter);
	
	//right
	if(node->Ylast != node->Y+1 && node->Y+1 < N){
		//printf("addRight\n");
		push(queue, node->E, node->X, node->Y+1, node->X, node->Y, node->iter+1);
	}

	//down
	if(node->Xlast != node->X+1 && node->X+1 < M){
		//printf("addDown\n");
		push(queue, node->E, node->X+1, node->Y, node->X, node->Y, node->iter+1);
	}
	
	//left
	if(node->Ylast != node->Y-1 && node->Y-1 >= 0){
		//printf("addleft\n");
		push(queue, node->E, node->X, node->Y-1, node->X, node->Y, node->iter+1);
	}

	//up
	if(node->Xlast != node->X-1 && node->X-1 >= 0){
		//printf("addUp\n");
		push(queue, node->E, node->X-1, node->Y, node->X, node->Y, node->iter+1);
	}

	free(node);
}
free(queue);
printf("%d\n", iteration);

}
