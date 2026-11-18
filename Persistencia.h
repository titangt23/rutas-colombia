// ============================================================
//  Persistencia.h
//  GUARDAR y CARGAR los datos en un archivo de texto.
//
//  Formato del archivo (una cosa por linea, separada por ';'):
//     CIUDAD;Bogota
//     CONEXION;Bogota;Tunja;140
//     PASAJERO;1;Ana Perez;20
//     SOLICITUD;1;Bogota;Cali
//  Las lineas que empiezan con # son comentarios.
//
//  Integridad al cargar:
//   - Si el archivo no existe, NO se borran los datos actuales.
//   - Cada linea se valida; si esta danada se ignora y se avisa.
//   - Una conexion solo entra si sus dos ciudades ya existen.
// ============================================================
#ifndef PERSISTENCIA_H
#define PERSISTENCIA_H

#include <iostream>
#include <fstream>
#include <string>
#include "Utilidades.h"
#include "Grafo.h"
#include "ListaPasajeros.h"
#include "Cola.h"
#include "Tipos.h"
using namespace std;

// Parte "linea" por cada ';' y guarda los pedazos en "campos".
// Devuelve cuantos pedazos hubo.
int dividir(string linea, string campos[], int maximo) {
    int n = 0;
    string pedazo = "";
    for (size_t i = 0; i < linea.size(); i++) {
        if (linea[i] == ';') {
            if (n < maximo) campos[n] = recortar(pedazo);
            n++;
            pedazo = "";
        } else {
            pedazo += linea[i];
        }
    }
    if (n < maximo) campos[n] = recortar(pedazo);
    n++;
    return n;
}

// GUARDAR.  O(V + E + P + S)
bool guardarDatos(string nombreArchivo, Grafo &grafo, ListaPasajeros &pasajeros,
                  Cola<Solicitud> &solicitudes) {
    ofstream archivo(nombreArchivo.c_str());
    if (!archivo) return false;

    archivo << "# Archivo de datos de SITRAC\n";

    // 1) Primero TODAS las ciudades (para que al cargar ya existan)
    Vertice* v = grafo.getPrimero();
    while (v != nullptr) {
        archivo << "CIUDAD;" << v->nombre << "\n";
        v = v->siguiente;
    }

    // 2) Conexiones (cada una una sola vez: solo si origen < destino)
    v = grafo.getPrimero();
    while (v != nullptr) {
        Arista* a = v->aristas;
        while (a != nullptr) {
            if (v->nombre < a->destino->nombre) {
                archivo << "CONEXION;" << v->nombre << ";" << a->destino->nombre
                        << ";" << a->distancia << "\n";
            }
            a = a->siguiente;
        }
        v = v->siguiente;
    }

    // 3) Pasajeros
    Pasajero* p = pasajeros.getCabeza();
    while (p != nullptr) {
        archivo << "PASAJERO;" << p->id << ";" << p->nombre << ";" << p->edad << "\n";
        p = p->siguiente;
    }

    // 4) Solicitudes pendientes (en el mismo orden de la cola)
    NodoCola<Solicitud>* s = solicitudes.getFrente();
    while (s != nullptr) {
        archivo << "SOLICITUD;" << s->dato.idPasajero << ";" << s->dato.origen
                << ";" << s->dato.destino << "\n";
        s = s->siguiente;
    }

    archivo.close();
    return true;
}

// CARGAR.  Devuelve false si el archivo no se pudo abrir.
// En "lineasMalas" deja cuantas lineas se ignoraron por estar danadas.
bool cargarDatos(string nombreArchivo, Grafo &grafo, ListaPasajeros &pasajeros,
                 Cola<Solicitud> &solicitudes, int &lineasMalas, bool mostrarAvisos) {
    lineasMalas = 0;
    ifstream archivo(nombreArchivo.c_str());
    if (!archivo) return false;      // archivo faltante: no se toca nada

    // Se reemplazan los datos actuales por los del archivo
    grafo.vaciar();
    pasajeros.vaciar();
    solicitudes.vaciar();

    string linea;
    int numeroLinea = 0;
    while (getline(archivo, linea)) {
        numeroLinea++;
        linea = recortar(linea);
        if (linea == "" || linea[0] == '#') continue;   // vacia o comentario

        string campos[4];
        int n = dividir(linea, campos, 4);
        bool ok = false;
        string error;
        int numero1, numero2;

        if (campos[0] == "CIUDAD" && n == 2) {
            ok = grafo.agregarCiudad(campos[1]);
        } else if (campos[0] == "CONEXION" && n == 4) {
            ok = convertirEntero(campos[3], numero1) &&
                 grafo.agregarConexion(campos[1], campos[2], numero1, error);
        } else if (campos[0] == "PASAJERO" && n == 4) {
            ok = convertirEntero(campos[1], numero1) &&
                 convertirEntero(campos[3], numero2) &&
                 pasajeros.insertar(numero1, campos[2], numero2);
        } else if (campos[0] == "SOLICITUD" && n == 4) {
            if (convertirEntero(campos[1], numero1) &&
                pasajeros.buscar(numero1) != nullptr &&
                grafo.buscarCiudad(campos[2]) != nullptr &&
                grafo.buscarCiudad(campos[3]) != nullptr) {
                Solicitud s;
                s.idPasajero = numero1;
                s.origen = campos[2];
                s.destino = campos[3];
                solicitudes.encolar(s);
                ok = true;
            }
        }

        if (!ok) {
            lineasMalas++;
            if (mostrarAvisos) {
                cout << "  [Aviso] Linea " << numeroLinea
                     << " ignorada (danada o invalida): " << linea << "\n";
            }
        }
    }
    archivo.close();
    return true;
}

#endif
