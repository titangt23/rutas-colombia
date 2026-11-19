// ============================================================
//  Utilidades.h
//  Funciones de ayuda para leer datos del teclado SIN que el
//  programa se dane si el usuario escribe algo incorrecto.
//  Ejemplo: si piden un numero y escriben "abc", se vuelve a pedir.
// ============================================================
#ifndef UTILIDADES_H
#define UTILIDADES_H

#include <iostream>
#include <string>
#include <sstream>
#include <cstdlib>
using namespace std;

// Quita los espacios del inicio y del final de un texto.
// "  Bogota  "  ->  "Bogota"
string recortar(string texto) {
    size_t inicio = texto.find_first_not_of(" \t\r\n");
    if (inicio == string::npos) return "";          // solo habia espacios
    size_t fin = texto.find_last_not_of(" \t\r\n");
    return texto.substr(inicio, fin - inicio + 1);
}

// Intenta convertir un texto a numero entero.
// Devuelve true si se pudo, false si el texto NO es un numero.
// "42" -> true     "abc" -> false     "12abc" -> false
bool convertirEntero(string texto, int &numero) {
    texto = recortar(texto);
    if (texto == "") return false;
    stringstream ss(texto);
    ss >> numero;
    // ss.fail()  -> no habia un numero
    // !ss.eof()  -> sobraron letras despues del numero
    if (ss.fail() || !ss.eof()) return false;
    return true;
}

// Lee una linea completa del teclado.
// Si la entrada se acaba (por ejemplo, Ctrl+Z), sale ordenadamente.
string leerLinea() {
    string linea;
    if (!getline(cin, linea)) {
        cout << "\nFin de la entrada. Saliendo del programa...\n";
        exit(0);
    }
    return linea;
}

// Pide un numero entero hasta que el usuario escriba uno valido.
int leerEntero(string mensaje) {
    int numero;
    while (true) {
        cout << mensaje;
        if (convertirEntero(leerLinea(), numero)) return numero;
        cout << "  [Error] Debe escribir un numero entero. Intente de nuevo.\n";
    }
}

// Pide un numero entero que este entre minimo y maximo.
int leerEnteroRango(string mensaje, int minimo, int maximo) {
    while (true) {
        int numero = leerEntero(mensaje);
        if (numero >= minimo && numero <= maximo) return numero;
        cout << "  [Error] El numero debe estar entre " << minimo
             << " y " << maximo << ".\n";
    }
}

// Pide un texto que no este vacio y que no tenga ';'
// (el ';' se usa como separador en el archivo de datos).
string leerTexto(string mensaje) {
    while (true) {
        cout << mensaje;
        string texto = recortar(leerLinea());
        if (texto == "") {
            cout << "  [Error] El texto no puede estar vacio.\n";
        } else if (texto.find(';') != string::npos) {
            cout << "  [Error] El texto no puede contener el caracter ';'.\n";
        } else {
            return texto;
        }
    }
}

// Espera a que el usuario presione Enter.
void pausar() {
    cout << "\nPresione Enter para continuar...";
    leerLinea();
}

#endif
