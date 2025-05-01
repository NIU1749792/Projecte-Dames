#ifndef POSICIO_H
#define POSICIO_H

#include <string>
#include <iostream>

class Posicio {
public:
    Posicio();
    Posicio(const std::string& posicio);

    int getFila() const;
    int getColumna() const;

    void setFila(int fila);
    void setColumna(int columna);

    bool operator==(const Posicio& otra) const;
    std::string toString() const;

    friend std::ostream& operator<<(std::ostream& os, const Posicio& pos) {
        os << pos.toString();
        return os;
    }

private:
    int m_fila;
    int m_columna;
    bool esNotacioValida(const std::string& posicio) const;
};

#endif // POSICIO_H