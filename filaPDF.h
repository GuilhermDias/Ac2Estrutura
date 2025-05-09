#ifndef FILAPDF_H
#define FILAPDF_H

#define CAMINHO_PASTA "C:\\Users\\Guilh\\Downloads\\PDFs"
#define CAMINHO_PDFINFO "C:\\poppler\\poppler-24.08.0\\Library\\bin\\pdfinfo.exe"
#define MAX_PDFS 100
#define TEMPO_POR_PAGINA 2 // segundos

typedef struct {
    char nome_arquivo[256];
    int num_paginas;
    long tamanho_bytes;
    int tempo_impressao;
} PDFInfo;

int terminaComPDF(const char *nome);
long obter_tamanho_arquivo(const char *caminho);
int obter_paginas_pdf(const char *caminho_pdf);
int carregar_arquivos_pdf(PDFInfo fila[]);
void imprimir_fila(PDFInfo fila[], int qtd);
void imprimir_ordenado_por_tamanho(PDFInfo fila[], int qtd);
int remover_fila(PDFInfo fila[], int qtd);
int remover_ordenado_por_tamanho(PDFInfo fila[], int qtd);
void ordenar_fila_por_tamanho(PDFInfo fila[], int qtd);


#endif
