#include "BlackScholesPricer.h"
#include <iostream>
#include <cmath>
#define _USE_MATH_DEFINES
#include <math.h>


#ifndef M_SQRT1_2 // check if M_SRQT1_2 is not defined
#define M_SQRT1_2 0.70710678118654752440 // define M_SQRT1_2 as square root of 1/2
#endif

BlackScholesPricer::BlackScholesPricer(EuropeanVanillaOption* option, double asset_price, double interest_rate, double volatility)
    : vanilla_option_(option), digital_option_(nullptr), asset_price_(asset_price), interest_rate_(interest_rate), volatility_(volatility) {} // Constructeur de la classe BSP avec initialisation des attributs pour une option Vanille Européenne

BlackScholesPricer::BlackScholesPricer(EuropeanDigitalOption* option, double asset_price, double interest_rate, double volatility)
    : vanilla_option_(nullptr), digital_option_(option), asset_price_(asset_price), interest_rate_(interest_rate), volatility_(volatility) {} // Constructeur de la classe BSP avec initialisation des attributs pour une option Digitale Européenne

double BlackScholesPricer::operator()() const // Opérateur () pour retourner le prix de l'option 
{
    double T, K, S = asset_price_, r = interest_rate_, sigma = volatility_;
    double d1, d2;

    if (vanilla_option_) {
        T = vanilla_option_->getExpiry(); // Maturité de l'option
        K = vanilla_option_->getStrike(); // Prix d'exercice de l'option

        d1 = (std::log(S / K) + (r + 0.5 * sigma * sigma) * T) / (sigma * std::sqrt(T));
        d2 = d1 - sigma * std::sqrt(T);

        if (vanilla_option_->GetOptionType() == EuropeanVanillaOption::optionType::call) {
            return S * 0.5 * std::erfc(-d1 * 1 / std::sqrt(2)) - K * std::exp(-r * T) * 0.5 * std::erfc(-d2 * 1 / std::sqrt(2));
        }
        else {
            return K * std::exp(-r * T) * 0.5 * std::erfc(d2 * 1 / std::sqrt(2)) - S * 0.5 * std::erfc(d1 * 1 / std::sqrt(2));
        }
    }
    else if (digital_option_) {
        T = digital_option_->getExpiry(); // Maturité de l'option
        K = digital_option_->getStrike(); // Prix d'exercice de l'option

		double alpha = S * std::exp(r * T) / K; // alpha dans le papier d'Evry

        if (digital_option_->GetOptionType() == EuropeanDigitalOption::optionType::call) {
			
            d2 = (std::log(alpha) / (sigma * std::sqrt(T))) - 0.5 * (sigma * std::sqrt(T)); // d0(alpha) dans le papier d'Evry

            return std::exp(-r * T) * (1 - 0.5 * std::erfc(d2 / std::sqrt(2)));
        }
        else {
            
            d1 = - std::log(alpha) / (sigma * std::sqrt(T)) + 0.5 * (sigma * std::sqrt(T));  //d1(1/alpha) dans le papier d'Evry
            
            return std::exp(-r * T) * (1 - 0.5 * std::erfc(d1 * 1 / std::sqrt(2)));
        }
    }

    return 0.0; // Retourne 0 si aucune option n'est définie
}

// Méthode pour retourner le Delta de l'option
double BlackScholesPricer::delta() const {
    double T, K, S = asset_price_, sigma = volatility_, r = interest_rate_;
    double d1;

    if (vanilla_option_) {
        T = vanilla_option_->getExpiry(); // Maturité de l'option
        K = vanilla_option_->getStrike(); // Prix d'exercice de l'option

        d1 = (std::log(S / K) + (r + 0.5 * sigma * sigma) * T) / (sigma * std::sqrt(T));

        if (vanilla_option_->GetOptionType() == EuropeanVanillaOption::optionType::call) {
            return 0.5 * std::erfc(-d1 * std::sqrt(2)); // CDF(d1) for a call
        }
        else {
            return 0.5 * std::erfc(d1 * std::sqrt(2)) - 1; // CDF(d1) - 1 for a put
        }
    }
    else if (digital_option_) {
        T = digital_option_->getExpiry(); // Maturité de l'option
        K = digital_option_->getStrike(); // Prix d'exercice de l'option


		d1 = (std::log(S / K) + (r - 0.5 * sigma * sigma) * T) / (sigma * std::sqrt(T)); // d1 dans le papier d'Evry

        if (digital_option_->GetOptionType() == EuropeanDigitalOption::optionType::call) {
            return std::exp(-r * T) * (std::exp(-d1*d1/2) / std::sqrt(2 * M_PI)) / (S * sigma * std::sqrt(T));
        }
        else {

			std::cout << "S: " << S << " T: " << T << " K: " << K << " sigma: " << sigma << " r: " << r << " d1: " << d1 << std::endl;
			std::cout << "exp(-r * T): " << std::exp(-r * T) << " exp(-d1*d1/2): " << std::exp(-d1 * d1 / 2) << " sqrt(2 * M_PI): " << std::sqrt(2 * M_PI) << " S: " << S << " sigma: " << sigma << " sqrt(T): " << std::sqrt(T) << std::endl;
			return - std::exp(-r * T) * (std::exp(-d1*d1/2) / std::sqrt(2 * M_PI)) / (S * sigma * std::sqrt(T));
        }
    }

    return 0.0; // Retourne 0 si aucune option n'est définie
}
