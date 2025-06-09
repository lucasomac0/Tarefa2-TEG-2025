#ifndef MINHEAP_H
#define MINHEAP_H

#include <stdio.h>
#include <stdlib.h>

// Estrutura para o nó do heap
typedef struct {
    double dist;
    int vertice;
} HeapNode;

// Estrutura do MinHeap
typedef struct {
    HeapNode *nodes;
    int size;
    int capacity;
} MinHeap;

// Função auxiliar para trocar dois nós
static void trocaNodes(HeapNode *a, HeapNode *b) {
    HeapNode temp = *a;
    *a = *b;
    *b = temp;
}

// Função auxiliar para reorganizar o heap subindo
static void heapifyUp(MinHeap *heap, int idx) {
    if (idx && heap->nodes[(idx-1)/2].dist > heap->nodes[idx].dist) {
        trocaNodes(&heap->nodes[(idx-1)/2], &heap->nodes[idx]);
        heapifyUp(heap, (idx-1)/2);
    }
}

// Função auxiliar para reorganizar o heap descendo
static void heapifyDown(MinHeap *heap, int idx) {
    int menor = idx;
    int esquerda = 2*idx + 1;
    int direita = 2*idx + 2;
    
    if (esquerda < heap->size && heap->nodes[esquerda].dist < heap->nodes[menor].dist)
        menor = esquerda;
    
    if (direita < heap->size && heap->nodes[direita].dist < heap->nodes[menor].dist)
        menor = direita;
    
    if (menor != idx) {
        trocaNodes(&heap->nodes[idx], &heap->nodes[menor]);
        heapifyDown(heap, menor);
    }
}

// Cria um novo MinHeap com capacidade especificada
MinHeap* criaHeap(int capacity) {
    if (capacity <= 0) return NULL;
    
    MinHeap *heap = malloc(sizeof(MinHeap));
    if (heap == NULL) return NULL;
    
    heap->nodes = malloc(sizeof(HeapNode) * capacity);
    if (heap->nodes == NULL) {
        free(heap);
        return NULL;
    }
    
    heap->size = 0;
    heap->capacity = capacity;
    return heap;
}

// Libera a memória do heap
void liberaHeap(MinHeap *heap) {
    if (heap != NULL) {
        free(heap->nodes);
        free(heap);
    }
}

// Insere um novo elemento no heap
void insereHeap(MinHeap *heap, double dist, int vertice) {
    if (heap == NULL || heap->size >= heap->capacity) {
        return; // Heap cheio ou inválido
    }
    
    heap->nodes[heap->size].dist = dist;
    heap->nodes[heap->size].vertice = vertice;
    heap->size++;
    heapifyUp(heap, heap->size - 1);
}

// Extrai o elemento mínimo do heap
HeapNode extraiMin(MinHeap *heap) {
    HeapNode min = {-1, -1}; // Valor inválido como padrão
    
    if (heap == NULL || heap->size == 0) {
        return min;
    }
    
    min = heap->nodes[0];
    heap->nodes[0] = heap->nodes[heap->size - 1];
    heap->size--;
    
    if (heap->size > 0) {
        heapifyDown(heap, 0);
    }
    
    return min;
}

// Verifica se o heap está vazio
int heapVazio(MinHeap *heap) {
    return (heap == NULL || heap->size == 0);
}

// Verifica se o heap está cheio
int heapCheio(MinHeap *heap) {
    return (heap != NULL && heap->size >= heap->capacity);
}

// Limpa o heap (remove todos os elementos)
void limpaHeap(MinHeap *heap) {
    if (heap != NULL) {
        heap->size = 0;
    }
}



#endif // MINHEAP_H