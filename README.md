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

4. TOP-IDP instance format
Each TOP-IDP instance contains the information required to construct the routing and temporal-profit problem, including:
number of customers;
number of vehicles;
number of intervals;
customer coordinates;
profit associated with each interval;
service time windows;
service durations;
interval boundaries;

5. Relationship between repository results and the paper
The repository contains two Excel workbooks with the detailed numerical results used in the manuscript.
### Workbook 1: `<workbook_1.xlsx>`
| Worksheet | Corresponding manuscript item | Description |
|---|---|---|
| `<sheet_name_1>` | Table 3 | Instance-level results of CPLEX and Full PG-GRILS |
| `<sheet_name_2>` | Table 4 | Instance-level results of ILS, ALNS, and Restricted PG-GRILS |
| `<sheet_name_3>` | Table 5 | Results of the HCP-only, PO-only, and TSI-only initialization configurations |
| `<sheet_name_4>` | Fig. 7 | Relative MeanObj improvements of TCR, PDP, and TCR+PDP under Restricted PG-GRILS |
| `<sheet_name_5>` | Fig. 8 | MeanObj comparison of reconstruction operators under Full PG-GRILS |

### Workbook 2: `<workbook_2.xlsx>`
| Worksheet | Corresponding manuscript item | Description |
|---|---|---|
| `<sheet_name_1>` | Fig. 9 | Temporal-refinement ablation results |
| `<sheet_name_2>` | Fig. 10 | Initial solution quality generated by HCP, PO, and TSI |
| `<sheet_name_3>` | Fig. 11 | Initialization ablation results |
| `<sheet_name_4>` | Fig. 12 | MeanObj and StdObj comparison with and without UCB |
| `<sheet_name_5>` | Fig. 14 | Comparison on the benchmark instances of Li et al. (2024a) |
| `<sheet_name_6>` | Fig. 15 | Comparison on the EOS benchmark instances of Li et al. (2024b) |

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
