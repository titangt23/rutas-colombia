// ============================================================
//  ABB.h
//  ARBOL BINARIO DE BUSQUEDA de las ciudades que se han buscado.
//  Cada nodo guarda el nombre de la ciudad y cuantas veces se busco.
//
//  Regla del ABB:  izquierda < nodo < derecha  (orden alfabetico)
// ============================================================
/*
                   Medellin
                  /        \
              Cali          Tunja
             /    \
        Bogota   Ibague
*/
#ifndef ABB_H
#define ABB_H

#include <iostream>
#include <string>
using namespace std;

struct NodoABB {
    string ciudad;
    int veces;          // cuantas veces se ha buscado
    NodoABB* izq;
    NodoABB* der;
};

class ABB {
private:
    NodoABB* raiz;
    int cantidad;

    // ---------- funciones recursivas internas ----------

    NodoABB* insertarRec(NodoABB* nodo, string ciudad) {
        if (nodo == nullptr) {                  // lugar vacio: crear el nodo aqui
            NodoABB* nuevo = new NodoABB;
            nuevo->ciudad = ciudad;
            nuevo->veces = 1;
            nuevo->izq = nullptr;
            nuevo->der = nullptr;
            cantidad++;
            return nuevo;
        }
        if (ciudad < nodo->ciudad) {
            nodo->izq = insertarRec(nodo->izq, ciudad);
        } else if (ciudad > nodo->ciudad) {
            nodo->der = insertarRec(nodo->der, ciudad);
        } else {
            nodo->veces++;                      // ya existia: solo se cuenta
        }
        return nodo;
    }

    // Busca el nodo mas pequeno (el de mas a la izquierda)
    NodoABB* minimo(NodoABB* nodo) {
        while (nodo->izq != nullptr) nodo = nodo->izq;
        return nodo;
    }

    NodoABB* eliminarRec(NodoABB* nodo, string ciudad, bool &eliminado) {
        if (nodo == nullptr) return nullptr;    // no se encontro

        if (ciudad < nodo->ciudad) {
            nodo->izq = eliminarRec(nodo->izq, ciudad, eliminado);
        } else if (ciudad > nodo->ciudad) {
            nodo->der = eliminarRec(nodo->der, ciudad, eliminado);
        } else {
            // Encontrado. Hay 3 casos:
            if (nodo->izq == nullptr) {         // Caso 1 y 2: 0 hijos o solo hijo derecho
                NodoABB* hijo = nodo->der;
                delete nodo;
                cantidad--;
                eliminado = true;
                return hijo;
            }
            if (nodo->der == nullptr) {         // Caso 2: solo hijo izquierdo
                NodoABB* hijo = nodo->izq;
                delete nodo;
                cantidad--;
                eliminado = true;
                return hijo;
            }
            // Caso 3: dos hijos -> se reemplaza por el sucesor
            // (el menor del subarbol derecho) y se borra el sucesor.
            NodoABB* sucesor = minimo(nodo->der);
            nodo->ciudad = sucesor->ciudad;
            nodo->veces = sucesor->veces;
            nodo->der = eliminarRec(nodo->der, sucesor->ciudad, eliminado);
        }
        return nodo;
    }

    void inordenRec(NodoABB* nodo, string &salida) {      // izq - raiz - der
        if (nodo == nullptr) return;
        inordenRec(nodo->izq, salida);
        salida += nodo->ciudad + "(" + to_string(nodo->veces) + ") ";
        inordenRec(nodo->der, salida);
    }

    void preordenRec(NodoABB* nodo, string &salida) {     // raiz - izq - der
        if (nodo == nullptr) return;
        salida += nodo->ciudad + "(" + to_string(nodo->veces) + ") ";
        preordenRec(nodo->izq, salida);
        preordenRec(nodo->der, salida);
    }

    void postordenRec(NodoABB* nodo, string &salida) {    // izq - der - raiz
        if (nodo == nullptr) return;
        postordenRec(nodo->izq, salida);
        postordenRec(nodo->der, salida);
        salida += nodo->ciudad + "(" + to_string(nodo->veces) + ") ";
    }

    int alturaRec(NodoABB* nodo) {
        if (nodo == nullptr) return 0;
        int a = alturaRec(nodo->izq);
        int b = alturaRec(nodo->der);
        return 1 + (a > b ? a : b);
    }

    void masBuscadaRec(NodoABB* nodo, NodoABB* &mejor) {
        if (nodo == nullptr) return;
        if (mejor == nullptr || nodo->veces > mejor->veces) mejor = nodo;
        masBuscadaRec(nodo->izq, mejor);
        masBuscadaRec(nodo->der, mejor);
    }

    void liberar(NodoABB* nodo) {
        if (nodo == nullptr) return;
        liberar(nodo->izq);
        liberar(nodo->der);
        delete nodo;
    }

public:
    ABB() {
        raiz = nullptr;
        cantidad = 0;
    }

    ~ABB() {
        vaciar();
    }

    // INSERTAR / registrar una busqueda.  O(h)  (h = altura del arbol)
    void registrar(string ciudad) {
        if (ciudad == "") return;
        raiz = insertarRec(raiz, ciudad);
    }

    // BUSCAR.  O(h)  - en cada paso se descarta la mitad del arbol
    NodoABB* buscar(string ciudad) {
        NodoABB* actual = raiz;
        while (actual != nullptr) {
            if (ciudad == actual->ciudad) return actual;
            if (ciudad < actual->ciudad) actual = actual->izq;
            else actual = actual->der;
        }
        return nullptr;
    }

    // ELIMINAR.  O(h)
    bool eliminar(string ciudad) {
        bool eliminado = false;
        raiz = eliminarRec(raiz, ciudad, eliminado);
        return eliminado;
    }

    // RECORRIDOS.  O(n)
    string inorden()   { string s = ""; inordenRec(raiz, s);   return s; }
    string preorden()  { string s = ""; preordenRec(raiz, s);  return s; }
    string postorden() { string s = ""; postordenRec(raiz, s); return s; }

    NodoABB* masBuscada() {
        NodoABB* mejor = nullptr;
        masBuscadaRec(raiz, mejor);
        return mejor;
    }

    int altura() { return alturaRec(raiz); }
    int getCantidad() { return cantidad; }
    bool estaVacio() { return raiz == nullptr; }

    void vaciar() {
        liberar(raiz);
        raiz = nullptr;
        cantidad = 0;
    }
};

#endif
