#define _CRT_SECURE_NO_WARNINGS
#include "tauler.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iomanip>
#include <cstring>

using namespace std;

Tauler::Tauler() {
    for (int i = 0; i < N_FILES; i++) {
        for (int j = 0; j < N_COLUMNES; j++) {
            m_tauler[i][j] = Fitxa();
        }
    }
}

std::string Tauler::coordenadasAPosicion(int columna, int fila) const {
    if (columna < 0 || columna > 7 || fila < 1 || fila > 8) {
        throw std::out_of_range("Coordenadas fuera de rango");
    }
    char letra = 'a' + columna;
    return std::string(1, letra) + std::to_string(fila);
}

void Tauler::inicialitza(const std::string& nomFitxer) {
    // Limpiar tablero
    for (int i = 0; i < N_FILES; i++) {
        for (int j = 0; j < N_COLUMNES; j++) {
            m_tauler[i][j] = Fitxa();
        }
    }

    ifstream fitxer(nomFitxer);
    if (!fitxer.is_open()) {
        cerr << "Error al abrir archivo: " << nomFitxer << endl;
        return;
    }

    char tipus;
    string pos;
    while (fitxer >> tipus >> pos) {
        try {
            Posicio p(pos);
            int fila = p.getFila() - 1;  // Convertir a índice 0-based
            int col = p.getColumna();

            TipusFitxa tf;
            ColorFitxa cf;

            switch (toupper(tipus)) {
            case 'X': tf = TIPUS_NORMAL; cf = COLOR_NEGRE; break;
            case 'O': tf = TIPUS_NORMAL; cf = COLOR_BLANC; break;
            case 'D': tf = TIPUS_DAMA; cf = COLOR_NEGRE; break;
            case 'R': tf = TIPUS_DAMA; cf = COLOR_BLANC; break;
            default: continue;  // Ignorar tipos inválidos
            }

            if (fila >= 0 && fila < N_FILES && col >= 0 && col < N_COLUMNES) {
                m_tauler[fila][col] = Fitxa(tf, cf, p);
            }
        }
        catch (...) {
            continue;  // Ignorar posiciones inválidas
        }
    }
}

std::string Tauler::toString() const {
    std::stringstream ss;
    for (int i = N_FILES - 1; i >= 0; i--) {  // Cambiado para iterar de 7 a 0
        ss << (i + 1) << ": ";  // Mostrar número de fila correcto (1-8)
        for (int j = 0; j < N_COLUMNES; j++) {
            char c = '_';
            if (!m_tauler[i][j].esBuida()) {
                switch (m_tauler[i][j].getTipus()) {
                case TIPUS_NORMAL: c = (m_tauler[i][j].getColor() == COLOR_NEGRE) ? 'X' : 'O'; break;
                case TIPUS_DAMA: c = (m_tauler[i][j].getColor() == COLOR_NEGRE) ? 'D' : 'R'; break;
                default: c = '_';
                }
            }
            ss << c;
            if (j != N_COLUMNES - 1) ss << " ";
        }
        ss << "\n";
    }
    ss << "   a b c d e f g h";
    return ss.str();
}

void Tauler::actualitzaMovimentsValids() {
    for (int i = 0; i < N_FILES; i++) {
        for (int j = 0; j < N_COLUMNES; j++) {
            if (!m_tauler[i][j].esBuida()) {
                m_tauler[i][j].netejaMovimentsValids();
                Posicio pos(coordenadasAPosicion(j, i + 1)); // i+1 porque filas van de 1-8

                if (m_tauler[i][j].getTipus() == TIPUS_NORMAL) {
                    calculaMovimentsNormals(pos);
                }
                else {
                    calculaMovimentsDama(pos);
                }
            }
        }
    }
}

/**
 * @brief Obtiene las posiciones posibles para una ficha
 * @param origen Posición de la ficha a consultar
 * @param[out] nPosicions Número de posiciones válidas encontradas
 * @param[out] posicionsPossibles Array donde almacenar las posiciones
 * @param maxPosicions Tamaño máximo del array de posiciones
 */
void Tauler::getPosicionsPossibles(const Posicio& origen, int& nPosicions, Posicio posicionsPossibles[]) {
    const int MAX_POSICIONS = 20; // Define un máximo razonable
    nPosicions = 0;
    int fila = origen.getFila() - 1;
    int col = origen.getColumna();

    if (fila < 0 || fila >= N_FILES || col < 0 || col >= N_COLUMNES) return;

    std::vector<Moviment> moviments;
    m_tauler[fila][col].getMovimentsValids(moviments);

    for (size_t i = 0; i < moviments.size() && nPosicions < MAX_POSICIONS; i++) {
        posicionsPossibles[nPosicions++] = moviments[i].getDesti();
    }
}

bool Tauler::mouFitxa(const Posicio& origen, const Posicio& desti) {
    int filaOrigen = origen.getFila() - 1;
    int colOrigen = origen.getColumna();
    int filaDesti = desti.getFila() - 1;
    int colDesti = desti.getColumna();

    // Validaciones básicas
    if (filaOrigen < 0 || filaOrigen >= N_FILES || colOrigen < 0 || colOrigen >= N_COLUMNES ||
        filaDesti < 0 || filaDesti >= N_FILES || colDesti < 0 || colDesti >= N_COLUMNES ||
        m_tauler[filaOrigen][colOrigen].esBuida() || !m_tauler[filaDesti][colDesti].esBuida()) {
        return false;
    }

    // Verificar si el movimiento es válido
    vector<Moviment> moviments;
    m_tauler[filaOrigen][colOrigen].getMovimentsValids(moviments);

    bool movimentValid = false;
    Moviment movimentRealitzat;

    for (const Moviment& mov : moviments) {
        if (mov.getDesti() == desti) {
            movimentValid = true;
            movimentRealitzat = mov;
            break;
        }
    }

    if (!movimentValid) return false;

    // Realizar el movimiento
    m_tauler[filaDesti][colDesti] = m_tauler[filaOrigen][colOrigen];
    m_tauler[filaDesti][colDesti].setPosicio(desti);
    m_tauler[filaOrigen][colOrigen] = Fitxa();

    // Eliminar fichas capturadas
    for (const Posicio& capturada : movimentRealitzat.getPecesMenjades()) {
        int fila = capturada.getFila() - 1;
        int col = capturada.getColumna();
        if (fila >= 0 && fila < N_FILES && col >= 0 && col < N_COLUMNES) {
            m_tauler[fila][col] = Fitxa();
        }
    }

    // Convertir a dama si llega al extremo opuesto
    if (m_tauler[filaDesti][colDesti].getTipus() == TIPUS_NORMAL) {
        if ((m_tauler[filaDesti][colDesti].getColor() == COLOR_NEGRE && filaDesti == N_FILES - 1) ||
            (m_tauler[filaDesti][colDesti].getColor() == COLOR_BLANC && filaDesti == 0)) {
            m_tauler[filaDesti][colDesti].converteixEnDama();
        }
    }

    return true;
}

bool Tauler::esCassellaValid(const Posicio& desti) const {
    int col = desti.getColumna() + 1; // a=1, h=8
    int fila = desti.getFila();
    return (col + fila) % 2 == 0;
}

bool Tauler::esMovimentValid(const Posicio& origen, const Posicio& desti) const {
    int filaOrigen = origen.getFila() - 1;
    int colOrigen = origen.getColumna();
    int filaDesti = desti.getFila() - 1;
    int colDesti = desti.getColumna();

    if (filaOrigen < 0 || filaOrigen >= N_FILES || colOrigen < 0 || colOrigen >= N_COLUMNES ||
        filaDesti < 0 || filaDesti >= N_FILES || colDesti < 0 || colDesti >= N_COLUMNES) {
        return false;
    }

    if (m_tauler[filaOrigen][colOrigen].esBuida()) return false;
    if (!m_tauler[filaDesti][colDesti].esBuida()) return false;

    // Implementación simplificada para la primera versión
    return true;
}

void Tauler::calculaMovimentsNormals(const Posicio& pos) {
    int fila = pos.getFila() - 1;
    int col = pos.getColumna();

    if (fila < 0 || fila >= N_FILES || col < 0 || col >= N_COLUMNES ||
        m_tauler[fila][col].esBuida()) {
        return;
    }

    ColorFitxa color = m_tauler[fila][col].getColor();
    int direccio = (color == COLOR_NEGRE) ? 1 : -1;  // Negras hacia abajo, blancas hacia arriba

    // Primero buscar capturas obligatorias
    bool tieneCapturas = false;

    // Comprobar capturas en las 4 direcciones diagonales
    for (int dfila : {direccio, -direccio}) {
        for (int dcol : {-1, 1}) {
            int newFila = fila + 2 * dfila;
            int newCol = col + 2 * dcol;
            int filaComida = fila + dfila;
            int colComida = col + dcol;

            if (newFila >= 0 && newFila < N_FILES && newCol >= 0 && newCol < N_COLUMNES &&
                !m_tauler[filaComida][colComida].esBuida() &&
                m_tauler[filaComida][colComida].getColor() != color &&
                m_tauler[newFila][newCol].esBuida()) {

                tieneCapturas = true;
                Moviment mov(pos);
                mov.afegeixPecaMenjada(Posicio(coordenadasAPosicion(colComida, filaComida + 1)));
                mov.afegeixPas(Posicio(coordenadasAPosicion(newCol, newFila + 1)));
                m_tauler[fila][col].afegeixMovimentValid(mov);
            }
        }
    }

    // Solo permitir movimientos simples si no hay capturas
    if (!tieneCapturas) {
        int newFila = fila + direccio;
        if (newFila >= 0 && newFila < N_FILES) {
            for (int dcol : {-1, 1}) {
                int newCol = col + dcol;
                if (newCol >= 0 && newCol < N_COLUMNES &&
                    m_tauler[newFila][newCol].esBuida()) {
                    Moviment mov(pos);
                    mov.afegeixPas(Posicio(coordenadasAPosicion(newCol, newFila + 1)));
                    m_tauler[fila][col].afegeixMovimentValid(mov);
                }
            }
        }
    }
}


void Tauler::calculaMovimentsDama(const Posicio& pos) {
    // Implementación básica para la primera versión
    // En versiones posteriores se implementará el movimiento completo de damas
}

void Tauler::comprovaMovimentMaxim(const Posicio& pos) {
    // Implementación pendiente para versiones futuras
}

void Tauler::bufaFitxa(const Posicio& pos) {
    // Implementación pendiente para versiones futuras
}