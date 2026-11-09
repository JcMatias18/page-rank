#include "PageRank.h"

#include <exception>
#include <iomanip>
#include <iostream>
#include <vector>

using namespace std;

int main() {
    try {
        const string archivoCompetencias = "competencias.txt";
        const string archivoDependencias = "dependencias.txt";

        const double alpha = 0.85;
        const int max_iter = 100;
        const double tolerancia = 1e-6;

        vector<Competencia> competencias = cargarCompetencias(archivoCompetencias);
        vector<vector<int>> grafo = cargarDependencias(archivoDependencias, competencias);

        imprimirRed(competencias, grafo);

        ResultadoPageRank resultado = calcularPageRank(grafo, alpha, max_iter, tolerancia);

        imprimirConvergencia(resultado.errores, tolerancia);
        imprimirRanking(competencias, resultado.ranking);

        cout << "\nIteraciones realizadas: " << resultado.iteraciones << "\n";
        cout << "Error final: " << scientific << setprecision(6) << resultado.error_final << "\n";

        imprimirFilasLatex(competencias, resultado.ranking);
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
