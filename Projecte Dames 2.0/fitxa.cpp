#include "fitxa.h"

Fitxa::Fitxa() :
    m_tipus(TIPUS_EMPTY),
    m_color(COLOR_BLANC),
    m_posicio(Posicio()) {
}

Fitxa::Fitxa(TipusFitxa tipus, ColorFitxa color, const Posicio& posicio) :
    m_tipus(tipus),
    m_color(color),
    m_posicio(posicio) {
}

TipusFitxa Fitxa::getTipus() const {
    return m_tipus;
}

ColorFitxa Fitxa::getColor() const {
    return m_color;
}

const Posicio& Fitxa::getPosicio() const {
    return m_posicio;
}

size_t Fitxa::getNMovimentsValids() const {
    return m_movimentsValids.size();
}

void Fitxa::getMovimentsValids(std::vector<Moviment>& moviments) const {
    moviments = m_movimentsValids;
}

void Fitxa::setTipus(TipusFitxa tipus) {
    m_tipus = tipus;
}

void Fitxa::setColor(ColorFitxa color) {
    m_color = color;
}

void Fitxa::setPosicio(const Posicio& posicio) {
    m_posicio = posicio;
}

void Fitxa::afegeixMovimentValid(const Moviment& moviment) {
    m_movimentsValids.push_back(moviment);
}

void Fitxa::netejaMovimentsValids() {
    m_movimentsValids.clear();
}

void Fitxa::converteixEnDama() {
    m_tipus = TIPUS_DAMA;
}

bool Fitxa::esBuida() const {
    return m_tipus == TIPUS_EMPTY;
}