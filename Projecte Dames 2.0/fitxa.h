#ifndef FITXA_H
#define FITXA_H

#include "posicio.h"
#include "moviment.h"
#include <vector>

/**
 * @enum TipusFitxa
 * @brief Tipos posibles de fichas en el juego
 */
typedef enum {
    TIPUS_NORMAL,  //< Ficha normal (no coronada)
    TIPUS_DAMA,    //< Dama (ficha coronada)
    TIPUS_EMPTY    //< Casilla vacía (sin ficha)
} TipusFitxa;

/**
 * @enum ColorFitxa
 * @brief Colores posibles de las fichas
 */
typedef enum {
    COLOR_NEGRE,   //< Ficha de color negro
    COLOR_BLANC,   //< Ficha de color blanco
} ColorFitxa;

class Fitxa {
public:
    /**
     * @brief Constructor por defecto
     * @post Crea una ficha vacía (TIPUS_EMPTY)
     */
    Fitxa();

    /**
     * @brief Constructor completo
     * @param tipus Tipo de la ficha (normal/dama)
     * @param color Color de la ficha (blanco/negro)
     * @param posicio Posición inicial en el tablero
     */
    Fitxa(TipusFitxa tipus, ColorFitxa color, const Posicio& posicio);

    TipusFitxa getTipus() const;
    ColorFitxa getColor() const;
    const Posicio& getPosicio() const;

    size_t getNMovimentsValids() const;
    void getMovimentsValids(std::vector<Moviment>& moviments) const;

    void setTipus(TipusFitxa tipus);
    void setColor(ColorFitxa color);
    void setPosicio(const Posicio& posicio);

    void afegeixMovimentValid(const Moviment& moviment);
    void netejaMovimentsValids();
    void converteixEnDama();

    bool esBuida() const;

private:
    TipusFitxa m_tipus;
    ColorFitxa m_color;
    Posicio m_posicio;
    std::vector<Moviment> m_movimentsValids;
};

#endif // FITXA_H