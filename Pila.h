// ============================================================
//  Pila.h
//  PILA (LIFO: el ultimo que entra es el primero que sale)
//  hecha con nodos enlazados.
//
//   tope -> [C] -> [B] -> [A] -> nullptr
//   apilar y desapilar trabajan SIEMPRE en el tope.
//
//  Se usa para el historial (deshacer) y para invertir el camino
//  de Dijkstra.  T es el tipo de dato que guarda.
// ============================================================
#ifndef PILA_H
#define PILA_H

#include <cstddef>

template <typename T>
struct NodoPila {
    T dato;
    NodoPila<T>* siguiente;
};

template <typename T>
class Pila {
private:
    NodoPila<T>* tope;
    int cantidad;

public:
    Pila() {
        tope = nullptr;
        cantidad = 0;
    }

    ~Pila() {
        vaciar();
    }

    // APILAR: pone un elemento encima.  O(1)
    void apilar(T dato) {
        NodoPila<T>* nuevo = new NodoPila<T>;
        nuevo->dato = dato;
        nuevo->siguiente = tope;   // el nuevo apunta al que era el tope
        tope = nuevo;              // y ahora el nuevo es el tope
        cantidad++;
    }

    // DESAPILAR: quita el de encima.  O(1)
    // Devuelve false si la pila esta vacia.
    bool desapilar(T &dato) {
        if (tope == nullptr) return false;
        NodoPila<T>* borrar = tope;
        dato = borrar->dato;
        tope = tope->siguiente;
        delete borrar;
        cantidad--;
        return true;
    }

    bool estaVacia() { return tope == nullptr; }
    int getCantidad() { return cantidad; }
    NodoPila<T>* getTope() { return tope; }   // para recorrerla y mostrarla

    void vaciar() {
        T basura;
        while (desapilar(basura)) { }
    }
};

#endif
