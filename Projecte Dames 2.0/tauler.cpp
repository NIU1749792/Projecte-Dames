#define _CRT_SECURE_NO_WARNINGS
#include "tauler.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iostream>

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
                Posicio pos(coordenadasAPosicion(j, i + 1));

                if (m_tauler[i][j].getTipus() == TIPUS_NORMAL) {
                    calculaMovimentsNormals(pos);
                }
                else {
                    calculaMovimentsDama(pos);
                }

                // Aplicar regla de captura obligatoria
                comprovaMovimentMaxim(pos);
            }
        }
    }

    // Eliminate duplicate movements
    eliminarMovimentsDuplicats();
}

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
                Fitxa fitxaMovida = m_tauler[filaOrigen][colOrigen];
                fitxaMovida.setPosicio(desti);

                // Eliminar ficha del origen
                m_tauler[filaOrigen][colOrigen] = Fitxa();

                // Eliminar todas las piezas capturadas
                const std::vector<Posicio>& pecesMenjades = mov.getPecesMenjades();
                for (const Posicio& pecaMenjada : pecesMenjades) {
                    int filaMenjada = pecaMenjada.getFila() - 1;
                    int colMenjada = pecaMenjada.getColumna();
                    m_tauler[filaMenjada][colMenjada] = Fitxa();
                }

                // Colocar ficha en destino
                m_tauler[filaDesti][colDesti] = fitxaMovida;

                // Verificar si se debe convertir en dama
                bufaFitxa(desti);

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
    bool esValida = (col >= 0 && col < N_COLUMNES) &&
        (fila >= 0 && fila < N_FILES) &&
        ((col + fila) % 2 == 0);
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

    if (esValid) {
        if (m_tauler[filaOrigen][colOrigen].esBuida() || !m_tauler[filaDesti][colDesti].esBuida()) {
            esValid = false;
        }
    }

    return esValid;
}

void Tauler::calculaMovimentsNormals(const Posicio& pos) {
    int fila = pos.getFila() - 1;
    int col = pos.getColumna();

    ColorFitxa color = m_tauler[fila][col].getColor();
    int direccio = (color == COLOR_NEGRE) ? -1 : 1;

    // Limpiar movimientos anteriores
    m_tauler[fila][col].netejaMovimentsValids();

    // Crear matriz dinámica de visitados
    auto visitados = crearMatriuVisitados();

    std::vector<Moviment> movimentsCompletos;

    // 1. Buscar capturas en todas las direcciones diagonales
    bool hayCaptures = false;

    // Direcciones diagonales: arriba-izquierda, arriba-derecha, abajo-izquierda, abajo-derecha
    const int direcciones[4][2] = { {-1, -1}, {-1, 1}, {1, -1}, {1, 1} };

    for (const auto& dir : direcciones) {
        int dfila = dir[0];  // -1 para arriba, 1 para abajo
        int dcol = dir[1];   // -1 para izquierda, 1 para derecha

        int newFila = fila + 2 * dfila;
        int newCol = col + 2 * dcol;
        int filaComida = fila + dfila;
        int colComida = col + dcol;

        if (newFila >= 0 && newFila < N_FILES && newCol >= 0 && newCol < N_COLUMNES &&
            filaComida >= 0 && filaComida < N_FILES && colComida >= 0 && colComida < N_COLUMNES &&
            !m_tauler[filaComida][colComida].esBuida() &&
            m_tauler[filaComida][colComida].getColor() != color &&
            m_tauler[newFila][newCol].esBuida()) {

            hayCaptures = true;

            // Crear movimiento inicial
            Moviment mov(pos);
            mov.afegeixPecaMenjada(Posicio(coordenadasAPosicion(colComida, filaComida + 1)));
            mov.afegeixPas(Posicio(coordenadasAPosicion(newCol, newFila + 1)));

            // Marcar la pieza como comida temporalmente
            visitados[filaComida][colComida] = true;

            // Buscar capturas adicionales desde la nueva posición
            calculaMovimentsEnCadena(Posicio(coordenadasAPosicion(newCol, newFila + 1)),
                mov, movimentsCompletos, visitados);

            // Desmarcar para próximas iteraciones
            visitados[filaComida][colComida] = false;
        }
    }

    // 2. Calcular movimientos simples (solo hacia adelante para fichas normales)
    int newFila = fila + direccio;
    if (newFila >= 0 && newFila < N_FILES) {
        for (int dcol : {-1, 1}) {
            int newCol = col + dcol;
            if (newCol >= 0 && newCol < N_COLUMNES &&
                m_tauler[newFila][newCol].esBuida()) {
                Moviment mov(pos);
                mov.afegeixPas(Posicio(coordenadasAPosicion(newCol, newFila + 1)));
                movimentsCompletos.push_back(mov);
            }
        }
    }

    // Añadir todos los movimientos encontrados
    for (const Moviment& mov : movimentsCompletos) {
        m_tauler[fila][col].afegeixMovimentValid(mov);
    }
}

void Tauler::calculaMovimentsDama(const Posicio& pos) {
    int fila = pos.getFila() - 1;
    int col = pos.getColumna();

    ColorFitxa color = m_tauler[fila][col].getColor();
    m_tauler[fila][col].netejaMovimentsValids();

    auto visitados = crearMatriuVisitados();
    std::vector<Moviment> movimentsCompletos;
    bool hayCaptures = false;

    const int direcciones[4][2] = { {1, 1}, {1, -1}, {-1, 1}, {-1, -1} };

    // 1. Buscar capturas en todas las direcciones diagonales
    for (const auto& dir : direcciones) {
        int dfila = dir[0];
        int dcol = dir[1];

        // Buscar enemigos en esta dirección y sus posibles aterrizajes
        for (int pasoEnemigo = 1; pasoEnemigo < 8; ++pasoEnemigo) {
            int filaEnemigo = fila + pasoEnemigo * dfila;
            int colEnemigo = col + pasoEnemigo * dcol;

            // Verificar límites
            if (filaEnemigo < 0 || filaEnemigo >= N_FILES ||
                colEnemigo < 0 || colEnemigo >= N_COLUMNES) {
                break;
            }

            if (m_tauler[filaEnemigo][colEnemigo].esBuida()) {
                // Casilla vacía, continuar buscando
                continue;
            }
            else if (m_tauler[filaEnemigo][colEnemigo].getColor() != color) {
                // Encontramos enemigo, buscar posiciones de aterrizaje
                for (int pasoLand = 1; pasoLand < 8; ++pasoLand) {
                    int filaLand = filaEnemigo + pasoLand * dfila;
                    int colLand = colEnemigo + pasoLand * dcol;

                    if (filaLand < 0 || filaLand >= N_FILES ||
                        colLand < 0 || colLand >= N_COLUMNES) {
                        break;
                    }

                    if (m_tauler[filaLand][colLand].esBuida()) {
                        // Posición de aterrizaje válida
                        hayCaptures = true;

                        Moviment mov(pos);
                        mov.afegeixPecaMenjada(Posicio(coordenadasAPosicion(colEnemigo, filaEnemigo + 1)));
                        mov.afegeixPas(Posicio(coordenadasAPosicion(colLand, filaLand + 1)));

                        // Marcar enemigo como visitado
                        visitados[filaEnemigo][colEnemigo] = true;

                        // Usar la misma lógica que las fichas normales para cadenas
                        calculaMovimentsEnCadena(Posicio(coordenadasAPosicion(colLand, filaLand + 1)),
                            mov, movimentsCompletos, visitados);

                        // Desmarcar para próximas iteraciones
                        visitados[filaEnemigo][colEnemigo] = false;
                    }
                    else {
                        // No se puede aterrizar más lejos, salir del bucle de aterrizaje
                        break;
                    }
                }
                // Salir del bucle de búsqueda de enemigos en esta dirección
                break;
            }
            else {
                // Encontramos pieza propia, no se puede continuar en esta dirección
                break;
            }
        }
    }

    // 2. Si no hay capturas, calcular movimientos normales
    if (!hayCaptures) {
        for (const auto& dir : direcciones) {
            int dfila = dir[0];
            int dcol = dir[1];

            for (int paso = 1; paso < 8; ++paso) {
                int newFila = fila + paso * dfila;
                int newCol = col + paso * dcol;

                if (newFila < 0 || newFila >= N_FILES ||
                    newCol < 0 || newCol >= N_COLUMNES) {
                    break;
                }

                if (m_tauler[newFila][newCol].esBuida()) {
                    // Posición vacía - agregar como movimiento normal
                    Moviment mov(pos);
                    mov.afegeixPas(Posicio(coordenadasAPosicion(newCol, newFila + 1)));
                    movimentsCompletos.push_back(mov);
                }
                else {
                    // Encontramos una pieza, no se puede continuar más allá
                    break;
                }
            }
        }
    }

    // Añadir todos los movimientos encontrados
    for (const Moviment& mov : movimentsCompletos) {
        m_tauler[fila][col].afegeixMovimentValid(mov);
    }
}

void Tauler::calculaMovimentsDamaEnCadena(const Posicio& pos, Moviment& movActual,
    std::vector<Moviment>& movimentsCompletos,
    std::vector<std::vector<bool>>& visitados) {
    int fila = pos.getFila() - 1;
    int col = pos.getColumna();

    // Obtener color de la dama original
    ColorFitxa color = m_tauler[movActual.getOrigen().getFila() - 1][movActual.getOrigen().getColumna()].getColor();

    bool hayMasCaptures = false;
    const int direcciones[4][2] = { {1, 1}, {1, -1}, {-1, 1}, {-1, -1} };

    // Buscar capturas adicionales en todas las direcciones
    for (const auto& dir : direcciones) {
        int dfila = dir[0];
        int dcol = dir[1];

        // Buscar enemigos en esta dirección
        for (int pasoEnemigo = 1; pasoEnemigo < 8; ++pasoEnemigo) {
            int filaEnemigo = fila + pasoEnemigo * dfila;
            int colEnemigo = col + pasoEnemigo * dcol;

            if (filaEnemigo < 0 || filaEnemigo >= N_FILES ||
                colEnemigo < 0 || colEnemigo >= N_COLUMNES) {
                break;
            }

            if (m_tauler[filaEnemigo][colEnemigo].esBuida()) {
                // Casilla vacía, continuar buscando
                continue;
            }
            else if (!visitados[filaEnemigo][colEnemigo] &&
                m_tauler[filaEnemigo][colEnemigo].getColor() != color) {
                // Encontramos enemigo no visitado, buscar posiciones de aterrizaje
                for (int pasoLand = 1; pasoLand < 8; ++pasoLand) {
                    int filaLand = filaEnemigo + pasoLand * dfila;
                    int colLand = colEnemigo + pasoLand * dcol;

                    if (filaLand < 0 || filaLand >= N_FILES ||
                        colLand < 0 || colLand >= N_COLUMNES) {
                        break;
                    }

                    if (m_tauler[filaLand][colLand].esBuida()) {
                        // Posición de aterrizaje válida
                        hayMasCaptures = true;

                        // Crear nuevo movimiento
                        Moviment nuevoMov = movActual;
                        nuevoMov.afegeixPecaMenjada(Posicio(coordenadasAPosicion(colEnemigo, filaEnemigo + 1)));
                        nuevoMov.afegeixPas(Posicio(coordenadasAPosicion(colLand, filaLand + 1)));

                        // Marcar como visitado
                        visitados[filaEnemigo][colEnemigo] = true;

                        // Continuar buscando desde la nueva posición
                        calculaMovimentsDamaEnCadena(Posicio(coordenadasAPosicion(colLand, filaLand + 1)),
                            nuevoMov, movimentsCompletos, visitados);

                        // Desmarcar
                        visitados[filaEnemigo][colEnemigo] = false;
                    }
                    else {
                        // No se puede aterrizar más lejos
                        break;
                    }
                }
                // Salir del bucle de búsqueda en esta dirección
                break;
            }
            else {
                // Encontramos pieza propia o ya visitada, no se puede continuar
                break;
            }
        }
    }

    // CAMBIO CLAVE: Solo añadir el movimiento si no hay más capturas posibles
    // Esto evita duplicados y asegura que solo se añadan los movimientos finales
    if (!hayMasCaptures) {
        movimentsCompletos.push_back(movActual);
    }
}

void Tauler::eliminarMovimentsDuplicats() {
    for (int i = 0; i < N_FILES; i++) {
        for (int j = 0; j < N_COLUMNES; j++) {
            if (!m_tauler[i][j].esBuida()) {
                std::vector<Moviment> moviments;
                m_tauler[i][j].getMovimentsValids(moviments);

                // Remove duplicates based on destination and captured pieces
                std::vector<Moviment> movimentsUnics;
                for (const auto& mov : moviments) {
                    bool isDuplicate = false;
                    for (const auto& existing : movimentsUnics) {
                        if (mov.getDesti() == existing.getDesti() &&
                            mov.getNPecesMenjades() == existing.getNPecesMenjades()) {
                            // Check if captured pieces are the same
                            const auto& capturadas1 = mov.getPecesMenjades();
                            const auto& capturadas2 = existing.getPecesMenjades();
                            bool sameCaptures = true;
                            if (capturadas1.size() == capturadas2.size()) {
                                for (size_t k = 0; k < capturadas1.size(); k++) {
                                    bool found = false;
                                    for (size_t l = 0; l < capturadas2.size(); l++) {
                                        if (capturadas1[k] == capturadas2[l]) {
                                            found = true;
                                            break;
                                        }
                                    }
                                    if (!found) {
                                        sameCaptures = false;
                                        break;
                                    }
                                }
                            }
                            else {
                                sameCaptures = false;
                            }

                            if (sameCaptures) {
                                isDuplicate = true;
                                break;
                            }
                        }
                    }
                    if (!isDuplicate) {
                        movimentsUnics.push_back(mov);
                    }
                }

                // Update the piece with unique movements
                m_tauler[i][j].netejaMovimentsValids();
                for (const auto& mov : movimentsUnics) {
                    m_tauler[i][j].afegeixMovimentValid(mov);
                }
            }
        }
    }
}

void Tauler::comprovaMovimentMaxim(const Posicio& pos) {
    // COMENTADO: Esta función implementa la regla de captura obligatoria
    // Si se descomenta, solo permitirá capturas cuando estén disponibles

    /*
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
    */
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

std::vector<std::vector<bool>> Tauler::crearMatriuVisitados() {
    return std::vector<std::vector<bool>>(N_FILES, std::vector<bool>(N_COLUMNES, false));
}

void Tauler::calculaMovimentsEnCadena(const Posicio& pos, Moviment& movActual,
    std::vector<Moviment>& movimentsCompletos,
    std::vector<std::vector<bool>>& visitados) {
    int fila = pos.getFila() - 1;
    int col = pos.getColumna();

    // Obtener color de la ficha original
    ColorFitxa color = m_tauler[movActual.getOrigen().getFila() - 1][movActual.getOrigen().getColumna()].getColor();

    bool hayMasCaptures = false;

    // Buscar capturas adicionales en todas las direcciones diagonales
    const int direcciones[4][2] = { {-1, -1}, {-1, 1}, {1, -1}, {1, 1} };

    for (const auto& dir : direcciones) {
        int dfila = dir[0];
        int dcol = dir[1];

        int newFila = fila + 2 * dfila;
        int newCol = col + 2 * dcol;
        int filaComida = fila + dfila;
        int colComida = col + dcol;

        if (newFila >= 0 && newFila < N_FILES && newCol >= 0 && newCol < N_COLUMNES &&
            filaComida >= 0 && filaComida < N_FILES && colComida >= 0 && colComida < N_COLUMNES &&
            !visitados[filaComida][colComida] &&
            !m_tauler[filaComida][colComida].esBuida() &&
            m_tauler[filaComida][colComida].getColor() != color &&
            m_tauler[newFila][newCol].esBuida()) {

            hayMasCaptures = true;

            // Crear copia del movimiento actual
            Moviment nuevoMov = movActual;
            nuevoMov.afegeixPecaMenjada(Posicio(coordenadasAPosicion(colComida, filaComida + 1)));
            nuevoMov.afegeixPas(Posicio(coordenadasAPosicion(newCol, newFila + 1)));

            // Marcar como visitado
            visitados[filaComida][colComida] = true;

            // Continuar buscando desde la nueva posición
            calculaMovimentsEnCadena(Posicio(coordenadasAPosicion(newCol, newFila + 1)),
                nuevoMov, movimentsCompletos, visitados);

            // Desmarcar
            visitados[filaComida][colComida] = false;
        }
    }

    // CLAVE: Siempre añadir el movimiento actual, independientemente de si hay más capturas
    // Esto asegura que se guarden tanto las posiciones intermedias como la final
    movimentsCompletos.push_back(movActual);
}