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

// Agregar estas funciones al archivo tauler.cpp

void Tauler::calculaMovimentsNormals(const Posicio& pos) {
    int fila = pos.getFila() - 1;
    int col = pos.getColumna();

    if (fila < 0 || fila >= N_FILES || col < 0 || col >= N_COLUMNES ||
        m_tauler[fila][col].esBuida()) {
        return;
    }

    ColorFitxa color = m_tauler[fila][col].getColor();

    // Direcciones de movimiento para fichas normales
    int direcciones[4][2] = { {1, -1}, {1, 1}, {-1, -1}, {-1, 1} };

    // Movimientos simples (sin captura) - solo hacia adelante
    int direccionesSimples[2][2];
    int numDirSimples = 0;

    if (color == COLOR_NEGRE) {
        // Las X (negras) se mueven hacia abajo (fila disminuye)
        direccionesSimples[numDirSimples][0] = -1; direccionesSimples[numDirSimples][1] = -1; numDirSimples++;
        direccionesSimples[numDirSimples][0] = -1; direccionesSimples[numDirSimples][1] = 1; numDirSimples++;
    }
    else {
        // Las O (blancas) se mueven hacia arriba (fila aumenta)
        direccionesSimples[numDirSimples][0] = 1; direccionesSimples[numDirSimples][1] = -1; numDirSimples++;
        direccionesSimples[numDirSimples][0] = 1; direccionesSimples[numDirSimples][1] = 1; numDirSimples++;
    }

    // Movimientos simples
    for (int i = 0; i < numDirSimples; i++) {
        int nuevaFila = fila + direccionesSimples[i][0];
        int nuevaCol = col + direccionesSimples[i][1];

        if (nuevaFila >= 0 && nuevaFila < N_FILES &&
            nuevaCol >= 0 && nuevaCol < N_COLUMNES &&
            m_tauler[nuevaFila][nuevaCol].esBuida()) {

            Posicio destino(coordenadasAPosicion(nuevaCol, nuevaFila + 1));
            if (esCassellaValid(destino)) {
                Moviment mov(pos);
                mov.afegeixPas(destino);
                m_tauler[fila][col].afegeixMovimentValid(mov);
            }
        }
    }

    // Buscar capturas en todas las direcciones
    std::vector<std::vector<bool>> visitado(N_FILES, std::vector<bool>(N_COLUMNES, false));
    buscarCapturas(pos, pos, Moviment(pos), visitado, color);
}

void Tauler::buscarCapturas(const Posicio& posOrigen, const Posicio& posActual, Moviment movActual, std::vector<std::vector<bool>>& visitado, ColorFitxa colorOriginal) {
    int fila = posActual.getFila() - 1;
    int col = posActual.getColumna();

    int direcciones[4][2] = { {1, -1}, {1, 1}, {-1, -1}, {-1, 1} };
    bool encontroCaptura = false;

    for (int i = 0; i < 4; i++) {
        int dirFila = direcciones[i][0];
        int dirCol = direcciones[i][1];

        // Buscar enemigo
        int filaEnemigo = fila + dirFila;
        int colEnemigo = col + dirCol;

        if (filaEnemigo < 0 || filaEnemigo >= N_FILES ||
            colEnemigo < 0 || colEnemigo >= N_COLUMNES) {
            continue;
        }

        if (m_tauler[filaEnemigo][colEnemigo].esBuida() ||
            m_tauler[filaEnemigo][colEnemigo].getColor() == colorOriginal ||
            visitado[filaEnemigo][colEnemigo]) {
            continue;
        }

        // Buscar casilla de aterrizaje
        int filaAterrizaje = filaEnemigo + dirFila;
        int colAterrizaje = colEnemigo + dirCol;

        if (filaAterrizaje < 0 || filaAterrizaje >= N_FILES ||
            colAterrizaje < 0 || colAterrizaje >= N_COLUMNES) {
            continue;
        }

        if (!m_tauler[filaAterrizaje][colAterrizaje].esBuida()) {
            continue;
        }

        Posicio posEnemigo(coordenadasAPosicion(colEnemigo, filaEnemigo + 1));
        Posicio posAterrizaje(coordenadasAPosicion(colAterrizaje, filaAterrizaje + 1));

        if (!esCassellaValid(posAterrizaje)) {
            continue;
        }

        // Crear nuevo movimiento
        Moviment nuevoMov = movActual;
        nuevoMov.afegeixPas(posAterrizaje);
        nuevoMov.afegeixPecaMenjada(posEnemigo);

        // Marcar enemigo como visitado
        visitado[filaEnemigo][colEnemigo] = true;
        encontroCaptura = true;

        // Primero, agregar este movimiento como válido
        int filaOrigen = posOrigen.getFila() - 1;
        int colOrigen = posOrigen.getColumna();
        m_tauler[filaOrigen][colOrigen].afegeixMovimentValid(nuevoMov);

        // Luego buscar capturas adicionales recursivamente
        std::vector<std::vector<bool>> visitadoCopia = visitado;
        buscarCapturas(posOrigen, posAterrizaje, nuevoMov, visitadoCopia, colorOriginal);

        // Desmarcar para otras rutas
        visitado[filaEnemigo][colEnemigo] = false;
    }
}

void Tauler::calculaMovimentsDama(const Posicio& pos) {
    int fila = pos.getFila() - 1;
    int col = pos.getColumna();

    if (fila < 0 || fila >= N_FILES || col < 0 || col >= N_COLUMNES ||
        m_tauler[fila][col].esBuida()) {
        return;
    }

    ColorFitxa color = m_tauler[fila][col].getColor();
    int direcciones[4][2] = { {1, -1}, {1, 1}, {-1, -1}, {-1, 1} };

    // Movimientos simples (sin captura)
    for (int i = 0; i < 4; i++) {
        int dirFila = direcciones[i][0];
        int dirCol = direcciones[i][1];

        // La dama puede moverse múltiples casillas en una dirección
        for (int paso = 1; paso < 8; paso++) {
            int nuevaFila = fila + dirFila * paso;
            int nuevaCol = col + dirCol * paso;

            if (nuevaFila < 0 || nuevaFila >= N_FILES ||
                nuevaCol < 0 || nuevaCol >= N_COLUMNES) {
                break;
            }

            if (!m_tauler[nuevaFila][nuevaCol].esBuida()) {
                break; // Hay una pieza bloqueando el camino
            }

            Posicio destino(coordenadasAPosicion(nuevaCol, nuevaFila + 1));
            if (esCassellaValid(destino)) {
                Moviment mov(pos);
                mov.afegeixPas(destino);
                m_tauler[fila][col].afegeixMovimentValid(mov);
            }
        }
    }

    // Movimientos con captura
    std::vector<std::vector<bool>> visitado(N_FILES, std::vector<bool>(N_COLUMNES, false));

    for (int i = 0; i < 4; i++) {
        Moviment movInicial(pos);
        calculaCapturasRecursivoDama(pos, movInicial, direcciones[i][0],
            direcciones[i][1], visitado, true);
    }
}

void Tauler::calculaCapturasRecursivo(const Posicio& posActual, Moviment& movActual,
    int dirFila, int dirCol,
    std::vector<std::vector<bool>>& visitado,
    bool esPrimerMovimiento) {
    int fila = posActual.getFila() - 1;
    int col = posActual.getColumna();

    // Obtener el color de la ficha original (no la actual si estamos en recursión)
    ColorFitxa miColor;
    if (esPrimerMovimiento) {
        miColor = m_tauler[fila][col].getColor();
    }
    else {
        // En recursión, obtener el color de la ficha original
        int filaOrigen = movActual.getOrigen().getFila() - 1;
        int colOrigen = movActual.getOrigen().getColumna();
        miColor = m_tauler[filaOrigen][colOrigen].getColor();
    }

    // Buscar enemigo en esta dirección
    int filaEnemigo = fila + dirFila;
    int colEnemigo = col + dirCol;

    if (filaEnemigo < 0 || filaEnemigo >= N_FILES ||
        colEnemigo < 0 || colEnemigo >= N_COLUMNES) {
        return;
    }

    if (m_tauler[filaEnemigo][colEnemigo].esBuida() ||
        m_tauler[filaEnemigo][colEnemigo].getColor() == miColor ||
        visitado[filaEnemigo][colEnemigo]) {
        return;
    }

    // Buscar casilla de aterrizaje después del enemigo
    int filaAterrizaje = filaEnemigo + dirFila;
    int colAterrizaje = colEnemigo + dirCol;

    if (filaAterrizaje < 0 || filaAterrizaje >= N_FILES ||
        colAterrizaje < 0 || colAterrizaje >= N_COLUMNES) {
        return;
    }

    if (!m_tauler[filaAterrizaje][colAterrizaje].esBuida()) {
        return;
    }

    Posicio posEnemigo(coordenadasAPosicion(colEnemigo, filaEnemigo + 1));
    Posicio posAterrizaje(coordenadasAPosicion(colAterrizaje, filaAterrizaje + 1));

    if (!esCassellaValid(posAterrizaje)) {
        return;
    }

    // Crear nuevo movimiento con esta captura
    Moviment nuevoMov = movActual;
    nuevoMov.afegeixPas(posAterrizaje);
    nuevoMov.afegeixPecaMenjada(posEnemigo);

    // Marcar enemigo como visitado para esta rama
    visitado[filaEnemigo][colEnemigo] = true;

    // Buscar capturas adicionales desde la posición de aterrizaje
    bool tieneCapturaAdicional = false;
    int todasDirecciones[4][2] = { {1, -1}, {1, 1}, {-1, -1}, {-1, 1} };

    for (int i = 0; i < 4; i++) {
        // Crear copia del estado para cada dirección
        std::vector<std::vector<bool>> visitadoCopia = visitado;
        Moviment movCopia = nuevoMov;

        int filaOrigen = movActual.getOrigen().getFila() - 1;
        int colOrigen = movActual.getOrigen().getColumna();
        size_t capturas_antes = nuevoMov.getNPecesMenjades();

        calculaCapturasRecursivo(posAterrizaje, movCopia, todasDirecciones[i][0],
            todasDirecciones[i][1], visitadoCopia, false);

        // Si se encontraron más capturas, agregar el movimiento completo
        if (movCopia.getNPecesMenjades() > capturas_antes) {
            tieneCapturaAdicional = true;
            m_tauler[filaOrigen][colOrigen].afegeixMovimentValid(movCopia);
        }
    }

    // Si no hay capturas adicionales, agregar el movimiento actual
    if (!tieneCapturaAdicional) {
        int filaOrigen = movActual.getOrigen().getFila() - 1;
        int colOrigen = movActual.getOrigen().getColumna();
        m_tauler[filaOrigen][colOrigen].afegeixMovimentValid(nuevoMov);
    }
}

void Tauler::calculaCapturasRecursivoDama(const Posicio& posActual, Moviment& movActual,
    int dirFila, int dirCol,
    std::vector<std::vector<bool>>& visitado,
    bool esPrimerMovimiento) {
    int fila = posActual.getFila() - 1;
    int col = posActual.getColumna();
    ColorFitxa miColor = m_tauler[fila][col].getColor();

    // La dama puede capturar a distancia
    for (int paso = 1; paso < 8; paso++) {
        int filaEnemigo = fila + dirFila * paso;
        int colEnemigo = col + dirCol * paso;

        if (filaEnemigo < 0 || filaEnemigo >= N_FILES ||
            colEnemigo < 0 || colEnemigo >= N_COLUMNES) {
            break;
        }

        if (m_tauler[filaEnemigo][colEnemigo].esBuida()) {
            continue; // Seguir buscando
        }

        if (m_tauler[filaEnemigo][colEnemigo].getColor() == miColor ||
            visitado[filaEnemigo][colEnemigo]) {
            break; // Pieza propia o ya visitada
        }

        // Encontramos un enemigo, buscar casillas de aterrizaje
        for (int pasoAterrizaje = 1; pasoAterrizaje < 8; pasoAterrizaje++) {
            int filaAterrizaje = filaEnemigo + dirFila * pasoAterrizaje;
            int colAterrizaje = colEnemigo + dirCol * pasoAterrizaje;

            if (filaAterrizaje < 0 || filaAterrizaje >= N_FILES ||
                colAterrizaje < 0 || colAterrizaje >= N_COLUMNES) {
                break;
            }

            if (!m_tauler[filaAterrizaje][colAterrizaje].esBuida()) {
                break;
            }

            Posicio posEnemigo(coordenadasAPosicion(colEnemigo, filaEnemigo + 1));
            Posicio posAterrizaje(coordenadasAPosicion(colAterrizaje, filaAterrizaje + 1));

            if (!esCassellaValid(posAterrizaje)) {
                continue;
            }

            // Crear nuevo movimiento con esta captura
            Moviment nuevoMov = movActual;
            nuevoMov.afegeixPas(posAterrizaje);
            nuevoMov.afegeixPecaMenjada(posEnemigo);

            // Marcar enemigo como visitado
            visitado[filaEnemigo][colEnemigo] = true;

            // Buscar capturas adicionales desde la posición de aterrizaje
            bool tieneCapturaAdicional = false;
            int todasDirecciones[4][2] = { {1, -1}, {1, 1}, {-1, -1}, {-1, 1} };

            for (int i = 0; i < 4; i++) {
                std::vector<std::vector<bool>> visitadoCopia = visitado;
                Moviment movCopia = nuevoMov;

                size_t movimientosAntes = movCopia.getNPecesMenjades();
                calculaCapturasRecursivoDama(posAterrizaje, movCopia, todasDirecciones[i][0],
                    todasDirecciones[i][1], visitadoCopia, false);

                if (movCopia.getNPecesMenjades() > movimientosAntes) {
                    tieneCapturaAdicional = true;
                    // Agregar este movimiento extendido
                    int filaOrigen = movActual.getOrigen().getFila() - 1;
                    int colOrigen = movActual.getOrigen().getColumna();
                    m_tauler[filaOrigen][colOrigen].afegeixMovimentValid(movCopia);
                }
            }

            // Si no hay capturas adicionales, agregar el movimiento actual
            if (!tieneCapturaAdicional) {
                int filaOrigen = movActual.getOrigen().getFila() - 1;
                int colOrigen = movActual.getOrigen().getColumna();
                m_tauler[filaOrigen][colOrigen].afegeixMovimentValid(nuevoMov);
            }

            // Desmarcar enemigo para otras rutas
            visitado[filaEnemigo][colEnemigo] = false;
        }

        break; // Solo procesamos el primer enemigo encontrado en esta dirección
    }
}