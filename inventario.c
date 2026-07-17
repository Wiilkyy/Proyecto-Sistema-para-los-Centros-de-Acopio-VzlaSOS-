#include "inventario.h"

// ==========================================
// FUNCIONES UTILITARIAS
// ==========================================
// Compara dos cadenas ignorando mayusculas y minusculas
int compararCadenas(const char *s1, const char *s2) {
    while (*s1 && *s2) {
        if (tolower((unsigned char)*s1) != tolower((unsigned char)*s2)) return 0;
        s1++; s2++;
    }
    return (*s1 == '\0' && *s2 == '\0');
}

// ===============================================
// Funcion del arbol fractal para el inventario 
// ===============================================
int obtenerAltura(NodoAVL *N) {
    if (N == NULL) return 0;
    return N->altura;
}

int maximo(int a, int b) {
    return (a > b) ? a : b;
}

NodoAVL* nuevoNodoAVL(Producto prod) {
    NodoAVL* nodo = (NodoAVL*)malloc(sizeof(NodoAVL));
    nodo->prod = prod;
    nodo->izquierdo = NULL;
    nodo->derecho = NULL;
    nodo->altura = 1;
    return nodo;
}

NodoAVL* rotacionDerecha(NodoAVL *y) {
    NodoAVL *x = y->izquierdo;
    NodoAVL *T2 = x->derecho;
    x->derecho = y;
    y->izquierdo = T2;
    y->altura = maximo(obtenerAltura(y->izquierdo), obtenerAltura(y->derecho)) + 1;
    x->altura = maximo(obtenerAltura(x->izquierdo), obtenerAltura(x->derecho)) + 1;
    return x;
}

NodoAVL* rotacionIzquierda(NodoAVL *x) {
    NodoAVL *y = x->derecho;
    NodoAVL *T2 = y->izquierdo;
    y->izquierdo = x;
    x->derecho = T2;
    x->altura = maximo(obtenerAltura(x->izquierdo), obtenerAltura(x->derecho)) + 1;
    y->altura = maximo(obtenerAltura(y->izquierdo), obtenerAltura(y->derecho)) + 1;
    return y;
}

int obtenerBalance(NodoAVL *N) {
    if (N == NULL) return 0;
    return obtenerAltura(N->izquierdo) - obtenerAltura(N->derecho);
}

NodoAVL* insertarAVL(NodoAVL* nodo, Producto prod) {
    if (nodo == NULL)
        return nuevoNodoAVL(prod);

    if (prod.fecha_vencimiento < nodo->prod.fecha_vencimiento)
        nodo->izquierdo = insertarAVL(nodo->izquierdo, prod);
    else if (prod.fecha_vencimiento > nodo->prod.fecha_vencimiento)
        nodo->derecho = insertarAVL(nodo->derecho, prod);
    else {
        // Agrupamos por ID si las fechas coinciden
        if (prod.id < nodo->prod.id) {
            nodo->izquierdo = insertarAVL(nodo->izquierdo, prod);
        } else if (prod.id > nodo->prod.id) {
            nodo->derecho = insertarAVL(nodo->derecho, prod);
        } else {
            // Fecha y ID igual => sumamos cantidades
            nodo->prod.cantidad += prod.cantidad; 
            return nodo;
        }
    }

    nodo->altura = 1 + maximo(obtenerAltura(nodo->izquierdo), obtenerAltura(nodo->derecho));
    int balance = obtenerBalance(nodo);

    if (balance > 1 && prod.fecha_vencimiento < nodo->izquierdo->prod.fecha_vencimiento)
        return rotacionDerecha(nodo);
    if (balance < -1 && prod.fecha_vencimiento > nodo->derecho->prod.fecha_vencimiento)
        return rotacionIzquierda(nodo);
    if (balance > 1 && prod.fecha_vencimiento > nodo->izquierdo->prod.fecha_vencimiento) {
        nodo->izquierdo = rotacionIzquierda(nodo->izquierdo);
        return rotacionDerecha(nodo);
    }
    if (balance < -1 && prod.fecha_vencimiento < nodo->derecho->prod.fecha_vencimiento) {
        nodo->derecho = rotacionDerecha(nodo->derecho);
        return rotacionIzquierda(nodo);
    }
    return nodo;
}

void inOrder(NodoAVL *raiz) {
    if (raiz != NULL) {
        inOrder(raiz->izquierdo);
        printf("Vence: %d | ID: %d | [%s] %s | Cant: %d\n", raiz->prod.fecha_vencimiento, raiz->prod.id, raiz->prod.categoria, raiz->prod.nombre, raiz->prod.cantidad);
        inOrder(raiz->derecho);
    }
}

// ==========================================
// Funcion de Busquedad y Despacho 
// ==========================================
void mostrarPorCategoria(NodoAVL *raiz, const char* categoria, int *encontrados) {
    if (raiz != NULL) {
        mostrarPorCategoria(raiz->izquierdo, categoria, encontrados);
        if (compararCadenas(raiz->prod.categoria, categoria)) {
            printf("- ID: %d | %s | Cant: %d | Vence: %d\n", raiz->prod.id, raiz->prod.nombre, raiz->prod.cantidad, raiz->prod.fecha_vencimiento);
            (*encontrados)++;
        }
        mostrarPorCategoria(raiz->derecho, categoria, encontrados);
    }
}

// Recorre el arbol en orden buscando el ID especifico
void encontrarMasViejoPorID(NodoAVL* raiz, int id, NodoAVL** encontrado) {
    if (raiz == NULL || *encontrado != NULL) return;
    
    encontrarMasViejoPorID(raiz->izquierdo, id, encontrado);
    
    if (*encontrado == NULL && raiz->prod.id == id) {
        *encontrado = raiz;
        return;
    }
    
    encontrarMasViejoPorID(raiz->derecho, id, encontrado);
}

// Calculamos el stock contando los items por el ID
int obtenerStockTotal(NodoAVL* raiz, int id) {
    if (raiz == NULL) return 0;
    int stock = 0;
    if (raiz->prod.id == id) {
        stock = raiz->prod.cantidad;
    }
    return stock + obtenerStockTotal(raiz->izquierdo, id) + obtenerStockTotal(raiz->derecho, id);
}

NodoAVL* nodoMinimo(NodoAVL* nodo) {
    NodoAVL* actual = nodo;
    while (actual && actual->izquierdo != NULL)
        actual = actual->izquierdo;
    return actual;
}

// Eliminación por ID + Fecha
NodoAVL* eliminarAVL(NodoAVL* raiz, int fecha_vencimiento, int id) {
    if (raiz == NULL) return raiz;

    if (fecha_vencimiento < raiz->prod.fecha_vencimiento)
        raiz->izquierdo = eliminarAVL(raiz->izquierdo, fecha_vencimiento, id);
    else if (fecha_vencimiento > raiz->prod.fecha_vencimiento)
        raiz->derecho = eliminarAVL(raiz->derecho, fecha_vencimiento, id);
    else {
        // usamos ID para ubicar
        if (id < raiz->prod.id) {
            raiz->izquierdo = eliminarAVL(raiz->izquierdo, fecha_vencimiento, id);
        } else if (id > raiz->prod.id) {
            raiz->derecho = eliminarAVL(raiz->derecho, fecha_vencimiento, id);
        } else {
            // Nodo exacto encontrado
            if ((raiz->izquierdo == NULL) || (raiz->derecho == NULL)) {
                NodoAVL *temp = raiz->izquierdo ? raiz->izquierdo : raiz->derecho;
                if (temp == NULL) {
                    temp = raiz;
                    raiz = NULL;
                } else
                    *raiz = *temp; 
                free(temp);
            } else {
                NodoAVL* temp = nodoMinimo(raiz->derecho);
                raiz->prod = temp->prod; 
                raiz->derecho = eliminarAVL(raiz->derecho, temp->prod.fecha_vencimiento, temp->prod.id); 
            }
        }
    }

    if (raiz == NULL) return raiz;

    raiz->altura = 1 + maximo(obtenerAltura(raiz->izquierdo), obtenerAltura(raiz->derecho));
    int balance = obtenerBalance(raiz);

    if (balance > 1 && obtenerBalance(raiz->izquierdo) >= 0)
        return rotacionDerecha(raiz);
    if (balance > 1 && obtenerBalance(raiz->izquierdo) < 0) {
        raiz->izquierdo = rotacionIzquierda(raiz->izquierdo);
        return rotacionDerecha(raiz);
    }
    if (balance < -1 && obtenerBalance(raiz->derecho) <= 0)
        return rotacionIzquierda(raiz);
    if (balance < -1 && obtenerBalance(raiz->derecho) > 0) {
        raiz->derecho = rotacionDerecha(raiz->derecho);
        return rotacionIzquierda(raiz);
    }
    return raiz;
}

// Despacho por ID y cantidad despachada 
void despacharProductoPorID(NodoAVL **raiz, NodoHistorial **histCabeza, NodoHistorial **histCola, int id, int cantidadPedida) {
    if (*raiz == NULL) {
        printf("\n[ERROR] Inventario vacio.\n");
        return;
    }

    int totalDespachado = 0;
    
    while (cantidadPedida > 0 && *raiz != NULL) {
        NodoAVL* loteMasViejo = NULL;
        encontrarMasViejoPorID(*raiz, id, &loteMasViejo);

        if (loteMasViejo == NULL) {
            break; // Eliminamos los lotes del producto si ya no hay más
        }

        Producto prodDespacho = loteMasViejo->prod;

        if (totalDespachado == 0) {
            printf("\n--- INICIANDO DESPACHO DE %s (ID: %d) ---\n", prodDespacho.nombre, id);
        }

        if (loteMasViejo->prod.cantidad > cantidadPedida) {
            loteMasViejo->prod.cantidad -= cantidadPedida;
            prodDespacho.cantidad = cantidadPedida;
            
            registrarHistorial(histCabeza, histCola, "SALIDA (PARCIAL)", prodDespacho);
            printf("> Despachados %d de %s. Quedan %d en lote (Vence: %d).\n", 
                   cantidadPedida, prodDespacho.nombre, loteMasViejo->prod.cantidad, prodDespacho.fecha_vencimiento);
            totalDespachado += cantidadPedida;
            cantidadPedida = 0; 
            
        } else {
            int entregado = loteMasViejo->prod.cantidad;
            prodDespacho.cantidad = entregado;
            
            registrarHistorial(histCabeza, histCola, "SALIDA (AGOTA LOTE)", prodDespacho);
            printf("> Lote de %s (Vence: %d) AGOTADO. Entregados %d.\n", 
                   prodDespacho.nombre, prodDespacho.fecha_vencimiento, entregado);
            
            *raiz = eliminarAVL(*raiz, loteMasViejo->prod.fecha_vencimiento, loteMasViejo->prod.id);
            totalDespachado += entregado;
            cantidadPedida -= entregado; 
        }
    }

    if (cantidadPedida > 0) {
        if (totalDespachado == 0) {
            printf("\n[ALERTA] No se encontro el ID '%d' en inventario.\n", id);
        } else {
            printf("\n[ALERTA] Nos quedamos sin stock del ID '%d'. Faltaron %d por despachar.\n", id, cantidadPedida);
        }
    } else {
        printf("\n[EXITO] Despacho completado correctamente.\n");
    }
}

// ================================================
// Funciones para exportar el arbol fractal en SVG
// ================================================
// dibuja las ramas del arbol
void dibujarLineasSVG(NodoAVL *raiz, int min_x, int max_x, int y, int dy, int px, int py, FILE *archivo) {
    if (!raiz) return;
    
    // ubicamos nuestro centro
    int x = min_x + (max_x - min_x) / 2;
    
    if (py != 0) {
        // rama del nodo al centro
        fprintf(archivo, "    <line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" class=\"line\" />\n", px, py + 25, x, y - 25);
    }
    
    // creamos las ramas del arbol de izquierda a derecha
    dibujarLineasSVG(raiz->izquierdo, min_x, x, y + dy, dy, x, y, archivo);
    dibujarLineasSVG(raiz->derecho, x, max_x, y + dy, dy, x, y, archivo);
}

// generamos las hojas del arbol (los nodos)
void dibujarNodosSVG(NodoAVL *raiz, int min_x, int max_x, int y, int dy, FILE *archivo) {
    if (!raiz) return;
    int x = min_x + (max_x - min_x) / 2;

    // limitamos el nombre del producto para la hora (14 caract max)
    char nombreCorto[16];
    strncpy(nombreCorto, raiz->prod.nombre, 14);
    nombreCorto[14] = '\0';

    // generamos un rectangulo con el ID, nombre, cant
    fprintf(archivo, "    <rect x=\"%d\" y=\"%d\" width=\"120\" height=\"55\" rx=\"8\" class=\"node\" />\n", x - 60, y - 25);
    fprintf(archivo, "    <text x=\"%d\" y=\"%d\" class=\"text\" style=\"fill:#A7F3D0\">ID: %d</text>\n", x, y - 8, raiz->prod.id);
    fprintf(archivo, "    <text x=\"%d\" y=\"%d\" class=\"text\">%s</text>\n", x, y + 7, nombreCorto);
    fprintf(archivo, "    <text x=\"%d\" y=\"%d\" class=\"text\" style=\"fill:#FCD34D\">Cant: %d</text>\n", x, y + 22, raiz->prod.cantidad);

    dibujarNodosSVG(raiz->izquierdo, min_x, x, y + dy, dy, archivo);
    dibujarNodosSVG(raiz->derecho, x, max_x, y + dy, dy, archivo);
}

// Funcion para exportar a SVG con nombre personalizado
void exportarArbolSVG(NodoAVL *raiz, const char *nombreArchivo) {
    if (raiz == NULL) {
        printf("\n[ERROR] El arbol esta vacio.\n");
        return;
    }

    FILE *archivo = fopen(nombreArchivo, "w");
    if (!archivo) {
        printf("\n[ERROR] No se pudo crear el archivo %s.\n", nombreArchivo);
        return;
    }

    // Definimos un espacio para que se vea bien
    int width = 4000;
    int height = (obtenerAltura(raiz) + 2) * 100;

    fprintf(archivo, "<svg xmlns=\"http://www.w3.org/2000/svg\" viewBox=\"0 0 %d %d\">\n", width, height);
    fprintf(archivo, "<rect width=\"100%%\" height=\"100%%\" fill=\"#111827\"/>\n");
    fprintf(archivo, "<style>\n");
    fprintf(archivo, "  .node { fill: #1F2937; stroke: #10B981; stroke-width: 2; }\n");
    fprintf(archivo, "  .line { stroke: #10B981; stroke-width: 2; }\n");
    fprintf(archivo, "  .text { font-family: Consolas, monospace; font-size: 13px; text-anchor: middle; fill: #ffffff; }\n");
    fprintf(archivo, "</style>\n");

    dibujarLineasSVG(raiz, 0, width, 50, 90, 0, 0, archivo);
    dibujarNodosSVG(raiz, 0, width, 50, 90, archivo);

    fprintf(archivo, "</svg>\n");
    fclose(archivo);
}

// Wrapper para exportar el árbol completo (nombre fijo)
void exportarArbolSVGCompleto(NodoAVL *raiz) {
    if (raiz == NULL) {
        printf("\n[ERROR] El arbol esta vacio. Carga donaciones primero.\n");
        return;
    }
    exportarArbolSVG(raiz, "arbol_fractal.svg");
    printf("\n[EXITO] Archivo 'arbol_fractal.svg' generado! Abrelo en tu navegador.\n");
}

// Liberar memoria del árbol AVL
void liberarAVL(NodoAVL *raiz) {
    if (raiz == NULL) return;
    liberarAVL(raiz->izquierdo);
    liberarAVL(raiz->derecho);
    free(raiz);
}

// Recolectar los primeros 50 productos (recorrido in-order)
void recolectarTop50(NodoAVL *raiz, Producto *arr, int *count) {
    if (raiz == NULL || *count >= 50) return;
    recolectarTop50(raiz->izquierdo, arr, count);
    if (*count < 50) {
        arr[*count] = raiz->prod;
        (*count)++;
    }
    recolectarTop50(raiz->derecho, arr, count);
}

// Exportar árbol con los 50 productos más próximos a vencer
void exportarArbolSVGTop50(NodoAVL *raiz) {
    if (raiz == NULL) {
        printf("\n[ERROR] El arbol esta vacio. Carga donaciones primero.\n");
        return;
    }
    Producto top[50];
    int count = 0;
    recolectarTop50(raiz, top, &count);
    if (count == 0) {
        printf("\n[ERROR] No hay productos para mostrar.\n");
        return;
    }
    NodoAVL *nuevoArbol = NULL;
    for (int i = 0; i < count; i++) {
        nuevoArbol = insertarAVL(nuevoArbol, top[i]);
    }
    exportarArbolSVG(nuevoArbol, "arbol_top50.svg");
    liberarAVL(nuevoArbol);
    printf("\n[EXITO] Archivo 'arbol_top50.svg' generado con los %d productos más próximos a vencer.\n", count);
}

// ======================================================================
// Funcion de Listas Doblemente Enlazadas para el historial de auditoria
// ======================================================================
void registrarHistorial(NodoHistorial **cabeza, NodoHistorial **cola, const char* accion, Producto prod) {
    NodoHistorial *nuevo = (NodoHistorial*)malloc(sizeof(NodoHistorial));
    strcpy(nuevo->accion, accion);
    nuevo->prod = prod;
    nuevo->siguiente = NULL;
    nuevo->anterior = *cola;

    if (*cola != NULL) {
        (*cola)->siguiente = nuevo;
    } else {
        *cabeza = nuevo; 
    }
    *cola = nuevo;
}

void mostrarHistorial(NodoHistorial *cola) {
    NodoHistorial *temp = cola;
    printf("\n--- HISTORIAL DE MOVIMIENTOS (Mas recientes primero) ---\n");
    while (temp != NULL) {
        printf("[%s] ID:%d - %s [%s] (Cant: %d)\n", temp->accion, temp->prod.id, temp->prod.nombre, temp->prod.categoria, temp->prod.cantidad);
        temp = temp->anterior;
    }
}

// ===================================================================
// Merge Sort para ordenar los productos por fecha de vencimiento
// ===================================================================
void merge(Producto arr[], int l, int m, int r) {
    int i, j, k;
    int n1 = m - l + 1;
    int n2 = r - m;

    Producto *L = (Producto*)malloc(n1 * sizeof(Producto));
    Producto *R = (Producto*)malloc(n2 * sizeof(Producto));

    for (i = 0; i < n1; i++) L[i] = arr[l + i];
    for (j = 0; j < n2; j++) R[j] = arr[m + 1 + j];

    i = 0; j = 0; k = l;
    while (i < n1 && j < n2) {
        if (L[i].fecha_vencimiento <= R[j].fecha_vencimiento) {
            arr[k] = L[i];
            i++;
        } else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }
    while (i < n1) { arr[k] = L[i]; i++; k++; }
    while (j < n2) { arr[k] = R[j]; j++; k++; }
    
    free(L);
    free(R);
}

void mergeSort(Producto arr[], int l, int r) {
    if (l < r) {
        int m = l + (r - l) / 2;
        mergeSort(arr, l, m);
        mergeSort(arr, m + 1, r);
        merge(arr, l, m, r);
    }
}