#include "inventario.h"

void cargarDesdeArchivo(NodoAVL **raiz, NodoHistorial **histCabeza, NodoHistorial **histCola, const char *nombreArchivo) {
    FILE *archivo = fopen(nombreArchivo, "r");
    if (!archivo) {
        printf("Error: No se pudo abrir el archivo %s\n", nombreArchivo);
        return;
    }

    Producto buffer[1000];
    int count = 0;
    char linea[256];

    while (fgets(linea, sizeof(linea), archivo)) {
        if (strlen(linea) < 10) continue;

        if (sscanf(linea, "%d;%49[^;];%99[^;];%d;%d", 
                   &buffer[count].id, 
                   buffer[count].categoria, 
                   buffer[count].nombre, 
                   &buffer[count].cantidad, 
                   &buffer[count].fecha_vencimiento) == 5) {
            count++;
        }
    }
    fclose(archivo);

    mergeSort(buffer, 0, count - 1);

    printf("Se cargaron y ordenaron %d productos desde %s\n", count, nombreArchivo);

    for (int i = 0; i < count; i++) {
        *raiz = insertarAVL(*raiz, buffer[i]);
        registrarHistorial(histCabeza, histCola, "ENTRADA (ARCHIVO)", buffer[i]);
    }
}

int main() {
    NodoAVL *raizAVL = NULL;
    NodoHistorial *histCabeza = NULL;
    NodoHistorial *histCola = NULL;

    int opcion;
    int cantDespacho;

    do {
        printf("\n=========================================\n");
        printf("   SISTEMA DE ACOPIO - CONTINGENCIA VZLA   \n");
        printf("=========================================\n");
        printf("1. Cargar archivo masivo de donaciones (.txt)\n");
        printf("2. Mostrar inventario actual (Ordenado por Vencimiento)\n");
        printf("3. Ver historial de auditoria (Movimientos)\n");
        printf("4. Generar despacho de producto\n");
        printf("5. Generar Arbol Fractal en SVG (todos)\n");
        printf("6. Generar Arbol Fractal de los 50 productos mas proximos a vencer\n");
        printf("7. Salir\n");
        printf("Ingrese opcion: ");
        
        if (scanf("%d", &opcion) != 1) {
            while (getchar() != '\n'); 
            continue;
        }

        switch (opcion) {
            case 1:
                cargarDesdeArchivo(&raizAVL, &histCabeza, &histCola, "datos.txt");
                break;
            case 2:
                printf("\n--- INVENTARIO DISPONIBLE (Mas criticos primero) ---\n");
                inOrder(raizAVL);
                break;
            case 3:
                mostrarHistorial(histCola);
                break;
            case 4: {
                char categoriaBuscada[50];
                int idBuscado;
                int encontrados = 0;

                printf("\nIngrese la Categoria (ej. Alimentos, Auxilios): ");
                scanf(" %49[^\n]", categoriaBuscada);

                printf("\n--- PRODUCTOS EN CATEGORIA: %s ---\n", categoriaBuscada);
                mostrarPorCategoria(raizAVL, categoriaBuscada, &encontrados);

                if (encontrados == 0) {
                    printf("No hay productos disponibles en esta categoria o la escribio mal.\n");
                } else {
                    printf("\nIngrese el ID del producto a despachar: ");
                    scanf("%d", &idBuscado);
                    
                    int stockDisponible = obtenerStockTotal(raizAVL, idBuscado);
                    
                    if (stockDisponible == 0) {
                        printf("\n[ERROR] El producto con ID %d esta completamente AGOTADO o no existe.\n", idBuscado);
                    } else {
                        printf("-> Stock disponible: %d unidades.\n", stockDisponible);
                        printf("Ingrese la cantidad a despachar: ");
                        scanf("%d", &cantDespacho);

                        if (cantDespacho > stockDisponible) {
                            printf("\n[ADVERTENCIA] Estas pidiendo %d pero solo hay %d. Se despachara el stock maximo.\n", cantDespacho, stockDisponible);
                        }

                        despacharProductoPorID(&raizAVL, &histCabeza, &histCola, idBuscado, cantDespacho);
                    }
                }
                break;
            }
            case 5:
                exportarArbolSVGCompleto(raizAVL);
                break;
            case 6:
                exportarArbolSVGTop50(raizAVL);
                break;
            case 7:
                printf("Cerrando sistema y liberando memoria...\n");
                break;
            default:
                printf("Opcion invalida.\n");
        }
    } while (opcion != 7);

    return 0;
}