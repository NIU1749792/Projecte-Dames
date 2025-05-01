#ifndef MOVIMENT_H
#define MOVIMENT_H

#include "posicio.h"
#include <vector>

class Moviment {
public:
    Moviment();
    Moviment(const Posicio& origen);

    Posicio getOrigen() const;
    size_t getNPassos() const;
    const std::vector<Posicio>& getPassos() const;

    size_t getNPecesMenjades() const;
    const std::vector<Posicio>& getPecesMenjades() const;

    void setOrigen(const Posicio& origen);
    void afegeixPas(const Posicio& pas);
    void afegeixPecaMenjada(const Posicio& peca);

    Posicio getDesti() const;
    bool esMenjada() const;

private:
    Posicio m_origen;
    std::vector<Posicio> m_passos;
    std::vector<Posicio> m_pecesMenjades;
};

#endif // MOVIMENT_H