#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <float.h>
#include <time.h>
#include "minheap.h"

typedef struct no{
    float x, y, z;
    int indice;
    char categoria;
}No;

typedef struct {
    No *vertice1;
    No *vertice2;
    double distancia;
} Tripla;

void limpaTela(){
    #ifdef _WIN32
        system("cls");
    #else 
        system("clear");
    #endif
}

void limpaBuffer(){
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

long long fat(int n) {
    if (n == 0) return 1;
    else return n * fat(n - 1);
}

long long combinatoria(int p, int n) {
    return fat(n) / (fat(p) * fat(n - p));
}

double calculaDE(struct no A, struct no B){
    return sqrt((A.x - B.x)*(A.x - B.x) + (A.y - B.y)*(A.y - B.y) + (A.z - B.z)*(A.z - B.z));
}

double **alocaMatriz(int tam){
    double **matriz = calloc(tam, sizeof(double*));
    if (matriz == NULL) return NULL;

    for (int i = 0; i < tam; i++){
        matriz[i] = calloc(tam, sizeof(double));
        if (matriz[i] == NULL){
            for (int j = 0; j < i; j++) free(matriz[j]);
            free(matriz);
            return NULL;
        }
    }

    return matriz;
}

double** criaMatrizDistancias(No *vertices, int totalVertices) {
    double **matriz = alocaMatriz(totalVertices);
    double aux;
    if (matriz == NULL) {
        return NULL;
    }
    
    printf("\nCalculando matriz de distâncias...\n");
    
    for (int i = 0; i < totalVertices; i++) {
        for (int j = i; j < totalVertices; j++) {
            if (i == j) {
                matriz[i][j] = 0.0; // Distância de um vértice para ele mesmo é 0
            } else {
                aux = calculaDE(vertices[i], vertices[j]);
                matriz[i][j] = aux;
                matriz[j][i] = aux;
            }
        }
    }
    
    return matriz;
}

void imprimeMatriz(double **matriz, int n, No *vertices) {
    printf("\nMatriz de Distâncias Euclidianas:\n");
    printf("     ");
    
    // Cabeçalho com índices
    for (int i = 0; i < n; i++) {
        printf("%8d ", i);
    }
    printf("\n");
    
    // Linhas da matriz
    for (int i = 0; i < n; i++) {
        printf("%3d: ", i);
        for (int j = 0; j < n; j++) {
            printf("%8.3f ", matriz[i][j]);
        }
        printf(" | %c\n", vertices[i].categoria);
    }
}

int alocaVertices(struct no **vertices, int alocar){
    No *temp = realloc(*vertices, sizeof(No) * alocar);
    if (temp == NULL) {
        printf("Erro ao alocar memória. Aperte enter para voltar.");
        limpaBuffer();
        getchar();
        return 0;
    }
    *vertices = temp; // Só atualize o ponteiro original após sucesso
    return 1;
}

void salvaMatrizArquivo(double **matriz, int n, No *vertices, char *nomeArquivo) {
    FILE *arquivo = fopen(nomeArquivo, "w");
    if (arquivo == NULL) {
        printf("Erro ao criar arquivo de saída.\n");
        return;
    }
    
    // Escreve dados
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            fprintf(arquivo, "%.6f,", matriz[i][j]);
        }
        fprintf(arquivo, "%c\n", vertices[i].categoria);
    }

    fclose(arquivo);
    printf("Matriz salva em '%s'\n", nomeArquivo);
}

int carregaArquivo(struct no **vertices, int *qtCategoria, int *qtAlocada, int *totalVertices, char nomeArquivo[100], char strCategorias[26]){
    FILE *arquivo = fopen(nomeArquivo, "r");

    if (arquivo == NULL){
        printf("\n\u274C Arquivo não encontrado, aperte enter para voltar. ");
        getchar();
        return 0;
    }

    char linha[100];
    float x,y,z;
    char categoria;
    int freqCat[26] = {0};

    fgets(linha, sizeof(linha), arquivo); // P0ula o cabecalho do arquivo

    while (fgets(linha, sizeof(linha), arquivo)){
        if(sscanf(linha, "%c, %f,%f,%f", &categoria, &x, &y, &z) == 4){
            if (*totalVertices >= *qtAlocada){
                *qtAlocada += 10;
                if (!alocaVertices(vertices, *qtAlocada)) return 0;
            }

            (*vertices)[*totalVertices].indice = (*totalVertices);
            (*vertices)[*totalVertices].x = x;
            (*vertices)[*totalVertices].y = y;
            (*vertices)[*totalVertices].z = z;
            (*vertices)[*totalVertices].categoria = categoria;

            (*totalVertices)++;

            freqCat[categoria - 'A']++;

            //printf("%c  %f  %f  %f", (*vertices)[*totalVertices].categoria, (*vertices)[*totalVertices].x,
            //                            (*vertices)[*totalVertices].y, (*vertices)[*totalVertices].z);
        }else{
            printf("\n\u274C Linha %d inválida. Aperte enter para voltar. ", *totalVertices+1);
            limpaBuffer();
            getchar();
            return 0;
        }
    }

    int cont = 0;
    for (int i = 0; i < 26; i++) {
        if (freqCat[i] > 0) {
            (*qtCategoria)++;
            strCategorias[cont++] = i+'A';  
        }
    }
    strCategorias[cont] = '\0';

    fclose(arquivo);
    return 1;
}

// ============= PRIM ===============
int extraiMinIndex(int tam, double chave[], bool vis[]){
    double min = DBL_MAX;
    int min_index = -1;

    for (int v = 0; v < tam; v++)
        if (!vis[v] && chave[v] < min){
            min = chave[v];
            min_index = v;
        }
    
    return min_index;  
}

void primMst(int tam, double **adj, int raiz, int pai[], double chave[]){
    bool vis[tam];

    for (int i = 0; i < tam; i++){
        chave[i] = DBL_MAX;
        vis[i] = false;
        pai[i] = -1;
    }

    chave[raiz] = 0;

    for (int i = 0; i < tam-1; i++){
        int u = extraiMinIndex(tam, chave, vis);

        if (u == -1) break;

        vis[u] = true;

        for (int v = 0; v < tam; v++){
            if (adj[u][v] != 0 && !vis[v] && adj[u][v] < chave[v]){
                pai[v] = u;
                chave[v] = adj[u][v];
            }
        }
    }
}

double calculaCaminho(No *origem, No *destino, No vertices[], int pai[], double chave[]) {
    bool vis[204] = {false};
    double dist = 0;

    int atual = origem->indice;

    while (atual != -1) {
        vis[atual] = true;
        atual = pai[atual];
    }

    atual = destino->indice;

    while (!vis[atual]) {
        if (vertices[atual].categoria != origem->categoria && vertices[atual].categoria != destino->categoria) return -1;
        dist += chave[atual];
        atual = pai[atual];
        if (atual == -1) return -1;
    }

    int ancestralComum = atual;

    atual = origem->indice;
    while (atual != ancestralComum) {
        if (vertices[atual].categoria != origem->categoria && vertices[atual].categoria != destino->categoria) return -1;
        dist += chave[atual];
        atual = pai[atual];
    }

    return dist;
}

Tripla* encontraCombinacoesPrim(int qtVertices, No vertices[], int qtCategoria, int *qtComb, char categorias[26], double **adj){
    //calcula prim
    int pai[qtVertices];
    double chave[qtVertices];
    primMst(qtVertices, adj, 0, pai, chave);  // usa vertice[0] como raiz

    //encontra menores distancias    
    *qtComb = combinatoria(2, qtCategoria);
    Tripla *combinacoes = malloc(*qtComb * sizeof(Tripla));
    int index = 0;

    
    for(int i = 0; i < qtCategoria; i++){
        for (int j = i+1; j < qtCategoria; j++){
            char tipoA = categorias[i];
            char tipoB = categorias[j];
            double menor = DBL_MAX;

            No *n1 = NULL, *n2 = NULL;

            for (int a = 0; a < qtVertices; a++) {
                if(vertices[a].categoria != tipoA) continue;

                for (int b = 0; b < qtVertices; b++) {
                    if (vertices[b].categoria != tipoB) continue;

                    double dist = calculaCaminho(&vertices[a], &vertices[b], vertices, pai, chave);
                    if (dist == -1) continue;  // quando passa por um vertice que não é origem nem destino
                    if (dist < menor) {
                        menor = dist;
                        n1 = &vertices[a];
                        n2 = &vertices[b];
                    }
                }
            }

            if (n1 && n2) {
                combinacoes[index].vertice1 = n1;
                combinacoes[index].vertice2 = n2;
                combinacoes[index].distancia = menor;
                index++;
            }
        }
    }

    (*qtComb) = index;
    return combinacoes;
}

// ============= DIJKSTRA ============
double* dijkstra(int s, double **matrizAdj, int totalVertices) {
    double *dist = malloc(sizeof(double) * totalVertices);
    if (dist == NULL) return NULL;
    
    // Inicializa todas as distâncias como infinito
    for (int i = 0; i < totalVertices; i++) dist[i] = DBL_MAX;
    
    MinHeap *pq = criaHeap(totalVertices * totalVertices); // Tamanho grande pois é um grafo denso

    if (pq == NULL) {
        free(dist);
        return NULL;
    }
    
    dist[s] = 0.0;
    insereHeap(pq, dist[s], s);
    
    while (!heapVazio(pq)) {
        HeapNode atual = extraiMin(pq);
        double d = atual.dist;
        int u = atual.vertice;
        
        if (d > dist[u]) continue;
        
        // Para cada vértice adjacente
        for (int v = 0; v < totalVertices; v++) {
            if (v != u && matrizAdj[u][v] > 0) { // Se existe aresta
                double peso = matrizAdj[u][v];
                if (dist[v] > d + peso) {
                    dist[v] = d + peso;
                    insereHeap(pq, dist[v], v);
                }
            }
        }
    }
    
    liberaHeap(pq);
    return dist;
}

Tripla* encontraCombinacoesDijkstra(int totalVertices, No vertices[], int qtCategoria, char categorias[], double **matrizDistancias) {    
    int qtComb = combinatoria(2, qtCategoria);
    Tripla *combinacoes = malloc(qtComb * sizeof(Tripla));
    if (!combinacoes) {
        printf("Erro de alocação.\n");
        return NULL;
    }

    int index = 0;

    for (int i = 0; i < qtCategoria; i++) {
        for (int j = i + 1; j < qtCategoria; j++) {
            char tipoA = categorias[i];
            char tipoB = categorias[j];
            double menor = DBL_MAX;

            int origemMenor = -1, destinoMenor = -1;

            for (int origem = 0; origem < totalVertices; origem++) {
                if (vertices[origem].categoria != tipoA) continue;

                double *distancias = dijkstra(origem, matrizDistancias, totalVertices); 

                for (int destino = 0; destino < totalVertices; destino++) {
                    if (vertices[destino].categoria != tipoB) continue;

                    double dist = distancias[destino];
                    if (dist < menor) {
                        menor = dist;
                        origemMenor = origem;
                        destinoMenor = destino;
                    }
                }

                free(distancias);
            }

            if (origemMenor != -1 && destinoMenor != -1) {
                combinacoes[index].vertice1 = &vertices[origemMenor];
                combinacoes[index].vertice2 = &vertices[destinoMenor];
                combinacoes[index].distancia = menor;
                index++;
            }
        }
    }

    return combinacoes;
}

int main(){
    int qtVertices = 0, qtAlocada = 10, opcao, **matrizADJ = NULL, qtCategoria = 0;
    bool continuar = true;

    double **matrizDistancias = NULL;
    char nomeArquivo[100] = "db_204x3_v1.csv";
    char arquivoSaida[100] = "matriz_distancias.csv";
    char categorias[26];
    No *vertices = malloc(sizeof(No) * qtAlocada);

    carregaArquivo(&vertices, &qtCategoria, &qtAlocada, &qtVertices, nomeArquivo, categorias);
    matrizDistancias = criaMatrizDistancias(vertices, qtVertices);
    //salvaMatrizArquivo(matrizDistancias, qtVertices, vertices, arquivoSaida);

    int qtCombinacoes;

    //Prim
    printf("\nPrim:\n");
    clock_t inicio = clock();
    Tripla *respostas1 = encontraCombinacoesPrim(qtVertices, vertices, qtCategoria, &qtCombinacoes, categorias, matrizDistancias);
    clock_t fim = clock();

    printf("Tempo de execução Prim: %.5f segundos\n", (double)(fim-inicio)/CLOCKS_PER_SEC);

    if (respostas1){
        for (int i = 0; i < qtCombinacoes; i++){
            printf("(%c[%i], %c[%i]) - Distância = %lf\n", respostas1[i].vertice1->categoria, respostas1[i].vertice1->indice, respostas1[i].vertice2->categoria, respostas1[i].vertice2->indice, respostas1[i].distancia);
        }
        free(respostas1);
    }
    
    qtCombinacoes = combinatoria(2, qtCategoria);

    //Dijkstra
    printf("\nDijkstra:\n");
    inicio = clock();
    Tripla *respostas2 = encontraCombinacoesDijkstra(qtVertices, vertices, qtCategoria, categorias, matrizDistancias);
    fim = clock();

    printf("Tempo de execução Dijkstra: %.5f segundos\n", (double)(fim-inicio)/CLOCKS_PER_SEC);

    if (respostas2 != NULL) {
        for (int i = 0; i < qtCombinacoes; i++){
            printf("(%c[%i], %c[%i]) - Distância = %lf\n", respostas2[i].vertice1->categoria, respostas2[i].vertice1->indice, respostas2[i].vertice2->categoria, respostas2[i].vertice2->indice, respostas2[i].distancia);
        }
        free(respostas2);
    }

    // libera
    for (int i = 0; i < qtVertices; i++) free(matrizDistancias[i]);
    free(matrizDistancias); 
    free(vertices); 
    return 0;
}
