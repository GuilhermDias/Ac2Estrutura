#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <windows.h>
#include "filaPDF.h"


int terminaComPDF(const char *nome) {
    const char *ext = strrchr(nome, '.');
    return (ext && _stricmp(ext, ".pdf") == 0);
}

long obter_tamanho_arquivo(const char *caminho) {
    struct stat st;
    if (stat(caminho, &st) == 0)
        return st.st_size;
    return -1;
}

int obter_paginas_pdf(const char *caminho_pdf) {
    char comando[1024];
    snprintf(comando, sizeof(comando),
             "cmd /C \"\"%s\" \"%s\" > temp_info.txt\"",
             CAMINHO_PDFINFO, caminho_pdf);

    int status = system(comando);
    if (status != 0) {
        fprintf(stderr, "Erro ao executar pdfinfo para %s\n", caminho_pdf);
        return -1;
    }

    FILE *fp = fopen("temp_info.txt", "r");
    if (!fp) {
        perror("Erro ao abrir temp_info.txt");
        return -1;
    }

    char linha[512];
    int paginas = -1;
    while (fgets(linha, sizeof(linha), fp)) {
        if (strncmp(linha, "Pages:", 6) == 0) {
            sscanf(linha + 6, "%d", &paginas);
            break;
        }
    }

    fclose(fp);
    return paginas;
}

int carregar_arquivos_pdf(PDFInfo fila[]) {
    DIR *dir;
    struct dirent *entrada;
    int contador = 0;

    DWORD tempo_inicio = GetTickCount();

    dir = opendir(CAMINHO_PASTA);
    if (!dir) {
        perror("Erro ao abrir a pasta");
        return 0;
    }

    while ((entrada = readdir(dir)) != NULL && contador < MAX_PDFS) {
        if (entrada->d_type == DT_REG && terminaComPDF(entrada->d_name)) {
            char caminho_pdf[1024];
            snprintf(caminho_pdf, sizeof(caminho_pdf), "%s\\%s", CAMINHO_PASTA, entrada->d_name);

            int paginas = obter_paginas_pdf(caminho_pdf);
            long tamanho = obter_tamanho_arquivo(caminho_pdf);

            if (paginas == -1 || tamanho == -1) {
                printf("\nErro ao processar arquivo: %s\n", entrada->d_name);
                continue;
            }

            PDFInfo info;
            strncpy(info.nome_arquivo, entrada->d_name, sizeof(info.nome_arquivo) - 1);
            info.nome_arquivo[sizeof(info.nome_arquivo) - 1] = '\0';
            info.num_paginas = paginas;
            info.tamanho_bytes = tamanho;
            info.tempo_impressao = paginas * TEMPO_POR_PAGINA;

            fila[contador++] = info;
        }
    }

    closedir(dir);
    DWORD tempo_fim = GetTickCount();
    printf("\nTempo de inserção na fila: %lu ms\n", tempo_fim - tempo_inicio);

    return contador;
}

void imprimir_fila(PDFInfo fila[], int qtd) {
    DWORD inicio = GetTickCount();
    int tempo_total = 0;

    printf("\n--- INICIANDO IMPRESSÃO ---\n");
    for (int i = 0; i < qtd; i++) {
        printf("Imprimindo: %s | Páginas: %d | Tempo estimado: %d segundos\n",
               fila[i].nome_arquivo, fila[i].num_paginas, fila[i].tempo_impressao);

        Sleep(fila[i].tempo_impressao * 1000);
        tempo_total += fila[i].tempo_impressao;
    }

    DWORD fim = GetTickCount();
    DWORD duracao = fim - inicio;

    printf("\n--- IMPRESSÃO CONCLUÍDA ---\n");
    printf("Tempo real de impressão: %lu ms (%.2f segundos)\n", duracao, duracao / 1000.0);
    printf("Tempo estimado total: %d segundos (%.2f minutos)\n", tempo_total, tempo_total / 60.0);
}

int comparar_tamanho(const void *a, const void *b) {
    PDFInfo *pdfA = (PDFInfo *)a;
    PDFInfo *pdfB = (PDFInfo *)b;
    return (int)(pdfA->tamanho_bytes - pdfB->tamanho_bytes);
}

void imprimir_ordenado_por_tamanho(PDFInfo fila[], int qtd) {
    PDFInfo copia[MAX_PDFS];
    memcpy(copia, fila, qtd * sizeof(PDFInfo));
    qsort(copia, qtd, sizeof(PDFInfo), comparar_tamanho);

    DWORD inicio = GetTickCount();
    int tempo_total = 0;

    printf("\n--- IMPRESSÃO ORDENADA POR TAMANHO ---\n");
    for (int i = 0; i < qtd; i++) {
        printf("Imprimindo: %s | Tamanho: %.2f KB | Páginas: %d | Tempo: %d s\n",
               copia[i].nome_arquivo, copia[i].tamanho_bytes / 1024.0,
               copia[i].num_paginas, copia[i].tempo_impressao);
        Sleep(copia[i].tempo_impressao * 1000);
        tempo_total += copia[i].tempo_impressao;
    }

    DWORD fim = GetTickCount();
    DWORD duracao = fim - inicio;

    printf("\nTempo total real: %lu ms (%.2f s)\n", duracao, duracao / 1000.0);
    printf("Tempo estimado total: %d s (%.2f min)\n", tempo_total, tempo_total / 60.0);
}

int remover_fila(PDFInfo fila[], int qtd) {
    if (qtd == 0) {
        printf("Fila vazia.\n");
        return 0;
    }

    DWORD inicio = GetTickCount();
    printf("\nRemovendo todos os arquivos da fila (FIFO)...\n");

    for (int i = 0; i < qtd; i++) {
        printf("Removido: %s\n", fila[i].nome_arquivo);
    }

    DWORD fim = GetTickCount();
    printf("Tempo total de remoção: %lu ms\n", fim - inicio);

    return 0; // fila esvaziada
}

int comparar_tamanho_desc(const void *a, const void *b) {
    PDFInfo *pdfA = (PDFInfo *)a;
    PDFInfo *pdfB = (PDFInfo *)b;
    return (int)(pdfB->tamanho_bytes - pdfA->tamanho_bytes);
}

int remover_ordenado_por_tamanho(PDFInfo fila[], int qtd) {
    if (qtd == 0) {
        printf("Fila vazia.\n");
        return 0;
    }

    PDFInfo copia[MAX_PDFS];
    memcpy(copia, fila, qtd * sizeof(PDFInfo));
    qsort(copia, qtd, sizeof(PDFInfo), comparar_tamanho_desc);

    DWORD inicio = GetTickCount();
    printf("\nRemovendo arquivos ordenados por tamanho (do maior para o menor)...\n");

    for (int i = 0; i < qtd; i++) {
        printf("Removido: %s | Tamanho: %.2f KB\n", copia[i].nome_arquivo, copia[i].tamanho_bytes / 1024.0);
    }

    DWORD fim = GetTickCount();
    printf("Tempo total de remoção: %lu ms\n", fim - inicio);

    return 0; // fila esvaziada
}

void ordenar_fila_por_tamanho(PDFInfo fila[], int qtd) {
    if (qtd == 0) {
        printf("Fila vazia.\n");
        return;
    }

    qsort(fila, qtd, sizeof(PDFInfo), comparar_tamanho); // reutiliza a função de comparação crescente

    printf("\nFila ordenada por tamanho (do menor para o maior):\n");
    for (int i = 0; i < qtd; i++) {
        printf("%d. %s | %.2f KB\n", i + 1, fila[i].nome_arquivo, fila[i].tamanho_bytes / 1024.0);
    }
}

