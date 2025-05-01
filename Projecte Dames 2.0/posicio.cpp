#include "posicio.h"
#include <stdexcept>
#include <cctype>

Posicio::Posicio() : m_fila(0), m_columna(0) {}

Posicio::Posicio(const string& posicio) {
    if (!esNotacioValida(posicio)) {
        throw invalid_argument("Notació de posició no vàlida: " + posicio);
    }

    m_columna = tolower(posicio[0]) - 'a';
    m_fila = posicio[1] - '0';
}

int Posicio::getFila() const {
    return m_fila;
}

int Posicio::getColumna() const {
    return m_columna;
}

void Posicio::setFila(int fila) {
    if (fila < 1 || fila > 8) {
        throw out_of_range("Fila ha d'estar entre 1 i 8");
    }
    m_fila = fila;
}

void Posicio::setColumna(int columna) {
    if (columna < 0 || columna > 7) {
        throw out_of_range("Columna ha d'estar entre 0 i 7");
    }
    m_columna = columna;
}

bool Posicio::operator==(const Posicio& otra) const {
    return (m_fila == otra.m_fila) && (m_columna == otra.m_columna);
}

string Posicio::toString() const {
    if (m_fila == 0 || m_columna < 0) return "  "; // Posición inválida

    string result;
    result += static_cast<char>('a' + m_columna);
    result += to_string(m_fila);
    return result;
}

bool Posicio::esNotacioValida(const string& posicio) const {
    if (posicio.length() != 2) return false;

    char col = tolower(posicio[0]);
    char fila = posicio[1];

    return (col >= 'a' && col <= 'h') && (fila >= '1' && fila <= '8');
}