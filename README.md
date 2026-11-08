# EJOR--TOP-IDP

Data, detailed computational results, and source code associated with the paper:

> **How interval-dependent profits affect team orienteering problems with two visits**

This repository provides the benchmark instances, instance-level computational results, and C++ source code used to evaluate the Profit-Guided Greedy Randomized Iterated Local Search (PG-GRILS) for the Team Orienteering Problem with Interval-Dependent Profits (TOP-IDP).

## 1. Repository contents

The repository contains three groups of datasets, five C++ source files, and two files containing detailed computational results.

EJOR--TOP-IDP/
│
├── README.md
│
├── data/
│   ├── TOP-IDP/
│   ├── Li2024a/
│   └── Li2024b/
│
├── code/
│   ├── <Full_PG_GRILS.cpp>
│   ├── <Restricted_PG_GRILS.cpp>
│   ├── <ILS.cpp>
│   ├── <ALNS.cpp>
│   └── <CPLEX_MILP.cpp>
│
└── results/
    ├── <detailed_results.xlsx>
    └── <figure_data.xlsx>

1.1 Datasets
Three datasets are provided:
TOP-IDP instances
These are the main instances generated for the TOP-IDP studied in the paper. The instances contain 10, 20, 30, 50, or 100 customers and 4 or 6 vehicles. For each problem scale, three independent instances were generated.

Li et al. (2024a) benchmark instances
These instances are derived from the interval-varying-profit TOP benchmark used in Li et al. (2024a). The original discrete interval representation was converted into the stepwise interval-dependent profit representation used in the present study.

Li et al. (2024b) EOS benchmark instances
These instances originate from the earth observation satellite scheduling problem studied in Li et al. (2024b). Orbital-period intervals were converted into stepwise time intervals for testing the generalization performance of PG-GRILS.
The data-generation and conversion procedures are described in Sections 5.1 and 5.5 of the paper.

1.2 Source code
The five C++ source files implement the main solution methods used in the computational study:

Source file	Description
<Full_PG_GRILS.cpp>	Full PG-GRILS with complete neighborhood exploration
<Restricted_PG_GRILS.cpp>	Restricted PG-GRILS with a total budget of 1,000 search iterations
<ILS.cpp>	Baseline Iterated Local Search
<ALNS.cpp>	Baseline Adaptive Large Neighborhood Search
<CPLEX_MILP.cpp>	MILP formulation solved using IBM ILOG CPLEX
For Restricted PG-GRILS, one search iteration consists of: generating and evaluating one perturbation–repair neighborhood; applying temporal refinement to the selected candidate; and performing the subsequent acceptance test. The iteration counter is accumulated over the entire run. 
Full PG-GRILS has no iteration limit and evaluates the complete neighborhood until no further improving solution exists at each greediness level.

1.3 Detailed computational results
The results/ directory contains the instance-level numerical data underlying the aggregated tables and figures reported in the paper.
The result files include:
objective values for each tested instance and algorithmic configuration;
computational times;
data used to calculate the reported averages;
data underlying the figures and ablation analyses;
results used in the statistical comparisons.
These files supplement the aggregated results reported in Tables 3–5 and Figs. 7–15 of the paper. Complete vehicle routes and service schedules are not included because the reported analyses are based on objective values and computational times.

2. Computational environment
The experiments reported in the paper were conducted using the following environment:

Operating system: Windows
Processor: 2.8 GHz Intel Core i7
Memory: 8 GB RAM
Number of threads: 1
Programming language: C++
Development environment: Microsoft Visual Studio 2022
MILP solver: IBM ILOG CPLEX 20.1
The heuristic algorithms are implemented in C++. The MILP formulation requires a valid installation and license of IBM ILOG CPLEX 20.1.

3. Main parameter settings
The principal parameters used in the experiments are:
Initial greediness parameter (StartGreed)	0.89
Final greediness parameter (EndGreed)	0.59
Greediness decrement (GreedDecrease)	0.1
Temporal discretization step	10 s
UCB exploration coefficient	2
Restricted PG-GRILS iteration limit	1,000
CPLEX time limit	10,800 s
Independent runs per stochastic algorithm	10
Computational threads	1
Additional parameter definitions are provided in Table 2 and the corresponding methodological sections of the paper.

4. Main parameter settings
The principal parameters used in the experiments are:

Parameter	Value
Initial greediness parameter (StartGreed)	0.89
Final greediness parameter (EndGreed)	0.59
Greediness decrement (GreedDecrease)	0.1
Temporal discretization step	10 s
UCB exploration coefficient	2
Restricted PG-GRILS iteration limit	1,000
CPLEX time limit	10,800 s
Independent runs per stochastic algorithm	10
Computational threads	1
Additional parameter definitions are provided in Table 2 and the corresponding methodological sections of the paper.

5. TOP-IDP instance format
Each TOP-IDP instance contains the information required to construct the routing and temporal-profit problem, including:
number of customers;
number of vehicles;
number of intervals;
customer coordinates;
profit associated with each interval;
service time windows;
service durations;
interval boundaries;

6. Relationship between repository results and the paper
This repository contains two Excel workbooks providing the instance-level and run-level numerical results underlying the tables and figures reported in the manuscript.

### Workbook 1: `Baseline Results.xlsx`
This workbook contains the main computational comparison among ILS, ALNS,
Restricted PG-GRILS, Full PG-GRILS, and CPLEX.

| Suggested worksheet name | Corresponding manuscript item | Description |
|---|---|---|
| `Main_Summary` | Tables 3 and 4 | Instance-level summary results for ILS, ALNS, Restricted PG-GRILS, Full PG-GRILS, and CPLEX |
| `ILS_Raw` | Table 4 | Objective value and computational time for each of the ten independent ILS runs |
| `ALNS_Raw` | Table 4 | Objective value and computational time for each of the ten independent ALNS runs |
| `Restricted_PGGRILS_Raw` | Table 4 | Objective value and computational time for each of the ten independent Restricted PG-GRILS runs |
| `Full_PGGRILS_Raw` | Table 3 | Objective value and computational time for each of the ten independent Full PG-GRILS runs |
| `CPLEX_Results` | Table 3 | Best feasible value, best bound, and computational time obtained by CPLEX |

In the manuscript:
- **Table 3** is obtained from the Full PG-GRILS and CPLEX columns in `Main_Summary`.
- **Table 4** is obtained from the ILS, ALNS, and Restricted PG-GRILS columns in `Main_Summary`.
- The corresponding raw worksheets provide the ten independent runs used to calculate `BestObj`, `MeanObj`, `StdObj`, and `MeanTime`.

### Workbook 2: `Other Experiments.xlsx`

This workbook contains the operator analysis, temporal-refinement analysis, initialization analysis, and UCB analysis.

| Suggested worksheet name | Corresponding manuscript item | Description |
|---|---|---|
| `Restricted_Operators` | Fig. 7 | Results of the standard removal baseline, TCR, PDP, and the combined TCR+PDP configuration under Restricted PG-GRILS |
| `Full_Operators` | Fig. 8 | MeanObj comparison of TCR, PDP, and TCR+PDP under Full PG-GRILS |
| `Temporal_Refinement` | Fig. 9 | Comparison between PG-GRILS without temporal refinement and PG-GRILS with temporal refinement |
| `Initialization_Heuristics` | Table 5 and Fig. 10 | Results of the HCP-only, PO-only, and TSI-only initialization configurations; the corresponding initial-solution data are used for Fig. 10 |
| `Initialization_Ablation` | Fig. 11 | Results of the complete hybrid strategy and the variants obtained by removing HCP, PO, or TSI |
| `UCB_Analysis` | Fig. 12 | Comparison of MeanObj and StdObj with and without the UCB-guided heuristic selection mechanism |

### External benchmark results
The numerical data underlying **Figs. 14 and 15** are provided with the
corresponding external benchmark datasets:
| Manuscript item | Dataset | Description |
|---|---|---|
| Fig. 14 | `Li2024a` | Comparison of PG-GRILS, BPC, and CPLEX on the benchmark instances of Li et al. (2024a) |
| Fig. 15 | `Li2024b` | Comparison of PG-GRILS, BP, NS, and the CPLEX lower bound on the EOS benchmark instances of Li et al. (2024b) |

### Reported metrics
For each stochastic algorithm, the manuscript reports statistics over ten
independent runs:
- `BestObj`: the largest objective value over the ten runs;
- `MeanObj`: the arithmetic mean of the objective values;
- `StdObj`: the sample standard deviation of the objective values;
- `MeanTime`: the arithmetic mean of the computational times in seconds.
For CPLEX:
- `LB`: the best feasible objective value;
- `UB`: the best bound returned by CPLEX;
- `TL`: the three-hour time limit was reached without proving optimality.
