#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <windows.h>
#include <string.h>
#include "filaPDF.h"

int main() {
    PDFInfo fila[MAX_PDFS];
    int contador = 0;
    int arquivos_carregados = 0;

    int opcao;
    do {
        printf("\n-------------------------------\n");
        printf("MENU DE OPÇÕES:\n");
        printf("[1] - Imprimir\n");
        printf("[2] - Imprimir ordenado por tamanho\n");
        printf("[3] - Remover da fila\n");
        printf("[4] - Remover ordenado por tamanho\n");
        printf("[5] - Ordenar fila por tamanho\n");
        printf("[0] - Sair\n");
        printf("-------------------------------\n");
        printf("Escolha uma opção: ");
        scanf("%d", &opcao);

        switch (opcao) {
            case 1:
                if (!arquivos_carregados) {
                    contador = carregar_arquivos_pdf(fila);
                    arquivos_carregados = 1;
                }
                imprimir_fila(fila, contador);
                break;
            case 2:
                if (!arquivos_carregados) {
                    contador = carregar_arquivos_pdf(fila);
                    arquivos_carregados = 1;
                }
                imprimir_ordenado_por_tamanho(fila, contador);
                break;
            case 3:
                if (!arquivos_carregados) {
                    contador = carregar_arquivos_pdf(fila);
                    arquivos_carregados = 1;
                }
                contador = remover_fila(fila, contador);
                break;
            case 4:
                if (!arquivos_carregados) {
                    contador = carregar_arquivos_pdf(fila);
                    arquivos_carregados = 1;
                }
                contador = remover_ordenado_por_tamanho(fila, contador);
                break;
            case 5:
                if (!arquivos_carregados) {
                    contador = carregar_arquivos_pdf(fila);
                    arquivos_carregados = 1;
                }
                ordenar_fila_por_tamanho(fila, contador);
                break;
            case 0:
                printf("Encerrando o programa...\n");
                break;
            default:
                printf("Opção inválida. Tente novamente.\n");
        }
    } while (opcao != 0);

    return 0;
}
