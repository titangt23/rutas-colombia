// ============================================================
//  pruebas.cpp  -  Pruebas automaticas de SITRAC
//
//  Compilar:  g++ pruebas.cpp -o pruebas
//  Ejecutar:  ./pruebas     (en Windows: pruebas.exe)
//
//  Cada prueba muestra [OK] o [FALLA]. Al final se muestra el total.
// ============================================================
#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>
#include "Utilidades.h"
#include "ListaPasajeros.h"
#include "Cola.h"
#include "Pila.h"
#include "ABB.h"
#include "Grafo.h"
#include "Tipos.h"
#include "Persistencia.h"
using namespace std;

int pasadas = 0;
int falladas = 0;

void verificar(bool condicion, string nombre) {
    if (condicion) { cout << "[OK]    " << nombre << "\n"; pasadas++; }
    else           { cout << "[FALLA] " << nombre << "\n"; falladas++; }
}

// Grafo pequeno pero no trivial para probar los algoritmos
//      A --4-- B --1-- D
//      |       |
//      2       5
//      |       |
//      C --8-- E        F (aislada)
void armarGrafo(Grafo &g) {
    string e;
    g.agregarCiudad("A"); g.agregarCiudad("B"); g.agregarCiudad("C");
    g.agregarCiudad("D"); g.agregarCiudad("E"); g.agregarCiudad("F");
    g.agregarConexion("A", "B", 4, e);
    g.agregarConexion("A", "C", 2, e);
    g.agregarConexion("B", "D", 1, e);
    g.agregarConexion("B", "E", 5, e);
    g.agregarConexion("C", "E", 8, e);
}

int main() {
    cout << "===== PRUEBAS DE SITRAC =====\n\n";
    string error;

    // ---------- GRAFO ----------
    {
        Grafo g; armarGrafo(g);
        verificar(!g.agregarCiudad("A") && g.getNumCiudades() == 6,
                  "01. Ciudad duplicada se rechaza sin alterar el grafo");
    }
    {
        Grafo g; armarGrafo(g);
        verificar(!g.agregarConexion("A", "F", -10, error) && g.contarConexiones() == 5,
                  "02. Distancia negativa se rechaza");
    }
    {
        Grafo g; armarGrafo(g);
        verificar(!g.agregarConexion("A", "F", 0, error), "03. Distancia cero se rechaza");
    }
    {
        Grafo g; armarGrafo(g);
        verificar(!g.agregarConexion("A", "Z", 10, error) && g.contarConexiones() == 5,
                  "04. Conexion con ciudad inexistente se rechaza");
    }
    {
        Grafo g; armarGrafo(g);
        verificar(!g.agregarConexion("A", "B", 9, error) && g.distanciaConexion("A", "B") == 4,
                  "05. Conexion duplicada se rechaza");
    }
    {
        Grafo g; armarGrafo(g);
        // B tiene 3 conexiones (A, D, E). Al borrar B deben quedar 2 (A-C, C-E)
        bool borrada = g.eliminarCiudad("B");
        verificar(borrada && g.getNumCiudades() == 5 && g.contarConexiones() == 2 &&
                  g.distanciaConexion("A", "B") == -1 && g.dfs("A") == "A -> C -> E",
                  "06. Eliminar ciudad borra todas sus conexiones");
    }
    {
        Grafo g; armarGrafo(g);
        verificar(!g.eliminarCiudad("Z") && g.getNumCiudades() == 6,
                  "07. Eliminar ciudad inexistente no cambia nada");
    }
    {
        Grafo g; armarGrafo(g);
        verificar(g.bfs("A") == "A[0] -> B[1] -> C[1] -> D[2] -> E[2]",
                  "08. BFS visita por niveles en el orden correcto");
    }
    {
        Grafo g; armarGrafo(g);
        verificar(g.dfs("A") == "A -> B -> D -> E -> C",
                  "09. DFS visita en profundidad en el orden correcto");
    }
    {
        Grafo g; armarGrafo(g);
        string camino;
        int km = g.dijkstra("C", "D", camino);
        // C->A->B->D = 2+4+1 = 7   (C->E->B->D = 8+5+1 = 14)
        verificar(km == 7 && camino == "C -> A -> B -> D",
                  "10. Dijkstra encuentra la ruta mas corta y el camino");
    }
    {
        Grafo g; armarGrafo(g);
        string camino;
        verificar(g.dijkstra("A", "F", camino) == -1 && g.dijkstra("A", "Z", camino) == -2,
                  "11. Dijkstra sin camino (-1) y ciudad inexistente (-2)");
    }

    // ---------- LISTA ----------
    {
        ListaPasajeros l;
        l.insertar(1, "Ana", 20);
        verificar(!l.insertar(1, "Otro", 30) && l.getCantidad() == 1,
                  "12. Pasajero con ID repetido se rechaza");
    }
    {
        ListaPasajeros l;
        l.insertar(1, "Ana", 20); l.insertar(2, "Luis", 30); l.insertar(3, "Sara", 25);
        verificar(!l.eliminar(99) && l.getCantidad() == 3 && l.idsEnOrden() == "1 2 3",
                  "13. Eliminar pasajero inexistente no corrompe la lista");
    }
    {
        ListaPasajeros l;
        l.insertar(1, "Ana", 20); l.insertar(2, "Luis", 30); l.insertar(3, "Sara", 25);
        bool ok = l.eliminar(2) && l.idsEnOrden() == "1 3";
        ok = ok && l.eliminar(1) && l.idsEnOrden() == "3";
        ok = ok && l.eliminar(3) && l.idsEnOrden() == "" && l.getCantidad() == 0;
        verificar(ok, "14. Eliminar en medio, al inicio y ultimo mantiene los enlaces");
    }
    {
        ListaPasajeros l;
        l.insertar(1, "Ana", 20);
        verificar(l.modificar(1, "Ana Maria", 21) && l.buscar(1)->nombre == "Ana Maria" &&
                  !l.modificar(5, "X", 10), "15. Modificar pasajero existente / inexistente");
    }

    // ---------- COLA ----------
    {
        Cola<int> c;
        int x = -1;
        verificar(!c.desencolar(x) && x == -1 && c.estaVacia(),
                  "16. Atender cola vacia no usa memoria invalida");
    }
    {
        Cola<int> c;
        c.encolar(10); c.encolar(20); c.encolar(30);
        int a, b, d;
        c.desencolar(a); c.desencolar(b); c.desencolar(d);
        verificar(a == 10 && b == 20 && d == 30 && c.estaVacia(), "17. Cola respeta FIFO");
    }

    // ---------- PILA ----------
    {
        Pila<int> p;
        p.apilar(1); p.apilar(2); p.apilar(3);
        int a, b, d, e;
        p.desapilar(a); p.desapilar(b); p.desapilar(d);
        verificar(a == 3 && b == 2 && d == 1 && !p.desapilar(e), "18. Pila respeta LIFO y vacia devuelve false");
    }

    // ---------- ABB ----------
    {
        ABB t;
        t.registrar("Medellin"); t.registrar("Cali"); t.registrar("Tunja");
        t.registrar("Cali");
        verificar(t.getCantidad() == 3 && t.buscar("Cali")->veces == 2 && t.buscar("Pasto") == nullptr,
                  "19. ABB inserta, cuenta repetidas y busca");
    }
    {
        ABB t;
        t.registrar("M"); t.registrar("C"); t.registrar("T");
        t.registrar("A"); t.registrar("E"); t.registrar("Z");
        // Eliminar "C" (tiene 2 hijos) -> lo reemplaza "E"
        bool ok = t.eliminar("C") && t.inorden() == "A(1) E(1) M(1) T(1) Z(1) ";
        ok = ok && t.eliminar("M") && t.inorden() == "A(1) E(1) T(1) Z(1) ";   // raiz
        ok = ok && !t.eliminar("Q") && t.getCantidad() == 4;
        verificar(ok, "20. ABB elimina (hoja, 1 hijo, 2 hijos, raiz) y sigue ordenado");
    }
    {
        ABB t;
        t.registrar("M"); t.registrar("C"); t.registrar("T");
        verificar(t.preorden() == "M(1) C(1) T(1) " && t.postorden() == "C(1) T(1) M(1) ",
                  "21. ABB recorridos preorden y postorden");
    }

    // ---------- ENTRADAS ----------
    {
        int n;
        verificar(!convertirEntero("abc", n) && !convertirEntero("12abc", n) &&
                  !convertirEntero("", n) && convertirEntero(" 42 ", n) && n == 42,
                  "22. Entrada alfabetica en campo numerico se detecta");
    }

    // ---------- PERSISTENCIA ----------
    {
        Grafo g; ListaPasajeros l; Cola<Solicitud> c;
        armarGrafo(g);
        int malas;
        bool cargo = cargarDatos("archivo_que_no_existe.txt", g, l, c, malas, false);
        verificar(!cargo && g.getNumCiudades() == 6, "23. Archivo faltante no borra los datos actuales");
    }
    {
        Grafo g; ListaPasajeros l; Cola<Solicitud> c;
        armarGrafo(g);
        l.insertar(1, "Ana", 20);
        Solicitud s; s.idPasajero = 1; s.origen = "A"; s.destino = "D";
        c.encolar(s);
        guardarDatos("prueba_tmp.txt", g, l, c);

        Grafo g2; ListaPasajeros l2; Cola<Solicitud> c2;
        int malas;
        cargarDatos("prueba_tmp.txt", g2, l2, c2, malas, false);
        string camino;
        verificar(malas == 0 && g2.getNumCiudades() == 6 && g2.contarConexiones() == 5 &&
                  l2.getCantidad() == 1 && c2.getCantidad() == 1 &&
                  g2.dijkstra("C", "D", camino) == 7,
                  "24. Guardar y cargar conserva todos los datos");
        remove("prueba_tmp.txt");
    }
    {
        ofstream f("corrupto_tmp.txt");
        f << "CIUDAD;X\n";
        f << "CIUDAD;Y\n";
        f << "basura sin sentido\n";
        f << "CONEXION;X;Y;abc\n";        // distancia no numerica
        f << "CONEXION;X;NoExiste;10\n";  // ciudad que no existe
        f << "CONEXION;X;Y;-5\n";         // distancia negativa
        f << "PASAJERO;uno;Ana;20\n";     // id no numerico
        f << "CONEXION;X;Y;7\n";          // esta si es valida
        f.close();
        Grafo g; ListaPasajeros l; Cola<Solicitud> c;
        int malas;
        cargarDatos("corrupto_tmp.txt", g, l, c, malas, false);
        verificar(malas == 5 && g.getNumCiudades() == 2 && g.contarConexiones() == 1,
                  "25. Archivo con lineas corruptas: se ignoran sin danar el sistema");
        remove("corrupto_tmp.txt");
    }

    cout << "\nResultado: " << pasadas << " pruebas OK, " << falladas << " fallaron.\n";
    return falladas == 0 ? 0 : 1;
}
