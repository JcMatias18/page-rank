#ifndef PAGERANK_H
#define PAGERANK_H

#include <string>
#include <vector>

struct Competencia {
    std::string codigo;
    std::string nombre;
};

struct ResultadoPageRank {
    std::vector<double> ranking;
    std::vector<double> errores;
    int iteraciones;
    double error_final;
};

std::vector<Competencia> cargarCompetencias(const std::string& archivo);

std::vector<std::vector<int>> cargarDependencias(
    const std::string& archivo,
    const std::vector<Competencia>& competencias
);

ResultadoPageRank calcularPageRank(
    const std::vector<std::vector<int>>& grafo,
    double alpha,
    int max_iter,
    double tolerancia
);

void imprimirRed(
    const std::vector<Competencia>& competencias,
    const std::vector<std::vector<int>>& grafo
);

void imprimirConvergencia(const std::vector<double>& errores, double tolerancia);

void imprimirRanking(
    const std::vector<Competencia>& competencias,
    const std::vector<double>& ranking
);

void imprimirFilasLatex(
    const std::vector<Competencia>& competencias,
    const std::vector<double>& ranking
);

#endif
