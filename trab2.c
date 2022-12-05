#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "trab2.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void converterEmBinario(char letra, int bin[8])
{
    int l = letra;
    int con = 7;
    for (int i = 0; i < 8; i++)
    {
        bin[i] = 0;
    }
    while (l > 0)
    {
        bin[con] = l % 2;
        con--;
        l = l / 2;
    }
    // for (int i = 0; i < 8; i++)
    // {
    //     printf("%d", bin[i]);
    // }
    // printf(" \n");
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int bitOndeDiferencia(char p1[30], char p2[30])
{
    char l1, l2;
    int bitsAnteriores = 0;
    for (int i = 0; i < 30; i++)
    {
        if (p1[i] != p2[i])
        {
            l1 = p1[i];
            l2 = p2[i];
            bitsAnteriores = 8 * (i);
            break;
        }
    }
    int bin1[8], bin2[8];
    converterEmBinario(l1, bin1);
    converterEmBinario(l2, bin2);
    for (int i = 0; i < 8; i++)
    {
        if (bin1[i] != bin2[i])
        {
            // printf("Diferencia no bit %d", (bitsAnteriores + i + 1));
            return bitsAnteriores + i + 1;
        }
    }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int getBit(char p[30], int numBit)
{
    numBit--;
    char l = p[numBit / 8];
    int bin[8];
    converterEmBinario(l, bin);
    if (numBit > 8)
        return bin[numBit - (numBit / 8) * 8];
    else
        return bin[numBit];
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void inserirPalavra(Palavra palavra, FILE *arvore, FILE *palavras)
{
    Infos infos;
    fseek(arvore, 0, SEEK_SET);
    fread(&infos, sizeof(Infos), 1, arvore);

    if (infos.init != 1) // nenhuma palavra inserida
    {
        infos.raiz = sizeof(Infos);
        infos.init = 1;
        fseek(arvore, 0, SEEK_SET);
        fwrite(&infos, sizeof(Infos), 1, arvore);

        Node no;
        no.bit = -1;
        no.pos = sizeof(Infos);
        no.pai = -1;
        no.posEsq = -1;
        no.posDir = -1;
        no.posPalavra = 0;
        fseek(arvore, sizeof(Infos), SEEK_SET);
        fwrite(&no, sizeof(Node), 1, arvore);

        fseek(palavras, 0, SEEK_SET);
        fwrite(&palavra, sizeof(Palavra), 1, palavras);
        printf("palavra inserida no dicionario: %s\n", palavra.palavra);
    }
    else if (infos.init == 1)
    {
        procurarOndeInserirPalavra(palavra, infos.raiz, arvore, palavras);
    }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void procurarOndeInserirPalavra(Palavra palavra, long pos, FILE *arvore, FILE *palavras)
{
    Node novoNoBit, novoNoPalavra;
    Node no;
    fseek(arvore, pos, SEEK_SET);
    fread(&no, sizeof(Node), 1, arvore);
    if (no.posPalavra != -1)
    { // folha
        Palavra palavra2;
        fseek(palavras, no.posPalavra, SEEK_SET);
        fread(&palavra2, sizeof(Palavra), 1, palavras);
        if (strcmp(palavra2.palavra, palavra.palavra) == 0) // palavra ja existente
        {
            printf("palavra ja existente: %s\n", palavra.palavra);
            return;
        }
        fseek(palavras, 0, SEEK_END);
        novoNoPalavra.posPalavra = ftell(palavras);
        fwrite(&palavra, sizeof(Palavra), 1, palavras); // gravando a palavra

        fseek(arvore, 0, SEEK_END);
        novoNoPalavra.bit = -1;
        novoNoPalavra.pos = ftell(arvore);
        novoNoPalavra.pai = ftell(arvore) + sizeof(Node);

        novoNoPalavra.posEsq = -1;
        novoNoPalavra.posDir = -1;
        fwrite(&novoNoPalavra, sizeof(Node), 1, arvore);
        fseek(arvore, no.pos, SEEK_SET);
        fwrite(&no, sizeof(Node), 1, arvore);
        novoNoBit.bit = bitOndeDiferencia(palavra.palavra, palavra2.palavra);
        novoNoBit.posPalavra = -1;
        novoNoBit.posEsq = -1;
        novoNoBit.posDir = -1;
        novoNoBit.pai = -1;
        int bit1, bit2;
        bit1 = getBit(palavra.palavra, novoNoBit.bit);
        bit2 = getBit(palavra2.palavra, novoNoBit.bit);
        // printf("palavra 1:%s , palavra 2:%s\n", palavra.palavra, palavra2.palavra);
        // printf("bit 1:%d , bit 2:%d\n", bit1, bit2);

        if (bit1 == 0)
        {
            novoNoBit.posEsq = novoNoPalavra.pos;
        }
        else if (bit1 == 1)
        {
            novoNoBit.posDir = novoNoPalavra.pos;
        }
        Infos infos;
        fseek(arvore, 0, SEEK_SET);
        fread(&infos, sizeof(Infos), 1, arvore);
        procurarOndeInserirNoBit(novoNoBit, palavra, infos.raiz, arvore, palavras);
        printf("palavra inserida no dicionario: %s\n", palavra.palavra);
    }
    else
    {
        int bit1 = getBit(palavra.palavra, no.bit);
        if (bit1 == 0)
        {
            procurarOndeInserirPalavra(palavra, no.posEsq, arvore, palavras);
        }
        else if (bit1 == 1)
        {
            procurarOndeInserirPalavra(palavra, no.posDir, arvore, palavras);
        }
    }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void procurarOndeInserirNoBit(Node noBit, Palavra palavra, long pos, FILE *arvore, FILE *palavras)
{
    Infos infos;
    fseek(arvore, 0, SEEK_SET);
    fread(&infos, sizeof(Infos), 1, arvore);

    Node no, noPai;
    fseek(arvore, pos, SEEK_SET);
    fread(&no, sizeof(Node), 1, arvore);

    //  (pos==infos.raiz && no.posPalavra==-1 && no.bit<noBit.bit)
    if ((pos == infos.raiz && no.posPalavra != -1))
    { // raiz no palavra

        if (noBit.posEsq == -1)
        {
            
            noBit.posEsq = infos.raiz;
        }
        else if (noBit.posDir == -1)
        {
            
            noBit.posDir = infos.raiz;
        }
        noBit.pai = -1;
        fseek(arvore, 0, SEEK_END);
        noBit.pos = ftell(arvore);

        fwrite(&noBit, sizeof(Node), 1, arvore);
        infos.raiz = noBit.pos;
        fseek(arvore, 0, SEEK_SET);
        fwrite(&infos, sizeof(Infos), 1, arvore);

        no.pai = noBit.pos;
        fseek(arvore, pos, SEEK_SET);
        fwrite(&no, sizeof(Node), 1, arvore);
    }
    else
    {
        if (no.bit != -1)
        {
            if (no.bit > noBit.bit)
            {
                if (noBit.posEsq == -1)
                {
                    
                    noBit.posEsq = no.pos;
                }
                else if (noBit.posDir == -1)
                {
                    
                    noBit.posDir = no.pos;
                }
                fseek(arvore, 0, SEEK_END);
                noBit.pos = ftell(arvore);
                long paiOriginal = no.pai;
                if (infos.raiz == no.pos)
                {
                    noBit.pai = -1;
                    infos.raiz = noBit.pos;
                    fseek(arvore, 0, SEEK_SET);
                    fwrite(&infos, sizeof(Infos), 1, arvore);
                }
                else
                {
                    fseek(arvore, no.pai, SEEK_SET);
                    fread(&noPai, sizeof(Node), 1, arvore);
                    if (noPai.posEsq == no.pos)
                    {
                        noPai.posEsq = noBit.pos;
                    }
                    else if (noPai.posDir == no.pos)
                    {
                        noPai.posDir = noBit.pos;
                    }
                    noBit.pai = noPai.pos;
                    fseek(arvore, noPai.pos, SEEK_SET);
                    fwrite(&noPai, sizeof(Node), 1, arvore);
                }
                no.pai = noBit.pos;
                fseek(arvore, no.pos, SEEK_SET);
                fwrite(&no, sizeof(Node), 1, arvore);

                fseek(arvore, 0, SEEK_END);
                fwrite(&noBit, sizeof(Node), 1, arvore);
            }
            else
            {
                if (no.bit < noBit.bit)
                {
                    int bitAux = getBit(palavra.palavra, no.bit);
                    
                    if (bitAux == 0)
                    {

                        procurarOndeInserirNoBit(noBit, palavra, no.posEsq, arvore, palavras);
                        return;
                    }
                    else if (bitAux == 1)
                    {
                        
                        procurarOndeInserirNoBit(noBit, palavra, no.posDir, arvore, palavras);
                        return;
                    }
                }
            }
        }
        else
        {
            fseek(arvore, pos, SEEK_SET);
            fread(&no, sizeof(Node), 1, arvore);
            fseek(arvore, 0, SEEK_END);
            fseek(arvore, no.pai, SEEK_SET);
            fread(&noPai, sizeof(Node), 1, arvore);
            if (noBit.posEsq == -1)
            {
                noBit.posEsq = no.pos;
            }
            else if (noBit.posDir == -1)
            {
                noBit.posDir = no.pos;
            }
            long paiOriginal = no.pai;

            fseek(arvore, 0, SEEK_END);
            noBit.pos = ftell(arvore);

            
            if (noPai.posEsq == no.pos)
            {
                
                noPai.posEsq = noBit.pos;
            }
            else if (noPai.posDir == no.pos)
            {
                
                noPai.posDir = noBit.pos;
            }
            noBit.pai = paiOriginal;
            fseek(arvore, paiOriginal, SEEK_SET);
            fwrite(&noPai, sizeof(Node), 1, arvore);

            no.pai = noBit.pos;
            fseek(arvore, no.pos, SEEK_SET);
            fwrite(&no, sizeof(Node), 1, arvore);

            fseek(arvore, 0, SEEK_END);
            fwrite(&noBit, sizeof(Node), 1, arvore);
        }
    }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void procurarPalavra(char comando, Palavra palavra, long pos, FILE *arvore, FILE *palavras)
{
    Infos infos;
    fseek(arvore, 0, SEEK_SET);
    fread(&infos, sizeof(Infos), 1, arvore);
    if (infos.init != 1) // palavra nao encontrada
    {
        printf("palavra inexistente no dicionario: %s\n", palavra.palavra);
        return;
    }
    Node no;
    fseek(arvore, pos, SEEK_SET);
    fread(&no, sizeof(Node), 1, arvore);

    if (no.posPalavra != -1)
    { // folha

        Palavra palavra2;
        fseek(palavras, no.posPalavra, SEEK_SET);
        fread(&palavra2, sizeof(Palavra), 1, palavras);

        if (strcmp(palavra2.palavra, palavra.palavra) == 0) // achou a palavra
        {
            if (comando == 't')
            { // traducoes
                printf("traducoes da palavra: %s\n", palavra2.palavra);
                for (int i = 0; i < palavra2.numTraducoes; i++)
                {
                    printf("%s\n", palavra2.traducoes[i]);
                }
            }
            else if (comando == 'c')
            { // classe
                if (palavra2.classe[0] == 'a')
                    printf("classe da palavra: %s: adjetivo\n", palavra2.palavra);
                else if (palavra2.classe[0] == 'v')
                    printf("classe da palavra: %s: verbo\n", palavra2.palavra);
                else if (palavra2.classe[0] == 's')
                    printf("classe da palavra: %s: substantivo\n", palavra2.palavra);
            }
        }
        else // palavra nao encontrada
        {
            printf("palavra inexistente no dicionario: %s\n", palavra.palavra);
        }
    }
    else
    {
        int bit1 = getBit(palavra.palavra, no.bit);
        if (bit1 == 0)
        {
            procurarPalavra(comando, palavra, no.posEsq, arvore, palavras);
        }
        else if (bit1 == 1)
        {
            procurarPalavra(comando, palavra, no.posDir, arvore, palavras);
        }
    }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void removerPalavra(Palavra palavra, long pos, FILE *arvore, FILE *palavras)
{
    Infos infos;
    fseek(arvore, 0, SEEK_SET);
    fread(&infos, sizeof(Infos), 1, arvore);
    if (infos.init != 1)
    {
        printf("palavra inexistente no dicionario: %s\n", palavra.palavra);
        return;
    }

    Node no;
    fseek(arvore, pos, SEEK_SET);
    fread(&no, sizeof(Node), 1, arvore);

    if (no.posPalavra != -1)
    { // folha

        Palavra palavra2;
        fseek(palavras, no.posPalavra, SEEK_SET);
        fread(&palavra2, sizeof(Palavra), 1, palavras);
        if (strcmp(palavra2.palavra, palavra.palavra) == 0)
        {
            // achou e remover
            if (no.pos == infos.raiz)
            { // raiz de uma unica palavra
                infos.raiz = -1;
                infos.init = -1;
                fseek(arvore, 0, SEEK_SET);
                fwrite(&infos, sizeof(Infos), 1, arvore);
                printf("palavra removida: %s\n", palavra.palavra);
                return;
            }

            Node noPai, noIrmao;
            fseek(arvore, no.pai, SEEK_SET);
            fread(&noPai, sizeof(Node), 1, arvore);
            if (noPai.pos == infos.raiz)
            { // removendo raiz
                if (noPai.posEsq == no.pos)
                {
                    infos.raiz = noPai.posDir;
                    fseek(arvore, noPai.posDir, SEEK_SET);
                    fread(&noIrmao, sizeof(Node), 1, arvore);
                    noIrmao.pai = -1;
                }
                else if (noPai.posDir == no.pos)
                {
                    infos.raiz = noPai.posEsq;
                    fseek(arvore, noPai.posEsq, SEEK_SET);
                    fread(&noIrmao, sizeof(Node), 1, arvore);
                    noIrmao.pai = -1;
                }

                fseek(arvore, 0, SEEK_SET);
                fwrite(&infos, sizeof(Infos), 1, arvore);
                fseek(arvore, noIrmao.pos, SEEK_SET);
                fread(&noIrmao, sizeof(Node), 1, arvore);
                printf("palavra removida: %s\n", palavra.palavra);
                return;
            }
            Node noAvo;
            fseek(arvore, noPai.pai, SEEK_SET);
            fread(&noAvo, sizeof(Node), 1, arvore);

            if (noAvo.posEsq == noPai.pos)
            {
                if (noPai.posEsq == no.pos)
                {
                    noAvo.posEsq = noPai.posDir;
                    fseek(arvore, noPai.posDir, SEEK_SET);
                    fread(&noIrmao, sizeof(Node), 1, arvore);
                }
                else if (noPai.posDir == no.pos)
                {
                    noAvo.posEsq = noPai.posEsq;
                    fseek(arvore, noPai.posEsq, SEEK_SET);
                    fread(&noIrmao, sizeof(Node), 1, arvore);
                }
            }

            else if (noAvo.posDir == noPai.pos)
            {
                if (noPai.posEsq == no.pos)
                {
                    noAvo.posDir = noPai.posDir;
                    fseek(arvore, noPai.posDir, SEEK_SET);
                    fread(&noIrmao, sizeof(Node), 1, arvore);
                }
                else if (noPai.posDir == no.pos)
                {
                    noAvo.posDir = noPai.posEsq;
                    fseek(arvore, noPai.posEsq, SEEK_SET);
                    fread(&noIrmao, sizeof(Node), 1, arvore);
                }
            }
            noIrmao.pai = noAvo.pos;
            fseek(arvore, noAvo.pos, SEEK_SET);
            fwrite(&noAvo, sizeof(Node), 1, arvore);
            fseek(arvore, noIrmao.pos, SEEK_SET);
            fwrite(&noIrmao, sizeof(Node), 1, arvore);
            printf("palavra removida: %s\n", palavra.palavra);
        }
        else
        {
            printf("palavra inexistente no dicionario: %s\n", palavra.palavra);
        }

        Infos infos;
        fseek(arvore, 0, SEEK_SET);
        fread(&infos, sizeof(Infos), 1, arvore);
    }
    else
    {
        int bit1 = getBit(palavra.palavra, no.bit);
        if (bit1 == 0)
        {
            removerPalavra(palavra, no.posEsq, arvore, palavras);
        }
        else if (bit1 == 1)
        {
            removerPalavra(palavra, no.posDir, arvore, palavras);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void printNode(long pos, FILE *arvore, FILE *palavras)
{
    Node no, noDir, noEsq;
    Palavra palavra;
    fseek(arvore, pos, SEEK_SET);
    fread(&no, sizeof(Node), 1, arvore);
    if (no.bit != -1)
    {
        fseek(arvore, no.posEsq, SEEK_SET);
        fread(&noEsq, sizeof(Node), 1, arvore);
        fseek(arvore, no.posDir, SEEK_SET);
        fread(&noDir, sizeof(Node), 1, arvore);
        printf("bit: %d ", no.bit);
        if (noEsq.bit != -1)
        {
            printf("fesq: %d ", noEsq.bit);
        }
        else
        {
            fseek(palavras, noEsq.posPalavra, SEEK_SET);
            fread(&palavra, sizeof(Palavra), 1, palavras);
            printf("fesq: %s ", palavra.palavra);
        }
        if (noDir.bit != -1)
        {
            printf("fdir: %d\n", noDir.bit);
        }
        else
        {
            fseek(palavras, noDir.posPalavra, SEEK_SET);
            fread(&palavra, sizeof(Palavra), 1, palavras);
            printf("fdir: %s\n", palavra.palavra);
        }
    }
    else
    {
        fseek(palavras, no.posPalavra, SEEK_SET);
        fread(&palavra, sizeof(Palavra), 1, palavras);
        printf("%s\n", palavra.palavra);
    }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void printNodePalavras(long pos, FILE *arvore, FILE *palavras)
{
    Node no, noDir, noEsq;
    Palavra palavra;
    fseek(arvore, pos, SEEK_SET);
    fread(&no, sizeof(Node), 1, arvore);
    if (no.bit == -1)
    {
        fseek(palavras, no.posPalavra, SEEK_SET);
        fread(&palavra, sizeof(Palavra), 1, palavras);
        printf("%s\n", palavra.palavra);
    }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void printNodePalavrasPorClasse(char classe, long pos, FILE *arvore, FILE *palavras)
{
    Node no, noDir, noEsq;
    Palavra palavra;
    fseek(arvore, pos, SEEK_SET);
    fread(&no, sizeof(Node), 1, arvore);
    if (no.bit == -1)
    {
        fseek(palavras, no.posPalavra, SEEK_SET);
        fread(&palavra, sizeof(Palavra), 1, palavras);

        if (palavra.classe[0] == classe)
            printf("%s\n", palavra.palavra);
    }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void printNodePalavrasEBinario(long pos, FILE *arvore, FILE *palavras)
{
    Node no, noDir, noEsq;
    Palavra palavra;
    fseek(arvore, pos, SEEK_SET);
    fread(&no, sizeof(Node), 1, arvore);
    if (no.bit == -1)
    {
        fseek(palavras, no.posPalavra, SEEK_SET);
        fread(&palavra, sizeof(Palavra), 1, palavras);

        int bin[8];
        printf("%s ", palavra.palavra);
        for (int i = 0; i < 30; i++)
        {
            char letraAux = palavra.palavra[i];
            if (letraAux == 00000000)
                break;
            printf(" ");
            converterEmBinario(letraAux, bin);
            char checkandoFimPalavra = 0;

            for (int j = 0; j < 8; j++)
            {
                printf("%d", bin[j]);
            }
        }
        printf(" 00000000");
        printf("\n");
    }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void imprimirCrescente(FILE *arvore, FILE *palavras)
{
    long vetor[1000];
    Node centro, esq, dir;
    int con = -1;
    Infos infos;
    fseek(arvore, 0, SEEK_SET);
    fread(&infos, sizeof(Infos), 1, arvore);
    if (infos.init == 1)
    {
        fseek(arvore, infos.raiz, SEEK_SET);
        fread(&centro, sizeof(Node), 1, arvore);
        if (centro.posPalavra != -1)
        {
            Palavra palavra;
            fseek(palavras, centro.posPalavra, SEEK_SET);
            fread(&palavra, sizeof(Palavra), 1, palavras);
            printNodePalavras(infos.raiz, arvore, palavras);
            return;
        }
        printNodePalavras(infos.raiz, arvore, palavras);
        con++;
        vetor[con] = centro.posDir;
        con++;
        vetor[con] = centro.posEsq;
        while (con > -1)
        {
            fseek(arvore, vetor[con], SEEK_SET);
            fread(&centro, sizeof(Node), 1, arvore);
            printNodePalavras(vetor[con], arvore, palavras);
            if (centro.bit != -1)
            {
                vetor[con] = centro.posDir;
                con++;
                vetor[con] = centro.posEsq;
            }
            else
            {
                con--;
            }
        }
    }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void imprimirDecrescente(FILE *arvore, FILE *palavras)
{
    long vetor[1000];
    Node centro, esq, dir;
    int con = -1;
    Infos infos;
    fseek(arvore, 0, SEEK_SET);
    fread(&infos, sizeof(Infos), 1, arvore);
    if (infos.init == 1)
    {
        fseek(arvore, infos.raiz, SEEK_SET);
        fread(&centro, sizeof(Node), 1, arvore);
        if (centro.posPalavra != -1)
        {
            Palavra palavra;
            fseek(palavras, centro.posPalavra, SEEK_SET);
            fread(&palavra, sizeof(Palavra), 1, palavras);
            printNodePalavras(infos.raiz, arvore, palavras);
            return;
        }
        printNodePalavras(infos.raiz, arvore, palavras);
        con++;
        vetor[con] = centro.posEsq;
        con++;
        vetor[con] = centro.posDir;
        while (con > -1)
        {
            fseek(arvore, vetor[con], SEEK_SET);
            fread(&centro, sizeof(Node), 1, arvore);
            printNodePalavras(vetor[con], arvore, palavras);
            if (centro.bit != -1)
            {
                vetor[con] = centro.posEsq;
                con++;
                vetor[con] = centro.posDir;
            }
            else
            {
                con--;
            }
        }
    }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void imprimirCrescentePorClasse(char classe, FILE *arvore, FILE *palavras)
{
    long vetor[1000];
    Node centro, esq, dir;
    int con = -1;
    Infos infos;
    fseek(arvore, 0, SEEK_SET);
    fread(&infos, sizeof(Infos), 1, arvore);
    if (infos.init == 1)
    {
        fseek(arvore, infos.raiz, SEEK_SET);
        fread(&centro, sizeof(Node), 1, arvore);
        if (centro.posPalavra != -1)
        {
            Palavra palavra;
            fseek(palavras, centro.posPalavra, SEEK_SET);
            fread(&palavra, sizeof(Palavra), 1, palavras);
            printNodePalavrasPorClasse(classe, infos.raiz, arvore, palavras);
            return;
        }
        printNodePalavrasPorClasse(classe, infos.raiz, arvore, palavras);
        con++;
        vetor[con] = centro.posDir;
        con++;
        vetor[con] = centro.posEsq;
        while (con > -1)
        {
            fseek(arvore, vetor[con], SEEK_SET);
            fread(&centro, sizeof(Node), 1, arvore);
            printNodePalavrasPorClasse(classe, vetor[con], arvore, palavras);
            if (centro.bit != -1)
            {
                vetor[con] = centro.posDir;
                con++;
                vetor[con] = centro.posEsq;
            }
            else
            {
                con--;
            }
        }
    }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void imprimirDecrescentePorClasse(char classe, FILE *arvore, FILE *palavras)
{
    long vetor[1000];
    Node centro, esq, dir;
    int con = -1;
    Infos infos;
    fseek(arvore, 0, SEEK_SET);
    fread(&infos, sizeof(Infos), 1, arvore);
    if (infos.init == 1)
    {
        fseek(arvore, infos.raiz, SEEK_SET);
        fread(&centro, sizeof(Node), 1, arvore);
        if (centro.posPalavra != -1)
        {
            Palavra palavra;
            fseek(palavras, centro.posPalavra, SEEK_SET);
            fread(&palavra, sizeof(Palavra), 1, palavras);
            printNodePalavrasPorClasse(classe, infos.raiz, arvore, palavras);
            return;
        }
        printNodePalavrasPorClasse(classe, infos.raiz, arvore, palavras);
        con++;
        vetor[con] = centro.posEsq;
        con++;
        vetor[con] = centro.posDir;
        while (con > -1)
        {
            fseek(arvore, vetor[con], SEEK_SET);
            fread(&centro, sizeof(Node), 1, arvore);
            printNodePalavrasPorClasse(classe, vetor[con], arvore, palavras);
            if (centro.bit != -1)
            {
                vetor[con] = centro.posEsq;
                con++;
                vetor[con] = centro.posDir;
            }
            else
            {
                con--;
            }
        }
    }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void imprimirPreOrdem(FILE *arvore, FILE *palavras)
{
    long vetor[10000];
    Node centro, esq, dir;
    int con = -1;
    Infos infos;
    fseek(arvore, 0, SEEK_SET);
    fread(&infos, sizeof(Infos), 1, arvore);
    if (infos.init == 1)
    {

        fseek(arvore, infos.raiz, SEEK_SET);
        fread(&centro, sizeof(Node), 1, arvore);
        if (centro.posPalavra != -1)
        {
            Palavra palavra;
            fseek(palavras, centro.posPalavra, SEEK_SET);
            fread(&palavra, sizeof(Palavra), 1, palavras);
            printf("%s\n", palavra.palavra);
            return;
        }
        printNode(infos.raiz, arvore, palavras);
        con++;
        vetor[con] = centro.posDir;
        con++;
        vetor[con] = centro.posEsq;
        while (con > -1)
        {
            fseek(arvore, vetor[con], SEEK_SET);
            fread(&centro, sizeof(Node), 1, arvore);
            printNode(vetor[con], arvore, palavras);
            if (centro.bit != -1)
            {
                vetor[con] = centro.posDir;
                con++;
                vetor[con] = centro.posEsq;
            }
            else
            {
                con--;
            }
        }
    }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void imprimirPreOrdemPalavrasEBinarios(FILE *arvore, FILE *palavras)
{
    long vetor[1000];
    Node centro, esq, dir;
    int con = -1;
    Infos infos;
    fseek(arvore, 0, SEEK_SET);
    fread(&infos, sizeof(Infos), 1, arvore);
    if (infos.init == 1)
    {
        fseek(arvore, infos.raiz, SEEK_SET);
        fread(&centro, sizeof(Node), 1, arvore);
        if (centro.posPalavra != -1)
        {
            Palavra palavra;
            fseek(palavras, centro.posPalavra, SEEK_SET);
            fread(&palavra, sizeof(Palavra), 1, palavras);
            printNodePalavrasEBinario(infos.raiz, arvore, palavras);
            return;
        }
        printNodePalavrasEBinario(infos.raiz, arvore, palavras);
        con++;
        vetor[con] = centro.posDir;
        con++;
        vetor[con] = centro.posEsq;
        while (con > -1)
        {
            fseek(arvore, vetor[con], SEEK_SET);
            fread(&centro, sizeof(Node), 1, arvore);
            printNodePalavrasEBinario(vetor[con], arvore, palavras);
            if (centro.bit != -1)
            {
                vetor[con] = centro.posDir;
                con++;
                vetor[con] = centro.posEsq;
            }
            else
            {
                con--;
            }
        }
    }
}
int lestring(char s[], int max)
{
    int i = 0;
    char letra;
    for (i = 0; i < (max - 1); i++)
    {
        letra = fgetc(stdin);
        if ((letra == '\n') && (i == 0))
        {
            i = i - 1;
            continue;
        }
        if (letra == '\n')
            break;
        s[i] = letra;
    }
    s[i] = 0;
    return (i);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int main()
{
    FILE *arvore = fopen("arvore.dat", "rb+");
    FILE *palavras = fopen("registros.dat", "rb+");
    Palavra palavra;
    char comando;
    while ((comando != 'e'))
    {

        fflush(stdin);

        scanf("%c", &comando);
        if (comando == 'e')
        {
            break;
        }
        else if (comando == 'i')
        {

            fflush(stdin);
            lestring(palavra.palavra, 30);
            // scanf("%s", palavra.palavra);

            fflush(stdin);

            scanf("%s", palavra.classe);

            fflush(stdin);

            scanf("%d", &palavra.numTraducoes);
            for (int i = 0; i < palavra.numTraducoes; i++)
            {

                fflush(stdin);
                lestring(palavra.traducoes[i], 50);
                // scanf("%s", palavra.traducoes[i]);
            }

            inserirPalavra(palavra, arvore, palavras);
        }
        else if (comando == 't')
        {

            fflush(stdin);
            lestring(palavra.palavra, 30);
            //scanf("%s", palavra.palavra);

            Infos infos;
            fseek(arvore, 0, SEEK_SET);
            fread(&infos, sizeof(Infos), 1, arvore);
            procurarPalavra(comando, palavra, infos.raiz, arvore, palavras);
        }
        else if (comando == 'c')
        {
            // consultando classe

            fflush(stdin);
            lestring(palavra.palavra, 30);
            //scanf("%s", palavra.palavra);

            Infos infos;
            fseek(arvore, 0, SEEK_SET);
            fread(&infos, sizeof(Infos), 1, arvore);
            procurarPalavra(comando, palavra, infos.raiz, arvore, palavras);
        }
        else if (comando == 'p')
        {
            imprimirPreOrdem(arvore, palavras);
            imprimirPreOrdemPalavrasEBinarios(arvore, palavras);
        }
        else if (comando == 'r')
        {

            fflush(stdin);
            lestring(palavra.palavra, 30);
            //scanf("%s", palavra.palavra);
            Infos infos;
            fseek(arvore, 0, SEEK_SET);
            fread(&infos, sizeof(Infos), 1, arvore);
            removerPalavra(palavra, infos.raiz, arvore, palavras);
        }
        else if (comando == 'l')
        {
            
            char ordem[2];

            fflush(stdin);

            scanf("%s", ordem);
            if (ordem[0] == 'c')
                imprimirCrescente(arvore, palavras);
            else if (ordem[0] == 'd')
                imprimirDecrescente(arvore, palavras);
        }
        else if (comando == 'a')
        {
            char classe[2];

            fflush(stdin);
            scanf("%s", classe);
            char ordem[2];

            fflush(stdin);
            scanf("%s", ordem);
            if (ordem[0] == 'c')
                imprimirCrescentePorClasse(classe[0], arvore, palavras);
            else if (ordem[0] == 'd')
                imprimirDecrescentePorClasse(classe[0], arvore, palavras);
        }
    }
    fclose(arvore);
    fclose(palavras);
    return 0;
}
