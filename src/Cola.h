// ============================================================
//  Cola.h
//  COLA (FIFO: el primero que entra es el primero que sale)
//  hecha con nodos enlazados.
//
//   frente -> [A] -> [B] -> [C] <- final
//   encolar agrega por el FINAL, desencolar saca por el FRENTE.
//
//  Es una "plantilla" (template): T es el tipo de dato que guarda.
//  Asi la misma cola sirve para Solicitudes y para el BFS.
// ============================================================
#ifndef COLA_H
#define COLA_H

#include <cstddef>

template <typename T>
struct NodoCola {
    T dato;
    NodoCola<T>* siguiente;
};

template <typename T>
class Cola {
private:
    NodoCola<T>* frente;   // de aqui se saca
    NodoCola<T>* final;    // aqui se agrega
    int cantidad;

public:
    Cola() {
        frente = nullptr;
        final = nullptr;
        cantidad = 0;
    }

    ~Cola() {
        vaciar();
    }

    // ENCOLAR: agrega al final.  O(1)
    void encolar(T dato) {
        NodoCola<T>* nuevo = new NodoCola<T>;
        nuevo->dato = dato;
        nuevo->siguiente = nullptr;

        if (final == nullptr) {        // cola vacia
            frente = nuevo;
        } else {
            final->siguiente = nuevo;
        }
        final = nuevo;
        cantidad++;
    }

    // DESENCOLAR: saca el del frente.  O(1)
    // Devuelve false si la cola esta vacia (NO usa memoria invalida).
    bool desencolar(T &dato) {
        if (frente == nullptr) return false;

        NodoCola<T>* borrar = frente;
        dato = borrar->dato;
        frente = frente->siguiente;
        if (frente == nullptr) final = nullptr;   // quedo vacia
        delete borrar;
        cantidad--;
        return true;
    }

    bool estaVacia() { return frente == nullptr; }
    int getCantidad() { return cantidad; }
    NodoCola<T>* getFrente() { return frente; }   // para recorrerla y mostrarla

    void vaciar() {
        T basura;
        while (desencolar(basura)) { }
    }
};

#endif
