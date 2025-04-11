#ifndef TAULER_H
#define TAULER_H

#include "fitxa.h"

#define N_FILES 8      
#define N_COLUMNES 8  

class Tauler {
public:
    /**
     * @brief Constructor por defecto
     * @post Inicializa un tablero vacío (todas las casillas a TIPUS_EMPTY)
     */
    Tauler();

    /*
     * @brief Carga desde archivo
     */
    void inicialitza(const char nomFitxer[]);

    /**
     * @brief Actualiza todos los movimientos válidos para todas las fichas
     * @post Cada ficha del tablero tendrá su lista de movimientos válidos actualizada
     */
    void actualitzaMovimentsValids();

    /**
     * @brief Obtiene las posiciones posibles para una ficha
     * @param origen Posición de la ficha a consultar
     * @param[out] nPosicions Número de posiciones válidas encontradas
     * @param[out] posicionsPossibles Array donde almacenar las posiciones
     */
    void getPosicionsPossibles(const Posicio& origen, int& nPosicions, Posicio posicionsPossibles[], int maxPosicions);

    /**
     * @brief Mueve una ficha a una nueva posición
     * @param origen Posición actual de la ficha
     * @param desti Posición final
     * @return true si el movimiento fue válido y se realizó con éxito
     * @return false si el movimiento no es válido
     * @post Si es válido: actualiza el estado del tablero, incluyendo:
     */
    bool mouFitxa(const Posicio& origen, const Posicio& desti);

    /**
     * @brief Genera una representación textual del tablero
     */
    void toString(char buffer[], int bufferSize) const;

private:
    Fitxa m_tauler[N_FILES][N_COLUMNES];

    /*
     * @brief Valida si el movimiento cumple la regla de suma par
     * @return true si (desti_x + desti_y) % 2 == 0
     *
     * Regla de validación:
     * 1. Convierte letras a números (a=1, h=8)
     * 2. Suma valores de destino
     * 3. El movimiento es válido si la suma total es par
     * Ejemplo:
     * - Destino "b2" (2+2) = 4 → 4%2=0 → válido
     */
    bool esCassellaValid(const Posicio& desti) const;

    bool esMovimentValid(const Posicio& origen, const Posicio& desti) const;

    /*
     * @brief Verifica si una posición está dentro del tablero
     */
    bool esPosicioValida(int fila, int columna) const;

    /**
     * @brief Calcula movimientos válidos para fichas normales
     * @param pos Posición de la ficha a evaluar
     * @post Actualiza los movimientos válidos para la ficha en pos
     */
    void calculaMovimentsNormals(const Posicio& pos);

    /**
     * @brief Calcula movimientos válidos para damas
     * @param pos Posición de la dama a evaluar
     * @post Actualiza los movimientos válidos para la dama en pos
     */
    void calculaMovimentsDama(const Posicio& pos);

    /**
     * @brief Verifica si se está capturando el máximo de fichas posibles
     * @param pos Posición de la ficha que se está moviendo
     * @post Si no se captura el máximo, aplica la regla "bufar"
     */
    void comprovaMovimentMaxim(const Posicio& pos);

    /**
     * @brief Aplica la penalización por no capturar el máximo de fichas
     * @param pos Posición de la ficha que incumplió la regla
     * @post Elimina la ficha del jugador infractor
     */
    void bufaFitxa(const Posicio& pos);
};

#endif // TAULER_H