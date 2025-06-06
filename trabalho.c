#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct no{
    float x, y, z;
    int indice;
    char categoria;
}No;

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
    double **matriz = calloc(tam, sizeof(int*));
    if (matriz == NULL) return NULL;

    for (int i = 0; i < tam; i++){
        matriz[i] = calloc(tam,sizeof(int));
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
    if (matriz == NULL) {
        return NULL;
    }
    
    printf("\nCalculando matriz de distâncias...\n");
    
    for (int i = 0; i < totalVertices; i++) {
        for (int j = 0; j < totalVertices; j++) {
            if (i == j) {
                matriz[i][j] = 0.0; // Distância de um vértice para ele mesmo é 0
            } else {
                matriz[i][j] = calculaDE(vertices[i], vertices[j]);
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

int carregaArquivo(struct no **vertices, int *qtAlocada, int *totalVertices, char nomeArquivo[100]){
    FILE *arquivo = fopen(nomeArquivo, "r");

    if (arquivo == NULL){
        printf("\n\u274C Arquivo não encontrado, aperte enter para voltar. ");
        getchar();
        return 0;
    }

    char linha[100];
    float x,y,z;
    char categoria;

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

            //printf("%c  %f  %f  %f", (*vertices)[*totalVertices].categoria, (*vertices)[*totalVertices].x,
            //                            (*vertices)[*totalVertices].y, (*vertices)[*totalVertices].z);
        }else{
            printf("\n\u274C Linha %d inválida. Aperte enter para voltar. ", *totalVertices+1);
            limpaBuffer();
            getchar();
            return 0;
        }
    }

    fclose(arquivo);
    return 1;
}

int main(){

    double **matrizDistancias = NULL;
    int qtVertices = 0, qtAlocada = 10, continuar = 1, opcao, **matrizADJ = NULL;
    char nomeArquivo[100] = "db_204x3_v1.csv";
    //char nomeArquivo[100];
    char arquivoSaida[100] = "matriz_distancias.csv";
    No *vertices = malloc(sizeof(No) * qtAlocada);

    carregaArquivo(&vertices, &qtAlocada, &qtVertices, nomeArquivo);
    matrizDistancias = criaMatrizDistancias(vertices, qtVertices);
    imprimeMatriz(matrizDistancias, qtVertices, vertices);
    salvaMatrizArquivo(matrizDistancias, qtVertices, vertices, arquivoSaida);
    

    return 0;
}
