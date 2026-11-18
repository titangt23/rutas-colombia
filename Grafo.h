// ============================================================
//  Grafo.h
//  GRAFO PONDERADO NO DIRIGIDO con LISTAS DE ADYACENCIA.
//
//  - Las ciudades (vertices) estan en una lista enlazada.
//  - Cada ciudad tiene su propia lista de conexiones (aristas).
//  - Cada conexion tiene una distancia en km (el "peso").
//
//   [Bogota] -> Tunja(140) -> Ibague(195)
//      |
//   [Tunja]  -> Bogota(140)
//      |
//   [Ibague] -> Bogota(195)
//
//  Como es NO dirigido, cada conexion se guarda dos veces:
//  Bogota->Tunja y Tunja->Bogota.
// ============================================================
#ifndef GRAFO_H
#define GRAFO_H

#include <iostream>
#include <string>
#include "Cola.h"
#include "Pila.h"
using namespace std;

const int INFINITO = 1000000000;

struct Vertice;   // se declara antes porque Arista lo usa

struct Arista {
    Vertice* destino;     // ciudad a la que llega
    int distancia;        // km
    Arista* siguiente;    // siguiente conexion de la misma ciudad
};

struct Vertice {
    string nombre;
    Arista* aristas;      // lista de conexiones de esta ciudad
    Vertice* siguiente;   // siguiente ciudad del grafo

    // Campos de ayuda para los algoritmos (BFS, DFS, Dijkstra)
    bool visitado;
    int distancia;        // en Dijkstra: costo acumulado. En BFS: nivel
    Vertice* anterior;    // para reconstruir el camino en Dijkstra
};

class Grafo {
private:
    Vertice* primero;
    int numCiudades;

    // Agrega una arista al final de la lista de "origen"
    void agregarArista(Vertice* origen, Vertice* destino, int distancia) {
        Arista* nueva = new Arista;
        nueva->destino = destino;
        nueva->distancia = distancia;
        nueva->siguiente = nullptr;

        if (origen->aristas == nullptr) {
            origen->aristas = nueva;
        } else {
            Arista* actual = origen->aristas;
            while (actual->siguiente != nullptr) actual = actual->siguiente;
            actual->siguiente = nueva;
        }
    }

    // Quita la arista origen->destino. Devuelve false si no existia.
    bool quitarArista(Vertice* origen, Vertice* destino) {
        Arista* actual = origen->aristas;
        Arista* anterior = nullptr;
        while (actual != nullptr && actual->destino != destino) {
            anterior = actual;
            actual = actual->siguiente;
        }
        if (actual == nullptr) return false;
        if (anterior == nullptr) origen->aristas = actual->siguiente;
        else anterior->siguiente = actual->siguiente;
        delete actual;
        return true;
    }

    // Busca la arista origen->destino
    Arista* buscarArista(Vertice* origen, Vertice* destino) {
        Arista* actual = origen->aristas;
        while (actual != nullptr) {
            if (actual->destino == destino) return actual;
            actual = actual->siguiente;
        }
        return nullptr;
    }

    // Pone todas las ciudades como "no visitadas" antes de un algoritmo
    void reiniciarMarcas() {
        Vertice* v = primero;
        while (v != nullptr) {
            v->visitado = false;
            v->distancia = INFINITO;
            v->anterior = nullptr;
            v = v->siguiente;
        }
    }

    // DFS recursivo: visita la ciudad y luego cada vecino no visitado
    void dfsRec(Vertice* v, string &recorrido) {
        v->visitado = true;
        if (recorrido != "") recorrido += " -> ";
        recorrido += v->nombre;

        Arista* a = v->aristas;
        while (a != nullptr) {
            if (!a->destino->visitado) dfsRec(a->destino, recorrido);
            a = a->siguiente;
        }
    }

public:
    Grafo() {
        primero = nullptr;
        numCiudades = 0;
    }

    ~Grafo() {
        vaciar();
    }

    // ===================== CIUDADES =====================

    // BUSCAR ciudad.  O(V)
    Vertice* buscarCiudad(string nombre) {
        Vertice* actual = primero;
        while (actual != nullptr) {
            if (actual->nombre == nombre) return actual;
            actual = actual->siguiente;
        }
        return nullptr;
    }

    // AGREGAR ciudad al final.  O(V)
    // Rechaza nombre vacio o ciudad duplicada SIN alterar el grafo.
    bool agregarCiudad(string nombre) {
        if (nombre == "") return false;
        if (buscarCiudad(nombre) != nullptr) return false;   // duplicada

        Vertice* nueva = new Vertice;
        nueva->nombre = nombre;
        nueva->aristas = nullptr;
        nueva->siguiente = nullptr;
        nueva->visitado = false;
        nueva->distancia = INFINITO;
        nueva->anterior = nullptr;

        if (primero == nullptr) {
            primero = nueva;
        } else {
            Vertice* actual = primero;
            while (actual->siguiente != nullptr) actual = actual->siguiente;
            actual->siguiente = nueva;
        }
        numCiudades++;
        return true;
    }

    // ELIMINAR ciudad.  O(V + E)
    // IMPORTANTE: tambien borra TODAS sus conexiones, incluso las que
    // otras ciudades tenian hacia ella (para no dejar punteros colgando).
    bool eliminarCiudad(string nombre) {
        Vertice* actual = primero;
        Vertice* anterior = nullptr;
        while (actual != nullptr && actual->nombre != nombre) {
            anterior = actual;
            actual = actual->siguiente;
        }
        if (actual == nullptr) return false;

        // 1) Borrar sus aristas y la arista "de regreso" en cada vecino
        while (actual->aristas != nullptr) {
            Arista* a = actual->aristas;
            quitarArista(a->destino, actual);     // vecino -> esta ciudad
            actual->aristas = a->siguiente;
            delete a;                             // esta ciudad -> vecino
        }

        // 2) Sacar la ciudad de la lista de ciudades
        if (anterior == nullptr) primero = actual->siguiente;
        else anterior->siguiente = actual->siguiente;

        delete actual;
        numCiudades--;
        return true;
    }

    // ===================== CONEXIONES =====================

    // AGREGAR conexion entre a y b.  O(V + grado)
    // Si algo falla devuelve false y explica el motivo en "error".
    bool agregarConexion(string a, string b, int distancia, string &error) {
        Vertice* va = buscarCiudad(a);
        Vertice* vb = buscarCiudad(b);
        if (va == nullptr || vb == nullptr) { error = "Una de las ciudades no existe."; return false; }
        if (va == vb) { error = "No se puede conectar una ciudad consigo misma."; return false; }
        if (distancia <= 0) { error = "La distancia debe ser mayor que 0."; return false; }
        if (buscarArista(va, vb) != nullptr) { error = "Esa conexion ya existe."; return false; }

        agregarArista(va, vb, distancia);   // ida
        agregarArista(vb, va, distancia);   // vuelta
        error = "";
        return true;
    }

    // ELIMINAR conexion.  O(V + grado)
    bool eliminarConexion(string a, string b) {
        Vertice* va = buscarCiudad(a);
        Vertice* vb = buscarCiudad(b);
        if (va == nullptr || vb == nullptr) return false;
        if (!quitarArista(va, vb)) return false;
        quitarArista(vb, va);
        return true;
    }

    // Distancia de la conexion a-b, o -1 si no existe
    int distanciaConexion(string a, string b) {
        Vertice* va = buscarCiudad(a);
        Vertice* vb = buscarCiudad(b);
        if (va == nullptr || vb == nullptr) return -1;
        Arista* arista = buscarArista(va, vb);
        if (arista == nullptr) return -1;
        return arista->distancia;
    }

    // Cantidad de conexiones de una ciudad (grado)
    int grado(Vertice* v) {
        int total = 0;
        Arista* a = v->aristas;
        while (a != nullptr) { total++; a = a->siguiente; }
        return total;
    }

    // Total de conexiones (cada una esta guardada 2 veces, por eso / 2)
    int contarConexiones() {
        int total = 0;
        Vertice* v = primero;
        while (v != nullptr) { total += grado(v); v = v->siguiente; }
        return total / 2;
    }

    // MOSTRAR la lista de adyacencia.  O(V + E)
    void mostrar() {
        if (primero == nullptr) {
            cout << "  (No hay ciudades registradas)\n";
            return;
        }
        Vertice* v = primero;
        while (v != nullptr) {
            cout << "  " << v->nombre << " -> ";
            Arista* a = v->aristas;
            if (a == nullptr) cout << "(sin conexiones)";
            while (a != nullptr) {
                cout << a->destino->nombre << "(" << a->distancia << " km)";
                if (a->siguiente != nullptr) cout << ", ";
                a = a->siguiente;
            }
            cout << "\n";
            v = v->siguiente;
        }
    }

    // ===================== ALGORITMOS =====================

    // BFS (recorrido en ANCHURA) usando nuestra COLA.  O(V + E)
    // Visita por niveles: primero los vecinos, luego los vecinos de los vecinos.
    // Devuelve "" si la ciudad no existe.  Formato: Bogota[0] -> Tunja[1] ...
    string bfs(string inicio) {
        Vertice* origen = buscarCiudad(inicio);
        if (origen == nullptr) return "";

        reiniciarMarcas();
        Cola<Vertice*> cola;
        origen->visitado = true;
        origen->distancia = 0;          // nivel 0
        cola.encolar(origen);

        string recorrido = "";
        Vertice* actual;
        while (cola.desencolar(actual)) {
            if (recorrido != "") recorrido += " -> ";
            recorrido += actual->nombre + "[" + to_string(actual->distancia) + "]";

            Arista* a = actual->aristas;
            while (a != nullptr) {
                if (!a->destino->visitado) {
                    a->destino->visitado = true;
                    a->destino->distancia = actual->distancia + 1;   // un nivel mas
                    cola.encolar(a->destino);
                }
                a = a->siguiente;
            }
        }
        return recorrido;
    }

    // DFS (recorrido en PROFUNDIDAD) usando RECURSION.  O(V + E)
    // Va lo mas profundo posible por un camino antes de regresar.
    string dfs(string inicio) {
        Vertice* origen = buscarCiudad(inicio);
        if (origen == nullptr) return "";
        reiniciarMarcas();
        string recorrido = "";
        dfsRec(origen, recorrido);
        return recorrido;
    }

    // DIJKSTRA: camino mas corto entre dos ciudades.  O(V^2)
    // Devuelve la distancia total, -1 si no hay camino,
    // -2 si alguna ciudad no existe.  En "camino" deja la ruta.
    int dijkstra(string inicio, string fin, string &camino) {
        camino = "";
        Vertice* origen = buscarCiudad(inicio);
        Vertice* destino = buscarCiudad(fin);
        if (origen == nullptr || destino == nullptr) return -2;

        reiniciarMarcas();
        origen->distancia = 0;

        while (true) {
            // 1) Elegir la ciudad NO visitada con menor distancia acumulada
            Vertice* actual = nullptr;
            Vertice* v = primero;
            while (v != nullptr) {
                if (!v->visitado && v->distancia != INFINITO &&
                    (actual == nullptr || v->distancia < actual->distancia)) {
                    actual = v;
                }
                v = v->siguiente;
            }
            if (actual == nullptr) break;     // no quedan ciudades alcanzables
            if (actual == destino) break;     // ya llegamos con el minimo
            actual->visitado = true;

            // 2) "Relajar": ver si pasando por actual se llega mas barato
            Arista* a = actual->aristas;
            while (a != nullptr) {
                Vertice* vecino = a->destino;
                int nuevaDistancia = actual->distancia + a->distancia;
                if (!vecino->visitado && nuevaDistancia < vecino->distancia) {
                    vecino->distancia = nuevaDistancia;
                    vecino->anterior = actual;      // recordar por donde vinimos
                }
                a = a->siguiente;
            }
        }

        if (destino->distancia == INFINITO) return -1;   // no hay camino

        // 3) Reconstruir el camino: desde el destino hacia atras con "anterior".
        //    Se usa una PILA para darle la vuelta (queda origen -> destino).
        Pila<string> pila;
        Vertice* paso = destino;
        while (paso != nullptr) {
            pila.apilar(paso->nombre);
            paso = paso->anterior;
        }
        string nombre;
        while (pila.desapilar(nombre)) {
            if (camino != "") camino += " -> ";
            camino += nombre;
        }
        return destino->distancia;
    }

    // ===================== OTROS =====================

    Vertice* getPrimero() { return primero; }
    int getNumCiudades() { return numCiudades; }

    // Borra todo el grafo (ciudades y conexiones)
    void vaciar() {
        while (primero != nullptr) {
            Vertice* v = primero;
            while (v->aristas != nullptr) {
                Arista* a = v->aristas;
                v->aristas = a->siguiente;
                delete a;
            }
            primero = v->siguiente;
            delete v;
        }
        numCiudades = 0;
    }
};

#endif
