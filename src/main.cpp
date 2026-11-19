// ============================================================
//  SITRAC - Sistema Inteligente de Transporte y Gestion de Rutas
//  Proyecto final - Estructura de Datos - C++
//
//  Compilar (desde la carpeta del proyecto):
//      g++ -std=c++11 src/main.cpp -o bin/sitrac
//  Ejecutar:
//      bin/sitrac      (en Windows: bin\sitrac.exe)
// ============================================================

#include <iostream>
#include <fstream>
#include <string>
#include "Utilidades.h"
#include "ListaPasajeros.h"
#include "Cola.h"
#include "Pila.h"
#include "ABB.h"
#include "Grafo.h"
#include "Tipos.h"
#include "Persistencia.h"
using namespace std;

// ---------------- Datos del sistema ----------------
Grafo grafo;                    // ciudades y conexiones
ListaPasajeros pasajeros;       // lista enlazada de pasajeros
Cola<Solicitud> solicitudes;    // cola de solicitudes de viaje
Pila<Accion> historial;         // pila de acciones (para deshacer)
ABB busquedas;                  // arbol de ciudades buscadas

// Archivo de datos de ejemplo. Se prueba desde la carpeta del proyecto
// y tambien desde bin/, para que funcione al abrir el .exe directamente.
string archivoPorDefecto() {
    const string candidatos[] = { "data/sitrac_datos.txt", "../data/sitrac_datos.txt" };
    for (int i = 0; i < 2; i++) {
        ifstream prueba(candidatos[i].c_str());
        if (prueba) return candidatos[i];
    }
    return candidatos[0];
}

// Guarda una accion en la pila de historial
void registrarAccion(string tipo, string texto1, string texto2,
                     int numero1, int numero2, string descripcion) {
    Accion a;
    a.tipo = tipo;
    a.texto1 = texto1;
    a.texto2 = texto2;
    a.numero1 = numero1;
    a.numero2 = numero2;
    a.descripcion = descripcion;
    historial.apilar(a);
}

void titulo(string texto) {
    cout << "\n==============================================\n";
    cout << "  " << texto << "\n";
    cout << "==============================================\n";
}

// ============================================================
//  1. GESTION DE CIUDADES
// ============================================================
void menuCiudades() {
    int opcion;
    do {
        titulo("1. GESTION DE CIUDADES");
        cout << "  1. Agregar ciudad\n";
        cout << "  2. Listar ciudades y sus conexiones\n";
        cout << "  3. Buscar ciudad\n";
        cout << "  4. Eliminar ciudad\n";
        cout << "  0. Volver\n";
        opcion = leerEnteroRango("Opcion: ", 0, 4);

        if (opcion == 1) {
            string nombre = leerTexto("Nombre de la ciudad: ");
            if (grafo.agregarCiudad(nombre)) {
                cout << "  Ciudad agregada.\n";
                registrarAccion("AGREGAR_CIUDAD", nombre, "", 0, 0, "Se agrego la ciudad " + nombre);
            } else {
                cout << "  [Error] La ciudad '" << nombre << "' ya existe. No se modifico nada.\n";
            }
        } else if (opcion == 2) {
            cout << "  Total de ciudades: " << grafo.getNumCiudades() << "\n";
            grafo.mostrar();
        } else if (opcion == 3) {
            string nombre = leerTexto("Ciudad a buscar: ");
            Vertice* v = grafo.buscarCiudad(nombre);
            if (v != nullptr) {
                cout << "  Encontrada: " << v->nombre << " con "
                     << grafo.grado(v) << " conexion(es).\n";
                busquedas.registrar(nombre);   // se guarda en el ABB
            } else {
                cout << "  La ciudad no existe.\n";
            }
        } else if (opcion == 4) {
            string nombre = leerTexto("Ciudad a eliminar: ");
            if (grafo.eliminarCiudad(nombre)) {
                cout << "  Ciudad eliminada junto con todas sus conexiones.\n";
                registrarAccion("ELIMINAR_CIUDAD", nombre, "", 0, 0, "Se elimino la ciudad " + nombre);
            } else {
                cout << "  [Error] La ciudad no existe.\n";
            }
        }
        if (opcion != 0) pausar();
    } while (opcion != 0);
}

// ============================================================
//  2. GESTION DE CONEXIONES
// ============================================================
void menuConexiones() {
    int opcion;
    do {
        titulo("2. GESTION DE CONEXIONES");
        cout << "  1. Agregar conexion\n";
        cout << "  2. Eliminar conexion\n";
        cout << "  3. Listar conexiones\n";
        cout << "  0. Volver\n";
        opcion = leerEnteroRango("Opcion: ", 0, 3);

        if (opcion == 1) {
            string a = leerTexto("Ciudad origen: ");
            string b = leerTexto("Ciudad destino: ");
            int km = leerEntero("Distancia en km: ");
            string error;
            if (grafo.agregarConexion(a, b, km, error)) {
                cout << "  Conexion agregada.\n";
                registrarAccion("AGREGAR_CONEXION", a, b, km, 0,
                                "Se conecto " + a + " - " + b + " (" + to_string(km) + " km)");
            } else {
                cout << "  [Error] " << error << " No se modifico nada.\n";
            }
        } else if (opcion == 2) {
            string a = leerTexto("Ciudad origen: ");
            string b = leerTexto("Ciudad destino: ");
            int km = grafo.distanciaConexion(a, b);
            if (grafo.eliminarConexion(a, b)) {
                cout << "  Conexion eliminada.\n";
                registrarAccion("ELIMINAR_CONEXION", a, b, km, 0,
                                "Se elimino la conexion " + a + " - " + b);
            } else {
                cout << "  [Error] Esa conexion no existe.\n";
            }
        } else if (opcion == 3) {
            cout << "  Total de conexiones: " << grafo.contarConexiones() << "\n";
            grafo.mostrar();
        }
        if (opcion != 0) pausar();
    } while (opcion != 0);
}

// ============================================================
//  3. GESTION DE PASAJEROS (lista enlazada)
// ============================================================
void menuPasajeros() {
    int opcion;
    do {
        titulo("3. GESTION DE PASAJEROS");
        cout << "  1. Registrar pasajero\n";
        cout << "  2. Listar pasajeros\n";
        cout << "  3. Buscar pasajero\n";
        cout << "  4. Modificar pasajero\n";
        cout << "  5. Eliminar pasajero\n";
        cout << "  0. Volver\n";
        opcion = leerEnteroRango("Opcion: ", 0, 5);

        if (opcion == 1) {
            int id = leerEnteroRango("ID (numero positivo): ", 1, 999999);
            if (pasajeros.buscar(id) != nullptr) {
                cout << "  [Error] Ya existe un pasajero con ese ID.\n";
            } else {
                string nombre = leerTexto("Nombre: ");
                int edad = leerEnteroRango("Edad (0-120): ", 0, 120);
                pasajeros.insertar(id, nombre, edad);
                cout << "  Pasajero registrado.\n";
                registrarAccion("AGREGAR_PASAJERO", nombre, "", id, edad,
                                "Se registro al pasajero " + to_string(id) + " - " + nombre);
            }
        } else if (opcion == 2) {
            cout << "  Total: " << pasajeros.getCantidad() << "\n";
            pasajeros.mostrar();
        } else if (opcion == 3) {
            int id = leerEntero("ID a buscar: ");
            Pasajero* p = pasajeros.buscar(id);
            if (p != nullptr) cout << "  Encontrado: " << p->nombre << ", " << p->edad << " anos.\n";
            else cout << "  No existe un pasajero con ese ID.\n";
        } else if (opcion == 4) {
            int id = leerEntero("ID a modificar: ");
            Pasajero* p = pasajeros.buscar(id);
            if (p == nullptr) {
                cout << "  [Error] No existe un pasajero con ese ID.\n";
            } else {
                string nombreViejo = p->nombre;
                int edadVieja = p->edad;
                string nombre = leerTexto("Nuevo nombre: ");
                int edad = leerEnteroRango("Nueva edad (0-120): ", 0, 120);
                pasajeros.modificar(id, nombre, edad);
                cout << "  Pasajero modificado.\n";
                // se guardan los datos VIEJOS para poder deshacer
                registrarAccion("MODIFICAR_PASAJERO", nombreViejo, "", id, edadVieja,
                                "Se modifico al pasajero " + to_string(id));
            }
        } else if (opcion == 5) {
            int id = leerEntero("ID a eliminar: ");
            Pasajero* p = pasajeros.buscar(id);
            if (p == nullptr) {
                cout << "  [Error] No existe un pasajero con ese ID. La lista no se modifico.\n";
            } else {
                string nombre = p->nombre;
                int edad = p->edad;
                pasajeros.eliminar(id);
                cout << "  Pasajero eliminado.\n";
                registrarAccion("ELIMINAR_PASAJERO", nombre, "", id, edad,
                                "Se elimino al pasajero " + to_string(id) + " - " + nombre);
            }
        }
        if (opcion != 0) pausar();
    } while (opcion != 0);
}

// ============================================================
//  4. COLA DE SOLICITUDES
// ============================================================
void menuCola() {
    int opcion;
    do {
        titulo("4. COLA DE SOLICITUDES DE VIAJE");
        cout << "  Solicitudes en espera: " << solicitudes.getCantidad() << "\n";
        cout << "  1. Nueva solicitud (encolar)\n";
        cout << "  2. Atender siguiente solicitud (desencolar)\n";
        cout << "  3. Ver solicitudes pendientes\n";
        cout << "  0. Volver\n";
        opcion = leerEnteroRango("Opcion: ", 0, 3);

        if (opcion == 1) {
            int id = leerEntero("ID del pasajero: ");
            if (pasajeros.buscar(id) == nullptr) {
                cout << "  [Error] Ese pasajero no existe.\n";
            } else {
                string origen = leerTexto("Ciudad origen: ");
                string destino = leerTexto("Ciudad destino: ");
                if (grafo.buscarCiudad(origen) == nullptr || grafo.buscarCiudad(destino) == nullptr) {
                    cout << "  [Error] Alguna de las ciudades no existe.\n";
                } else {
                    Solicitud s;
                    s.idPasajero = id;
                    s.origen = origen;
                    s.destino = destino;
                    solicitudes.encolar(s);
                    cout << "  Solicitud agregada al final de la cola.\n";
                    registrarAccion("ENCOLAR", origen, destino, id, 0,
                                    "Solicitud del pasajero " + to_string(id) + ": " + origen + " -> " + destino);
                }
            }
        } else if (opcion == 2) {
            Solicitud s;
            if (!solicitudes.desencolar(s)) {
                cout << "  La cola esta vacia. No hay solicitudes por atender.\n";
            } else {
                cout << "  Atendiendo al pasajero " << s.idPasajero << ": "
                     << s.origen << " -> " << s.destino << "\n";
                string camino;
                int km = grafo.dijkstra(s.origen, s.destino, camino);
                if (km >= 0) cout << "  Ruta asignada: " << camino << " (" << km << " km)\n";
                else if (km == -1) cout << "  No existe ruta entre esas ciudades.\n";
                else cout << "  Alguna ciudad ya no existe en el sistema.\n";
                registrarAccion("ATENDER", s.origen, s.destino, s.idPasajero, 0,
                                "Se atendio la solicitud del pasajero " + to_string(s.idPasajero));
            }
        } else if (opcion == 3) {
            NodoCola<Solicitud>* actual = solicitudes.getFrente();
            if (actual == nullptr) cout << "  (Cola vacia)\n";
            int posicion = 1;
            while (actual != nullptr) {
                cout << "  " << posicion << ". Pasajero " << actual->dato.idPasajero << ": "
                     << actual->dato.origen << " -> " << actual->dato.destino << "\n";
                actual = actual->siguiente;
                posicion++;
            }
        }
        if (opcion != 0) pausar();
    } while (opcion != 0);
}

// ============================================================
//  5. PILA DE HISTORIAL (deshacer)
// ============================================================
void deshacer() {
    Accion a;
    if (!historial.desapilar(a)) {
        cout << "  No hay acciones para deshacer.\n";
        return;
    }
    cout << "  Deshaciendo: " << a.descripcion << "\n";
    string error;
    bool ok = true;

    if (a.tipo == "AGREGAR_CIUDAD") {
        ok = grafo.eliminarCiudad(a.texto1);
    } else if (a.tipo == "AGREGAR_CONEXION") {
        ok = grafo.eliminarConexion(a.texto1, a.texto2);
    } else if (a.tipo == "ELIMINAR_CONEXION") {
        ok = grafo.agregarConexion(a.texto1, a.texto2, a.numero1, error);
    } else if (a.tipo == "AGREGAR_PASAJERO") {
        ok = pasajeros.eliminar(a.numero1);
    } else if (a.tipo == "ELIMINAR_PASAJERO") {
        ok = pasajeros.insertar(a.numero1, a.texto1, a.numero2);
    } else if (a.tipo == "MODIFICAR_PASAJERO") {
        ok = pasajeros.modificar(a.numero1, a.texto1, a.numero2);
    } else {
        // ELIMINAR_CIUDAD, ENCOLAR y ATENDER solo quedan registradas
        cout << "  Esta accion solo se registra; no se puede deshacer.\n";
        return;
    }

    if (ok) cout << "  Accion deshecha correctamente.\n";
    else cout << "  No se pudo deshacer (los datos cambiaron despues).\n";
}

void menuPila() {
    int opcion;
    do {
        titulo("5. PILA DE HISTORIAL");
        cout << "  Acciones guardadas: " << historial.getCantidad() << "\n";
        cout << "  1. Ver historial (la mas reciente primero)\n";
        cout << "  2. Deshacer ultima accion\n";
        cout << "  0. Volver\n";
        opcion = leerEnteroRango("Opcion: ", 0, 2);

        if (opcion == 1) {
            NodoPila<Accion>* actual = historial.getTope();
            if (actual == nullptr) cout << "  (Historial vacio)\n";
            int numero = 1;
            while (actual != nullptr) {
                cout << "  " << numero << ". " << actual->dato.descripcion << "\n";
                actual = actual->siguiente;
                numero++;
            }
        } else if (opcion == 2) {
            deshacer();
        }
        if (opcion != 0) pausar();
    } while (opcion != 0);
}

// ============================================================
//  6. ARBOL DE BUSQUEDAS (ABB)
// ============================================================
void menuABB() {
    int opcion;
    do {
        titulo("6. ARBOL DE BUSQUEDAS (ABB)");
        cout << "  Nodos: " << busquedas.getCantidad() << " | Altura: " << busquedas.altura() << "\n";
        cout << "  1. Insertar (registrar busqueda de una ciudad)\n";
        cout << "  2. Buscar en el arbol\n";
        cout << "  3. Eliminar del arbol\n";
        cout << "  4. Recorridos (inorden, preorden, postorden)\n";
        cout << "  0. Volver\n";
        opcion = leerEnteroRango("Opcion: ", 0, 4);

        if (opcion == 1) {
            string nombre = leerTexto("Ciudad: ");
            busquedas.registrar(nombre);
            cout << "  Registrada. Veces buscada: " << busquedas.buscar(nombre)->veces << "\n";
        } else if (opcion == 2) {
            string nombre = leerTexto("Ciudad a buscar en el arbol: ");
            NodoABB* n = busquedas.buscar(nombre);
            if (n != nullptr) cout << "  Esta en el arbol. Buscada " << n->veces << " vez/veces.\n";
            else cout << "  No esta en el arbol.\n";
        } else if (opcion == 3) {
            string nombre = leerTexto("Ciudad a eliminar del arbol: ");
            if (busquedas.eliminar(nombre)) cout << "  Eliminada del arbol.\n";
            else cout << "  [Error] No esta en el arbol.\n";
        } else if (opcion == 4) {
            if (busquedas.estaVacio()) {
                cout << "  (Arbol vacio)\n";
            } else {
                cout << "  Inorden   (alfabetico): " << busquedas.inorden() << "\n";
                cout << "  Preorden  : " << busquedas.preorden() << "\n";
                cout << "  Postorden : " << busquedas.postorden() << "\n";
            }
        }
        if (opcion != 0) pausar();
    } while (opcion != 0);
}

// ============================================================
//  7. RUTAS Y ALGORITMOS
// ============================================================
void menuRutas() {
    int opcion;
    do {
        titulo("7. RUTAS Y ALGORITMOS");
        cout << "  1. BFS - recorrido por niveles\n";
        cout << "  2. DFS - recorrido en profundidad\n";
        cout << "  3. Dijkstra - ruta mas corta\n";
        cout << "  0. Volver\n";
        opcion = leerEnteroRango("Opcion: ", 0, 3);

        if (opcion == 1) {
            string inicio = leerTexto("Ciudad de inicio: ");
            string r = grafo.bfs(inicio);
            if (r == "") cout << "  [Error] La ciudad no existe.\n";
            else {
                cout << "  Orden de visita [nivel]:\n  " << r << "\n";
                busquedas.registrar(inicio);
            }
        } else if (opcion == 2) {
            string inicio = leerTexto("Ciudad de inicio: ");
            string r = grafo.dfs(inicio);
            if (r == "") cout << "  [Error] La ciudad no existe.\n";
            else {
                cout << "  Orden de visita:\n  " << r << "\n";
                busquedas.registrar(inicio);
            }
        } else if (opcion == 3) {
            string origen = leerTexto("Ciudad origen: ");
            string destino = leerTexto("Ciudad destino: ");
            string camino;
            int km = grafo.dijkstra(origen, destino, camino);
            if (km == -2) cout << "  [Error] Alguna de las ciudades no existe.\n";
            else if (km == -1) cout << "  No hay ningun camino entre esas ciudades.\n";
            else {
                cout << "  Ruta mas corta: " << camino << "\n";
                cout << "  Distancia total: " << km << " km\n";
                busquedas.registrar(destino);
            }
        }
        if (opcion != 0) pausar();
    } while (opcion != 0);
}

// ============================================================
//  8. REPORTES
// ============================================================
void reportes() {
    titulo("8. REPORTES");
    cout << "  Ciudades:               " << grafo.getNumCiudades() << "\n";
    cout << "  Conexiones:             " << grafo.contarConexiones() << "\n";
    cout << "  Pasajeros:              " << pasajeros.getCantidad() << "\n";
    cout << "  Solicitudes en cola:    " << solicitudes.getCantidad() << "\n";
    cout << "  Acciones en historial:  " << historial.getCantidad() << "\n";
    cout << "  Ciudades en el ABB:     " << busquedas.getCantidad() << "\n";

    // Ciudad con mas conexiones y ciudades aisladas
    Vertice* mayor = nullptr;
    string aisladas = "";
    Vertice* v = grafo.getPrimero();
    while (v != nullptr) {
        if (mayor == nullptr || grafo.grado(v) > grafo.grado(mayor)) mayor = v;
        if (grafo.grado(v) == 0) aisladas += v->nombre + " ";
        v = v->siguiente;
    }
    if (mayor != nullptr)
        cout << "  Ciudad mas conectada:   " << mayor->nombre << " (" << grafo.grado(mayor) << ")\n";
    cout << "  Ciudades aisladas:      " << (aisladas == "" ? "ninguna" : aisladas) << "\n";

    NodoABB* top = busquedas.masBuscada();
    if (top != nullptr)
        cout << "  Ciudad mas buscada:     " << top->ciudad << " (" << top->veces << " veces)\n";

    cout << "\n  --- Complejidad (Big O) de las operaciones ---\n";
    cout << "  Lista pasajeros: insertar/buscar/eliminar  O(n)\n";
    cout << "  Cola: encolar / desencolar                 O(1)\n";
    cout << "  Pila: apilar / desapilar                   O(1)\n";
    cout << "  ABB: insertar/buscar/eliminar              O(h)  (O(log n) balanceado, O(n) peor caso)\n";
    cout << "  ABB: recorridos                            O(n)\n";
    cout << "  Grafo: agregar/buscar ciudad               O(V)\n";
    cout << "  Grafo: eliminar ciudad                     O(V + E)\n";
    cout << "  BFS y DFS                                  O(V + E)\n";
    cout << "  Dijkstra (sin monticulo)                   O(V^2)\n";
    pausar();
}

// ============================================================
//  9. GUARDAR / CARGAR
// ============================================================
string pedirNombreArchivo() {
    string porDefecto = archivoPorDefecto();
    cout << "Nombre del archivo (Enter = " << porDefecto << "): ";
    string nombre = recortar(leerLinea());
    if (nombre == "") nombre = porDefecto;
    return nombre;
}

void menuArchivos() {
    int opcion;
    do {
        titulo("9. GUARDAR / CARGAR");
        cout << "  1. Guardar datos\n";
        cout << "  2. Cargar datos\n";
        cout << "  0. Volver\n";
        opcion = leerEnteroRango("Opcion: ", 0, 2);

        if (opcion == 1) {
            string nombre = pedirNombreArchivo();
            if (guardarDatos(nombre, grafo, pasajeros, solicitudes))
                cout << "  Datos guardados en '" << nombre << "'.\n";
            else
                cout << "  [Error] No se pudo crear el archivo.\n";
        } else if (opcion == 2) {
            string nombre = pedirNombreArchivo();
            int malas;
            if (cargarDatos(nombre, grafo, pasajeros, solicitudes, malas, true)) {
                historial.vaciar();   // el historial viejo ya no aplica
                cout << "  Datos cargados: " << grafo.getNumCiudades() << " ciudades, "
                     << grafo.contarConexiones() << " conexiones, "
                     << pasajeros.getCantidad() << " pasajeros, "
                     << solicitudes.getCantidad() << " solicitudes.\n";
                if (malas > 0) cout << "  Lineas ignoradas por estar danadas: " << malas << "\n";
            } else {
                cout << "  [Error] No se encontro el archivo '" << nombre
                     << "'. Los datos actuales NO se modificaron.\n";
            }
        }
        if (opcion != 0) pausar();
    } while (opcion != 0);
}

// ============================================================
//  MENU PRINCIPAL
// ============================================================
int main() {
    int opcion;
    do {
        titulo("SITRAC - Sistema de Transporte y Rutas");
        cout << "  1. Gestion de ciudades\n";
        cout << "  2. Gestion de conexiones\n";
        cout << "  3. Gestion de pasajeros\n";
        cout << "  4. Cola de solicitudes\n";
        cout << "  5. Pila de historial\n";
        cout << "  6. Arbol de busquedas\n";
        cout << "  7. Rutas y algoritmos\n";
        cout << "  8. Reportes\n";
        cout << "  9. Guardar / cargar\n";
        cout << "  0. Salir\n";
        opcion = leerEnteroRango("Opcion: ", 0, 9);

        switch (opcion) {
            case 1: menuCiudades();   break;
            case 2: menuConexiones(); break;
            case 3: menuPasajeros();  break;
            case 4: menuCola();       break;
            case 5: menuPila();       break;
            case 6: menuABB();        break;
            case 7: menuRutas();      break;
            case 8: reportes();       break;
            case 9: menuArchivos();   break;
            case 0: cout << "  Hasta luego.\n"; break;
        }
    } while (opcion != 0);
    return 0;
}
