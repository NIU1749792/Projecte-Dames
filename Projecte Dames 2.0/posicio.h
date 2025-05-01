#ifndef POSICIO_H
#define POSICIO_H

#include <string>  

using namespace std;

class Posicio {
public:
    /*
     * @brief Constructor por defecto
     * @post Inicializa (0,0) no existeix al tauler
     */
    Posicio();

    /*
     * @param posicio Cadena en formato letra(a-h) + número(1-8) (ej: "a1", "h8")
     * @post La posición queda inicializada con los valores
     * Ejemplo
     * Posicio p("c5");  // Fila 5, Columna 2 ('c' → 2)
     */
    Posicio(const string& posicio);

    /*
     * @brief Obtiene el número de fila
     * @return Entero en el rango [1,8] (1 = fila inferior)
     */
    int getFila() const;

    /*
     * @brief Obtiene el número de columna
     * @return Entero en el rango [0,7] (0 = columna 'a')
     */
    int getColumna() const;

    /*
     * @brief Establece el número de fila
     * @param fila Valor entero entre 1 y 8
     */
    void setFila(int fila);

    /**
     * @brief Establece el número de columna
     * @param columna Valor entero entre 0 y 7
     */
    void setColumna(int columna);

    /*
     * @brief Operador de igualdad
     * @param otra Posición a comparar
     * @return true si ambas posiciones tienen las mismas coordenadas, ejemplos: comparar el movimiento esta en la casilla que se quiere, que dos fichas no esten en la misma posicion, etc
     */
    bool operator==(const Posicio& otra) const;

    /*
     * @return String en formato "a1"-"h8"
     * @post El string devuelto siempre tendrá 2 caracteres
     */
    string toString() const;

private:
    int m_fila;     //< Almacena el número de fila (1-8)
    int m_columna;  //< Almacena el número de columna (0-7)

    /*
     * @param posicio Cadena a validar
     * @return true si cumple el formato letra(a-h) + número(1-8)
     */
    bool esNotacioValida(const string& posicio) const;
};

#endif // POSICIO_H