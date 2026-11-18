# Guía para explicar SITRAC (fácil)

## La idea en una frase
"Es un sistema de transporte: tiene **ciudades** unidas por **carreteras** (grafo), **pasajeros** (lista), **solicitudes de viaje** (cola), un **historial para deshacer** (pila) y un **árbol** con las ciudades que más se buscan."

## Cada estructura en 30 segundos

### 1. Lista enlazada → pasajeros (`ListaPasajeros.h`)
- Cada pasajero es un **nodo** con sus datos y un puntero `siguiente`.
- `cabeza` apunta al primero; el último apunta a `nullptr`.
- **Eliminar:** busco el nodo y su anterior, hago `anterior->siguiente = actual->siguiente` (el nodo queda "saltado") y luego `delete actual`.
- Si el ID no existe, devuelvo `false` y no toco ningún puntero, así que la lista no se daña.

### 2. Cola → solicitudes de viaje (`Cola.h`)
- **FIFO:** la primera solicitud que llega es la primera que se atiende, como la fila de un banco.
- Tengo dos punteros: `frente` (por donde se saca) y `final` (por donde se agrega). Por eso las dos operaciones son **O(1)**.
- Si está vacía, `desencolar` devuelve `false` y no lee memoria inválida.
- Al atender una solicitud se calcula su ruta con Dijkstra.

### 3. Pila → historial / deshacer (`Pila.h`)
- **LIFO:** lo último que hice es lo primero que se deshace, como Ctrl+Z.
- Todo pasa en el `tope`: **O(1)**.
- Cada acción guarda lo necesario para revertirla. Por ejemplo, si eliminé un pasajero, guardo su id, nombre y edad para volver a insertarlo.

### 4. ABB → ciudades buscadas (`ABB.h`)
- Regla: **lo menor va a la izquierda y lo mayor a la derecha** (orden alfabético).
- Si busco una ciudad que ya está, solo le sumo 1 a `veces`.
- **Eliminar tiene 3 casos:**
  1. Sin hijos: se borra el nodo.
  2. Un hijo: el hijo toma el lugar del nodo.
  3. Dos hijos: se copia el **sucesor** (el menor del lado derecho) y se borra ese sucesor.
- **Recorridos:** inorden = izq-raíz-der (sale en orden alfabético), preorden = raíz-izq-der, postorden = izq-der-raíz.

### 5. Grafo → ciudades y carreteras (`Grafo.h`)
- Uso **listas de adyacencia**: cada ciudad tiene su propia lista de conexiones `(destino, km)`.
- Es **no dirigido**: si hay carretera Bogotá–Tunja, se guarda en Bogotá y también en Tunja.
- **Ponderado:** cada conexión tiene una distancia en km, que debe ser mayor que 0.
- **Al eliminar una ciudad** también borro todas sus conexiones, incluidas las que los vecinos tenían hacia ella. Si no lo hiciera, quedarían punteros a memoria liberada.

## Los algoritmos

| | Cómo funciona | Qué estructura usa | Para qué sirve |
|---|---|---|---|
| **BFS** | Visita por **niveles**: primero los vecinos, luego los vecinos de los vecinos | **Cola** + marca de visitado | Saber a cuántos "saltos" queda cada ciudad |
| **DFS** | Se va **lo más profundo posible** por un camino y luego regresa | **Recursión** (pila implícita) | Explorar todo lo que está conectado (componentes) |
| **Dijkstra** | Siempre elige la ciudad no visitada con **menor distancia acumulada** y actualiza a sus vecinos si por ahí sale más barato | Campos `distancia` y `anterior` + una **Pila** para armar el camino | Ruta más corta en km |

**Dijkstra explicado con el ejemplo (Bogota → Cali):**
1. Bogota = 0 y las demás ciudades = infinito.
2. Desde Bogota: Villavicencio 120, Tunja 140, Ibague 195, Manizales 290.
3. Tomo la menor no visitada y sigo. Por Ibague, Armenia queda en 195 + 90 = 285.
4. Por Armenia, Cali queda en 285 + 180 = **465**. Por Pereira serían 550, que es peor.
5. Para armar el camino sigo `anterior` desde Cali hacia atrás y lo meto en una pila para que salga al derecho: `Bogota -> Ibague -> Armenia -> Cali`.
- **¿Por qué no se permiten pesos negativos?** Porque Dijkstra da por cerrada una ciudad cuando la visita. Con un peso negativo podría aparecer después un camino más barato y el resultado quedaría mal.

## Preguntas que te pueden hacer

- **¿Por qué la lista es O(n)?** Porque para buscar hay que recorrerla nodo por nodo, no se puede saltar directo a una posición.
- **¿Por qué el ABB es O(log n) y a veces O(n)?** Si está balanceado, cada comparación descarta la mitad del árbol. Si se insertan las ciudades ya ordenadas, el árbol queda como una línea y se vuelve O(n).
- **¿Por qué BFS y DFS son O(V + E)?** Cada ciudad se visita una vez y cada conexión se revisa una vez.
- **¿Por qué Dijkstra es O(V²)?** En cada uno de los V pasos recorro las V ciudades para encontrar la menor. Con un montículo sería más rápido.
- **¿Por qué usaron `template`?** Para escribir la cola y la pila una sola vez y usarlas con distintos tipos: `Cola<Solicitud>` y `Cola<Vertice*>`, `Pila<Accion>` y `Pila<string>`.
- **¿Qué pasa si escribo letras en un número?** `convertirEntero` (en `Utilidades.h`) lo detecta y el menú vuelve a preguntar.
- **¿Y si el archivo no existe o está dañado?** Si no existe, no se borra nada. Si tiene líneas dañadas, esas líneas se saltan con un aviso y el resto se carga.

## Dónde está cada cosa (para "localizar una función")

| Quiero… | Archivo → función |
|---|---|
| Agregar ciudad | `Grafo.h` → `agregarCiudad` |
| Validar distancia | `Grafo.h` → `agregarConexion` |
| Borrar ciudad y sus conexiones | `Grafo.h` → `eliminarCiudad` |
| BFS / DFS / Dijkstra | `Grafo.h` → `bfs`, `dfs`/`dfsRec`, `dijkstra` |
| CRUD pasajeros | `ListaPasajeros.h` → `insertar`, `buscar`, `modificar`, `eliminar` |
| Encolar / atender | `Cola.h` → `encolar`, `desencolar`; menú en `main.cpp` → `menuCola` |
| Deshacer | `main.cpp` → `deshacer` |
| ABB | `ABB.h` → `registrar`, `buscar`, `eliminar`, `inorden`… |
| Guardar / cargar | `Persistencia.h` → `guardarDatos`, `cargarDatos` |
| Leer números seguros | `Utilidades.h` → `leerEntero`, `convertirEntero` |

## Cambios sorpresa típicos (y dónde se hacen)

- **"Que la distancia máxima sea 2000 km"** → en `Grafo.h`, `agregarConexion`, agregar:
  `if (distancia > 2000) { error = "Maximo 2000 km."; return false; }`
- **"Que la edad mínima sea 18"** → en `main.cpp`, `menuPasajeros`, cambiar `leerEnteroRango("Edad (0-120): ", 0, 120)` por `18, 120`.
- **"Mostrar cuántas solicitudes se atendieron"** → crear un contador global en `main.cpp` y sumarle 1 en `menuCola`, opción 2.
- **"Que el grafo sea dirigido"** → en `agregarConexion`, quitar la línea `agregarArista(vb, va, distancia);` (la "vuelta"). También en `eliminarConexion`, quitar `quitarArista(vb, va);`.
- **"Insertar pasajeros al inicio de la lista"** → en `insertar`, cambiar el bloque del final por `nuevo->siguiente = cabeza; cabeza = nuevo;`.

Después del cambio: compilar, probar y hacer `git commit -m "Cambio: ..."`.

## Demostración rápida (orden sugerido)
1. `9 → 2 → Enter`: carga los datos de ejemplo.
2. `1 → 2`: muestra el grafo.
3. `7 → 1 → Bogota`: BFS. `7 → 2 → Bogota`: DFS. `7 → 3 → Bogota → Cali`: Dijkstra (465 km).
4. `7 → 3 → Bogota → Leticia`: no hay camino.
5. `1 → 1 → Bogota`: ciudad duplicada rechazada. `2 → 1 → Bogota → Leticia → -5`: distancia negativa rechazada.
6. `4 → 2` dos veces y después una vez más con la cola vacía.
7. `3 → 5 → 2`, luego `5 → 2`: deshacer y el pasajero vuelve.
8. `6 → 4`: recorridos del árbol. `8`: reportes.
9. Escribir `abc` en cualquier menú: el programa no se cae.
10. Ejecutar `pruebas.exe`: 25 de 25 OK.
