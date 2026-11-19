// ============================================================
//  Tipos.h
//  Estructuras sencillas que se guardan en la Cola y en la Pila.
// ============================================================
#ifndef TIPOS_H
#define TIPOS_H

#include <string>
using namespace std;

// Lo que se guarda en la COLA de solicitudes
struct Solicitud {
    int idPasajero;
    string origen;
    string destino;
};

// Lo que se guarda en la PILA de historial.
// Con estos datos se puede DESHACER la accion.
struct Accion {
    string tipo;          // ej: "AGREGAR_CIUDAD", "ELIMINAR_PASAJERO"...
    string texto1;        // ej: nombre de ciudad o de pasajero
    string texto2;        // ej: segunda ciudad de una conexion
    int numero1;          // ej: id del pasajero o distancia
    int numero2;          // ej: edad del pasajero
    string descripcion;   // texto que se muestra en el historial
};

#endif
