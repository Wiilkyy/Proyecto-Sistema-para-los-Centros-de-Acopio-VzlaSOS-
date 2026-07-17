#ifndef INVENTARIO_H
#define INVENTARIO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Estruc del producto 
typedef struct Producto {
    int id;
    char nombre[100];     
    char categoria[50]; 
    int cantidad;
    int fecha_vencimiento; //  YYYYMMDD
} Producto;

// Nodo Arbol fractal 
typedef struct NodoAVL {
    Producto prod;
    struct NodoAVL *izquierdo;
    struct NodoAVL *derecho;
    int altura;
} NodoAVL;

// Nodo para la Lista Doblemente Enlazada 
typedef struct NodoHistorial {
    char accion[50]; 
    Producto prod;
    struct NodoHistorial *siguiente;
    struct NodoHistorial *anterior;
} NodoHistorial;

// Funciones utilitarias
int compararCadenas(const char *s1, const char *s2);

// Funciones del Árbol fractal
int obtenerAltura(NodoAVL *N);
int maximo(int a, int b);
NodoAVL* nuevoNodoAVL(Producto prod);
NodoAVL* rotacionDerecha(NodoAVL *y);
NodoAVL* rotacionIzquierda(NodoAVL *x);
int obtenerBalance(NodoAVL *N);
NodoAVL* insertarAVL(NodoAVL* nodo, Producto prod);
void inOrder(NodoAVL *raiz);

// Nuevas funciones para el despacho por categoría y ID
void mostrarPorCategoria(NodoAVL *raiz, const char* categoria, int *encontrados);
void encontrarMasViejoPorID(NodoAVL* raiz, int id, NodoAVL** encontrado);
int obtenerStockTotal(NodoAVL* raiz, int id);
NodoAVL* nodoMinimo(NodoAVL* nodo);
NodoAVL* eliminarAVL(NodoAVL* raiz, int fecha_vencimiento, int id);
void despacharProductoPorID(NodoAVL **raiz, NodoHistorial **histCabeza, NodoHistorial **histCola, int id, int cantidadPedida);

// Funciones para Dibujar el arbol Fractal (nuevas)
void dibujarLineasSVG(NodoAVL *raiz, int min_x, int max_x, int y, int dy, int px, int py, FILE *archivo);
void dibujarNodosSVG(NodoAVL *raiz, int min_x, int max_x, int y, int dy, FILE *archivo);
void exportarArbolSVG(NodoAVL *raiz, const char *nombreArchivo);
void exportarArbolSVGCompleto(NodoAVL *raiz);
void exportarArbolSVGTop50(NodoAVL *raiz);
void liberarAVL(NodoAVL *raiz);
void recolectarTop50(NodoAVL *raiz, Producto *arr, int *count);

// Funciones de la Lista Doblemente Enlazada
void registrarHistorial(NodoHistorial **cabeza, NodoHistorial **cola, const char* accion, Producto prod);
void mostrarHistorial(NodoHistorial *cola);

// Funciones de Ordenamiento (Merge Sort)
void merge(Producto arr[], int l, int m, int r);
void mergeSort(Producto arr[], int l, int r);

#endif