# PageRank for Curriculum Competency Networks (C++)

A modular C++ implementation of the PageRank algorithm applied to a directed network of mathematics curriculum competencies. The example dataset represents prerequisite relationships among competencies, allowing an exploratory ranking of their structural importance in the network.

# Project overview

Each competency is a node. A directed edge `C3 C1` means that **C3 depends on C1** (dependent → prerequisite). With this orientation, PageRank assigns weight to prerequisites through the links they receive from other competencies. The score measures structural centrality within the supplied graph.

[Read the full project report (Spanish)](docs/informe-pagerank.pdf)

# Features

- Loads competency names and prerequisite relationships from text files.
- Calculates PageRank using power iteration, damping, and redistribution from dangling nodes.
- Reports the L1 difference between successive ranking vectors and why iteration stopped.
- Prints the ranked competencies and LaTeX-ready table rows.
- Separates the entry point (`main.cpp`), interface (`PageRank.h`), and implementation (`PageRank.cpp`).

## Requirements!!!

A C++ compiler with C++11 support or newer (for example, `g++`). No third-party libraries are required.

## Build and run

Run these commands from the repository root:

**Linux / macOS:**

```bash
g++ -std=c++17 -Wall -Wextra -pedantic main.cpp PageRank.cpp -o pagerank
./pagerank
```

**Windows (PowerShell, with g++ installed):**

```powershell
g++ -std=c++17 -Wall -Wextra -pedantic main.cpp PageRank.cpp -o pagerank.exe
.\pagerank.exe
```

The program reads `competencias.txt` and `dependencias.txt` from the **current working directory**. Keep both files there when running the executable.

## Input files

- [`competencias.txt`](competencias.txt) lists competencies as `code|description`, for example `C1|Sistemas numericos`. Codes must be unique.
- [`dependencias.txt`](dependencias.txt) lists one `dependent prerequisite` pair per line, for example `C3 C1`. Both codes must be defined in the competency file.

Both files accept blank lines and lines beginning with `#` as comments.

## Dataset and scope

The bundled example contains **32 nodes and 102 directed edges**. The academic project used Peru's mathematics learning progression maps and secondary education curriculum as references. The two text files define the graph used by this implementation. The edges encode modeled relationships, including reciprocal dependencies such as `C8 → T4` and `T4 → C8`.

## Algorithm and configuration

The iterative update is

$$
p_{t+1}(v)=\frac{1-d}{N}+d\frac{S_t}{N}+d\sum_{u\to v}\frac{p_t(u)}{\deg^+(u)},
$$

where `d` is the damping factor, `N` the number of nodes, and `S_t` the total rank of nodes with no outgoing edges. The implementation uses `d = 0.85`, a maximum of 100 iterations, and an L1 stopping tolerance of `1e-6` (configured in `main.cpp`).

## Example results

With the included dataset, the first three ranked competencies are:

| Rank | Code | Competency | PageRank |
|---:|---|---|---:|
| 1 | C1 | Sistemas numericos | 0.254591 |
| 2 | C2 | Operaciones y propiedades | 0.113641 |
| 3 | C3 | Fracciones, decimales y porcentajes | 0.046915 |

With the bundled files, the program converges in **20 iterations** with a final L1 difference of **7.584137 × 10⁻⁷**. See [the full sample output](docs/sample-output.txt).

## Limitations

The ranking depends on the selected competencies, the modeled dependencies, and the direction of the edges. Evaluating pedagogical importance would require independent evidence about learning outcomes.

## Academic context and attribution

This implementation accompanies a Group 6 project for Linear Algebra I at Universidad Nacional de Ingeniería, Peru (2026-I).

The academic project team included  José Carlos Matías Muñoz and Alejandra Enríquez Aranda.

## References

- Page, L., Brin, S., Motwani, R., & Winograd, T. (1999). [*The PageRank Citation Ranking: Bringing Order to the Web*](https://www.cs.uml.edu/~haim/teaching/iws/tirsaa/sources/google/PageRank_Citation_cover.html). Stanford InfoLab.

- IPEBA and Ministerio de Educación del Perú. [*Mapas de progreso del aprendizaje: Matemática*](https://repositorio.minedu.gob.pe/handle/20.500.12799/5236).

- Ministerio de Educación del Perú. (2025). [*Programa Curricular de Educación Secundaria*](https://www.gob.pe/institucion/minedu/informes-publicaciones/6791708-programa-curricular-de-educacion-secundaria).
