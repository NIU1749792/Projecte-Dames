#define _CRT_SECURE_NO_WARNINGS
#include "tauler.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iomanip>

using namespace std;

// Función auxiliar para convertir coordenadas numéricas a notación "a1"
string coordenadasAPosicion(int columna, int fila) {
    if (columna < 0 || columna > 7 || fila < 1 || fila > 8) {
        throw out_of_range("Coordenadas fuera de rango");
    }
    char letra = 'a' + columna;
    return string(1, letra) + to_string(fila);
}

Tauler::Tauler() {
    // Inicializar todas las casillas como vacías
    for (int i = 0; i < N_FILES; i++) {
        for (int j = 0; j < N_COLUMNES; j++) {
            m_tauler[i][j] = Fitxa();
        }
    }
}

void Tauler::inicialitza(const char nomFitxer[]) {
    ifstream fitxer(nomFitxer);
    if (!fitxer.is_open()) {
        throw runtime_error("No s'ha pogut obrir el fitxer: " + string(nomFitxer));
    }

    // Limpiar el tablero
    for (int i = 0; i < N_FILES; i++) {
        for (int j = 0; j < N_COLUMNES; j++) {
            m_tauler[i][j] = Fitxa();
        }
    }

    char tipus;
    string pos;
    while (fitxer >> tipus >> pos) {
        Posicio p(pos);
        int fila = p.getFila() - 1; // Convertir a índice 0-7
        int col = p.getColumna();

        TipusFitxa tf;
        ColorFitxa cf;

        switch (toupper(tipus)) {
        case 'X':
            tf = TIPUS_NORMAL;
            cf = COLOR_NEGRE;
            break;
        case 'O':
            tf = TIPUS_NORMAL;
            cf = COLOR_BLANC;
            break;
        case 'D':
            tf = TIPUS_DAMA;
            cf = COLOR_NEGRE;
            break;
        case 'R':
            tf = TIPUS_DAMA;
            cf = COLOR_BLANC;
            break;
        default:
            throw invalid_argument("Tipus de fitxa no vàlid: " + string(1, tipus));
        }

        m_tauler[fila][col] = Fitxa(tf, cf, p);
    }
    fitxer.close();
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
void Tauler::getPosicionsPossibles(const Posicio& origen, int& nPosicions, Posicio posicionsPossibles[], int maxPosicions) {
    nPosicions = 0;
    int fila = origen.getFila() - 1;
    int col = origen.getColumna();

    if (fila < 0 || fila >= N_FILES || col < 0 || col >= N_COLUMNES) return;

    std::vector<Moviment> moviments;
    m_tauler[fila][col].getMovimentsValids(moviments);

    for (size_t i = 0; i < moviments.size() && nPosicions < maxPosicions; i++) {
        posicionsPossibles[nPosicions++] = moviments[i].getDesti();
    }
}

bool Tauler::mouFitxa(const Posicio& origen, const Posicio& desti) {
    if (!esCassellaValid(desti) || !esMovimentValid(origen, desti)) {
        return false;
    }

    int filaOrigen = origen.getFila() - 1;
    int colOrigen = origen.getColumna();
    int filaDesti = desti.getFila() - 1;
    int colDesti = desti.getColumna();

    // Mover la ficha
    m_tauler[filaDesti][colDesti] = m_tauler[filaOrigen][colOrigen];
    m_tauler[filaDesti][colDesti].setPosicio(desti);
    m_tauler[filaOrigen][colOrigen] = Fitxa();

    // Comprobar si se convierte en dama
    if (m_tauler[filaDesti][colDesti].getTipus() == TIPUS_NORMAL) {
        if ((m_tauler[filaDesti][colDesti].getColor() == COLOR_NEGRE && filaDesti == N_FILES - 1) ||
            (m_tauler[filaDesti][colDesti].getColor() == COLOR_BLANC && filaDesti == 0)) {
            m_tauler[filaDesti][colDesti].converteixEnDama();
        }
    }

    return true;
}

void Tauler::toString(char buffer[], int bufferSize) const {
    stringstream ss;

    for (int i = 0; i < N_FILES; i++) {
        ss << (N_FILES - i) << ": ";
        for (int j = 0; j < N_COLUMNES; j++) {
            char c = '_';
            if (!m_tauler[i][j].esBuida()) {
                switch (m_tauler[i][j].getTipus()) {
                case TIPUS_NORMAL:
                    c = (m_tauler[i][j].getColor() == COLOR_NEGRE) ? 'X' : 'O';
                    break;
                case TIPUS_DAMA:
                    c = (m_tauler[i][j].getColor() == COLOR_NEGRE) ? 'D' : 'R';
                    break;
                default:
                    c = '_';
                }
            }
            ss << c;
            if (j != N_COLUMNES - 1) ss << " ";
        }
        ss << endl;
    }
    ss << "   a b c d e f g h";

    string result = ss.str();
    strncpy(buffer, result.c_str(), bufferSize - 1);
    buffer[bufferSize - 1] = '\0';
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
    int fila_idx = pos.getFila() - 1;
    int col_idx = pos.getColumna();

    if (m_tauler[fila_idx][col_idx].esBuida()) return;

    ColorFitxa color = m_tauler[fila_idx][col_idx].getColor();
    int direccio = (color == COLOR_NEGRE) ? 1 : -1; // Negras avanzan hacia abajo, blancas hacia arriba

    // Movimientos simples (sin captura)
    int newFila_idx = fila_idx + direccio;
    if (newFila_idx >= 0 && newFila_idx < N_FILES) {
        // Diagonal izquierda
        int newCol_idx = col_idx - 1;
        if (newCol_idx >= 0 && m_tauler[newFila_idx][newCol_idx].esBuida()) {
            Moviment mov(pos);
            mov.afegeixPas(Posicio(coordenadasAPosicion(newCol_idx, newFila_idx + 1)));
            m_tauler[fila_idx][col_idx].afegeixMovimentValid(mov);
        }

        // Diagonal derecha
        newCol_idx = col_idx + 1;
        if (newCol_idx < N_COLUMNES && m_tauler[newFila_idx][newCol_idx].esBuida()) {
            Moviment mov(pos);
            mov.afegeixPas(Posicio(coordenadasAPosicion(newCol_idx, newFila_idx + 1)));
            m_tauler[fila_idx][col_idx].afegeixMovimentValid(mov);
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