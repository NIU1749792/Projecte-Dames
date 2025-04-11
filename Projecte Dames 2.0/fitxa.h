#ifndef FITXA_H
#define FITXA_H

#include "posicio.h"
#include "moviment.h"

#define MAX_MOVIMENTS 16  //< Número máximo de movimientos que puede tener una ficha

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

    /**
     * @brief Obtiene el tipo de la ficha
     * @return El tipo actual de la ficha
     */
    TipusFitxa getTipus() const;

    /**
     * @brief Obtiene el color de la ficha
     * @return El color actual de la ficha
     */
    ColorFitxa getColor() const;

    /**
     * @brief Obtiene la posición actual
     * @return Referencia constante a la posición actual
     */
    const Posicio& getPosicio() const;

    /**
     * @brief Obtiene el número de movimientos válidos
     * @return Cantidad de movimientos válidos actuales
     */
    int getNMovimentsValids() const;

    /**
     * @brief Obtiene los movimientos válidos de la ficha
     */
    int getMovimentsValids(Moviment moviments[], int maxMoviments) const;


    /*
     * @brief Establece el tipo de ficha
     * @param tipus Nuevo tipo de ficha
     */
    void setTipus(TipusFitxa tipus);

    /*
     * @brief Establece el color de la ficha
     * @param color Nuevo color de ficha
     */
    void setColor(ColorFitxa color);

    /*
     * @brief Establece la posición de la ficha
     * @param posicio Nueva posición en el tablero
     */
    void setPosicio(const Posicio& posicio);

    /*
     * @brief Añade un movimiento válido
     * @param moviment Movimiento a añadir
     * @return true si se añadió correctamente, false si no hay espacio
     */
    bool afegeixMovimentValid(const Moviment& moviment);

    /*
     * @brief Limpia todos los movimientos válidos
     * @post La lista de movimientos válidos queda vacía
     */
    void netejaMovimentsValids();

    /*
     * @brief Convierte la ficha en dama
     * @post El tipo de la ficha pasa a ser TIPUS_DAMA
     */
    void converteixEnDama();

    /*
     * @brief Comprueba si la ficha está vacía
     * @return true si es una casilla vacía (TIPUS_EMPTY)
     */
    bool esBuida() const;

private:
    TipusFitxa m_tipus;                   //< Tipo actual de la ficha
    ColorFitxa m_color;                   //< Color actual de la ficha
    Posicio m_posicio;                    //< Posición actual en el tablero
    Moviment m_movimentsValids[MAX_MOVIMENTS]; //< Array de movimientos válidos
    int m_nMovimentsValids;               //< Contador de movimientos válidos
};

#endif // FITXA_H