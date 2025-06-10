#ifndef TAULER_H
#define TAULER_H

#include "fitxa.h"
#include <string>
#include <vector>

#define N_FILES 8
#define N_COLUMNES 8

class Tauler {
public:
    Tauler();
    void inicialitza(const std::string& nomFitxer);
    void actualitzaMovimentsValids();
    void getPosicionsPossibles(const Posicio& origen, int& nPosicions, Posicio posicionsPossibles[]);
    bool mouFitxa(const Posicio& origen, const Posicio& desti);
    std::string toString() const;

private:
    Fitxa m_tauler[N_FILES][N_COLUMNES];

    bool esCassellaValid(const Posicio& desti) const;
    bool esMovimentValid(const Posicio& origen, const Posicio& desti) const;
    bool esPosicioValida(int fila, int columna) const;

    void calculaMovimentsNormals(const Posicio& pos);
    void calculaMovimentsDama(const Posicio& pos);
    void comprovaMovimentMaxim(const Posicio& pos);
    void bufaFitxa(const Posicio& pos);

    std::string coordenadasAPosicion(int columna, int fila) const;

    // Función para cadenas de movimientos de fichas normales
    void calculaMovimentsEnCadena(const Posicio& pos, Moviment& movActual,
        std::vector<Moviment>& movimentsCompletos,
        std::vector<std::vector<bool>>& visitados);

    // Función para cadenas de movimientos de damas
    void calculaMovimentsDamaEnCadena(const Posicio& pos, Moviment& movActual,
        std::vector<Moviment>& movimentsCompletos,
        std::vector<std::vector<bool>>& visitados);

    // Función para crear matriz de visitados
    std::vector<std::vector<bool>> crearMatriuVisitados();
    void eliminarMovimentsDuplicats();
};

#endif // TAULER_H