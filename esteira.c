#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>

#define TAM_BUFFER 10
#define NUM_PRODUTORES 3
#define NUM_CONSUMIDORES 2
#define TOTAL_PRODUTOS 50

// Estrutura do produto
typedef struct {
    int id;
    int qualidade; // 0 = ruim, 1 = mediocre, 2 = boa
    int tipo;
    char nome[30];
    float preco;
} Produto;

// Buffer (esteira)
Produto buffer[TAM_BUFFER];
int in = 0;
int out = 0;

// Controle
pthread_mutex_t mutex;
sem_t cheio;
sem_t vazio;

// Estatísticas
int produzidos = 0;
int consumidos = 0;
int bons = 0;
int mediocres = 0;
int ruins = 0;

// Strings de qualidade
char *q_str[] = {"RUIM", "MEDIOCRE", "BOA"};

// Função produtor
void* produtor(void* arg) {
    int id = *((int*)arg);

    while (1) {
        // Tempo maior (produtor mais lento)
        usleep((rand() % 500 + 500) * 1000);

        pthread_mutex_lock(&mutex);
        if (produzidos >= TOTAL_PRODUTOS) {
            pthread_mutex_unlock(&mutex);
            break;
        }
        produzidos++;
        int id_produto = produzidos;
        pthread_mutex_unlock(&mutex);

        Produto p;
        p.id = id_produto;

        // Nomes possíveis
        char *nomes[] = {"Sensor", "Motor", "Placa", "Cabo", "Engrenagem"};
        int idx = rand() % 5;
        sprintf(p.nome, "%s_%d", nomes[idx], p.id);

        // Preço entre 10 e 100
        p.preco = (rand() % 900 + 100) / 10.0;

        // Diferença entre produtores (entradas distintas)
        if (id == 1) {
            p.qualidade = rand() % 100 < 70 ? 2 : (rand() % 2);
            p.tipo = 0;
        } else if (id == 2) {
            p.qualidade = rand() % 100 < 50 ? 1 : 0;
            p.tipo = 1;
        } else {
            p.qualidade = rand() % 3;
            p.tipo = 2;
        }

        sem_wait(&vazio);
        pthread_mutex_lock(&mutex);

        buffer[in] = p;
        in = (in + 1) % TAM_BUFFER;

        printf("[Produtor %d] Inseriu %s | R$%.2f | Qualidade: %s\n",
               id, p.nome, p.preco, q_str[p.qualidade]);

        pthread_mutex_unlock(&mutex);
        sem_post(&cheio);
    }

    pthread_exit(NULL);
}

// Função consumidor
void* consumidor(void* arg) {
    int id = *((int*)arg);

    while (1) {
        sem_wait(&cheio);

        pthread_mutex_lock(&mutex);

        if (consumidos >= TOTAL_PRODUTOS) {
            pthread_mutex_unlock(&mutex);
            sem_post(&cheio);
            break;
        }

        Produto p = buffer[out];
        out = (out + 1) % TAM_BUFFER;
        consumidos++;

        pthread_mutex_unlock(&mutex);
        sem_post(&vazio);

        // Consumidor mais rápido
        usleep((rand() % 300 + 200) * 1000);

        // Classificação
        if (p.qualidade == 2) {
            bons++;
        } else if (p.qualidade == 1) {
            mediocres++;
        } else {
            ruins++;
        }

        printf("    [Consumidor %d] %s | R$%.2f -> %s\n",
               id, p.nome, p.preco, q_str[p.qualidade]);
    }

    pthread_exit(NULL);
}

int main() {
    srand(time(NULL));

    pthread_t prod[NUM_PRODUTORES];
    pthread_t cons[NUM_CONSUMIDORES];

    int id_prod[NUM_PRODUTORES];
    int id_cons[NUM_CONSUMIDORES];

    pthread_mutex_init(&mutex, NULL);
    sem_init(&cheio, 0, 0);
    sem_init(&vazio, 0, TAM_BUFFER);

    // Criar produtores
    for (int i = 0; i < NUM_PRODUTORES; i++) {
        id_prod[i] = i + 1;
        pthread_create(&prod[i], NULL, produtor, &id_prod[i]);
    }

    // Criar consumidores
    for (int i = 0; i < NUM_CONSUMIDORES; i++) {
        id_cons[i] = i + 1;
        pthread_create(&cons[i], NULL, consumidor, &id_cons[i]);
    }

    // Esperar produtores
    for (int i = 0; i < NUM_PRODUTORES; i++) {
        pthread_join(prod[i], NULL);
    }

    // Liberar consumidores restantes
    for (int i = 0; i < NUM_CONSUMIDORES; i++) {
        sem_post(&cheio);
    }

    // Esperar consumidores
    for (int i = 0; i < NUM_CONSUMIDORES; i++) {
        pthread_join(cons[i], NULL);
    }

    // Estatísticas finais
    printf("\n===== ESTATÍSTICAS =====\n");
    printf("Produzidos: %d\n", produzidos);
    printf("Consumidos: %d\n", consumidos);
    printf("Bons: %d\n", bons);
    printf("Mediocres: %d\n", mediocres);
    printf("Ruins: %d\n", ruins);

    pthread_mutex_destroy(&mutex);
    sem_destroy(&cheio);
    sem_destroy(&vazio);

    return 0;
}