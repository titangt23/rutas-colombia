// ============================================================
//  ListaPasajeros.h
//  LISTA SIMPLEMENTE ENLAZADA (hecha por nosotros, sin STL).
//  Cada nodo es un pasajero y apunta al siguiente pasajero.
//
//   cabeza -> [1|Ana] -> [2|Luis] -> [3|Sara] -> nullptr
// ============================================================
#ifndef LISTAPASAJEROS_H
#define LISTAPASAJEROS_H

#include <iostream>
#include <string>
using namespace std;

// Nodo de la lista: datos del pasajero + puntero al siguiente
struct Pasajero {
    int id;
    string nombre;
    int edad;
    Pasajero* siguiente;
};

class ListaPasajeros {
private:
    Pasajero* cabeza;   // primer nodo de la lista
    int cantidad;       // cuantos pasajeros hay

public:
    ListaPasajeros() {
        cabeza = nullptr;
        cantidad = 0;
    }

    ~ListaPasajeros() {
        vaciar();       // liberar la memoria al terminar
    }

    // INSERTAR al final.  O(n) porque hay que recorrer para
    // verificar que el id no este repetido.
    // Devuelve false si el id ya existe o los datos son invalidos.
    bool insertar(int id, string nombre, int edad) {
        if (id <= 0 || nombre == "" || edad < 0 || edad > 120) return false;
        if (buscar(id) != nullptr) return false;     // id repetido

        Pasajero* nuevo = new Pasajero;               // pedir memoria
        nuevo->id = id;
        nuevo->nombre = nombre;
        nuevo->edad = edad;
        nuevo->siguiente = nullptr;

        if (cabeza == nullptr) {                      // lista vacia
            cabeza = nuevo;
        } else {
            Pasajero* actual = cabeza;
            while (actual->siguiente != nullptr) {    // ir hasta el ultimo
                actual = actual->siguiente;
            }
            actual->siguiente = nuevo;                // enlazar al final
        }
        cantidad++;
        return true;
    }

    // BUSCAR por id.  O(n)
    // Devuelve el puntero al nodo, o nullptr si no existe.
    Pasajero* buscar(int id) {
        Pasajero* actual = cabeza;
        while (actual != nullptr) {
            if (actual->id == id) return actual;
            actual = actual->siguiente;
        }
        return nullptr;
    }

    // MODIFICAR nombre y edad.  O(n)
    bool modificar(int id, string nuevoNombre, int nuevaEdad) {
        if (nuevoNombre == "" || nuevaEdad < 0 || nuevaEdad > 120) return false;
        Pasajero* p = buscar(id);
        if (p == nullptr) return false;
        p->nombre = nuevoNombre;
        p->edad = nuevaEdad;
        return true;
    }

    // ELIMINAR por id.  O(n)
    // Se "salta" el nodo: anterior->siguiente = actual->siguiente
    bool eliminar(int id) {
        Pasajero* actual = cabeza;
        Pasajero* anterior = nullptr;

        while (actual != nullptr && actual->id != id) {
            anterior = actual;
            actual = actual->siguiente;
        }
        if (actual == nullptr) return false;          // no existe: no se toca nada

        if (anterior == nullptr) {                    // era el primero
            cabeza = actual->siguiente;
        } else {                                      // estaba en medio o al final
            anterior->siguiente = actual->siguiente;
        }
        delete actual;                                // liberar memoria
        cantidad--;
        return true;
    }

    // MOSTRAR todos.  O(n)
    void mostrar() {
        if (cabeza == nullptr) {
            cout << "  (No hay pasajeros registrados)\n";
            return;
        }
        Pasajero* actual = cabeza;
        while (actual != nullptr) {
            cout << "  ID: " << actual->id
                 << " | Nombre: " << actual->nombre
                 << " | Edad: " << actual->edad << "\n";
            actual = actual->siguiente;
        }
    }

    // Devuelve los ids en orden, ej: "1 2 3"  (se usa en las pruebas)
    string idsEnOrden() {
        string texto = "";
        Pasajero* actual = cabeza;
        while (actual != nullptr) {
            if (texto != "") texto += " ";
            texto += to_string(actual->id);
            actual = actual->siguiente;
        }
        return texto;
    }

    int getCantidad() { return cantidad; }
    Pasajero* getCabeza() { return cabeza; }

    // Borra todos los nodos.  O(n)
    void vaciar() {
        while (cabeza != nullptr) {
            Pasajero* borrar = cabeza;
            cabeza = cabeza->siguiente;
            delete borrar;
        }
        cantidad = 0;
    }
};

#endif
