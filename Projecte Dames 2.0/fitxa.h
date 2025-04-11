#ifndef FITXA_H
#define FITXA_H

#include "posicio.h"
#include "moviment.h"

#define MAX_MOVIMENTS 16  //< N�mero m�ximo de movimientos que puede tener una ficha

/**
 * @enum TipusFitxa
 * @brief Tipos posibles de fichas en el juego
 */
typedef enum {
    TIPUS_NORMAL,  //< Ficha normal (no coronada)
    TIPUS_DAMA,    //< Dama (ficha coronada)
    TIPUS_EMPTY    //< Casilla vac�a (sin ficha)
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
     * @post Crea una ficha vac�a (TIPUS_EMPTY)
     */
    Fitxa();

    /**
     * @brief Constructor completo
     * @param tipus Tipo de la ficha (normal/dama)
     * @param color Color de la ficha (blanco/negro)
     * @param posicio Posici�n inicial en el tablero
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
     * @brief Obtiene la posici�n actual
     * @return Referencia constante a la posici�n actual
     */
    const Posicio& getPosicio() const;

    /**
     * @brief Obtiene el n�mero de movimientos v�lidos
     * @return Cantidad de movimientos v�lidos actuales
     */
    int getNMovimentsValids() const;

    /**
     * @brief Obtiene los movimientos v�lidos de la ficha
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
     * @brief Establece la posici�n de la ficha
     * @param posicio Nueva posici�n en el tablero
     */
    void setPosicio(const Posicio& posicio);

    /*
     * @brief A�ade un movimiento v�lido
     * @param moviment Movimiento a a�adir
     * @return true si se a�adi� correctamente, false si no hay espacio
     */
    bool afegeixMovimentValid(const Moviment& moviment);

    /*
     * @brief Limpia todos los movimientos v�lidos
     * @post La lista de movimientos v�lidos queda vac�a
     */
    void netejaMovimentsValids();

    /*
     * @brief Convierte la ficha en dama
     * @post El tipo de la ficha pasa a ser TIPUS_DAMA
     */
    void converteixEnDama();

    /*
     * @brief Comprueba si la ficha est� vac�a
     * @return true si es una casilla vac�a (TIPUS_EMPTY)
     */
    bool esBuida() const;

private:
    TipusFitxa m_tipus;                   //< Tipo actual de la ficha
    ColorFitxa m_color;                   //< Color actual de la ficha
    Posicio m_posicio;                    //< Posici�n actual en el tablero
    Moviment m_movimentsValids[MAX_MOVIMENTS]; //< Array de movimientos v�lidos
    int m_nMovimentsValids;               //< Contador de movimientos v�lidos
};

#endif // FITXA_H