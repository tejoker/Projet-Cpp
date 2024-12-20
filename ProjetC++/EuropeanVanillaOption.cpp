#include "EuropeanVanillaOption.h"

// Constructeur qui initialise _expiry et _strike
EuropeanVanillaOption::EuropeanVanillaOption(double expiry, double strike)
    : Option(expiry), _strike(strike) {
    if (expiry < 0 || strike < 0) {
        throw std::invalid_argument("Expiry and strike must be non-negative");
    }
}

// Méthode getter pour _strike
double EuropeanVanillaOption::getStrike() const {
    return _strike;
}

//Override de isAsianOption pour retourner false pour EVO - (Probleme de compilation si on ne le fait pas)
bool EuropeanVanillaOption::isAsianOption() const {
    return false;
}
