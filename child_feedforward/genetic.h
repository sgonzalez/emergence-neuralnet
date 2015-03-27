/////////////////////////
/// Santiago Gonzalez ///
/////////////////////////

#pragma once

#include <iostream>
#include <vector>
#include <math.h>

struct Chromosome {
	std::vector<double> genes;
    double fitness;
    Chromosome() : fitness(0) {}
    Chromosome(std::vector<double> genes, double fitness) : genes(genes), fitness(fitness) {}
    friend bool operator<(const Chromosome& lhs, const Chromosome& rhs) { return (lhs.fitness < rhs.fitness); } // used for sorting
};

/// Genetic is the class that encapsulates the genetic algorithm itself
class Genetic {
    static const double maximumMutation;
    static const int numberEliteCopies;
    static const int numberElite;
    
    std::vector<Chromosome> population;
    int populationSize;
    int chromosomeLength;
    double totalFitness;
    double bestFitness;
    double averageFitness;
    double worstFitness;
    int bestChromosome;
    double mutationRate;
    double crossoverRate;
    int generation;
    
    void crossover(const std::vector<double> &progenitor1, const std::vector<double> &progenitor2, std::vector<double> &progeny1, std::vector<double> &progeny2);
    void mutate(std::vector<double> &chromosome);
    
    Chromosome getChromosomeRoulette();
    
    void takeBest(int num, const int numcopies, std::vector<Chromosome> &pop); // used to introduce elitism
    
    void calculateFitnessMetrics();
    
    void reset();
    
public:
    Genetic(int populationSize, double mutationRate, double crossoverRate, int chromosomeLength);
    
    std::vector<Chromosome> runEpoch(std::vector<Chromosome> &previousPopulation);
    
    std::vector<Chromosome> getChromosomes() const { return population; }
    double getAverageFitness() const {return totalFitness / populationSize; }
    double getBestFitness() const {return bestFitness; }
};
