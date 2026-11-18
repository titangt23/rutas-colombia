# 🚌 SITRAC – Sistema Inteligente de Transporte y Gestión de Rutas

Proyecto integrador final de **Estructura de Datos** (USTA · 2026-II), hecho en **C++**.

SITRAC administra una red de ciudades unidas por carreteras, los pasajeros, las solicitudes de viaje y el historial de acciones. Con esa red calcula recorridos (**BFS**, **DFS**) y la ruta más corta (**Dijkstra**).

Todas las estructuras de datos están **implementadas desde cero** con nodos y punteros: lista enlazada, cola, pila, árbol binario de búsqueda y grafo. De la biblioteca estándar solo se usan `string`, `iostream`, `fstream` y `sstream`.

---

## ✨ Funcionalidades

| Menú | Módulo | Estructura de datos |
|---|---|---|
| 1 | Gestión de ciudades (agregar, listar, buscar, eliminar) | Grafo |
| 2 | Gestión de conexiones con distancia en km | Grafo ponderado |
| 3 | Gestión de pasajeros (CRUD) | Lista simplemente enlazada |
| 4 | Cola de solicitudes de viaje (encolar, atender) | Cola FIFO |
| 5 | Historial de acciones con **deshacer** | Pila LIFO |
| 6 | Árbol de ciudades buscadas (insertar, buscar, eliminar, recorridos) | ABB |
| 7 | Rutas: BFS, DFS y Dijkstra con reconstrucción del camino | Grafo + Cola + Pila |
| 8 | Reportes y tabla de complejidad Big O | — |
| 9 | Guardar / cargar en archivo de texto | — |

---

## ⚙️ Compilar y ejecutar

Se necesita `g++` con C++11 o una versión más nueva (MinGW / WinLibs en Windows).

```bash
g++ -std=c++11 main.cpp -o sitrac
./sitrac            # en Windows: sitrac.exe
```

Pruebas automáticas:

```bash
g++ -std=c++11 pruebas.cpp -o pruebas
./pruebas           # en Windows: pruebas.exe
```

**Datos de ejemplo:** dentro del programa, entrar a **9 → 2** y presionar Enter. Se carga `sitrac_datos.txt`, con 11 ciudades de Colombia y 12 carreteras.

> **Visual Studio Code:** abrir la carpeta del proyecto y presionar **Ctrl+Shift+B** para compilar y ejecutar. Para depurar se necesita la extensión **C/C++** de Microsoft.
> **Dev-C++ / Code::Blocks:** si sale un error con `nullptr` o `to_string`, agregar `-std=c++11` en las opciones del compilador.

### Ejemplo de uso

```
7. Rutas y algoritmos -> 3. Dijkstra
Ciudad origen: Bogota
Ciudad destino: Cali
  Ruta mas corta: Bogota -> Ibague -> Armenia -> Cali
  Distancia total: 465 km

7. Rutas y algoritmos -> 1. BFS
Ciudad de inicio: Bogota
  Bogota[0] -> Tunja[1] -> Villavicencio[1] -> Ibague[1] -> Manizales[1] ->
  Bucaramanga[2] -> Armenia[2] -> Pereira[2] -> Medellin[2] -> Cali[3]
```

---

## 📁 Estructura del proyecto

```
SITRAC/
├── main.cpp            # Menú principal y submenús (solo llama a los módulos)
├── Utilidades.h        # Lectura segura del teclado (números y textos)
├── ListaPasajeros.h    # Lista simplemente enlazada (CRUD de pasajeros)
├── Cola.h              # Cola FIFO genérica con nodos  -> Cola<T>
├── Pila.h              # Pila LIFO genérica con nodos  -> Pila<T>
├── ABB.h               # Árbol binario de búsqueda
├── Grafo.h             # Grafo ponderado no dirigido + BFS, DFS, Dijkstra
├── Tipos.h             # Structs Solicitud y Accion
├── Persistencia.h      # Guardar y cargar el archivo de datos
├── pruebas.cpp         # 25 pruebas automáticas
├── sitrac_datos.txt    # Datos de ejemplo
└── GUIA_PARA_EXPLICAR.md  # Guía para la sustentación
```

---

## 🧠 Diseño

- **Lista enlazada (pasajeros):** `cabeza → [id|nombre|edad|sig] → … → nullptr`. Se inserta al final y no se aceptan IDs repetidos. Al eliminar se hace `anterior->siguiente = actual->siguiente` y luego `delete`.
- **Cola (solicitudes):** guarda punteros a `frente` y a `final`, así que encolar y desencolar son O(1). Si está vacía, `desencolar` devuelve `false` sin tocar la memoria. El BFS también la usa.
- **Pila (historial):** todo se hace en el `tope`. Cada acción guarda los datos necesarios para deshacerla. Dijkstra también usa una pila para poner el camino en orden.
- **ABB (búsquedas):** la clave es el nombre de la ciudad (orden alfabético) y cada nodo cuenta las veces que se buscó. La eliminación cubre los 3 casos: hoja, un hijo y dos hijos (con sucesor inorden).
- **Grafo:** las ciudades forman una lista enlazada y cada una tiene su lista de adyacencia `(destino, km)`. Como es **no dirigido**, cada conexión se guarda en las dos ciudades.

### Integridad y manejo de errores
- Ciudad duplicada → se rechaza sin alterar el grafo.
- Distancia ≤ 0, conexión de una ciudad consigo misma, conexión repetida o con una ciudad inexistente → se rechaza.
- Eliminar una ciudad → también borra **todas** sus conexiones en los vecinos, para que no queden punteros colgando.
- Letras en un campo numérico → el programa avisa y vuelve a preguntar. Nunca se cierra.
- Archivo faltante → los datos actuales no se tocan. Las líneas corruptas se ignoran con un aviso.

### Deshacer (pila)
Se puede deshacer: agregar ciudad, agregar o eliminar conexión, y registrar, modificar o eliminar pasajero.
Eliminar ciudad, encolar y atender solicitudes quedan en el historial, pero no se pueden deshacer.

---

## 📊 Complejidad (Big O)

V = ciudades · E = conexiones · n = elementos · h = altura del ABB

| Operación | Complejidad | Por qué |
|---|---|---|
| Lista: insertar / buscar / eliminar | O(n) | se recorre nodo por nodo |
| Cola: encolar / desencolar | O(1) | punteros `frente` y `final` |
| Pila: apilar / desapilar | O(1) | todo ocurre en el `tope` |
| ABB: insertar / buscar / eliminar | O(h) | O(log n) balanceado, O(n) peor caso |
| ABB: recorridos | O(n) | cada nodo se visita una vez |
| Grafo: agregar / buscar ciudad | O(V) | se recorre la lista de ciudades |
| Grafo: eliminar ciudad | O(V + E) | se borran sus aristas en los vecinos |
| BFS / DFS | O(V + E) | cada vértice y arista una vez |
| Dijkstra | O(V²) | selección lineal del mínimo (sin montículo) |

---

## ✅ Pruebas

`pruebas.cpp` tiene **25 pruebas automáticas**. Incluyen todas las pruebas adversariales que pide el proyecto.

```
Resultado: 25 pruebas OK, 0 fallaron.
```

| # | Prueba |
|---|---|
| 01 | Ciudad duplicada se rechaza sin alterar el grafo |
| 02-03 | Distancia negativa o cero se rechaza |
| 04-05 | Conexión con ciudad inexistente o repetida se rechaza |
| 06-07 | Eliminar ciudad borra sus conexiones · eliminar una inexistente no cambia nada |
| 08-09 | Orden de visita de BFS (con niveles) y DFS |
| 10-11 | Dijkstra: distancia y camino · sin camino · ciudad inexistente |
| 12-15 | Lista: ID repetido, eliminar inexistente, eliminar inicio/medio/final, modificar |
| 16-17 | Cola vacía y orden FIFO |
| 18 | Pila LIFO y pila vacía |
| 19-21 | ABB: insertar, buscar, eliminar (3 casos) y recorridos |
| 22 | Entrada alfabética en campo numérico |
| 23-25 | Archivo faltante · guardar → cargar sin pérdida · archivo corrupto |

**Prueba manual:** con los datos de ejemplo, la ruta más corta Bogota → Cali es de **465 km**. Leticia está aislada, así que desde Bogota no hay camino hasta ella.

---

## ⚠️ Limitaciones

- Los nombres distinguen mayúsculas y minúsculas ("Bogota" ≠ "bogota") y no pueden contener `;`.
- El ABB no se auto-balancea, así que en el peor caso degenera en una lista.
- Dijkstra es O(V²) porque no usa montículo; es suficiente para redes pequeñas.
- El historial (pila) y el árbol de búsquedas no se guardan en el archivo.
- No se puede deshacer eliminar una ciudad, ni encolar o atender solicitudes.

---

## 👥 Integrantes

| Nombre | Aporte principal |
|---|---|
| _(nombre)_ | _(ej: Grafo, BFS/DFS/Dijkstra)_ |
| _(nombre)_ | _(ej: Lista, Cola, Pila, ABB)_ |
| _(nombre)_ | _(ej: Menús, persistencia, pruebas)_ |

Universidad Santo Tomás · Estructura de Datos · 2026-II
