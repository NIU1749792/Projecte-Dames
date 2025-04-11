#ifndef MOVIMENT_H
#define MOVIMENT_H

#include "posicio.h"

#define MAX_PASSOS 7          // Máximo de pasos: 7 (diagonal si una pieza coge la diagonal mas grande)
#define MAX_PECES_MENJADES 4  // Regla teorica del juego

class Moviment {
public:
    /*
     * @brief Constructor por defecto
     * @post Inicializa un moviment en origen (0,0)
     */
    Moviment();

    /*
     * @brief Constructor con posición inicial
     * @param origen Posición de partida del movimiento
     * @post Inicializa el movimiento con posición origen y 0 pasos/capturas
     */
    Moviment(const Posicio& origen);  // Eliminado explicit

    /*
     * @brief Obtiene posición de inicio
     * @return Posición origen del movimiento
     */
    Posicio getOrigen() const;

    /*
     * @brief Obtiene número de pasos realizados
     * @return Entero entre 0 y MAX_PASSOS
     */
    int getNPassos() const;

    /**
     * @brief Accede a la secuencia de pasos
     * @return Array de Posiciones con la trayectoria
     * @note El array tiene longitud MAX_PASSOS
     */
    const Posicio* getPassos() const;

    /**
     * @brief Obtiene número de piezas capturadas
     * @return Entero entre 0 y MAX_PECES_MENJADES
     */
    int getNPecesMenjades() const;

    /**
     * @brief Accede a las posiciones de piezas capturadas
     * @return Array de Posiciones donde ocurrieron capturas
     * @note El array tiene longitud MAX_PECES_MENJADES
     */
    const Posicio* getPecesMenjades() const;

    /**
     * @brief Establece nueva posición origen
     * @param origen Nueva posición inicial
     * @post Reinicia los contadores de pasos y capturas
     */
    void setOrigen(const Posicio& origen);

    /**
     * @brief Añade un paso
     * @return true si se añadió, false si se superó MAX_PASSOS
     * @post Incrementa m_nPassos si hay capacidad
     */
    bool afegeixPas(const Posicio& pas);

    /**
     * @brief Registra una pieza capturada
     * @param peca Posición de la pieza eliminada
     * @return true si se añadió, false si se superó MAX_PECES_MENJADES
     * @post Incrementa m_nPecesMenjades
     */
    bool afegeixPeçaMenjada(const Posicio& peca);

    /*
     * @brief Calcula la posición final
     * @return Última posición del array de pasos
     */
    Posicio getDesti() const;

    /*
     * @brief Verifica si es un movimiento con capturas
     * @return true si m_nPecesMenjades > 0
     */
    bool esMenjada() const;

private:
    Posicio m_origen;          //< Punto de partida (siempre inicializado)
    Posicio m_passos[MAX_PASSOS];      //< Trayectoria completa
    int m_nPassos;             //< Pasos actuales (0 ≤ n ≤ MAX_PASSOS)
    Posicio m_pecesMenjades[MAX_PECES_MENJADES]; //< Posiciones capturadas
    int m_nPecesMenjades;      //< Capturas actuales (0 ≤ n ≤ MAX_PECES_MENJADES)
};

#endif // MOVIMENT_H