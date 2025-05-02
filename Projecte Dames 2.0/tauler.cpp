#define _CRT_SECURE_NO_WARNINGS
#include "tauler.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>


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
    if (!fitxer.is_open())
    { cerr << "Error al abrir archivo: " << nomFitxer << endl;
    }

    char tipus;
    string pos;
    while (fitxer >> tipus >> pos) 
    {
        try {
            Posicio p(pos);
            if (!esCassellaValid(p)) continue; // Solo colocar en casillas válidas

            int fila = p.getFila() - 1;
            int col = p.getColumna();

            TipusFitxa tf;
            ColorFitxa cf;

            switch (toupper(tipus)) {
            case 'X': tf = TIPUS_NORMAL; cf = COLOR_NEGRE; break;
            case 'O': tf = TIPUS_NORMAL; cf = COLOR_BLANC; break;
            case 'D': tf = TIPUS_DAMA; cf = COLOR_NEGRE; break;
            case 'R': tf = TIPUS_DAMA; cf = COLOR_BLANC; break;
            default: continue; // Ignorar tipos inválidos
            }

            if (fila >= 0 && fila < N_FILES && col >= 0 && col < N_COLUMNES) {
                m_tauler[fila][col] = Fitxa(tf, cf, p);
            }
        }
        catch (...) {
            continue; // Ignorar posiciones inválidas
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

    std::vector<Moviment> moviments;
    m_tauler[fila][col].getMovimentsValids(moviments);

    for (size_t i = 0; i < moviments.size() && nPosicions < MAX_POSICIONS; i++) {
        posicionsPossibles[nPosicions++] = moviments[i].getDesti();
    }
}

bool Tauler::mouFitxa(const Posicio& origen, const Posicio& desti) {
    bool movimentExit = false;

    if (esMovimentValid(origen, desti)) {
        int filaOrigen = origen.getFila() - 1;
        int colOrigen = origen.getColumna();
        int filaDesti = desti.getFila() - 1;
        int colDesti = desti.getColumna();

        // Verificar si el movimiento está en la lista de válidos
        vector<Moviment> moviments;
        m_tauler[filaOrigen][colOrigen].getMovimentsValids(moviments);

        for (const Moviment& mov : moviments) {
            if (mov.getDesti() == desti) {
                // Realizar el movimiento
                m_tauler[filaDesti][colDesti] = m_tauler[filaOrigen][colOrigen];
                m_tauler[filaDesti][colDesti].setPosicio(desti);
                m_tauler[filaOrigen][colOrigen] = Fitxa();
                movimentExit = true;
                break;
            }
        }
    }

    return movimentExit;
}

bool Tauler::esCassellaValid(const Posicio& pos) const {
    int col = pos.getColumna();
    int fila = pos.getFila() - 1; // Convertir a índice 0-based
    bool esValida = (col >= 0 && col < N_COLUMNES) &&(fila >= 0 && fila < N_FILES) && ((col + fila) % 2 == 0);
    return esValida;
}

bool Tauler::esMovimentValid(const Posicio& origen, const Posicio& desti) const {
    bool esValid = true;

    // Verificar rangos del tablero
    if (!esCassellaValid(origen) || !esCassellaValid(desti)) {
        esValid = false;
    }

    // Verificar que la ficha de origen existe y el destino está vacío
    int filaOrigen = origen.getFila() - 1;
    int colOrigen = origen.getColumna();
    int filaDesti = desti.getFila() - 1;
    int colDesti = desti.getColumna();

    if (esValid) 
    {
        if (m_tauler[filaOrigen][colOrigen].esBuida() || !m_tauler[filaDesti][colDesti].esBuida()) 
        {
            esValid = false;
        }
    }

    return esValid;
}

void Tauler::calculaMovimentsNormals(const Posicio& pos) {
    int fila = pos.getFila() - 1;
    int col = pos.getColumna();

    ColorFitxa color = m_tauler[fila][col].getColor();
    int direccio = (color == COLOR_NEGRE) ? -1 : 1; // Mantenemos esto sin cambios

    // Limpiar movimientos anteriores
    m_tauler[fila][col].netejaMovimentsValids();

    // 1. Buscar todas las capturas posibles
    for (int dcol : {-1, 1}) {
        int newFila = fila + 2 * direccio;
        int newCol = col + 2 * dcol;
        int filaComida = fila + direccio;
        int colComida = col + dcol;

        if (newFila >= 0 && newFila < N_FILES && newCol >= 0 && newCol < N_COLUMNES &&
            !m_tauler[filaComida][colComida].esBuida() &&
            m_tauler[filaComida][colComida].getColor() != color &&
            m_tauler[newFila][newCol].esBuida()) {

            Moviment mov(pos);
            mov.afegeixPecaMenjada(Posicio(coordenadasAPosicion(colComida, filaComida + 1)));
            mov.afegeixPas(Posicio(coordenadasAPosicion(newCol, newFila + 1)));
            m_tauler[fila][col].afegeixMovimentValid(mov);
        }
    }

    // 2. Buscar todos los movimientos simples posibles
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

void Tauler::calculaMovimentsDama(const Posicio& pos) {
    int fila = pos.getFila() - 1;  // Convertir a índice 0-based
    int col = pos.getColumna();

    ColorFitxa color = m_tauler[fila][col].getColor();
    m_tauler[fila][col].netejaMovimentsValids();

    // Direcciones diagonales (4 posibles)
    const int direcciones[4][2] = { {1, 1}, {1, -1}, {-1, 1}, {-1, -1} };

    for (const auto& dir : direcciones) {
        int dfila = dir[0];
        int dcol = dir[1];
        bool encontradoEnemigo = false;
        Posicio posEnemigo;

        for (int paso = 1; ; ++paso) {
            int newFila = fila + paso * dfila;
            int newCol = col + paso * dcol;

            // Check bounds before accessing array
            if (newFila < 0 || newFila >= N_FILES || newCol < 0 || newCol >= N_COLUMNES) {
                break;  // Out of bounds, stop checking this direction
            }

            // Caso 1: Casilla vacía (movimiento válido)
            if (m_tauler[newFila][newCol].esBuida()) {
                Moviment mov(pos);
                mov.afegeixPas(Posicio(coordenadasAPosicion(newCol, newFila + 1)));
                if (encontradoEnemigo) {
                    mov.afegeixPecaMenjada(posEnemigo);
                }
                m_tauler[fila][col].afegeixMovimentValid(mov);
            }
            // Caso 2: Ficha enemiga (posible captura)
            else if (!encontradoEnemigo && m_tauler[newFila][newCol].getColor() != color) {
                encontradoEnemigo = true;
                posEnemigo = Posicio(coordenadasAPosicion(newCol, newFila + 1));
            }
            // Caso 3: Ficha del mismo color o segunda ficha enemiga - bloquea el camino
            else {
                break;
            }
        }
    }
}

void Tauler::comprovaMovimentMaxim(const Posicio& pos) {
    int fila = pos.getFila() - 1;
    int col = pos.getColumna();

    // Limpiar movimientos no capturas si hay capturas posibles
    std::vector<Moviment> moviments;
    m_tauler[fila][col].getMovimentsValids(moviments);

    bool teCaptures = false;
    for (const auto& mov : moviments) {
        if (mov.esMenjada()) {
            teCaptures = true;
        }
    }

    // Si hay capturas, eliminar movimientos sin captura
    if (teCaptures) {
        std::vector<Moviment> movimentsFiltrats;
        for (const auto& mov : moviments) {
            if (mov.esMenjada()) {
                movimentsFiltrats.push_back(mov);
            }
        }
        m_tauler[fila][col].netejaMovimentsValids();
        for (const auto& mov : movimentsFiltrats) {
            m_tauler[fila][col].afegeixMovimentValid(mov);
        }
    }
}

void Tauler::bufaFitxa(const Posicio& pos) {
    int fila = pos.getFila() - 1;
    int col = pos.getColumna();

    // Verificar que la posición es válida y hay una ficha normal
    if (fila < 0 || fila >= N_FILES || col < 0 || col >= N_COLUMNES ||
        m_tauler[fila][col].esBuida() || m_tauler[fila][col].getTipus() != TIPUS_NORMAL) {
        return;
    }

    ColorFitxa color = m_tauler[fila][col].getColor();
    bool esExtremo = (color == COLOR_NEGRE && fila == N_FILES - 1) ||
        (color == COLOR_BLANC && fila == 0);

    if (esExtremo) {
        m_tauler[fila][col].converteixEnDama();
    }
}