#pragma once
#include <random>
#include <chrono>
using namespace std;

namespace npylm{
	namespace sampler{
		int seed = chrono::system_clock::now().time_since_epoch().count();
		// int seed = 1;
		mt19937 mt(seed);
		double gamma(double a, double b){
			gamma_distribution<double> distribution(a, 1.0 / b);
			return distribution(mt);
		}
		double beta(double a, double b){
			double ga = gamma(a, 1.0);
			double gb = gamma(b, 1.0);
			return ga / (ga + gb);
		}
		double bernoulli(double p){
			uniform_real_distribution<double> rand(0, 1);
			double r = rand(mt);
			if(r > p){
				return 0;
			}
			return 1;
		}
		double uniform(double min = 0, double max = 0){
			uniform_real_distribution<double> rand(min, max);
			return rand(mt);
		}
	} // namespace sampler
} // namespace npylm