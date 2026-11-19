# SITRAC: Sistema Inteligente de Transporte y Gestión de Rutas

Este es mi proyecto final para Estructura de Datos, que esta hecho en C++.

La idea de este proyecto es simular un sistema de transporte entre ciudades de Colombia. Se pueden registrar ciudades y carreteras, anotar pasajeros, poner solicitudes de viaje en una fila y deshacer lo último que se hizo. Con la red de ciudades el programa también calcula recorridos (BFS y DFS) y la ruta más corta entre dos ciudades (Dijkstra).

Algo importante: **ninguna estructura viene de la librería estándar**. La lista, la cola, la pila, el árbol y el grafo las programamos nosotros con nodos y punteros. De la librería solo usamos `string` y las de entrada y salida (`iostream`, `fstream`, `sstream`).

## funcinalidad 

Al abrir el programa aparece un menú con estas opciones:

| Opción | Qué hace | Estructura que usa |
|---|---|---|
| 1 | Agregar, listar, buscar y eliminar ciudades | Grafo |
| 2 | Agregar y eliminar carreteras con su distancia en km | Grafo ponderado |
| 3 | Registrar, buscar, modificar y eliminar pasajeros | Lista enlazada |
| 4 | Poner solicitudes de viaje y atenderlas en orden de llegada | Cola |
| 5 | Ver el historial y deshacer la última acción | Pila |
| 6 | Guardar las ciudades que se han buscado, con cuántas veces | Árbol binario de búsqueda |
| 7 | Recorridos BFS y DFS, y ruta más corta con Dijkstra | Grafo, cola y pila |
| 8 | Reportes y la tabla de complejidad Big O | |
| 9 | Guardar y cargar los datos en un archivo de texto | |
| 0 | Salir | |

Cada submenú tiene su opción "0. Volver".

## Cómo compilarlo y ejecutarlo

Solo se necesita un compilador de C++ como `g++` (en Windows sirve MinGW o WinLibs), con C++11 o más reciente.

Desde la carpeta del proyecto (la que tiene `src`, `tests` y `data`):

```bash
mkdir bin
g++ -std=c++11 src/main.cpp -o bin/sitrac
./bin/sitrac
```

En Windows el ejecutable se llama `bin\sitrac.exe`.

Las pruebas automáticas se compilan igual:

```bash
g++ -std=c++11 tests/pruebas.cpp -o bin/pruebas
./bin/pruebas
```

Al abrir el programa se cargan solos los datos de ejemplo de `data/sitrac_datos.txt`, que trae 26 ciudades de Colombia, 34 carreteras entre ellas (con distancias aproximadas), 8 pasajeros y 2 solicitudes. Si ese archivo no existe, el programa arranca vacío. Para volver a cargarlo, entren a la opción **9**, luego a **2**, y presionen Enter.

Si usan Visual Studio Code, abran la carpeta del proyecto y presionen Ctrl+Shift+B para compilar y ejecutar. Si usan Dev-C++ o Code::Blocks y les sale un error con `nullptr` o `to_string`, hay que agregar `-std=c++11` en las opciones del compilador.

### Un ejemplo

Con los datos de ejemplo cargados, así se ve pedir la ruta más corta de Bogota a Cali:

```
7. Rutas y algoritmos -> 3. Dijkstra
Ciudad origen: Bogota
Ciudad destino: Cali
  Ruta mas corta: Bogota -> Ibague -> Armenia -> Cali
  Distancia total: 465 km
```

Y el recorrido BFS desde Bogota, que muestra entre corchetes a cuántos saltos queda cada ciudad:

```
Bogota[0] -> Tunja[1] -> Villavicencio[1] -> Ibague[1] -> Manizales[1] ->
Bucaramanga[2] -> Armenia[2] -> Pereira[2] -> Medellin[2] -> Cali[3]
```

Leticia está en los datos sin ninguna carretera a propósito, para poder probar qué pasa cuando no existe un camino.

## Qué hay en cada archivo

Separamos el código por estructura para que cada archivo se pueda leer y explicar por separado.

```
SITRAC/
├── src/        el programa
├── tests/      las pruebas automáticas
├── data/       los datos de ejemplo
├── bin/        los ejecutables (se crea al compilar y no se sube a Git)
└── README.md
```

**Carpeta `src`**

| Archivo | Qué contiene |
|---|---|
| `main.cpp` | El menú principal y los submenús. Solo llama a las demás estructuras. |
| `Utilidades.h` | Funciones para leer del teclado sin que el programa se caiga si escriben algo incorrecto. |
| `ListaPasajeros.h` | La lista enlazada con el CRUD de pasajeros. |
| `Cola.h` | La cola (primero en entrar, primero en salir). Es una plantilla, así que sirve para varios tipos de dato. |
| `Pila.h` | La pila (último en entrar, primero en salir). También es una plantilla. |
| `ABB.h` | El árbol binario de búsqueda. |
| `Grafo.h` | El grafo con BFS, DFS y Dijkstra. |
| `Tipos.h` | Los dos `struct` que se guardan en la cola y en la pila: `Solicitud` y `Accion`. |
| `Persistencia.h` | Guardar y cargar el archivo de datos. |

**Otras carpetas**

| Archivo | Qué contiene |
|---|---|
| `tests/pruebas.cpp` | Las 25 pruebas automáticas. |
| `data/sitrac_datos.txt` | Los datos de ejemplo. |

## Cómo está hecho

**Lista enlazada de pasajeros.** Cada pasajero es un nodo que apunta al siguiente. Se agrega al final y no deja repetir el ID. Para eliminar, el nodo anterior se salta al que se borra (`anterior->siguiente = actual->siguiente`) y después se libera la memoria con `delete`.

**Cola de solicitudes.** Guarda un puntero al frente y otro al final, por eso encolar y desencolar no tienen que recorrer nada. Si alguien intenta atender con la cola vacía, el programa avisa y no toca memoria que no existe. El BFS usa esta misma cola.

**Pila de historial.** Todo ocurre en la cima. Cada acción guarda los datos necesarios para poder revertirla, por ejemplo, al eliminar un pasajero se guarda su ID, nombre y edad para volver a insertarlo. Dijkstra también usa una pila, para mostrar el camino en el orden correcto.

**Árbol binario de búsqueda.** Ordena las ciudades alfabéticamente y cada nodo cuenta cuántas veces se buscó esa ciudad. Al eliminar se manejan los tres casos: nodo sin hijos, con un hijo y con dos hijos (en ese caso se reemplaza por el sucesor inorden). Tiene los tres recorridos: inorden, preorden y postorden.

**Grafo.** Las ciudades están en una lista y cada ciudad tiene su propia lista de carreteras con el destino y los kilómetros. Como las carreteras van en los dos sentidos, cada una se guarda dos veces, una en cada ciudad.

### Qué pasa cuando algo sale mal

Una parte importante del proyecto era que ninguna operación fallida dañara los datos:

- Si se agrega una ciudad repetida, se rechaza y el grafo queda igual.
- Si la distancia es cero o negativa, la carretera conecta una ciudad consigo misma, ya existe, o alguna de las dos ciudades no existe, se rechaza.
- Al eliminar una ciudad también se borran todas sus carreteras, incluidas las que sus vecinas tenían hacia ella. Si no, quedarían punteros apuntando a memoria ya liberada.
- Si escriben letras donde se pide un número, el programa avisa y vuelve a preguntar. No se cierra.
- Si el archivo de datos no existe, los datos que ya estaban cargados no se pierden. Si trae líneas dañadas, esas líneas se saltan con un aviso y el resto sí se carga.

### Qué se puede deshacer

Se puede deshacer agregar una ciudad, agregar o eliminar una carretera, y registrar, modificar o eliminar un pasajero. Eliminar una ciudad, encolar y atender solicitudes sí quedan en el historial, pero no se pueden deshacer.

## Complejidad (Big O)

V es el número de ciudades, E el de carreteras, n la cantidad de elementos y h la altura del árbol.

| Operación | Complejidad | Por qué |
|---|---|---|
| Lista: insertar, buscar, eliminar | O(n) | hay que recorrerla nodo por nodo |
| Cola: encolar, desencolar | O(1) | se usan los punteros al frente y al final |
| Pila: apilar, desapilar | O(1) | siempre se trabaja en la cima |
| Árbol: insertar, buscar, eliminar | O(h) | O(log n) si está equilibrado, O(n) en el peor caso |
| Árbol: recorridos | O(n) | cada nodo se visita una vez |
| Grafo: agregar o buscar ciudad | O(V) | se recorre la lista de ciudades |
| Grafo: eliminar ciudad | O(V + E) | hay que borrar sus carreteras en las ciudades vecinas |
| BFS y DFS | O(V + E) | cada ciudad y cada carretera se revisan una sola vez |
| Dijkstra | O(V²) | en cada paso se busca el mínimo recorriendo todas las ciudades, sin usar montículo |

## Pruebas

`pruebas.cpp` tiene 25 pruebas automáticas, entre ellas todos los casos difíciles que pide la presentación del proyecto. Al ejecutarlo debe terminar así:

```
Resultado: 25 pruebas OK, 0 fallaron.
```

| Pruebas | Qué revisan |
|---|---|
| 01 | Una ciudad duplicada se rechaza sin alterar el grafo |
| 02 y 03 | Una distancia negativa o cero se rechaza |
| 04 y 05 | Una carretera con una ciudad que no existe, o repetida, se rechaza |
| 06 y 07 | Eliminar una ciudad borra sus carreteras; eliminar una que no existe no cambia nada |
| 08 y 09 | El orden de visita de BFS (con niveles) y de DFS |
| 10 y 11 | Dijkstra: distancia y camino, caso sin camino y ciudad inexistente |
| 12 a 15 | Lista: ID repetido, eliminar uno que no existe, eliminar al inicio, en medio y al final, y modificar |
| 16 y 17 | Cola vacía y orden de llegada |
| 18 | Pila y pila vacía |
| 19 a 21 | Árbol: insertar, buscar, eliminar en sus tres casos y los recorridos |
| 22 | Texto en un campo que pide un número |
| 23 a 25 | Archivo que no existe, guardar y volver a cargar sin perder nada, y archivo con líneas dañadas |

## Limitaciones

Cosas que sabemos que se podrían mejorar:

- Los nombres distinguen mayúsculas de minúsculas ("Bogota" y "bogota" serían ciudades distintas) y no pueden llevar el signo `;`, porque es el separador del archivo.
- El árbol no se equilibra solo. Si se insertan las ciudades ya ordenadas, queda como una lista.
- Dijkstra funciona bien para pocas ciudades, pero con una red muy grande habría que usar un montículo.
- El historial y el árbol de búsquedas no se guardan en el archivo.
- No se puede deshacer eliminar una ciudad, ni encolar o atender solicitudes.

## Integrantes

| Nombre | Qué hizo |
|---|---|
| Christian Perez | |
| | |
| | |

Universidad Santo Tomás · Estructura de Datos · 2026-II
