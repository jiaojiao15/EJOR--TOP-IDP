#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <algorithm>


using namespace std;

int nc, nt, nm;

struct Node {
    int id;
    float x, y;
};

Node* node;
float** dist;
float** timeMat;
double** profit;
float** twStart, ** twEnd;
float* service;
float** intervalLow, ** intervalHigh;

struct Route {
    vector<int> nodes;
    vector<float> startTime;
    double totalProfit;
    vector<int> served;

    Route() {
        nodes = { 0, 2 * nc + 1 };
        startTime = { 0, 0 };
        totalProfit = 0;
    }
};

vector<Route> routes;
vector<Route> curRoutes, bestRoutes;
vector<bool> curServed, bestServed;

double bestTotal = -1e18;

// ================= DIST =================
void buildDistAndTime() {
    int total = 2 * nc + 2;
    dist = new float* [total];
    timeMat = new float* [total];

    for (int i = 0; i < total; i++) {
        dist[i] = new float[total];
        timeMat[i] = new float[total];
        for (int j = 0; j < total; j++) {
            float dx = node[i].x - node[j].x;
            float dy = node[i].y - node[j].y;
            dist[i][j] = sqrt(dx * dx + dy * dy);
            timeMat[i][j] = dist[i][j] / 20 *3600;
            //cout << timeMat[i][j] << setw(9);
        }
    }
}

// ================= INSERT SINGLE =================
bool insertSingle(const vector<int>& ns, const vector<float>& ts,
    int pos, int u, int vid,
    vector<int>& newNs, vector<float>& newTs) {

    newNs = ns;
    newTs = ts;

    newNs.insert(newNs.begin() + pos, u);
    newTs.insert(newTs.begin() + pos, 0);

    float arr = (pos == 0)
        ? timeMat[0][u]
        : ts[pos - 1] + service[ns[pos - 1]] + timeMat[ns[pos - 1]][u];

    float st = max(arr, twStart[u][vid]);
    if (st > twEnd[u][vid]) return false;

    newTs[pos] = st;

    for (int i = pos + 1; i < newNs.size(); i++) {
        int prev = newNs[i - 1];
        int cur = newNs[i];

        arr = newTs[i - 1] + service[prev] + timeMat[prev][cur];
        st = max(arr, twStart[cur][vid]);

        if (st > twEnd[cur][vid]) return false;
        newTs[i] = st;
    }

    return true;
}

// ================= CROSS ROUTE INSERT =================
bool insertTwoNodesDiffRoute(int c,
    int v1, int v2,
    vector<int>& bestN1, vector<float>& bestT1,
    vector<int>& bestN2, vector<float>& bestT2,
    double& bestGain) {

    int n1 = c;
    int n2 = c + nc;

    auto baseN1 = curRoutes[v1].nodes;
    auto baseT1 = curRoutes[v1].startTime;
    auto baseN2 = curRoutes[v2].nodes;
    auto baseT2 = curRoutes[v2].startTime;

    bestGain = -1e18;

    for (int p1 = 1; p1 < baseN1.size(); p1++) {

        vector<int> nA1;
        vector<float> tA1;

        if (!insertSingle(baseN1, baseT1, p1, n1, v1, nA1, tA1))
            continue;

        for (int p2 = 1; p2 < baseN2.size(); p2++) {

            if (p2 == p1 && v1 == v2)
                continue;

            vector<int> nA2;
            vector<float> tA2;

            if (!insertSingle(baseN2, baseT2, p2, n2, v2, nA2, tA2))
                continue;

            float iv = fabs(tA2[p2] - tA1[p1]);

            int m = -1;
            for (int mm = 0; mm < nm; mm++) {
                if (iv >= intervalLow[c][mm] && iv < intervalHigh[c][mm]) {
                    m = mm;
                    break;
                }
            }
            if (m == -1) continue;

            double g = profit[c][m];

            if (g > bestGain) {
                bestGain = g;
                bestN1 = nA1;
                bestT1 = tA1;
                bestN2 = nA2;
                bestT2 = tA2;
            }
        }
    }

    return bestGain > 0;
}

void rebuildAllRouteTime()
{
    for (int v = 0; v < nt; v++)
    {
        auto& r = curRoutes[v];

        for (int i = 1; i < r.nodes.size(); i++)
        {
            int prev = r.nodes[i - 1];
            int cur = r.nodes[i];

            float arr = r.startTime[i - 1] + service[prev] + timeMat[prev][cur];
            float st = max(arr, twStart[cur][v]);

            r.startTime[i] = st;
        }
    }
}


// ================= REMOVE =================
void removeCustomer(int c) {

    if (!curServed[c]) return;
    curServed[c] = false;

    for (auto& r : curRoutes) {

        for (int i = 0; i < r.nodes.size();) {
            if (r.nodes[i] == c || r.nodes[i] == c + nc) {
                r.nodes.erase(r.nodes.begin() + i);
                r.startTime.erase(r.startTime.begin() + i);
            }
            else i++;
        }

        auto it = find(r.served.begin(), r.served.end(), c);
        if (it != r.served.end())
            r.served.erase(it);
    }
}

// ================= RECOMPUTE =================
void recompute() {

    for (auto& r : curRoutes)
        r.totalProfit = 0;

    for (int c = 1; c <= nc; c++) {

        if (!curServed[c]) continue;

        float t1 = -1, t2 = -1;
        int routeIndex = -1;

        for (int v = 0; v < nt; v++) {
            auto& r = curRoutes[v];

            for (int i = 0; i < r.nodes.size(); i++) {
                if (r.nodes[i] == c) {
                    t1 = r.startTime[i];
                    routeIndex = v;
                }
                if (r.nodes[i] == c + nc)
                    t2 = r.startTime[i];
            }
        }

        if (t1 < 0 || t2 < 0) continue;

        float iv = fabs(t2 - t1);

        int m = 0;
        for (int mm = 0; mm < nm; mm++) {
            if (iv >= intervalLow[c][mm] && iv < intervalHigh[c][mm]) {
                m = mm;
                break;
            }
        }

        if (routeIndex != -1)
            curRoutes[routeIndex].totalProfit += profit[c][m];
    }
}

// ================= INITIAL =================
void initialSolution()
{
    curRoutes.clear();
    for (int i = 0; i < nt; i++)
        curRoutes.push_back(Route());

    curServed.assign(nc + 1, false);

    vector<int> order;
    for (int c = 1; c <= nc; c++)
        order.push_back(c);

    random_shuffle(order.begin(), order.end());

    for (int c : order)
    {
        int bestV1 = -1, bestV2 = -1;
        double bestGain = -1e18;

        vector<int> bestN1, bestN2;
        vector<float> bestT1, bestT2;

        for (int v1 = 0; v1 < nt; v1++)
        {
            for (int v2 = 0; v2 < nt; v2++)
            {
                vector<int> n1, n2;
                vector<float> t1, t2;
                double g;

                if (insertTwoNodesDiffRoute(c, v1, v2, n1, t1, n2, t2, g))
                {
                    if (g > bestGain)
                    {
                        bestGain = g;
                        bestV1 = v1;
                        bestV2 = v2;
                        bestN1 = n1;
                        bestT1 = t1;
                        bestN2 = n2;
                        bestT2 = t2;
                    }
                }
            }
        }

        if (bestGain > 0)
        {
            curServed[c] = true;
            curRoutes[bestV1].nodes = bestN1;
            curRoutes[bestV1].startTime = bestT1;
            curRoutes[bestV2].nodes = bestN2;
            curRoutes[bestV2].startTime = bestT2;
        }
    }

    rebuildAllRouteTime();
}

void PrintInitialSolution() {

    double total = 0;
    for (auto& r : curRoutes)
        total += r.totalProfit;

   /*cout << "\n================= INITIAL SOLUTION =================\n";
    cout << "Total Profit: " << fixed << setprecision(2) << total << "\n\n";

    for (int k = 0; k < nt; k++) {

        auto& r = curRoutes[k];

        cout << "Route " << k + 1 << " Profit: "
            << fixed << setprecision(2) << r.totalProfit << "\n";

        cout << "Nodes: ";
        for (int x : r.nodes)
            cout << x << " -> ";
        cout << "END\n";

        cout << "Start Time: ";
        for (float t : r.startTime)
            cout << fixed << setprecision(1) << t << " ";
        cout << "\n";

        cout << "-----------------------------\n";
    }

    cout << "==================================================\n";*/
}



// ================= OUTPUT =================
double getTotalProfit() {
    double s = 0;
    for (auto& r : routes) s += r.totalProfit;
    return s;
}
double getCustomerProfit(int c) {

    float t1 = -1, t2 = -1;

    for (int v = 0; v < nt; v++) {
        auto& r = curRoutes[v];

        for (int i = 0; i < r.nodes.size(); i++) {
            if (r.nodes[i] == c) t1 = r.startTime[i];
            if (r.nodes[i] == c + nc) t2 = r.startTime[i];
        }
    }

    if (t1 < 0 || t2 < 0) return 0;

    float iv = fabs(t2 - t1);

    for (int m = 0; m < nm; m++) {
        if (iv >= intervalLow[c][m] && iv < intervalHigh[c][m]) {
            return profit[c][m];
        }
    }

    return 0;
}



void removeRandom(int k) {

    vector<int> candidates;

    // 收集所有已服务客户
    for (int c = 1; c <= nc; c++) {
        if (curServed[c]) {
            candidates.push_back(c);
        }
    }

    random_shuffle(candidates.begin(), candidates.end());

    int cnt = 0;

    for (int idx = 0; idx < candidates.size() && cnt < k; idx++) {

        int c = candidates[idx];
        curServed[c] = false;
        cnt++;

        // 从所有路线中删除 c 和 c+nc
        for (auto& r : curRoutes) {

            for (int i = 0; i < r.nodes.size();) {

                if (r.nodes[i] == c || r.nodes[i] == c + nc) {
                    r.nodes.erase(r.nodes.begin() + i);
                    r.startTime.erase(r.startTime.begin() + i);
                }
                else i++;
            }
        }
    }
}


void removeWorstProfit(int k) {

    vector<pair<double, int>> vec;

    for (int c = 1; c <= nc; c++) {
        if (!curServed[c]) continue;
        vec.push_back({ getCustomerProfit(c), c });
    }

    sort(vec.begin(), vec.end()); // 从小到大

    int cnt = 0;
    for (auto& p : vec) {
        if (cnt >= k) break;

        int c = p.second;
        curServed[c] = false;

        for (auto& r : curRoutes) {
            for (int i = 0; i < r.nodes.size();) {
                if (r.nodes[i] == c || r.nodes[i] == c + nc) {
                    r.nodes.erase(r.nodes.begin() + i);
                    r.startTime.erase(r.startTime.begin() + i);
                }
                else i++;
            }
        }

        cnt++;
    }
}

double relatedScore(int i, int j) {

    double spatial = dist[i][j];

    double timeDiff = 1e9;

    for (int v = 0; v < nt; v++) {
        timeDiff = min(timeDiff,
            (double)fabs(twStart[i][v] - twStart[j][v]));
    }

    return spatial + 0.1 * timeDiff;
}
void removeRelated(int seed, int k) {

    vector<pair<double, int>> cand;

    for (int c = 1; c <= nc; c++) {
        if (!curServed[c]) continue;
        if (c == seed) continue;

        cand.push_back({ relatedScore(seed, c), c });
    }

    sort(cand.begin(), cand.end());

    curServed[seed] = false;

    for (auto& r : curRoutes) {
        for (int i = 0; i < r.nodes.size();) {
            if (r.nodes[i] == seed || r.nodes[i] == seed + nc) {
                r.nodes.erase(r.nodes.begin() + i);
                r.startTime.erase(r.startTime.begin() + i);
            }
            else i++;
        }
    }

    int cnt = 0;
    for (auto& p : cand) {
        if (cnt >= k) break;

        int c = p.second;
        curServed[c] = false;

        for (auto& r : curRoutes) {
            for (int i = 0; i < r.nodes.size();) {
                if (r.nodes[i] == c || r.nodes[i] == c + nc) {
                    r.nodes.erase(r.nodes.begin() + i);
                    r.startTime.erase(r.startTime.begin() + i);
                }
                else i++;
            }
        }

        cnt++;
    }
}

void destroy(int type, int k) {

    if (type == 0) {
        removeWorstProfit(k);
    }
    else if (type == 1) {
        int seed = rand() % nc + 1;
        removeRelated(seed, k);
    }
    else if (type == 2) {
        removeRandom(k);   // ⭐ 新增
    }
}

void repair2()
{
    vector<int> unserved;

    for (int c = 1; c <= nc; c++)
        if (!curServed[c])
            unserved.push_back(c);

    // ⭐ 随机顺序（很关键）
    random_shuffle(unserved.begin(), unserved.end());

    for (int c : unserved)
    {
        bool inserted = false;

        // ⭐ 随机车辆顺序（避免固定结构）
        vector<int> orderV(nt);
        for (int i = 0; i < nt; i++) orderV[i] = i;
        random_shuffle(orderV.begin(), orderV.end());

        for (int v1 : orderV)
        {
            for (int v2 : orderV)
            {
                auto baseN1 = curRoutes[v1].nodes;
                auto baseT1 = curRoutes[v1].startTime;
                auto baseN2 = curRoutes[v2].nodes;
                auto baseT2 = curRoutes[v2].startTime;

                for (int p1 = 1; p1 < baseN1.size(); p1++)
                {
                    vector<int> n1;
                    vector<float> t1;

                    if (!insertSingle(baseN1, baseT1, p1, c, v1, n1, t1))
                        continue;

                    for (int p2 = 1; p2 < baseN2.size(); p2++)
                    {
                        if (v1 == v2 && p2 == p1) continue;

                        vector<int> n2;
                        vector<float> t2;

                        if (!insertSingle(baseN2, baseT2, p2, c + nc, v2, n2, t2))
                            continue;

                        // ===== 收益检查 =====
                        float iv = fabs(t2[p2] - t1[p1]);

                        bool ok = false;
                        for (int m = 0; m < nm; m++)
                        {
                            if (iv >= intervalLow[c][m] &&
                                iv <= intervalHigh[c][m])
                            {
                                ok = true;
                                break;
                            }
                        }

                        if (!ok) continue;

                        // ===== ⭐ 找到一个就插 =====
                        curRoutes[v1].nodes = n1;
                        curRoutes[v1].startTime = t1;

                        curRoutes[v2].nodes = n2;
                        curRoutes[v2].startTime = t2;

                        curServed[c] = true;

                        inserted = true;
                        break;
                    }

                    if (inserted) break;
                }

                if (inserted) break;
            }

            if (inserted) break;
        }
    }

    rebuildAllRouteTime();
}


void repair1() {

    for (int c = 1; c <= nc; c++) {

        if (curServed[c]) continue;

        double bestGain = -1e18;
        int bestV1 = -1, bestV2 = -1;
        vector<int> bn1, bn2;
        vector<float> bt1, bt2;

        for (int v1 = 0; v1 < nt; v1++) {
            for (int v2 = 0; v2 < nt; v2++) {

                vector<int> n1, n2;
                vector<float> t1, t2;
                double g;

                if (insertTwoNodesDiffRoute(c, v1, v2, n1, t1, n2, t2, g)) {

                    if (g > bestGain) {
                        bestGain = g;
                        bestV1 = v1;
                        bestV2 = v2;
                        bn1 = n1;
                        bt1 = t1;
                        bn2 = n2;
                        bt2 = t2;
                    }
                }
            }
        }

        if (bestV1 != -1) {

            curServed[c] = true;

            curRoutes[bestV1].nodes = bn1;
            curRoutes[bestV1].startTime = bt1;

            curRoutes[bestV2].nodes = bn2;
            curRoutes[bestV2].startTime = bt2;
        }
    }

    recompute();
}
void repair()
{
    vector<int> unserved;

    for (int c = 1; c <= nc; c++)
        if (!curServed[c])
            unserved.push_back(c);

    // ⭐ baseline：可以排序（不随机也行）
    sort(unserved.begin(), unserved.end());

    for (int c : unserved)
    {
        bool inserted = false;

        // ===== baseline：简单顺序扫描车辆 =====
        for (int v1 = 0; v1 < nt && !inserted; v1++)
        {
            for (int v2 = 0; v2 < nt && !inserted; v2++)
            {
                auto baseN1 = curRoutes[v1].nodes;
                auto baseT1 = curRoutes[v1].startTime;
                auto baseN2 = curRoutes[v2].nodes;
                auto baseT2 = curRoutes[v2].startTime;

                // ===== 只找 first feasible =====
                for (int p1 = 1; p1 < baseN1.size() && !inserted; p1++)
                {
                    vector<int> n1;
                    vector<float> t1;

                    if (!insertSingle(baseN1, baseT1, p1, c, v1, n1, t1))
                        continue;

                    for (int p2 = 1; p2 < baseN2.size(); p2++)
                    {
                        if (v1 == v2 && p2 == p1) continue;

                        vector<int> n2;
                        vector<float> t2;

                        if (!insertSingle(baseN2, baseT2, p2, c + nc, v2, n2, t2))
                            continue;

                        // ===== interval check =====
                        float iv = fabs(t2[p2] - t1[p1]);

                        bool ok = false;
                        for (int m = 0; m < nm; m++)
                        {
                            if (iv >= intervalLow[c][m] &&
                                iv < intervalHigh[c][m])
                            {
                                ok = true;
                                break;
                            }
                        }

                        if (!ok) continue;

                        // ===== baseline核心：直接接受第一个可行 =====
                        curRoutes[v1].nodes = n1;
                        curRoutes[v1].startTime = t1;

                        curRoutes[v2].nodes = n2;
                        curRoutes[v2].startTime = t2;

                        curServed[c] = true;

                        inserted = true;
                        break;
                    }
                }
            }
        }
    }

    rebuildAllRouteTime();
}


void Output(string strSolutionFile) {
    ofstream f(strSolutionFile);

    double total = getTotalProfit();

    f << "================================================\n";
    f << "                ILS Solution\n";
    f << "================================================\n";
    f << "Total Profit: " << fixed << setprecision(2) << total << "\n\n";

    for (int k = 0; k < nt; k++) {

        auto& r = routes[k];

        f << "Route " << k + 1 << " Profit: "
            << fixed << setprecision(2) << r.totalProfit << "\n";

        f << "Nodes: ";
        for (int x : r.nodes) f << x << " -> ";
        f << "END\n";

        f << "Start Time: ";
        for (float t : r.startTime)
            f << fixed << setprecision(1) << t << " ";
        f << "\n";

        f << "-----------------------------\n";
    }

    // ===== 客户级别（跨路径） =====
    f << "\n=========== Customer Detail ===========\n";

    for (int c = 1; c <= nc; c++) {

        if (!bestServed[c]) continue;

        float t1 = -1, t2 = -1;
        int v1 = -1, v2 = -1;

        for (int v = 0; v < nt; v++) {
            auto& r = routes[v];

            for (int i = 0; i < r.nodes.size(); i++) {
                if (r.nodes[i] == c) {
                    t1 = r.startTime[i];
                    v1 = v;
                }
                if (r.nodes[i] == c + nc) {
                    t2 = r.startTime[i];
                    v2 = v;
                }
            }
        }

        if (t1 < 0 || t2 < 0) continue;

        float iv = fabs(t2 - t1);

        double pf = 0;
        for (int m = 0; m < nm; m++) {
            if (iv >= intervalLow[c][m] && iv < intervalHigh[c][m]) {
                pf = profit[c][m];
                break;
            }
        }

        f << "Customer " << c
            << " | Route1=" << v1 + 1
            << " | Route2=" << v2 + 1
            << " | Interval=" << fixed << setprecision(1) << iv
            << " | Profit=" << pf << "\n";
    }

    f.close();

    cout << "ILS 完成！Total Profit = " << total << endl;
}


void ILS(vector<double>& bestCurve) {

    initialSolution();
    PrintInitialSolution();

    bestTotal = 0;
    for (auto& r : curRoutes) bestTotal += r.totalProfit;
    rebuildAllRouteTime();
    recompute();
    bestRoutes = curRoutes;
    bestServed = curServed;

    clock_t startTime1 = clock();
    for (int iter = 0; iter < 1000; iter++) {
        int op = rand() % 3;
        int k = max(4, (int)(nc * 0.1));  // 移除10%


        destroy(op, k);
        rebuildAllRouteTime();   // ⭐ 新增
        repair();
        rebuildAllRouteTime();   // ⭐ repair后也要再修一次
        recompute();

        double cur = 0;
        for (auto& r : curRoutes)
            cur += r.totalProfit;

        // ================= 每次迭代打印 =================
        
        //cout << "\n================ ITER " << iter + 1 << " ================\n";
        //cout << "Operator: " << op << "  Remove k=" << k << "\n";
        //cout << "Current Profit: " << cur << "\n";
        //cout << "Best Profit: " << bestTotal << "\n";9

        // 打印当前解（简化版）
        /*for (int r = 0; r < nt; r++) {

            cout << "Route " << r + 1 << ": ";
            for (int x : curRoutes[r].nodes)
                cout << x << " ";
            cout << "\n";
        }*/

        // ================= 更新最优解 =================
        if (cur > bestTotal) {
            bestTotal = cur;
            bestRoutes = curRoutes;
            bestServed = curServed;

        }
        double curTime = (double)(clock() - startTime1) / CLOCKS_PER_SEC;

        bestCurve.push_back(bestTotal);

    }
}



void Solve(string fn, vector<double>& bestCurve) {
    ILS(bestCurve);

}

// ================= INPUT =================
void Input_Data(string strProblemInstace) {

    fstream f(strProblemInstace);
    string s;
    getline(f, s); getline(f, s); getline(f, s);
    f >> nc >> nt >> nm;

    node = new Node[2 * nc + 2];
    getline(f, s); getline(f, s);
    for (int i = 0; i < 2 * nc + 2; i++)
        f >> node[i].id >> node[i].x >> node[i].y;

    buildDistAndTime();

    profit = new double* [nc + 1];
    for (int i = 0; i <= nc; i++)
        profit[i] = new double[nm];

    getline(f, s); getline(f, s);
    for (int i = 1; i <= nc; i++)
        for (int m = 0; m < nm; m++)
            f >> profit[i][m];

    twStart = new float* [2 * nc + 2];
    twEnd = new float* [2 * nc + 2];
    for (int i = 0; i < 2 * nc + 2; i++) {
        twStart[i] = new float[nt];
        twEnd[i] = new float[nt];
    }

    getline(f, s); getline(f, s);
    for (int i = 1; i <= nc; i++)
        for (int k = 0; k < nt; k++) {
            f >> twStart[i][k];
            twStart[i + nc][k] = twStart[i][k];
        }

    getline(f, s); getline(f, s);
    for (int i = 1; i <= nc; i++)
        for (int k = 0; k < nt; k++) {
            f >> twEnd[i][k];
            twEnd[i + nc][k] = twEnd[i][k];
        }

    for (int k = 0; k < nt; k++) {
        twStart[0][k] = 0;
        twEnd[0][k] = 86400;
        twStart[2 * nc + 1][k] = 0;
        twEnd[2 * nc + 1][k] = 86400;
    }

    service = new float[2 * nc + 2]();
    getline(f, s); getline(f, s);
    for (int i = 1; i <= nc; i++) {
        f >> service[i];
        service[i + nc] = service[i];
    }

    intervalLow = new float* [nc + 1];
    intervalHigh = new float* [nc + 1];
    for (int i = 0; i <= nc; i++) {
        intervalLow[i] = new float[nm];
        intervalHigh[i] = new float[nm];
    }

    getline(f, s); getline(f, s);
    for (int i = 1; i <= nc; i++)
        for (int m = 0; m < nm; m++)
            f >> intervalLow[i][m];

    getline(f, s); getline(f, s);
    for (int i = 1; i <= nc; i++)
        for (int m = 0; m < nm; m++)
            f >> intervalHigh[i][m];

    cout << "数据读取完成\n";
}

void CheckSolution()
{
    cout << "\n========== CHECK SOLUTION ==========\n";

    vector<int> visitCount(nc + 1, 0);
    double recomputedTotal = 0;

    for (int v = 0; v < nt; v++)
    {
        auto& r = routes[v];

        cout << "\n[Route " << v + 1 << "]\n";

        for (int i = 0; i < r.nodes.size(); i++)
        {
            int u = r.nodes[i];
            float st = r.startTime[i];

            // ================= 时间窗 =================
            if (st < twStart[u][v] - 1e-6 || st > twEnd[u][v] + 1e-6)
            {
                cout << "❌ 时间窗违规: node " << u
                    << " time=" << st
                    << " TW=[" << twStart[u][v]
                    << "," << twEnd[u][v] << "]\n";
            }

            // ================= 路径可行性（核心） =================
            if (i > 0)
            {
                int prev = r.nodes[i - 1];

                float expected = r.startTime[i - 1]
                    + service[prev]
                    + timeMat[prev][u];

                if (st < expected - 1e-6)
                {
                    cout << "❌ 路径不可行: "
                        << prev << " -> " << u
                        << " expected=" << expected
                        << " actual=" << st << "\n";
                }
            }

            // ================= 访问计数 =================
            if (u >= 1 && u <= nc)
                visitCount[u]++;
            if (u > nc && u <= 2 * nc)
                visitCount[u - nc]++;
        }
    }

    // ================= 客户完整性 =================
    for (int c = 1; c <= nc; c++)
    {
        if (!bestServed[c]) continue;

        if (visitCount[c] != 2)
        {
            cout << "❌ 客户 " << c
                << " 访问次数错误: "
                << visitCount[c] << " (应为2)\n";
        }
    }

    // ================= 收益重算 =================
    cout << "\n[Profit Check]\n";

    for (int c = 1; c <= nc; c++)
    {
        if (!bestServed[c]) continue;

        float t1 = -1, t2 = -1;

        for (int v = 0; v < nt; v++)
        {
            auto& r = routes[v];

            for (int i = 0; i < r.nodes.size(); i++)
            {
                if (r.nodes[i] == c) t1 = r.startTime[i];
                if (r.nodes[i] == c + nc) t2 = r.startTime[i];
            }
        }

        if (t1 < 0 || t2 < 0)
        {
            cout << "❌ 客户 " << c << " 缺少访问\n";
            continue;
        }

        float iv = fabs(t2 - t1);

        double pf = 0;
        bool found = false;

        for (int m = 0; m < nm; m++)
        {
            if (iv >= intervalLow[c][m] && iv < intervalHigh[c][m])
            {
                pf = profit[c][m];
                found = true;
                break;
            }
        }

        if (!found)
        {
            cout << "❌ 客户 " << c << " interval 无收益区间 iv=" << iv << "\n";
        }

        recomputedTotal += pf;
    }

    double algoTotal = getTotalProfit();

    cout << "\n[Total Profit]\n";
    cout << "算法输出: " << algoTotal << endl;
    cout << "重新计算: " << recomputedTotal << endl;

    if (fabs(algoTotal - recomputedTotal) > 1e-6)
        cout << "❌ 收益不一致！\n";
    else
        cout << "✅ 收益一致\n";

    cout << "====================================\n";
}


void RunExperimentToSummary(ofstream& summary,
    ofstream& workbook,   // ⭐ 新增：汇总表
    string instanceName,
    string instanceFile,
    int runs = 10)
{
    vector<double> objs;
    vector<double> times;
    Input_Data(instanceFile);
    vector<vector<double>> allBestCurves;
    for (int r = 0; r < runs; r++)
    {

        clock_t start = clock();
        vector<double> bestCurve;

        Solve(instanceFile, bestCurve);
        allBestCurves.push_back(bestCurve);
        clock_t end = clock();

        double t = (double)(end - start) / CLOCKS_PER_SEC;
        double obj = bestTotal;

        objs.push_back(obj);
        times.push_back(t);

        // ===== 明细表 =====
        summary << instanceName << ","
            << r + 1 << ","
            << obj << ","
            << t << "\n";
    }

    int T = allBestCurves[0].size();

    ofstream fcurve("convergence_ILS_avg.csv");
    fcurve << "Iter,BestAvg\n";

    for (int i = 0; i < T; i++) {

        double sum = 0;

        for (int r = 0; r < allBestCurves.size(); r++)
            sum += allBestCurves[r][i];

        fcurve << i + 1 << "," << sum / allBestCurves.size() << "\n";
    }

    fcurve.close();



    // ===== 统计 =====
// ===== 统计 =====
    double sumObj = 0, sumTime = 0;
    double bestObj = -1e18;

    for (int i = 0; i < runs; i++)
    {
        sumObj += objs[i];
        sumTime += times[i];
        bestObj = max(bestObj, objs[i]);
    }

    double meanObj = sumObj / runs;
    double meanTime = sumTime / runs;

    // ===== ⭐ 标准差计算 =====
    double varObj = 0;
    double varTime = 0;

    for (int i = 0; i < runs; i++)
    {
        varObj += (objs[i] - meanObj) * (objs[i] - meanObj);
        varTime += (times[i] - meanTime) * (times[i] - meanTime);
    }

    double stdObj = sqrt(varObj / (runs - 1));
    double stdTime = sqrt(varTime / (runs - 1));


    // ===== 明细表补充 =====
    summary << instanceName << ",Mean,"
        << meanObj << ","
        << stdObj << ","
        << meanTime << ","
        << stdTime << "\n";


    summary << instanceName << ",Best,"
        << bestObj << ",-\n";

    // ===== ⭐ 工作簿（核心）=====
    workbook << instanceName << ","
        << bestObj << ","
        << meanObj << ","
        << stdObj << ","
        << meanTime << ","
        << stdTime << "\n";

}


int main(int argc, char** argv)
{
    srand(time(NULL));

    ofstream summary("summary.csv");     // 明细
    ofstream workbook("workbook.csv");   // ⭐ 汇总

    // 表头
    summary << "Instance,Run,Obj,Time\n";
    summary << "Instance,Stat,MeanObj,StdObj,MeanTime,StdTime\n";

    workbook << "Instance,BestObj,MeanObj,StdObj,MeanTime,StdTime\n";


    char ss[100];

    for (int i = 50; i <= 50; i += 50)
    {
        for (int k = 6; k <= 6; k += 2)
        {
            for (int p = 3; p <= 3; p++)
            {
                sprintf_s(ss, "%d_%d_%d", i, k, p);
                string instanceName = ss;

                sprintf_s(ss, "Problem Instances\\%d_%d_%d.txt", i, k, p);
                string instanceFile = ss;

                cout << "\n=============================\n";
                cout << "Instance: " << instanceName << endl;
                cout << "=============================\n";

                RunExperimentToSummary(summary, workbook,
                    instanceName, instanceFile, 10);
            }
        }
    }

    summary.close();
    workbook.close();

    cout << "\n✅ 明细已输出 summary.csv";
    cout << "\n✅ 汇总已输出 workbook.csv\n";

    return 0;
}
