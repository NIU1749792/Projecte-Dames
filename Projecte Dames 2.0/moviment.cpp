#include "moviment.h"

Moviment::Moviment() : m_origen(Posicio()) {}

Moviment::Moviment(const Posicio& origen) : m_origen(origen) {}

Posicio Moviment::getOrigen() const {
    return m_origen;
}

size_t Moviment::getNPassos() const {
    return m_passos.size();
}

const std::vector<Posicio>& Moviment::getPassos() const {
    return m_passos;
}

size_t Moviment::getNPecesMenjades() const {
    return m_pecesMenjades.size();
}

const std::vector<Posicio>& Moviment::getPecesMenjades() const {
    return m_pecesMenjades;
}

void Moviment::setOrigen(const Posicio& origen) {
    m_origen = origen;
    m_passos.clear();
    m_pecesMenjades.clear();
}

void Moviment::afegeixPas(const Posicio& pas) {
    m_passos.push_back(pas);
}

void Moviment::afegeixPeçaMenjada(const Posicio& peca) {
    m_pecesMenjades.push_back(peca);
}

Posicio Moviment::getDesti() const {
    return m_passos.empty() ? m_origen : m_passos.back();
}

bool Moviment::esMenjada() const {
    return !m_pecesMenjades.empty();
}