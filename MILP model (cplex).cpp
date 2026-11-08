#include <ilcplex/ilocplex.h>//调用cplex的头文件    注：配置好环境后，此头文件就不报错了
#include "stdafx.h"//vc生成工程时生成的用户头文件，属于工程的一部分，是vc工程的预编译头文件，包含工程的每个文件都要包含，用于加速编译速度。
#include<iostream>//iostream库，输入输出流
#include <windows.h>


#include<iomanip>//IO流控制头文件，主要是对cin，cout，等一些操纵运算子，比如，setfill，setw
#include <fstream>//file stream 文件流，对文件操作的合集，包含了所有文件操作
#include<string>//c风格字符串相关函数的头文件，支持string类型，可以像处理普通变量一样处理字符串，区别于字符数组
#include <cstdlib>//函数库，等价于stdlib.h
#include <sstream>//string stream，用来左数据转换

using  std::cout;//std是一个类（输入输出标准），包括了cin成员和cout成员，using namespace std以后才能使用它的成员
using  std::endl;
using  std::string;//字符串
using  std::ostringstream;//流的输出

using namespace std;


/////用typedef在函数体外定义全局变量，用于整个文件TOP.cpp，从而扩大作用域（区别于定义在函数体内的局部变量）
typedef IloArray<IloNumVarArray> NumVarMatrix;
typedef IloArray<NumVarMatrix> NumVar3Matrix;
typedef IloArray<IloNumArray> NumMatrix;
#define speed 20 //行驶速度，常变量


/////定义node类，将编号，横纵坐标
ILOSTLBEGIN
class Node
{
public:
	int Node_No;
	float Node_X;
	float Node_Y;
public:
	Node()
	{
		Node_No = -1;
		Node_X = -1;
		Node_Y = -1;
	}
};


///行驶成本（矩阵形式）
void Get_Distance_Matrix(Node* nod, float** Distance_Matrix, int nc)//括号内参数为（所有节点数包括起始点，成本，节点数量）
{
	for (int I = 0; I < 2 * nc + 2; I++)
	{
		for (int J = 0; J < 2 * nc + 2; J++)
		{
			if (I == J)
			{
				Distance_Matrix[I][J] = 1.0e5;//同一点的距离设为足够大，以使其不发生
				cout << setiosflags(ios::left) << setw(8) << Distance_Matrix[I][J];//以左对齐，宽度为8的形式显示最终成本矩阵
			}
			else
			{
				float X1 = nod[I].Node_X;//变量x1是i点的横坐标
				float Y1 = nod[I].Node_Y;//变量y1是i点的纵坐标
				float X2 = nod[J].Node_X;
				float Y2 = nod[J].Node_Y;
				Distance_Matrix[I][J] = sqrt((X1 - X2) * (X1 - X2) + (Y1 - Y2) * (Y1 - Y2));//用坐标位置计算两点间成本
				cout << setiosflags(ios::left) << setw(12) << Distance_Matrix[I][J];	//以左对齐，宽度为12的形式显示最终成本矩阵
			}
		}
		cout << endl;
	}
}

///行驶时间函数（矩阵形式）
void Get_Time_Matrix(float** Time_Matrix, float** Distance_Matrix, int nc)//括号内参数为（时间矩阵，成本，节点数量）
{
	for (int I = 0; I < 2 * nc + 2; I++)
	{
		for (int J = 0; J < 2 * nc + 2; J++)
		{
			if (I == J)
				Time_Matrix[I][J] = Distance_Matrix[I][J];//同上，意思是同一点的时间设为足够大，以使其不发生
			else
				Time_Matrix[I][J] = Distance_Matrix[I][J] / speed * 3600;//计算行驶时间，speed为速度，之前已将其定义为常变量
			cout << setiosflags(ios::left) << setw(12) << Time_Matrix[I][J];//以左对齐，宽度为12的形式显示最终时间矩阵
		}
		cout << endl;
	}
}



//////（cplex模型导入部分）
//定义cplex_solve_cut函数，返回值是float形式，括号内为参数（表达式，节点数量，路线数量，参数1表示足够大的正整数，节点i完成访问的收益（二维），节点i的执行时间（一维），间隔时间方案，时间窗最早时间（二维），时间窗最晚时间（二维），三维决策变量路线方案，二维决策变量收益方案，上界，开始执行时间方案，结果，计时）
float Cplex_Solve_Cut(IloEnv env, int taskNum, int routeNum, int timeNum, float parameter1, float** p, float* duration_time, float** ws, float** we, float** Time_MX, Node* nod, float** a, float** b, DWORD dwStart, string strSolutionFile)
{
	//////////定义几个整数常量
	IloInt Node_Number = taskNum;                 //整数变量，实际任务节点
	IloInt All_Node_Number = taskNum * 2 + 2;     //整数变量，总节点数量为2nc（2*实际任务节点个数）+2（起点，终点）
	IloInt Tour_Number = routeNum;                 //整数变量，路线数量为nt
	IloInt Time_Number = timeNum;                 //整数变量，时间段数量为nm

	/////////定义四个决策变量
	typedef IloArray<IloNumVarArray> NumVarMatrix; //数组（数组）为二维矩阵
	typedef IloArray<NumVarMatrix> NumVar3Matrix;//数组（二维矩阵）为三维矩阵
	NumVar3Matrix Route_Vars(env, All_Node_Number);   //定义三维矩阵，路径变量（节点数量）
	for (int i = 0; i < All_Node_Number; i++)       //第一步i循环，得到二维矩阵，即路径变量（env，节点数量）
	{
		Route_Vars[i] = NumVarMatrix(env, All_Node_Number);
		for (int j = 0; j < All_Node_Number; j++)   //第二步j循环，得到一维数组，即路径变量（env，路线数量）
		{
			Route_Vars[i][j] = IloNumVarArray(env, Tour_Number);
			for (int t = 0; t < Tour_Number; t++)  //第三步t循环，得到三维路径决策变量，Route_Vars[i][j][t]
			{
				Route_Vars[i][j][t] = IloNumVar(env, 0, 1, ILOBOOL);//0到1的布尔型决策变量    注：其中，ILOBOOL为布尔型变量，ILOINT为整数型变量，ILOFLOAT为连续性变量
			}
		}
	}

	NumVarMatrix  Profit_Vars(env, Node_Number + 1);//定义二维矩阵，收益变量（节点数量）
	for (int i = 0; i <= Node_Number; i++)//第一步i循环，得到一维数组，即收益变量（时间段数量）
	{
		Profit_Vars[i] = IloNumVarArray(env, Time_Number);
		for (int m = 0; m < Time_Number; m++)//第二步m循环，得到收益决策变量
		{
			Profit_Vars[i][m] = IloNumVar(env, 0, 1, ILOBOOL);//0到1的布尔型决策变量
		}
	}

	////三、下面定义二维的开始时间变量
	NumVarMatrix  STime_Vars(env, All_Node_Number);//定义二维矩阵，开始时间变量（节点数量）
	for (int i = 0; i < All_Node_Number; i++)//第一步i循环，得到一维数组，即开始时间变量（路线数量）
	{
		STime_Vars[i] = IloNumVarArray(env, Tour_Number);
		for (int t = 0; t < Tour_Number; t++)//第二步t循环，得到开始时间决策变量
		{
			STime_Vars[i][t] = IloNumVar(env, 0.0, 86400.0, ILOFLOAT);//0到24的连续型变量
		}
	}

	////四、下面定义一维的间隔时间变量
	IloNumVarArray W_Vars(env, Node_Number + 1);
	for (int i = 0; i <= Node_Number; i++)//第一步i循环，得到一维数组，即间隔时间变量（节点数量）
	{
		W_Vars[i] = IloNumVar(env, 0.0, 57600.0, ILOFLOAT);//0到12的连续型变量
	}


	///////四个决策变量定义完毕，下面开始建模！
	IloModel Cplex_Solve(env);           //定义模型Cplex_Solve(env)，之后逐项往该模型中添加约束式子

	/////一、添加(1)式
	IloExpr Total_Profit(env);       //定义总收益Total_Profit
	for (int i = 1; i <= Node_Number; i++)//
	{
		for (int m = 0; m < Time_Number; m++)
			Total_Profit += Profit_Vars[i][m] * p[i][m];//收益决策变量*单位收益（给定），总收益等于所有值求和 
	}

	Cplex_Solve.add(IloMaximize(env, Total_Profit)); // 添加(1)式目标函数（收益最大值），即将cplex的(1)式转化为c++调用语言，从而成功调用(1)式
	//Cplex_Solve.add(Total_Profit>=106);
	Total_Profit.end();  //关闭函数


	/////二、添加(2)式
	for (int t = 0; t < Tour_Number; t++)
	{
		IloExpr X2(env);
		for (int i = 1; i < All_Node_Number - 1; i++)
		{
			X2 += Route_Vars[0][i][t];
		}
		Cplex_Solve.add(X2 == 1);//添加(2)式，任何一条路线都从0（起点）出发
		X2.end();
	}
	for (int t = 0; t < Tour_Number; t++)
	{
		for (int i = 1; i < All_Node_Number - 1; i++)
		{
			IloExpr X2(env);
			X2 = Route_Vars[i][i][t];
			Cplex_Solve.add(X2 == 0);
			X2.end();
		}
	}
	// cout<<"(2)式添加成功！"<<endl;

/////三、添加(3)式
	for (int t = 0; t < Tour_Number; t++)
	{
		IloExpr X3(env);
		for (int i = 1; i < All_Node_Number - 1; i++)
		{
			X3 += Route_Vars[i][All_Node_Number - 1][t];
		}
		Cplex_Solve.add(X3 == 1);//添加(3)式，任何一条路线回到All_Node_Number - 1（终点）
		X3.end();
	}
	// cout<<"(3)式添加成功！"<<endl;

/////四、添加(4)式
	for (int k = 1; k < All_Node_Number - 1; k++)  //需要去掉两个点（起点和终点）！
	{
		for (int t = 0; t < Tour_Number; t++)
		{
			IloExpr X4(env);
			for (int i = 0; i < All_Node_Number - 1; i++)
			{
				if (i != k)
					X4 += Route_Vars[i][k][t];
			}
			IloExpr X5(env);
			for (int i = 1; i < All_Node_Number; i++)
			{
				if (i != k)
					X5 += Route_Vars[k][i][t];
			}
			Cplex_Solve.add(X4 == X5);//添加第(4)个式子，车流量平衡约束
			X4.end();
			X5.end();
		}
	}
	// cout<<"（4）式添加成功！"<<endl;

/////五、添加(5)式
	for (int j = 1; j < All_Node_Number - 1; j++)//需要去掉两个点（起点和终点）！
	{
		IloExpr X5(env);
		for (int i = 0; i < All_Node_Number - 1; i++)
		{
			for (int t = 0; t < Tour_Number; t++)
			{
				X5 += Route_Vars[i][j][t];
			}
		}
		Cplex_Solve.add(X5 <= 1);//添加第(5)个式子，一次访问约束
		X5.end();
	}
	// cout<<"（5）式添加成功！"<<endl;

/////六、添加(6)式
	for (int j = 1; j <= Node_Number; j++)
	{
		IloExpr X6(env);
		for (int i = 0; i < All_Node_Number - 1; i++)
		{
			for (int t = 0; t < Tour_Number; t++)
			{
				X6 += Route_Vars[i][j][t] - Route_Vars[i][j + Node_Number][t];//添加第(6)个式子，(i+n)节点的访问一定是在i节点访问之后。
			}
		}
		Cplex_Solve.add(X6 == 0);
		X6.end();
	}
	// cout<<"（6）式添加成功！"<<endl;

/////七、添加(7)式
	for (int j = 1; j <= Node_Number; j++)
	{
		for (int m = 0; m < Time_Number; m++)
		{
			IloExpr X7(env);
			for (int i = 0; i < All_Node_Number - 1; i++)
			{
				for (int t = 0; t < Tour_Number; t++)
				{
					X7 += Route_Vars[i][j][t] + Route_Vars[i][j + Node_Number][t];
				}
			}
			Cplex_Solve.add(Profit_Vars[j][m] <= 0.5 * X7);//添加第（7）个式子，建立两个决策变量的关系
			X7.end();
		}
	}


	/////八、添加(8)式
	for (int i = 1; i <= Node_Number; i++)
	{
		IloExpr X8(env);
		for (int m = 0; m < Time_Number; m++)
		{
			X8 += Profit_Vars[i][m];
		}
		Cplex_Solve.add(X8 <= 1);//添加第（8）个式子，只能在一个间隔时间段完成两次访问
		X8.end();
	}

	/////九、添加（9）式
	for (int i = 0; i < All_Node_Number - 1; i++)
	{
		for (int j = 1; j < All_Node_Number; j++)
		{
			for (int t = 0; t < Tour_Number; t++)
			{
				if (i != j)
				{
					IloExpr X9(env);
					X9 += STime_Vars[i][t] + duration_time[i] + Time_MX[i][j] - STime_Vars[j][t] - parameter1 * (1 - Route_Vars[i][j][t]);//开始时间+执行时间+行驶时间-节点j的开始时间-100000*（1-路径决策变量）
					Cplex_Solve.add(X9 <= 0);//添加第（9）个式子，时间先后衔接约束
					X9.end();
				}
			}
		}
	}


	for (int i = 1; i < All_Node_Number - 1; i++)
	{
		for (int t = 0; t < Tour_Number; t++)
		{
			if (ws[i][t] == 0)
			{
				for (int j = 1; j < All_Node_Number; j++)
				{
					IloExpr X10(env);
					X10 += Route_Vars[i][j][t];
					Cplex_Solve.add(Route_Vars[i][j][t] == 0);
					X10.end();
				}
			}
		}
	}

	/////十、添加(10）式
	for (int i = 1; i < All_Node_Number - 1; i++)
	{
		for (int t = 0; t < Tour_Number; t++)
		{
			IloExpr X10(env);
			for (int j = 1; j < All_Node_Number; j++)
			{
				if (i != j)
				{
					X10 += Route_Vars[i][j][t];
				}
			}
			Cplex_Solve.add(STime_Vars[i][t] >= ws[i][t] * X10);//添加第（10）个式子，开始时间大于时间窗的最早开始时间		
			X10.end();
		}
	}


	/////十一、添加(11）式
	for (int i = 1; i < All_Node_Number - 1; i++)
	{
		for (int t = 0; t < Tour_Number; t++)
		{
			IloExpr X11(env);
			for (int j = 1; j < All_Node_Number; j++)
			{
				if (i != j)
				{
					X11 += Route_Vars[i][j][t];
				}
			}
			Cplex_Solve.add(STime_Vars[i][t] <= (we[i][t] - duration_time[i]) * X11); //添加第（11）个式子，开始时间小于时间窗的最晚开始时间减去任务执行时间
			X11.end();
		}
	}
	// cout<<"（11）式添加成功！"<<endl;

/////十二、添加(12）式
	for (int i = 1; i <= Node_Number; i++)
	{
		IloExpr X12(env);
		for (int t = 0; t < Tour_Number; t++)
		{
			X12 += STime_Vars[i + Node_Number][t] - STime_Vars[i][t];
		}
		Cplex_Solve.add(X12 == W_Vars[i]);//添加第（12）个式子，间隔时间等于两次访问的开始时间差值
		X12.end();
	}


	/////十三、添加(13）式
	for (int i = 1; i <= Node_Number; i++)
	{
		for (int m = 0; m < Time_Number; m++)
		{
			IloExpr X13(env);
			X13 += a[i][m] * Profit_Vars[i][m];//间隔时间-某一时间段的开始时间*收益决策变量		
			Cplex_Solve.add(W_Vars[i] >= X13);//添加第（13）个式子，间隔时间大于某一时间段的开始时间
			X13.end();
		}
	}


	/////十四、添加(14）式
	for (int i = 1; i <= Node_Number; i++)
	{
		for (int m = 0; m < Time_Number; m++)
		{
			IloExpr X14(env);
			X14 += b[i][m] + parameter1 * (1 - Profit_Vars[i][m]);//间隔时间-某一时间段的结束时间*收益决策变量-L*（1-收益决策变量）
			Cplex_Solve.add(W_Vars[i] <= X14);//添加第（14）个式子，间隔时间小于某一时间段的结束时间
			Cplex_Solve.add(W_Vars[i] != X14);
			X14.end();
		}
	}


	//////求解器
	IloCplex Cplex_Solver(Cplex_Solve);//定义求解器，调用Cplex_Solve模型
	Cplex_Solver.setParam(IloCplex::TiLim, 10800);//给定求解时间上限
	cout << "求解开始" << endl;
	Cplex_Solver.solve();//求解器开始求解
	cout << "求解成功" << endl;

	DWORD dwEnd = GetTickCount();//取结束时间
	cout << "总耗时：" << dwEnd - dwStart << endl;//显示总耗时：

	int Obj_Value = Cplex_Solver.getObjValue();//目标函数值变量定义
	cout << "Obj_Value=" << Obj_Value << endl;//显示目标解

	//////////////////////////在显示各个解	
	//显示收益解
	for (int i = 1; i <= Node_Number; i++)
	{
		for (int m = 0; m < Time_Number; m++)
		{
			cout << setw(10) << Cplex_Solver.getValue(Profit_Vars[i][m]);
		}
		cout << endl;
	}

	cout << endl;//换行
	//显示开始时间解
	for (int i = 1; i <= 2 * Node_Number; i++)
	{
		for (int t = 0; t < Tour_Number; t++)
		{
			cout << setw(10) << Cplex_Solver.getValue(STime_Vars[i][t]);
		}
		cout << endl;
	}

	cout << endl;//换行
	//显示间隔时间解
	for (int i = 1; i <= Node_Number; i++)
	{
		cout << setw(10) << Cplex_Solver.getValue(W_Vars[i]);
	}

	for (int t = 0; t < Tour_Number; t++)
	{
		cout << "路线" << t + 1 << endl;
		int i = 0;
		cout << "\t" << i;
		while (i < All_Node_Number - 1)
		{
			for (int j = 1; j < All_Node_Number; j++)
			{
				if (Cplex_Solver.getValue(Route_Vars[i][j][t]) != 0)
				{
					cout << "\t" << j;
					i = j;
				}
			}
		}
		cout << endl;
		cout << endl;
	}
	cout << endl;


	/////文件输出流（把结果按以下形式输出到文件result.txt文本文档中）
	ofstream outFile0(strSolutionFile.c_str());//打开输出文件，按以下内容输出
	outFile0 << "总耗时:" << '\t' << dwEnd - dwStart << endl;//总耗时
	outFile0 << "Cplex_Solver.getObjValue=" << Cplex_Solver.getObjValue() << endl;//目标值
	outFile0 << "Cplex_Solver.getBestObjValue=" << Cplex_Solver.getBestObjValue() << endl;//最优解
	outFile0 << "Cplex_Solver.getCplexTime=" << Cplex_Solver.getCplexTime() << endl;//运行时间
	outFile0 << "Cplex_Solver.getNodes=" << Cplex_Solver.getNnodes() << endl;//节点数
	outFile0 << "solution status is " << Cplex_Solver.getStatus() << endl;//当前解的状态是（最优解还是..）
	outFile0 << "Cplex_Solver.getNodesLeft=" << Cplex_Solver.getNnodesLeft() << endl;//剩余节点数
	outFile0 << "Cplex_Solver.getIncumbentNode=" << Cplex_Solver.getIncumbentNode() << endl;//下界
	outFile0 << "Cplex_Solver.iterations=" << Cplex_Solver.getNiterations() << endl;//迭代次数


	for (int t = 0; t < Tour_Number; t++)
	{
		outFile0 << "路线" << t + 1 << endl;
		int i = 0;
		outFile0 << "\t" << i;
		while (i < All_Node_Number - 1)
		{
			for (int j = 1; j < All_Node_Number; j++)
			{
				if (Cplex_Solver.getValue(Route_Vars[i][j][t]) != 0)
				{
					outFile0 << "\t" << j;
					i = j;
				}
			}
		}
		outFile0 << endl;
		outFile0 << endl;
	}
	outFile0 << endl;

	outFile0 << "y变量" << endl;
	for (int i = 1; i <= Node_Number; i++)
	{
		for (int m = 0; m < Time_Number; m++)
		{
			outFile0 << "\t" << Cplex_Solver.getValue(Profit_Vars[i][m]);
		}
		outFile0 << endl;
	}

	outFile0 << endl;//换行

	//显示开始时间解
	outFile0 << "开始时间" << endl;
	for (int i = 0; i < All_Node_Number; i++)
	{
		for (int t = 0; t < Tour_Number; t++)
		{
			outFile0 << "\t" << Cplex_Solver.getValue(STime_Vars[i][t]);
		}
		outFile0 << endl;
	}
	outFile0 << endl;
	outFile0 << "间隔时间" << endl;
	for (int i = 1; i <= Node_Number; i++)
	{
		outFile0 << "\t" << Cplex_Solver.getValue(W_Vars[i]);
	}

	outFile0.close();//关闭输出文件
	return Obj_Value;//返回目标值
}


/////（求解部分，定义Solve_Problem_by_Branch_Cut函数）
void Solve_Problem_by_Branch_Cut(string strProblemInstace, string strSolutionFile)
{
	/////（数据导入部分）
	std::fstream file;
	file.open(strProblemInstace, std::ios::in);
	if (!file.is_open())
	{
		cout << "The file is failed to be opened." << endl;
		return;
	}
	std::string str_temp;
	int taskNum, routeNum, timeNum;
	getline(file, str_temp);
	getline(file, str_temp);
	getline(file, str_temp);
	file >> taskNum >> routeNum >> timeNum;

	float parameter1 = 1000000;  //参数，类似于足够大的正整数

	////节点数据、行驶成本数据，行驶时间数据（都是二维数组）
	Node* nod;//定义了一个Nod新对象
	nod = new Node[2 * taskNum + 2];
	getline(file, str_temp);
	getline(file, str_temp);
	for (int i = 0; i < 2 * taskNum + 2; i++)
	{
		file >> nod[i].Node_No >> nod[i].Node_X >> nod[i].Node_Y;//给Customer类的 Cust对象输入编号，横坐标，纵坐标
		//cout << setiosflags(ios::left) << setw(8) << nod[i].Node_No << setw(8) << nod[i].Node_X << setw(8) << nod[i].Node_Y << endl;
	}

	float** Distance_MX = new float* [2 * taskNum + 2];//距离指针
	for (int i = 0; i < 2 * taskNum + 2; i++)
		Distance_MX[i] = new float[2 * taskNum + 2];
	cout << "距离矩阵如下：" << endl;
	Get_Distance_Matrix(nod, Distance_MX, taskNum);//调用Get_Distance_Matrix函数，运用node数值求各节点之间的距离

	float** Time_MX = new float* [2 * taskNum + 2];//时间指针
	for (int i = 0; i < 2 * taskNum + 2; i++)
		Time_MX[i] = new float[2 * taskNum + 2];
	cout << "时间矩阵如下：" << endl;
	Get_Time_Matrix(Time_MX, Distance_MX, taskNum);//调用Get_Time_Matrix函数，运用距离、速度数值求各节点之间的行驶时间


	/////单位收益（二维数组）
	float** p = new float* [taskNum + 1];
	for (int i = 0; i <= taskNum; i++)
		p[i] = new float[timeNum];//收益
	getline(file, str_temp);
	getline(file, str_temp);
	for (int i = 1; i <= taskNum; i++)
	{
		for (int m = 0; m < timeNum; m++)
			file >> p[i][m];
	}

	/////1时间窗最早时间（二维数组）
	float** ws = new float* [2 * taskNum + 2];//时间窗
	float** we = new float* [2 * taskNum + 2];
	for (int i = 0; i < 2 * taskNum + 2; i++)
	{
		ws[i] = new float[routeNum];
		we[i] = new float[routeNum];
	}
	for (int i = 0; i < 2 * taskNum + 2; i++)
	{
		for (int k = 0; k < routeNum; k++)
		{
			ws[i][k] = 0;
			we[i][k] = 0;
		}
	}
	getline(file, str_temp);
	getline(file, str_temp);
	for (int i = 1; i < taskNum + 1; i++)
	{
		for (int k = 0; k < routeNum; k++)
		{
			file >> ws[i][k];
			ws[i + taskNum][k] = ws[i][k];
		}
	}

	getline(file, str_temp);
	getline(file, str_temp);
	for (int i = 1; i < taskNum + 1; i++)
	{
		for (int k = 0; k < routeNum; k++)
		{
			file >> we[i][k];
			we[i + taskNum][k] = we[i][k];
		}
	}
	for (int k = 0; k < routeNum; k++)
	{
		ws[0][k] = 0;
		ws[2 * taskNum + 1][k] = 86400;
		we[0][k] = 86400;
		we[2 * taskNum + 1][k] = 86400;
	}

	/////任务执行数据（一维数组）
	float* duration_time = new float[2 * taskNum + 2];
	getline(file, str_temp);
	getline(file, str_temp);
	for (int i = 1; i < taskNum + 1; i++)
	{
		file >> duration_time[i];
		duration_time[i + taskNum] = duration_time[i];
	}
	duration_time[0] = 0;
	duration_time[2 * taskNum + 1] = 0;


	/////1时间段最早时间（二维数组）
	float** a = new float* [taskNum + 1];//时间窗
	float** b = new float* [taskNum + 1];
	for (int i = 0; i < taskNum + 1; i++)
	{
		a[i] = new float[timeNum];
		b[i] = new float[timeNum];
	}
	getline(file, str_temp);
	getline(file, str_temp);
	for (int i = 1; i < taskNum + 1; i++)
	{
		for (int m = 0; m < timeNum; m++)
		{
			file >> a[i][m];
		}
	}

	getline(file, str_temp);
	getline(file, str_temp);
	for (int i = 1; i < taskNum + 1; i++)
	{
		for (int m = 0; m < timeNum; m++)
		{
			file >> b[i][m];
			cout << setw(8) << b[i][m];
		}
		cout << endl;
	}
	cout << "需要用的数据导入完毕！" << endl;


	//计时
	DWORD dwStart = GetTickCount();

	float  Obj_Value;//定义目标函数值
	IloEnv env;//调用求解函数，进行求解，返回目标值Obj_Value
	{
		Obj_Value = Cplex_Solve_Cut(env, taskNum, routeNum, timeNum, parameter1, p, duration_time, ws, we, Time_MX, nod, a, b, dwStart, strSolutionFile);
	}
	env.end();

	//执行结束后删除以下存储位置
	delete[]nod;//节点的nod对象
	delete[]Distance_MX;//行驶距离
	delete[]Time_MX;//行驶时间
}

/////////////////主函数(此处添加的for和while循环目的是让程序自动运行下一个算例，直至求解完毕)
int main(int argc, char** argv)
{
	char  ss[100];
	for (int i = 10; i <= 10; i += 50)  //节点数
	{
		for (int k = 5; k <= 5; k += 2)  //路线数
		{
			for (int p = 2; p <= 2; p += 1)  //第几个算例
			{
				sprintf_s(ss, "Problem Instances\\%d_%d_%d.txt", i, k, p);
				string strProblemInstace = ss;
				sprintf_s(ss, "Solution results\\%d_%d_%d.txt", i, k, p);
				string strSolutionFile = ss;

				Solve_Problem_by_Branch_Cut(strProblemInstace, strSolutionFile);
			}
		}
	}
	return 0;
}
