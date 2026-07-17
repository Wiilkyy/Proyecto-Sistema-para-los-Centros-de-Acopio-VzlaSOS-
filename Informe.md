# Informe Técnico – Sistema de gestion para Centros de Acopio - SOSVzla

## ¿De qué va esto?

Bueno, el proyecto consiste en un sistema para gestionar donaciones en situaciones de emergencia. La idea es poder cargar productos desde un archivo, tenerlos ordenados por fecha de vencimiento para saber cuáles están más críticos, despachar cantidades respetando ese orden (los que vencen primero salen primero), llevar un historial de todo lo que entra y sale, y además generar unos dibujos bien visuales en SVG que muestran el árbol de productos y un top 50 de los que están por vencerse pronto.

## Las estructuras y algoritmos que usé

### El árbol AVL

Desde el principio necesitaba una estructura que mantuviera los productos ordenados por fecha de vencimiento, y en caso de empate por el ID. Como las operaciones de inserción, eliminación y búsqueda se iban a hacer todo el tiempo, no podía usar un árbol binario normal porque se podía desbalancear y volverse lento. Por eso elegí un AVL, que con sus rotaciones garantiza que la altura siempre sea logarítmica.

**Complejidades:**

- Inserción: O(log n) – porque bajo por el árbol, inserto y luego hago hasta dos rotaciones que son O(1) cada una.
- Eliminación: también O(log n) – busco el nodo, lo reemplazo si tiene dos hijos y rebalanceo.
- Búsqueda por ID o fecha: O(log n) – voy bajando por la clave.
- Recorrido in‑orden: O(n) – toca visitar todos los nodos para mostrar el inventario o sacar los primeros 50.
- La altura máxima siempre es O(log n), así que todo funciona rápido.

En espacio, el árbol ocupa O(n) porque guardo un nodo por producto.

### Merge Sort para la carga inicial

Cuando leo el archivo de donaciones, los productos vienen en cualquier orden. Para insertarlos en el AVL sin que se descontrole, primero los ordeno por fecha usando Merge Sort. La ventaja es que es estable y siempre corre en O(n log n) en el peor caso, a diferencia de Quick Sort que a veces se puede ir a O(n²). Además, el espacio extra O(n) que necesita no es problema porque los archivos no son tan grandes.

### Lista doblemente enlazada para el historial

El historial tiene que mostrar los movimientos más recientes primero, así que necesitaba recorrer desde el final hacia el principio. Con una lista simplemente enlazada no se puede hacer eso fácilmente, así que usé una doblemente enlazada. Insertar al final es O(1) y mostrar el historial es O(k) donde k es la cantidad de movimientos que quiero mostrar. Ocupa O(m) espacio, siendo m el total de movimientos registrados.

### Despacho con FIFO

Para que no se desperdicien productos, los que vencen primero deben salir primero. Como el AVL está ordenado por fecha, para un ID dado el lote más viejo es el que tiene la menor fecha. Hago una búsqueda recursiva in‑orden para encontrar ese nodo, descuento la cantidad y si se agota elimino el nodo. En el peor caso puede ser O(n) si el ID está al final, pero en promedio es más rápido porque el árbol está balanceado y la búsqueda se puede podar. Si tengo que despachar muchos lotes, cada eliminación es O(log n) y a lo sumo hay tantos lotes como nodos, así que en total puede ser O(n).

### Generación de SVG (árbol fractal)

Para visualizar el inventario, genero un SVG con un diseño fractal. Recorro el árbol en preorden, calculando coordenadas x e y recursivamente, y dibujo rectángulos con el ID, nombre y cantidad, unidos por líneas.

Para el top 50, hago un recorrido in‑orden para recolectar los primeros 50 productos (los de fecha más pequeña), construyo un nuevo árbol AVL con ellos y lo exporto igual.

Dibujar todo el árbol es O(n) porque visito cada nodo una vez. Recolectar el top 50 también es O(n) en el peor caso, pero en la práctica es O(50) porque los primeros 50 suelen estar al principio del in‑orden. Construir el árbol top 50 es O(50 log 50), que es prácticamente constante.

## Tabla resumen de complejidades

| Operación                              | Estructura/Algoritmo          | Tiempo (peor caso) | Espacio adicional |
|----------------------------------------|-------------------------------|--------------------|-------------------|
| Inserción de producto                  | Árbol AVL                     | O(log n)           | O(1)              |
| Eliminación de producto                | Árbol AVL                     | O(log n)           | O(1)              |
| Búsqueda por ID (lote más viejo)       | Recorrido in‑orden con poda   | O(n)               | O(1)              |
| Mostrar inventario (in‑orden)          | Recorrido AVL                 | O(n)               | O(1)              |
| Ordenar archivo de entrada             | Merge Sort                    | O(n log n)         | O(n)              |
| Insertar en historial (al final)       | Lista doblemente enlazada     | O(1)               | O(1)              |
| Mostrar historial (inverso)            | Lista doblemente enlazada     | O(k) (k movs)      | O(1)              |
| Generar SVG completo                   | Recorrido preorden            | O(n)               | O(1) (archivo)    |
| Generar SVG top 50                     | Recolección + nuevo AVL       | O(n) + O(50 log 50)| O(50)             |

*Nota:* n = número total de productos, k = movimientos mostrados.

## ¿Por qué no usé otras cosas?

Algunos me preguntan por qué no usé un heap en lugar del AVL. Pues porque un heap solo me da el mínimo rápido, pero no puedo buscar por ID ni eliminar cualquier elemento de forma eficiente. Además, el AVL me permite recorrer todo el inventario ordenado, cosa que con un heap no se puede.

Y con el Merge Sort, lo elegí en vez de Quick Sort porque siempre es O(n log n) y es estable, lo cual está bueno cuando los datos pueden tener fechas iguales y no quiero que se desordenen más de lo necesario.

Para el historial, pensé en un arreglo circular, pero como no sé cuántos movimientos van a haber, la lista enlazada me pareció más flexible y fácil de implementar.

## Conclusión final

Al final, las estructuras y algoritmos que elegí cumplen con lo que se pedía y funcionan bien en la práctica. El AVL me da operaciones rápidas para el manejo del inventario, el Merge Sort ordena la carga inicial sin dramas, la lista doble enlazada permite ver el historial al revés sin complicaciones, y el SVG ayuda a visualizar todo de una forma más amigable. El sistema queda robusto y con complejidades teóricas aceptables para el contexto de una contingencia.

