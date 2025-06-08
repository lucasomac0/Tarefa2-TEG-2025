#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <float.h>

typedef struct no{
    float x, y, z;
    int indice;
    char categoria;
}No;

typedef struct {
    No *a;
    No *b;
    double dist;
}Tupla;

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
int extraiMin(int tam, double chave[], bool vis[]){
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
        int u = extraiMin(tam, chave, vis);

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

long long fat(int n) {
    if (n == 0) return 1;
    else return n * fat(n - 1);
}

long long combinatoria(int p, int n) {
    return fat(n) / (fat(p) * fat(n - p));
}

double calculaCaminho(int u, int v, int pai[], double chave[]) {
    bool vis[204] = {false};
    double dist = 0;

    int atual = u;
    while (atual != -1) {
        vis[atual] = true;
        atual = pai[atual];
    }

    atual = v;
    while (!vis[atual]) {
        dist += chave[atual];
        atual = pai[atual];
    }

    int ancestrorComum = atual;

    atual = u;
    while (atual != ancestrorComum) {
        dist += chave[atual];
        atual = pai[atual];
    }

    return dist;
}

Tupla *encontraCombinacoes(int qtVertices, No vertices[], int pai[], double chave[], int qtCategoria, char categorias[26]){
    int qtComb = combinatoria(2, qtCategoria);
    Tupla *combinacoes = malloc(qtComb * sizeof(Tupla));
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

                    double dist = calculaCaminho(vertices[a].indice, vertices[b].indice, pai, chave);
                    if (dist < menor) {
                        menor = dist;
                        n1 = &vertices[a];
                        n2 = &vertices[b];
                    }
                }
            }

            if (n1 && n2) {
                combinacoes[index].a = n1;
                combinacoes[index].b = n2;
                combinacoes[index].dist = menor;
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
    // imprimeMatriz(matrizDistancias, qtVertices, vertices);
    salvaMatrizArquivo(matrizDistancias, qtVertices, vertices, arquivoSaida);


    //prim
    int pai[qtVertices];
    double chave[qtVertices];
    primMst(qtVertices, matrizDistancias, 0, pai, chave);
    Tupla *combinacoes = encontraCombinacoes(qtVertices, vertices, pai, chave, qtCategoria, categorias);
    
    for (int i = 0; i < combinatoria(2, qtCategoria); i++){
        printf("(%c[%i], %c[%i]) - DE = %lf\n", combinacoes[i].a->categoria, combinacoes[i].a->indice, combinacoes[i].b->categoria, combinacoes[i].b->indice, combinacoes[i].dist);
    }
    
    return 0;
}
