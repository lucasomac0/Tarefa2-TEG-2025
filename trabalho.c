#include <stdio.h>
#include <stdlib.h>

typedef struct no{
    float x, y, z;
    int indice;
    char categoria;
}No;


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

    fgets(linha, sizeof(linha), arquivo); // pula o cabecalho do arquivo

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

            printf("%c  %f  %f  %f", (*vertices)[*totalVertices].categoria, (*vertices)[*totalVertices].x,
                                        (*vertices)[*totalVertices].y, (*vertices)[*totalVertices].z);
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

    int qtVertices = 0, qtAlocada = 10, continuar = 1, opcao, **matrizADJ = NULL;
    char nomeArquivo[100] = "db_204x3_v1.csv";
    //char nomeArquivo[100];
    No *vertices = malloc(sizeof(No) * qtAlocada);

    carregaArquivo(&vertices, &qtAlocada, &qtVertices, nomeArquivo);

    return 0;
}
