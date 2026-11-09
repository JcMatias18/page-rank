#include "PageRank.h"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <numeric>
#include <set>
#include <sstream>
#include <stdexcept>

using namespace std;

static string trim(const string& s) {
    size_t ini = s.find_first_not_of(" \t\r\n");
    if (ini == string::npos) return "";
    size_t fin = s.find_last_not_of(" \t\r\n");
    return s.substr(ini, fin - ini + 1);
}

static map<string, int> construirIndice(const vector<Competencia>& competencias) {
    map<string, int> indice;
    for (int i = 0; i < static_cast<int>(competencias.size()); ++i) {
        indice[competencias[i].codigo] = i;
    }
    return indice;
}

vector<Competencia> cargarCompetencias(const string& archivo) {
    ifstream in(archivo);
    if (!in) throw runtime_error("No se pudo abrir el archivo: " + archivo);

    vector<Competencia> competencias;
    set<string> codigos;
    string linea;
    int numero_linea = 0;

    while (getline(in, linea)) {
        ++numero_linea;
        linea = trim(linea);
        if (linea.empty() || linea[0] == '#') continue;

        size_t pos = linea.find('|');
        if (pos == string::npos) throw runtime_error("Linea invalida en " + archivo + ": " + linea);

        Competencia c;
        c.codigo = trim(linea.substr(0, pos));
        c.nombre = trim(linea.substr(pos + 1));

        if (c.codigo.empty() || c.nombre.empty()) throw runtime_error("Competencia incompleta: " + linea);
        if (!codigos.insert(c.codigo).second) {
            throw runtime_error(
                "Codigo de competencia duplicado '" + c.codigo + "' en "
                + archivo + ", linea " + to_string(numero_linea)
            );
        }
        competencias.push_back(c);
    }

    if (competencias.empty()) throw runtime_error("No se cargaron competencias.");
    return competencias;
}

vector<vector<int>> cargarDependencias(const string& archivo, const vector<Competencia>& competencias) {
    ifstream in(archivo);
    if (!in) throw runtime_error("No se pudo abrir el archivo: " + archivo);

    map<string, int> indice = construirIndice(competencias);
    vector<vector<int>> grafo(competencias.size());

    string linea;
    int numero_linea = 0;

    while (getline(in, linea)) {
        numero_linea++;
        linea = trim(linea);
        if (linea.empty() || linea[0] == '#') continue;

        string dependiente, prerrequisito;
        stringstream ss(linea);
        ss >> dependiente >> prerrequisito;

        if (dependiente.empty() || prerrequisito.empty()) {
            throw runtime_error("Linea invalida en dependencias.txt, linea " + to_string(numero_linea));
        }
        if (!indice.count(dependiente)) throw runtime_error("Codigo no reconocido: " + dependiente);
        if (!indice.count(prerrequisito)) throw runtime_error("Codigo no reconocido: " + prerrequisito);

        int u = indice[dependiente];
        int v = indice[prerrequisito];

        // Convencion del informe:
        // u -> v significa competencia dependiente -> competencia prerrequisito.
        grafo[u].push_back(v);
    }

    for (auto& vecinos : grafo) {
        sort(vecinos.begin(), vecinos.end());
        vecinos.erase(unique(vecinos.begin(), vecinos.end()), vecinos.end());
    }

    return grafo;
}

ResultadoPageRank calcularPageRank(const vector<vector<int>>& grafo, double alpha, int max_iter, double tolerancia) {
    int n = static_cast<int>(grafo.size());
    vector<double> ranking(n, 1.0 / n);
    vector<double> nuevo(n, 0.0);
    vector<double> errores;
    int iteraciones = 0;
    double error = 0.0;

    for (int it = 1; it <= max_iter; ++it) {
        fill(nuevo.begin(), nuevo.end(), (1.0 - alpha) / n);

        double masa_colgante = 0.0;
        for (int u = 0; u < n; ++u) {
            if (grafo[u].empty()) masa_colgante += ranking[u];
        }
        for (int i = 0; i < n; ++i) {
            nuevo[i] += alpha * masa_colgante / n;
        }

        for (int u = 0; u < n; ++u) {
            if (!grafo[u].empty()) {
                double aporte = alpha * ranking[u] / static_cast<double>(grafo[u].size());
                for (int v : grafo[u]) nuevo[v] += aporte;
            }
        }

        error = 0.0;
        for (int i = 0; i < n; ++i) error += fabs(nuevo[i] - ranking[i]);

        ranking = nuevo;
        errores.push_back(error);
        iteraciones = it;
        if (error < tolerancia) break;
    }

    return {ranking, errores, iteraciones, error};
}

void imprimirRed(const vector<Competencia>& competencias, const vector<vector<int>>& grafo) {
    cout << "===== RED CURRICULAR =====\n";
    cout << "Convencion: competencia dependiente -> competencia prerrequisito\n\n";

    for (int u = 0; u < static_cast<int>(competencias.size()); ++u) {
        cout << setw(4) << left << competencias[u].codigo
             << setw(45) << left << competencias[u].nombre << " -> ";
        if (grafo[u].empty()) {
            cout << "(sin prerrequisito interno)";
        } else {
            for (int v : grafo[u]) cout << competencias[v].codigo << " ";
        }
        cout << "\n";
    }
}

void imprimirConvergencia(const vector<double>& errores, double tolerancia) {
    cout << "\n===== CONVERGENCIA DEL METODO DE LA POTENCIA =====\n";
    cout << setw(12) << left << "Iteracion" << setw(15) << left << "Error L1" << "Observacion\n";

    vector<int> mostrar = {1, 5, 10, static_cast<int>(errores.size())};
    vector<int> usados;

    for (int it : mostrar) {
        if (it >= 1 && it <= static_cast<int>(errores.size()) && find(usados.begin(), usados.end(), it) == usados.end()) {
            usados.push_back(it);
            cout << setw(12) << left << it
                 << setw(15) << left << scientific << setprecision(6) << errores[it - 1];
            if (it == static_cast<int>(errores.size())) {
                cout << (errores.back() < tolerancia
                    ? "Convergencia alcanzada"
                    : "Limite de iteraciones alcanzado");
            } else if (it == 1) {
                cout << "Primera iteracion";
            }
            cout << "\n";
        }
    }
}

static vector<int> ordenarRanking(const vector<Competencia>& competencias, const vector<double>& ranking) {
    vector<int> orden(ranking.size());
    iota(orden.begin(), orden.end(), 0);
    sort(orden.begin(), orden.end(), [&](int a, int b) {
        if (ranking[a] != ranking[b]) return ranking[a] > ranking[b];
        return competencias[a].codigo < competencias[b].codigo;
    });
    return orden;
}

void imprimirRanking(const vector<Competencia>& competencias, const vector<double>& ranking) {
    const vector<int> orden = ordenarRanking(competencias, ranking);
    cout << "\n===== RANKING FINAL SEGUN PAGERANK =====\n";
    cout << setw(8) << left << "Puesto" << setw(8) << left << "Nodo" << setw(45) << left << "Competencia" << "PageRank\n";
    for (int pos = 0; pos < static_cast<int>(orden.size()); ++pos) {
        int i = orden[pos];
        cout << setw(8) << left << (pos + 1)
             << setw(8) << left << competencias[i].codigo
             << setw(45) << left << competencias[i].nombre
             << fixed << setprecision(6) << ranking[i] << "\n";
    }
    double suma = accumulate(ranking.begin(), ranking.end(), 0.0);
    cout << "\nSuma de PageRank = " << fixed << setprecision(6) << suma << "\n";
}

void imprimirFilasLatex(const vector<Competencia>& competencias, const vector<double>& ranking) {
    const vector<int> orden = ordenarRanking(competencias, ranking);

    cout << "\n===== FILAS PARA TABLA LATEX =====\n";
    for (int pos = 0; pos < static_cast<int>(orden.size()); ++pos) {
        int i = orden[pos];
        cout << (pos + 1) << " & $" << competencias[i].codigo << "$ & "
             << competencias[i].nombre << " & "
             << fixed << setprecision(6) << ranking[i] << " \\\\\n";
    }
}
