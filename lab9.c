#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>

int *buffer; // Buffer
sem_t prod, cons; // Semáforos utilizados
int in=0; // Utilizada para que o produtor insira os elementos corretos no buffer
int tam; // Tamanho do buffer
int cont=0; // Contador

// Função responsável por inicializar o buffer
void inibuffer(int N){
    for(int i=0;i<N;i++){
        buffer[i]=0;
    }
}

// Função responsável por mostrar o estado do buffer
void imprimebuffer(){
    printf("Buffer:  %d  ",buffer[0]);
    for(int i = 1 ; i<tam ; i++){
        printf("  %d  " ,buffer[i]);
    }
    printf("\n");
}

// Função responsável por produzir os elementos no buffer (números de 1 até o tamanho do buffer)
void produz(){
    buffer[in]=(in+1);
    in++;
    cont++;
    imprimebuffer();
}

// Função responsável por consumir os elementos do buffer
void consome(int n){
   int id = n;
   // O primeiro passo da iteração foi feito fora do for para deixar o print mais legível
   printf("Thread %d consumiu %d",id+1,buffer[0]);
   buffer[0] = 0;
   for(int i = 1 ; i<tam ; i++){
       printf(", %d ",buffer[i]);
       buffer[i]=0;
   }
   printf("\n");
   cont = 0; // Reiniciando o contador
   imprimebuffer();
   sem_post(&prod); // Permitindo que as threads produtoras voltem a produzir
}

// Thread produtora
void *produtor(void *arg){
    int id = (int) arg;
    printf("Produtor %d criado\n",id+1);
    while(1){
    sem_wait(&prod); // Apenas uma thread produzirá por vez
    if(cont==tam){  // Testando se o buffer está cheio
        in=0;
        sem_post(&cons); // Thread produtora libera thread consumidora para consumir o buffer
        sem_wait(&prod); // Thread produtora se bloqueará até que a thread consumidora consuma os elementos
    }
    printf("Produtor %d produzindo\n",id+1);
    produz();
    sem_post(&prod); // Thread produtora atual encerrou sua tarefa, permitindo que outra thread produtora inicie
    }
    pthread_exit(NULL);
}

// Thread consumidora
void * consumidor(void *arg){
    int id = (int) arg;
    printf("Consumidor %d criado\n",id+1);
    while(1){
    sem_wait(&cons); // Thread só iniciará quando o buffer estiver cheio
    printf("consumidor %d vai consumir\n", id+1);
    consome(id);
    }
}

int main(int argc ,char *argv[]){
    // Checando se os valores de entrada foram passados corretamente 
    if(argc<4){
        puts("A entrada deve ser: ./lab9 <tamanho do buffer> <quantidade de threads produtoras> <quantidade de threads consumidoras");
        return 1;
    }
    
    int p , c; // Quantidade de threads produras e consumidoras respectivamente
    // Atribuindo os valores de entrada às variáveis
    tam=atoi(argv[1]);
    p=atoi(argv[2]);
    c=atoi(argv[3]);
    //Inicializando os semáforos, definindo o espaço das threads, alocando o buffer e o inicializando
    sem_init(&prod,0,1);
    sem_init(&cons,0,0);
    pthread_t tid[p+c];
    buffer = malloc(sizeof(int)*tam);
    inibuffer(tam);

    //Criando as threads produtoras
    for(int i=0;i<p;i++){
        if(pthread_create(&tid[i], NULL, produtor, (void*) i)){
            return 2;
        }
    }
    //Criando as thhreads consumidoras
    for(int i=0;i<c;i++){
        if(pthread_create(&tid[i+p], NULL, consumidor, (void*) i)){
            return 2;
        }
    }

    pthread_exit(NULL);
    
    sem_destroy(&prod);
    sem_destroy(&cons);
    free(buffer);

    return 0;
}
