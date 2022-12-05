#ifndef  TRAB2_H
#define  TRAB2_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef struct
{
    long raiz;
    int init;
} Infos;

typedef struct
{
    char palavra[30];
    int numTraducoes;
    char classe[1];
    char traducoes[10][50];
} Palavra;

typedef struct
{
    int bit;
    long pai;
    long pos;
    long posEsq;
    long posDir;
    long posPalavra;
} Node;

void converterEmBinario(char letra, int bin[8]);
int bitOndeDiferencia(char p1[30], char p2[30]);
int getBit(char p[30], int numBit);
void inserirPalavra(Palavra palavra, FILE *arvore, FILE *palavras);
void procurarOndeInserirPalavra(Palavra palavra, long pos, FILE *arvore, FILE *palavras);
void procurarOndeInserirNoBit(Node noBit, Palavra palavra, long pos, FILE *arvore, FILE *palavras);
void procurarPalavra(char comando, Palavra palavra, long pos, FILE *arvore, FILE *palavras);
void removerPalavra(Palavra palavra, long pos, FILE *arvore, FILE *palavras);
void printNode(long pos, FILE *arvore, FILE *palavras);
void printNodePalavras(long pos, FILE *arvore, FILE *palavras);
void printNodePalavrasPorClasse(char classe,long pos, FILE *arvore, FILE *palavras);
void printNodePalavrasEBinario(long pos, FILE *arvore, FILE *palavras);
void imprimirCrescente(FILE *arvore, FILE *palavras);
void imprimirDecrescente(FILE *arvore, FILE *palavras);
void imprimirCrescentePorClasse(char classe,FILE *arvore, FILE *palavras);
void imprimirDecrescentePorClasse(char classe,FILE *arvore, FILE *palavras);
void imprimirPreOrdem(FILE *arvore, FILE *palavras);
void imprimirPreOrdemPalavrasEBinarios(FILE *arvore, FILE *palavras);
int lestring(char s[], int max);
#endif
