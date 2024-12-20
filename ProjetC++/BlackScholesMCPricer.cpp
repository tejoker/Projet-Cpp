#include "BlackScholesMCPricer.h"

BlackScholesMCPricer::BlackScholesMCPricer(Option* option, double initial_price, double interest_rate, double volatility)
    : option(option), S0(initial_price), r(interest_rate), sigma(volatility), nb_paths_generated(0), current_estimate(0.0) {}

void BlackScholesMCPricer::generate(int nb_paths) {
    double T = option->getExpiry();
    double payoff_sum = 0.0;

    for (int i = 0; i < nb_paths; ++i) {
		double Z = MT::rand_norm(); // Z ~ N(0,1)
		double ST = S0 * std::exp((r - 0.5 * sigma * sigma) * T + sigma * std::sqrt(T) * Z); //cf eq apr�s 5.1, avec Z*SQRT(T) = Wt,
		double payoff = option->payoff(ST); // cf eq apr�s 5.1.1
        payoff_sum += payoff;
    }

    double mean_payoff = payoff_sum / nb_paths;  
    current_estimate = (current_estimate * nb_paths_generated + mean_payoff * nb_paths) / (nb_paths_generated + nb_paths); 
	nb_paths_generated += nb_paths; // mise � jour du nombre de chemins g�n�r�s
}

double BlackScholesMCPricer::operator()() const {
    if (nb_paths_generated == 0) {
        throw std::runtime_error("Estimation non d�finie : aucun chemin g�n�r�.");
    }
    return current_estimate; // 
}

std::vector<double> BlackScholesMCPricer::confidenceInterval() const {
    if (nb_paths_generated == 0) {
        throw std::runtime_error("Intervalle de confiance non d�fini : aucun chemin g�n�r�.");
    }

    double variance = current_estimate * (1 - current_estimate) / nb_paths_generated;
    double std_error = std::sqrt(variance);
    double margin_of_error = 1.96 * std_error;

    return { current_estimate - margin_of_error, current_estimate + margin_of_error };
}

int BlackScholesMCPricer::getNbPaths() const {
    return nb_paths_generated;
}