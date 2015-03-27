/////////////////////////
/// Santiago Gonzalez ///
/////////////////////////

#include "genetic.h"
#include "utils.h"


const double Genetic::maximumMutation = 0.3;
const int Genetic::numberEliteCopies = 1;
const int Genetic::numberElite = 4;

Genetic::Genetic(int populationSize, double mutationRate, double crossoverRate, int chromosomeLength) :
populationSize(populationSize),
mutationRate(mutationRate),
crossoverRate(crossoverRate),
chromosomeLength(chromosomeLength),
totalFitness(0),
generation(0),
bestChromosome(0),
bestFitness(0),
worstFitness(99999999),
averageFitness(0) {
    // create random chromosomes with zero fitness
    for (int i = 0; i < populationSize; i++) {
		population.push_back(Chromosome());
		for (int j = 0; j < chromosomeLength; j++) {
			population[i].genes.push_back(randomClamped());
		}
	}
}


void Genetic::mutate(std::vector<double> &chromosome) {
    for (int i = 0; i < chromosome.size(); i++) {
        if (randFloat() < mutationRate) { // should this gene be mutated
            chromosome[i] += randomClamped() * maximumMutation;
        }
    }
}

Chromosome Genetic::getChromosomeRoulette() {
    double slice = (double)(randFloat() * totalFitness);
    Chromosome c;
    double cumulativeFitness = 0;
    for (int i = 0; i < populationSize; i++) {
        cumulativeFitness += population[i].fitness;
        if (cumulativeFitness >= slice) {
            c = population[i];
            break;
        }
    }
    return c;
}

void Genetic::crossover(const std::vector<double> &progenitor1, const std::vector<double> &progenitor2, std::vector<double> &progeny1, std::vector<double> &progeny2) {
    if (randFloat() > crossoverRate || progenitor1 == progenitor2) { // if we are not doing crossover or progenitor chromosomes are the same
        progeny1 = progenitor1;
        progeny2 = progenitor2;
    } else { // crossover
        int crossoverPoint = randInt(0, chromosomeLength - 1);
        for (int i = 0; i < crossoverPoint; i++) {
            progeny1.push_back(progenitor1[i]);
            progeny2.push_back(progenitor2[i]);
        }
        for (int i = crossoverPoint; i < progenitor1.size(); i++) {
            progeny1.push_back(progenitor1[i]);
            progeny2.push_back(progenitor2[i]);
        }
    }
}

void Genetic::takeBest(int num, const int numcopies, std::vector<Chromosome> &pop) {
    while (num--) {
        for (int i = 0; i < numcopies; i++) {
            pop.push_back(population[(populationSize - 1) - num]);
        }
    }
}

void Genetic::calculateFitnessMetrics() {
    totalFitness = 0;
    double highestSoFar = 0;
    double lowestSoFar = 99999999;
    for (int i = 0; i < populationSize; i++) {
        if (population[i].fitness > highestSoFar) { // better chromosome
            highestSoFar = population[i].fitness;
            bestChromosome = i;
            bestFitness = highestSoFar;
        }
        if (population[i].fitness < lowestSoFar) { // worse chromosome
            lowestSoFar = population[i].fitness;
            worstFitness = lowestSoFar;
        }
        totalFitness += population[i].fitness;
    }
    averageFitness = totalFitness / populationSize;
}

void Genetic::reset() {
    totalFitness = 0;
    bestFitness = 0;
    worstFitness = 99999999;
    averageFitness = 0;
}


std::vector<Chromosome> Genetic::runEpoch(std::vector<Chromosome> &previousPopulation) {
    population = previousPopulation;
    reset();
    sort(population.begin(), population.end()); // order the chromosomes acording to their fitness
    calculateFitnessMetrics();
    
    std::vector<Chromosome> newPopulation;

    // introduce elitism
    if (!(numberEliteCopies * numberElite % 2)) { // ensure we have an even number, or roulette wheel sampling breaks
        takeBest(numberElite, numberEliteCopies, newPopulation);
    }
    
    // repeat until we have generated a new population
    while (newPopulation.size() < populationSize) {
        Chromosome progenitor1 = getChromosomeRoulette(); // take a chromosome
        Chromosome progenitor2 = getChromosomeRoulette(); // take a chromosome
        std::vector<double> progeny1, progeny2;
        crossover(progenitor1.genes, progenitor2.genes, progeny1, progeny2);
        mutate(progeny1);
        mutate(progeny2);
        newPopulation.push_back(Chromosome(progeny1, 0));
        newPopulation.push_back(Chromosome(progeny2, 0));
    }
    population = newPopulation;
    return population;
}





