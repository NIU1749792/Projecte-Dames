#include <iostream>
#include "tauler.h"

using namespace std;

void mostrarMenu() {
    cout << "\n=== MENU DE PRUEBAS ===" << endl;
    cout << "1. Mostrar tablero" << endl;
    cout << "2. Cargar tablero desde archivo" << endl;
    cout << "3. Actualizar movimientos validos" << endl;
    cout << "4. Mostrar posiciones posibles para una ficha" << endl;
    cout << "5. Mover ficha" << endl;
    cout << "6. Salir" << endl;
    cout << "Seleccione una opcion: ";
}

void cargarTablero(Tauler& tauler) {
    char nombreArchivo[100];
    cout << "Introduzca el nombre del archivo de configuracion: ";
    cin >> nombreArchivo;

    try {
        tauler.inicialitza(nombreArchivo);
        cout << "Tablero cargado correctamente." << endl;
    }
    catch (const exception& e) {
        cerr << "Error al cargar el archivo: " << e.what() << endl;
    }
}

void mostrarPosicionesPosibles(Tauler& tauler) {
    string posicionStr;
    cout << "Introduzca la posicion de la ficha (ej: a3): ";
    cin >> posicionStr;

    try {
        Posicio origen(posicionStr);
        std::vector<Posicio> posiciones;
        int nPosiciones = 0;

        // Crear array temporal para mantener compatibilidad
        Posicio tempPos[50];
        tauler.getPosicionsPossibles(origen, nPosiciones, tempPos, 50);

        if (nPosiciones == 0) {
            cout << "No hay movimientos validos para esta ficha." << endl;
        }
        else {
            cout << "Posiciones posibles: ";
            for (int i = 0; i < nPosiciones; i++) {
                cout << tempPos[i].toString() << " ";
            }
            cout << endl;
        }
    }
    catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
    }
}

void moverFicha(Tauler& tauler) {
    string origenStr, destiStr;
    cout << "Introduzca la posicion de origen (ej: a3): ";
    cin >> origenStr;
    cout << "Introduzca la posicion de destino (ej: b4): ";
    cin >> destiStr;

    try {
        Posicio origen(origenStr);
        Posicio desti(destiStr);

        if (tauler.mouFitxa(origen, desti)) {
            cout << "Movimiento realizado con exito." << endl;
        }
        else {
            cout << "Movimiento no valido." << endl;
        }
    }
    catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
    }
}

int main() {
    Tauler tauler;
    char buffer[1000];
    int opcion;

    do {
        tauler.toString(buffer, sizeof(buffer));
        cout << buffer << endl;

        mostrarMenu();
        cin >> opcion;

        switch (opcion) {
        case 1:
            // El tablero ya se muestra al inicio del bucle
            break;
        case 2:
            cargarTablero(tauler);
            break;
        case 3:
            tauler.actualitzaMovimentsValids();
            cout << "Movimientos validos actualizados." << endl;
            break;
        case 4:
            mostrarPosicionesPosibles(tauler);
            break;
        case 5:
            moverFicha(tauler);
            break;
        case 6:
            cout << "Saliendo del programa..." << endl;
            break;
        default:
            cout << "Opcion no valida. Intente de nuevo." << endl;
        }

    } while (opcion != 6);

    return 0;
}