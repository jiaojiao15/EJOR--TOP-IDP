#include <iostream>
#include"conio.h"
#include"math.h"
#include"stdlib.h"
#include"stdio.h"
#include <windows.h>
#include <sys/timeb.h>
#include<iomanip>
#include <fstream>
#include<string>
#include<ctime>
#include <cstdlib>
#include <sstream>
#include <vector>
#include <cassert>
#include <map>
#include <list>
#include<time.h>
#include"parameter.h"
#include <random>
#include <limits>
double bestTotal = -1e18;
using  std::cout;
using  std::endl;
using  std::string;
using  std::ostringstream;
using  std::vector;

using namespace std;

//以下是算例所设计的算法参数
#define speed 20 //行驶速度
#define Max_Tour 10 //最多的路线
#define Max_Node 200 //最多的节点数
#define Random(x) (rand() % x)+1 //通过取余取得指定范围的随机数
#define min(x,y) ((x)<(y)?(x):(y))//两数取小
#define max(x,y) ((x)>(y)?(x):(y))//两数取大
#define  RemoveOperatorNum 3
#define  RepairOperatorNum 2
#define  ChangeOperatorNum 2

//######################################读取数据

///行驶成本（矩阵形式）
void Get_Distance_Matrix(Node *nod, float **Distance_Matrix, int nc)//括号内参数为（所有节点数包括起始点，成本，节点数量）
{
	for (int I = 0; I < 2 * nc + 2; I++)
	{
		for (int J = 0; J < 2 * nc + 2; J++)
		{
			if (I == J)
			{
				Distance_Matrix[I][J] = 1.0e5;//同一点的距离设为足够大，以使其不发生
				//cout << setiosflags(ios::left) << setw(8) << Distance_Matrix[I][J];//以左对齐，宽度为8的形式显示最终成本矩阵
			}
			else
			{
				float X1 = nod[I].Coordinate_X;//变量x1是i点的横坐标
				float Y1 = nod[I].Coordinate_Y;//变量y1是i点的纵坐标
				float X2 = nod[J].Coordinate_X;
				float Y2 = nod[J].Coordinate_Y;
				Distance_Matrix[I][J] = sqrt((X1 - X2)*(X1 - X2) + (Y1 - Y2)*(Y1 - Y2));//用坐标位置计算两点间成本
				//cout << setiosflags(ios::left) << setw(12) << Distance_Matrix[I][J];	//以左对齐，宽度为12的形式显示最终成本矩阵
			}
		}
		//cout << endl;
	}
}
///行驶时间函数（矩阵形式）
void Get_Time_Matrix(float **Time_Matrix, float **Distance_Matrix, int nc)//括号内参数为（时间矩阵，成本，节点数量）
{
	for (int I = 0; I < 2 * nc + 2; I++)
	{
		for (int J = 0; J < 2 * nc + 2; J++)
		{
			if (I == J)
				Time_Matrix[I][J] = Distance_Matrix[I][J];//同上，意思是同一点的时间设为足够大，以使其不发生
			else
				Time_Matrix[I][J] = Distance_Matrix[I][J] / speed *3600;//计算行驶时间，speed为速度，之前已将其定义为常变量
			//cout << setiosflags(ios::left) << setw(12) << Time_Matrix[I][J];//以左对齐，宽度为12的形式显示最终时间矩阵
		}
		//cout << endl;
	}
}
///读取duration
void Read_array_Data(string arrayRoute, float *array, int n)   //括号内参数为（路径，数组，节点）   
{
	ifstream inarrayFile(arrayRoute.c_str());//文件流
	for (int i = 0; i < 2 * n + 2; i++)
	{
		inarrayFile >> array[i];//逐个数据输入给数组array
		cout << setiosflags(ios::left) << setw(8) << array[i];//以左对齐，宽度为8的形式显示最终数组
	}
	cout << endl;
	inarrayFile.close();//关闭文件
}
///用于读取时间间隔A，B
void Read_1array_Data(string arrayRoute, float **array, int n, int m)   //括号内参数为（路径，数组，节点）   
{
	ifstream in2arrayFile(arrayRoute.c_str());//文件流

	for (int i = 1; i <= n; i++)
	{
		for (int j = 0; j < m; j++)
		{
			in2arrayFile >> array[i][j];//逐个数据输入给数组array
			cout << setiosflags(ios::left) << setw(8) << array[i][j];//以左对齐，宽度为8的形式显示最终数组
		}
		cout << endl;
	}
	in2arrayFile.close();//关闭文件
}
///读取时间窗数据starttimewindow，endtimewindow
void Read_2array_Data(string arrayRoute, float **array, int n, int t)   //括号内参数为（路径，数组，节点,路线）   
{
	ifstream in3arrayFile(arrayRoute.c_str());//文件流
	for (int i = 0; i < 2 * n + 2; i++)
	{
		for (int j = 0; j < t; j++)
		{
			in3arrayFile >> array[i][j];//逐个数据输入给数组array
			cout << setiosflags(ios::left) << setw(8) << array[i][j];//以左对齐，宽度为8的形式显示最终数组
		}
		cout << endl;
	}
	in3arrayFile.close();//关闭文件
}
     

//按插入后增加的收益从大到小排序
void sorting_Inserted_Sequence(int *Inserted_Sequence, int FeasibleInsertion, InsertionNode *Insertion)
{
	int Temp;
	for (int i = 0; i < FeasibleInsertion; i++)
		for (int j = FeasibleInsertion - 1; i < j; j--)
			if (Insertion[Inserted_Sequence[j]].Increase_Profit > Insertion[Inserted_Sequence[i]].Increase_Profit)// 
			{
				Temp = Inserted_Sequence[i];
				Inserted_Sequence[i] = Inserted_Sequence[j];
				Inserted_Sequence[j] = Temp;
			}
}
//按时间窗从小到大排序
void sorting_Inserted_Sequence_t1(int *NodeSequence, float **starttimewindow, int a)
{
	int Temp;
	for (int i = 1; i <= a; i++)
		for (int j = a; i < j; j--)
			if (starttimewindow[NodeSequence[i]][0] > starttimewindow[NodeSequence[j]][0])
			{
				Temp = NodeSequence[i];
				NodeSequence[i] = NodeSequence[j];
				NodeSequence[j] = Temp;
			}
}


//导入数据
void Input_Data(string strProblemInstace)
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

	getline(file, str_temp);
	getline(file, str_temp);
	getline(file, str_temp);
	file >> nc >> nt >> nm;


	////节点数据、行驶成本数据，行驶时间数据（都是二维数组）
	nod = new Node[2 * nc + 2];
	getline(file, str_temp);
	getline(file, str_temp);
	for (int i = 0; i < 2 * nc + 2; i++)
	{
		file >> nod[i].Node_No >> nod[i].Coordinate_X >> nod[i].Coordinate_Y;//给Customer类的 Cust对象输入编号，横坐标，纵坐标
		//cout << setiosflags(ios::left) << setw(8) << nod[i].Node_No << setw(8) << nod[i].Node_X << setw(8) << nod[i].Node_Y << endl;
	}

	D = new float*[2 * nc + 2];//距离指针
	for (int i = 0; i < 2 * nc + 2; i++)
		D[i] = new float[2 * nc + 2];
	//cout << "距离矩阵如下：" << endl;
	Get_Distance_Matrix(nod, D, nc);//调用Get_Distance_Matrix函数，运用node数值求各节点之间的距离

	T = new float*[2 * nc + 2];//时间指针
	for (int i = 0; i < 2 * nc + 2; i++)
		T[i] = new float[2 * nc + 2];
	//cout << "时间矩阵如下：" << endl;
	Get_Time_Matrix(T, D, nc);//调用Get_Time_Matrix函数，运用距离、速度数值求各节点之间的行驶时间


	/////单位收益（二维数组）
	Profit = new float*[nc + 1];
	for (int i = 0; i <= nc; i++)
		Profit[i] = new float[nm];//收益
	getline(file, str_temp);
	getline(file, str_temp);
	for (int i = 1; i <= nc; i++)
	{
		for (int m = 0; m < nm; m++)
			file >> Profit[i][m];
	}

	/////1时间窗最早时间（二维数组）
	starttimewindow = new float *[2 * nc + 2];//时间窗
	endtimewindow = new float *[2 * nc + 2];
	for (int i = 0; i < 2 * nc + 2; i++)
	{
		starttimewindow[i] = new float[nt];
		endtimewindow[i] = new float[nt];
	}
	getline(file, str_temp);
	getline(file, str_temp);
	for (int i = 1; i < nc + 1; i++)
	{
		for (int k = 0; k < nt; k++)
		{
			file >> starttimewindow[i][k];
			starttimewindow[i + nc][k] = starttimewindow[i][k];
		}
	}

	getline(file, str_temp);
	getline(file, str_temp);
	for (int i = 1; i < nc + 1; i++)
	{
		for (int k = 0; k < nt; k++)
		{
			file >> endtimewindow[i][k];
			endtimewindow[i + nc][k] = endtimewindow[i][k];
		}
	}
	for (int k = 0; k < nt; k++)
	{
		starttimewindow[0][k] = 0;
		starttimewindow[2 * nc + 1][k] = 86400;
		endtimewindow[0][k] = 86400;
		endtimewindow[2 * nc + 1][k] = 86400;
	}

	/////任务执行数据（一维数组）
	Duration_Time = new float[2 * nc + 2];
	getline(file, str_temp);
	getline(file, str_temp);
	for (int i = 1; i < nc + 1; i++)
	{
		file >> Duration_Time[i];
		Duration_Time[i + nc] = Duration_Time[i];
	}
	Duration_Time[0] = 0;
	Duration_Time[2 * nc + 1] = 0;


	/////1时间段最早时间（二维数组）
	A_time = new float *[nc + 1];//时间窗
	B_time = new float *[nc + 1];
	for (int i = 0; i < nc + 1; i++)
	{
		A_time[i] = new float[nm];
		B_time[i] = new float[nm];
	}
	getline(file, str_temp);
	getline(file, str_temp);
	for (int i = 1; i < nc + 1; i++)
	{
		for (int m = 0; m < nm; m++)
		{
			file >> A_time[i][m];
		}
	}

	getline(file, str_temp);
	getline(file, str_temp);
	for (int i = 1; i < nc + 1; i++)
	{
		for (int m = 0; m < nm; m++)
		{
			file >> B_time[i][m];
		}
	}
	cout << "需要用的数据导入完毕！" << endl;

}

//初始化参数
void Initial_Parameter(Solution&FirIn, Solution&New, Solution& Current, InsertionNode*Insertion3, InsertionNode*Insertion4)
{
	Node_Number = nc;
	for (int n = 0; n <= Max_Node; n++)
		NodeSequence[n] = n;

	for (int n = 0; n <= Max_Node; n++)
		isChoose_Remove[n] = 0;
	for (int n = 0; n <= Max_Node; n++)
		isChoose_Change[n] = 0;

	for (int k = 0; k < nt; k++)
		Node_Number_In_Tour[k] = 0;

	for (int p = 0; p < 1000000; p++)
	{
		Insertion3[p].NodeNO1 = -1; //一次访问节点编号
		Insertion3[p].InsertionPos1 = -1;//一次访问节点插入当前路线中的位置编号
		Insertion3[p].NodeNO2 = -1; //二次访问节点编号
		Insertion3[p].InsertionPos2 = -1; //二次访问节点插入当前路线中的位置编号
		Insertion3[p].Increase_Profit = 0.00; //两次访问产生的收益	
		Insertion3[p].Insertiontour = -1;
	}
	for (int p = 0; p < 1000000; p++)
	{
		Insertion4[p].NodeNO1 = -1; //一次访问节点编号
		Insertion4[p].InsertionPos1 = -1;//一次访问节点插入当前路线中的位置编号
		Insertion4[p].NodeNO2 = -1; //二次访问节点编号
		Insertion4[p].InsertionPos2 = -1; //二次访问节点插入当前路线中的位置编号
		Insertion4[p].Increase_Profit = 0.00; //两次访问产生的收益			
		Insertion4[p].Insertiontour = -1;
		Insertion4[p].Insertiontour1 = -1;
	}
	for (int p = 0; p < 1000000; p++)
		Inserted_Sequence3[p] = -1;
	for (int p = 0; p < 1000000; p++)
		Inserted_Sequence4[p] = -1;


	//初始化初始解
	for (int k = 0; k < Max_Tour; k++)
	{
		for (int n = 0; n < 2 * Max_Node + 2; n++)
			FirIn.Route_Visit_Nodes[k][n] = -1;
	}
	for (int n = 0; n < 2 * Max_Node + 2; n++)
	{
		for (int k = 0; k < Max_Tour; k++)
			FirIn.Route_Arrive_Time[n][k] = 0.00;
	}
	for (int n = 1; n <= Max_Node; n++)
		FirIn.Gap_Time[n] = -1;
	for (int n = 1; n <= Max_Node; n++)
		FirIn.Node_Profit[n] = 0.00;
	FirIn.Profit = 0.00;

	for (int k = 0; k < Max_Tour; k++)
	{
		for (int n = 0; n < 2 * Max_Node + 2; n++)
			New.Route_Visit_Nodes[k][n] = -1;
	}
	for (int n = 0; n < 2 * Max_Node + 2; n++)
	{
		for (int k = 0; k < Max_Tour; k++)
			New.Route_Arrive_Time[n][k] = 0.00;
	}
	for (int n = 1; n <= Max_Node; n++)
		New.Gap_Time[n] = -1;
	for (int n = 1; n <= Max_Node; n++)
		New.Node_Profit[n] = 0.00;
	New.Profit = 0.00;

	for (int k = 0; k < Max_Tour; k++)
	{
		for (int n = 0; n < 2 * Max_Node + 2; n++)
			Current.Route_Visit_Nodes[k][n] = -1;
	}
	for (int n = 0; n < 2 * Max_Node + 2; n++)
	{
		for (int k = 0; k < Max_Tour; k++)
			Current.Route_Arrive_Time[n][k] = 0.00;
	}
	for (int n = 1; n <= Max_Node; n++)
		Current.Gap_Time[n] = -1;
	for (int n = 1; n <= Max_Node; n++)
		Current.Node_Profit[n] = 0.00;
	Current.Profit = 0.00;

	///////////以下四个为路线，用来记录插入节点，四个交换使用，不断更新，最后的”-1"来判断线路的结束
	Current_Tour = new int*[2 * nc + 2];//暂存路线，用于交换
	Temp_Tour = new int*[2 * nc + 2];//暂存路线，用于交换
	Tour = new int*[2 * nc + 2];//开辟新路线（只包含起点和终点），与Temp_Tour交换，确定当前路线各位置插入节点情况
	Tour1 = new int*[2 * nc + 2];//最终路线，与Current_Tour交换，确定最终路线，并将值赋给初始解保存
	for (int p = 0; p < 2 * nc + 2; p++)//初始各路线各位置值为-1
	{
		Temp_Tour[p] = new int[nt];
		Tour[p] = new int[nt];
		Tour1[p] = new int[nt];
		Current_Tour[p] = new int[nt];
	}
	for (int p = 0; p < 2 * nc + 2; p++)//初始各路线各位置值为-1
	{
		for (int k = 0; k < nt; k++)
		{
			Temp_Tour[p][k] = -1;
			Tour[p][k] = -1;
			Tour1[p][k] = -1;
			Current_Tour[p][k] = -1;
		}
	}

	
	Insertion = new InsertionNode[1000000];
	for (int p = 0; p < 1000000; p++)
	{
		Insertion[p].NodeNO1 = -1; //一次访问节点编号
		Insertion[p].InsertionPos1 = -1;//一次访问节点插入当前路线中的位置编号
		Insertion[p].NodeNO2 = -1; //二次访问节点编号
		Insertion[p].InsertionPos2 = -1; //二次访问节点插入当前路线中的位置编号
		Insertion[p].Increase_Profit = 0.00; //两次访问产生的收益	
		Insertion[p].Insertiontour = -1;
	}

	
	Insertion2 = new InsertionNode[1000000];
	for (int p = 0; p < 1000000; p++)
	{
		Insertion2[p].NodeNO1 = -1; //一次访问节点编号
		Insertion2[p].InsertionPos1 = -1;//一次访问节点插入当前路线中的位置编号
		Insertion2[p].NodeNO2 = -1; //二次访问节点编号
		Insertion2[p].InsertionPos2 = -1; //二次访问节点插入当前路线中的位置编号
		Insertion2[p].Increase_Profit = 0.00; //两次访问产生的收益			
		Insertion2[p].Insertiontour = -1;
		Insertion2[p].Insertiontour1 = -1;
	}
	//节点插入后，在各位置收益的增加按大到小排序
	Inserted_Sequence = new int[1000000];
	for (int p = 0; p < 1000000; p++)
		Inserted_Sequence[p] = -1;
	Inserted_Sequence2 = new int[1000000];
	for (int p = 0; p < 1000000; p++)
		Inserted_Sequence2[p] = -1;


	//开辟新路线，新路线只包含起点和终点
	for (int k = 0; k < nt; k++)
	{
		Tour[0][k] = 0;
		Tour[1][k] = 2 * nc + 1;
		Tour1[0][k] = 0;
		Tour1[1][k] = 2 * nc + 1;
		FirIn.Route_Visit_Nodes[k][0] = 0;
		FirIn.Route_Visit_Nodes[k][0] = 0;
		FirIn.Route_Visit_Nodes[k][1] = 2 * nc + 1;
		FirIn.Route_Visit_Nodes[k][1] = 2 * nc + 1;
	}

	//计算当前路线的长度，即节点数，利于后面计算各点可插入的位置,初始为2
	for (int k = 0; k < nt; k++)
	{
		for (int p = 0; p < 2 * nc + 2; p++)
		{
			if (Tour[p][k] != -1)
				Node_Number_In_Tour[k]++;
		}
	}

	for (int k = 0; k < nt; k++)
	{
		FirIn.Route_Arrive_Time[0][k] = 0;//起点的开始访问时间
		FirIn.Route_Arrive_Time[2 * nc + 1][k] = 86400;//终点的开始访问时间
	}
}
//输出初始解，定义为当前最优解
void Show_InitialSolution()
{
	cout << "初始解为：" << endl;
	/*for (int k = 0; k < nt; k++)
	{
		cout << "第" << k + 1 << "条路线的次序如下：" << endl;
		for (int p = 0; p < 2 * nc + 2; p++)
		{
			if (FirIn.Route_Visit_Nodes[k][p] == -1)
				break;
			cout << setw(8) << FirIn.Route_Visit_Nodes[k][p];
		}
		cout << endl;
	}
	cout << endl;
	for (int k = 0; k < nt; k++)
	{
		cout << "第" << k + 1 << "条路线的开始访问时间如下：" << endl;
		for (int n = 0; n < 2 * nc + 2; n++)
		{
			cout << setw(10) << FirIn.Route_Arrive_Time[n][k];
		}
		cout << endl;
	}
	cout << endl;
	cout << "各节点两次访问的间隔时间:" << endl;
	for (int n = 1; n <= nc; n++)
	{
		cout << setw(10) << FirIn.Gap_Time[n];
	}
	cout << endl;
	cout << "各节点两次访问的收益：" << endl;
	for (int n = 1; n <= nc; n++)
	{
		cout << setw(8) << FirIn.Node_Profit[n];
	}
	cout << endl;*/
	cout << "初始收益 =     " << FirIn.Profit << endl;
	

	//将初始路线,收益，时间，总收益赋值给当前解Current
	for (int k = 0; k < nt; k++)
	{
		for (int p = 0; p < 2 * nc + 2; p++)
			Current.Route_Visit_Nodes[k][p] = FirIn.Route_Visit_Nodes[k][p];
	}
	for (int k = 0; k < nt; k++)
	{
		for (int n = 0; n < 2 * nc + 2; n++)
			Current.Route_Arrive_Time[n][k] = FirIn.Route_Arrive_Time[n][k];
	}
	for (int n = 1; n <= nc; n++)
	{
		Current.Gap_Time[n] = FirIn.Gap_Time[n];
		Current.Node_Profit[n] = FirIn.Node_Profit[n];
	}
	Current.Profit = FirIn.Profit;

	//将初始路线,收益，时间，总收益赋值给新解New
	for (int k = 0; k < nt; k++)
	{
		for (int p = 0; p < 2 * nc + 2; p++)
			New.Route_Visit_Nodes[k][p] = Current.Route_Visit_Nodes[k][p];
	}
	for (int k = 0; k < nt; k++)
	{
		for (int n = 0; n < 2 * nc + 2; n++)
			New.Route_Arrive_Time[n][k] = Current.Route_Arrive_Time[n][k];
	}
	for (int n = 1; n <= nc; n++)
	{
		New.Gap_Time[n] = Current.Gap_Time[n];
		New.Node_Profit[n] = Current.Node_Profit[n];
	}

}

//输出最优解
void Show_optimalSolution()
{
	cout << "最优解为：" << endl;
	for (int k = 0; k < nt; k++)
	{
		cout << "第" << k + 1 << "条路线的次序如下：" << endl;
		for (int p = 0; p < 2 * nc + 2; p++)
		{
			if (Final.Route_Visit_Nodes[k][p] == -1)
				break;
			cout << setw(8) << Final.Route_Visit_Nodes[k][p];
		}
		cout << endl;
	}
	cout << endl;
	for (int k = 0; k < nt; k++)
	{
		cout << "第" << k + 1 << "条路线的开始访问时间如下：" << endl;
		for (int n = 0; n < 2 * nc + 2; n++)
		{
			cout << setw(10) << Final.Route_Arrive_Time[n][k];
		}
		cout << endl;
	}
	cout << endl;
	cout << "各节点两次访问的间隔时间:" << endl;
	for (int n = 1; n <= nc; n++)
	{
		cout << setw(10) << Final.Gap_Time[n];
	}
	cout << endl;
	cout << "各节点两次访问的收益：" << endl;
	for (int n = 1; n <= nc; n++)
	{
		cout << setw(8) << Final.Node_Profit[n];
	}
	cout << endl;
	cout << "最优收益 =     " << Final.Profit << endl;
}


void peidui_check(Solution& New, Solution& Current, Solution& Final)
{
	//若只有一次访问，则尝试插入缺失访问到使收益最大的位置，若无法合法插入，则标记为删除
	for (int n = 1; n <= nc; n++)
	{
		bool is = 0;
		bool is1 = 0;
		int route = 0;
		int position = 0;
		double max_zhibiao = -100000;//
		int optimal_NodeNO1 = 0;//节点编号
		int optimal_InsertionPos1 = 0;//当前点插入当前路线中的位置编号
		int optimal_Insertiontour = 0;//当前点插入的路线编号
		double optimal_Profit = 0;
		int optimal_time = 0;
		double optimal_Gap_time = 0;
		for (int t = 0; t < nt; t++)
		{
			for (int p = 0; p < 2 * nc + 2; p++)
			{
				if (Final.Route_Visit_Nodes[t][p] == -1)
					break;

				if (Final.Route_Visit_Nodes[t][p] == n)
				{
					is = 1;
					route = t;
					position = p;
				}
				if (Final.Route_Visit_Nodes[t][p] == n + nc)
				{
					is1 = 1;
					route = t;
					position = p;
				}
			}
		}
		if (is == 1 && is1 == 0)
		{
			//尝试插入第二次访问
			for (int k = route; k < nt; k++)
			{
				if (k != route)
					position = 0;
				for (int i = position + 1; i < Node_Number_In_Tour[k]; i++)
				{
					//////计算这一对节点的开始访问时间
					double Arrive_Time = 0;
					double Gap_Time = 0;
					int last_node = Final.Route_Visit_Nodes[k][i - 1];
					int next_node = Final.Route_Visit_Nodes[k][i];

					if (Final.Route_Arrive_Time[last_node][k] + T[last_node][n] + Duration_Time[last_node] <= starttimewindow[n][k])
						Arrive_Time = starttimewindow[n][k];
					else if (Final.Route_Arrive_Time[last_node][k] + T[last_node][n] + Duration_Time[last_node] <= (endtimewindow[n][k] - Duration_Time[n]))
						Arrive_Time = Final.Route_Arrive_Time[last_node][k] + T[last_node][n] + Duration_Time[last_node];

					/////////////如果这对节点都满足时间窗约束并且不影响路线下一节点的开始访问时间，则记录，计算收益，以备比较	
					if (Arrive_Time >= starttimewindow[n][k] && (Arrive_Time <= endtimewindow[n][k] - Duration_Time[n]))
					{
						if (Arrive_Time + T[n][next_node] + Duration_Time[n] <= Final.Route_Arrive_Time[next_node][k])
						{
							double end = Final.Route_Arrive_Time[next_node][k] - T[n][next_node] - Duration_Time[n];
							if (end > (endtimewindow[n][k] - Duration_Time[n]))
								end = endtimewindow[n][k] - Duration_Time[n];
							for (int ti = ceil(Arrive_Time); ti <= floor(end); ti ++)
							{
								double temp_zhibiao1 = 0;
								double temp_zhibiao2 = 0;
								double tim = ti - Arrive_Time;

								//计算新增收益
								Gap_Time = ti - Final.Route_Arrive_Time[n][route];

								for (int m = 0; m < nm; m++)
								{
									if (Gap_Time >= A_time[n][m] && Gap_Time < B_time[n][m])
										temp_zhibiao2 += Profit[n][m];
								}


								if (temp_zhibiao2 > max_zhibiao)
								{
									optimal_NodeNO1 = n;//节点编号
									optimal_InsertionPos1 = i;//当前点插入当前路线中的位置编号
									optimal_Insertiontour = k;//当前点插入的路线编号
									optimal_Profit = temp_zhibiao2;
									optimal_time = ti;
									optimal_Gap_time = Gap_Time;
									max_zhibiao = temp_zhibiao2;
								}
							}
						}
					}
				}
			}

			//找不到可以插入的，删掉这个节点
			if (max_zhibiao == -1)
			{
				for (int p = position; p < 2 * nc + 2; p++)
					Final.Route_Visit_Nodes[route][p] = Final.Route_Visit_Nodes[route][p + 1];
				Final.Route_Arrive_Time[n][route] = 0;
				Final.Node_Profit[n] = 0;
			}
			//选指标最大的节点和车辆和位置，插入
			if (max_zhibiao != -1)
			{
				int n = optimal_NodeNO1;
				int i = optimal_InsertionPos1;
				int k = optimal_Insertiontour;
				int ti = optimal_time;

				//////为这节点腾出位置
				for (int p = 2 * nc + 1; p > i; p--)
					Final.Route_Visit_Nodes[k][p] = Final.Route_Visit_Nodes[k][p - 1];
				Final.Route_Visit_Nodes[k][i] = n + nc;


				/////////选择节点最终插入的位置的开始时间
				Final.Route_Arrive_Time[n+nc][k] = ti;
				//else if (FirIn.Route_Arrive_Time[FirIn.Route_Visit_Nodes[k][i - 1]][k] + T[FirIn.Route_Visit_Nodes[k][i - 1]][n] + Duration_Time[FirIn.Route_Visit_Nodes[k][i - 1]] <= (endtimewindow[n][k] - Duration_Time[n]))
					//FirIn.Route_Arrive_Time[n][k] = FirIn.Route_Arrive_Time[FirIn.Route_Visit_Nodes[k][i - 1]][k] + T[FirIn.Route_Visit_Nodes[k][i - 1]][n] + Duration_Time[FirIn.Route_Visit_Nodes[k][i - 1]];

				Final.Node_Profit[n] = optimal_Profit;
				Final.Gap_Time[n] = optimal_Gap_time;

				//计算当前路线的长度，利于后面计算各点可插入的位置
				Node_Number_In_Tour[k] += 1;

			}
		}
		Final.Profit += Final.Node_Profit[n];
	}


	//删除只访问一次的节点
	for (int n = 1; n <= nc; n++)
	{
		bool is = 0;
		bool is1 = 0;
		int route = 0;
		int position = 0;
		int route1 = 0;
		int position1 = 0;
		for (int t = 0; t < nt; t++)
		{
			for (int p = 0; p < 2 * nc + 2; p++)
			{
				if (Final.Route_Visit_Nodes[t][p] == -1)
					break;

				if (Final.Route_Visit_Nodes[t][p] == n)
				{
					is = 1;
					route = t;
					position = p;
				}
				if (Final.Route_Visit_Nodes[t][p] == n + nc)
				{
					is1 = 1;
					route1 = t;
					position1 = p;
				}
			}
		}

		if (is == 1 && is1 == 0)
		{
			//delete 第1次
			for (int p = position; p < 2 * nc + 2; p++)
				Final.Route_Visit_Nodes[route][p] = Final.Route_Visit_Nodes[route][p + 1];
			Final.Route_Arrive_Time[n][route] = 0;
			Node_Number_In_Tour[route] -= 1;
		}
		if (is == 0 && is1 == 1)
		{
			//delete 第2次
			for (int p = position1; p < 2 * nc + 2; p++)
				Final.Route_Visit_Nodes[route1][p] = Final.Route_Visit_Nodes[route1][p + 1];
			Final.Route_Arrive_Time[n + nc][route1] = 0;
			Node_Number_In_Tour[route1] -= 1;
		}
	}
}

void peidui_check_Ini(Solution& New, Solution& Current, Solution& FirIn)
{
	//若只有一次访问，则尝试插入缺失访问到使收益最大的位置，若无法合法插入，则标记为删除
	/*FirIn.Profit = 0;
	for (int n = 1; n <= nc; n++)
	{
		bool is = 0;
		bool is1 = 0;
		int route = 0;
		int position = 0;
		double max_zhibiao = -100000;//
		int optimal_NodeNO1 = 0;//节点编号
		int optimal_InsertionPos1 = 0;//当前点插入当前路线中的位置编号
		int optimal_Insertiontour = 0;//当前点插入的路线编号
		double optimal_Profit = 0;
		int optimal_time = 0;
		double optimal_Gap_time = 0;
		for (int t = 0; t < nt; t++)
		{
			for (int p = 1; p < 2 * nc + 2; p++)
			{
				if (FirIn.Route_Visit_Nodes[t][p] == 2*nc+1)
					break;

				if (FirIn.Route_Visit_Nodes[t][p] == n)
				{
					is = 1;
					route = t;
					position = p;
				}
				if (FirIn.Route_Visit_Nodes[t][p] == n + nc)
				{
					is1 = 1;
					route = t;
					position = p;
				}
			}
		}
		if (is == 1 && is1 == 0)
		{
			//尝试插入第二次访问
			for (int k = route; k < nt; k++)
			{
				if (k != route)
					position = 0;
				for (int i = position + 1; i < Node_Number_In_Tour[k]; i++)
				{
					//////计算这一对节点的开始访问时间
					double Arrive_Time = 0;
					double Gap_Time = 0;
					int last_node = FirIn.Route_Visit_Nodes[k][i - 1];
					int next_node = FirIn.Route_Visit_Nodes[k][i];

					if (FirIn.Route_Arrive_Time[last_node][k] + T[last_node][n] + Duration_Time[last_node] <= starttimewindow[n][k])
						Arrive_Time = starttimewindow[n][k];
					else if (FirIn.Route_Arrive_Time[last_node][k] + T[last_node][n] + Duration_Time[last_node] <= (endtimewindow[n][k] - Duration_Time[n]))
						Arrive_Time = FirIn.Route_Arrive_Time[last_node][k] + T[last_node][n] + Duration_Time[last_node];

					/////////////如果这对节点都满足时间窗约束并且不影响路线下一节点的开始访问时间，则记录，计算收益，以备比较	
					if (Arrive_Time >= starttimewindow[n][k] && (Arrive_Time <= endtimewindow[n][k] - Duration_Time[n]))
					{
						if (Arrive_Time + T[n][next_node] + Duration_Time[n] <= FirIn.Route_Arrive_Time[next_node][k])
						{
							double end = FirIn.Route_Arrive_Time[next_node][k] - T[n][next_node] - Duration_Time[n];
							if (end > (endtimewindow[n][k] - Duration_Time[n]))
								end = endtimewindow[n][k] - Duration_Time[n];
							for (int ti = Arrive_Time; ti <= end; ti+=100)
							{
								double temp_zhibiao1 = 0;
								double temp_zhibiao2 = 0;
								double tim = ti - Arrive_Time;

								//计算新增收益
								Gap_Time = ti - FirIn.Route_Arrive_Time[n][route];

								for (int m = 0; m < nm; m++)
								{
									if (Gap_Time >= A_time[n][m] && Gap_Time < B_time[n][m])
										temp_zhibiao2 += Profit[n][m];
								}


								if (temp_zhibiao2 > max_zhibiao)
								{
									optimal_NodeNO1 = n;//节点编号
									optimal_InsertionPos1 = i;//当前点插入当前路线中的位置编号
									optimal_Insertiontour = k;//当前点插入的路线编号
									optimal_Profit = temp_zhibiao2;
									optimal_time = ti;
									optimal_Gap_time = Gap_Time;
									max_zhibiao = temp_zhibiao2;
								}
							}
						}
					}
				}
			}

			//找不到可以插入的，删掉这个节点
			if (max_zhibiao == -1)
			{
				for (int p = position; p < 2 * nc + 2; p++)
					FirIn.Route_Visit_Nodes[route][p] = FirIn.Route_Visit_Nodes[route][p + 1];
				FirIn.Route_Arrive_Time[n][route] = 0;
			}
			//选指标最大的节点和车辆和位置，插入
			if (max_zhibiao != -1)
			{
				int n = optimal_NodeNO1;
				int i = optimal_InsertionPos1;
				int k = optimal_Insertiontour;
				int ti = optimal_time;

				//////为这节点腾出位置
				for (int p = 2 * nc + 1; p > i; p--)
					FirIn.Route_Visit_Nodes[k][p] = FirIn.Route_Visit_Nodes[k][p - 1];
				FirIn.Route_Visit_Nodes[k][i] = n + nc;


				/////////选择节点最终插入的位置的开始时间
				FirIn.Route_Arrive_Time[n][k] = ti;
				FirIn.Node_Profit[n] = optimal_Profit;
				FirIn.Gap_Time[n] = optimal_Gap_time;

				//计算当前路线的长度，利于后面计算各点可插入的位置
				Node_Number_In_Tour[k] += 1;

			}
		}
		FirIn.Profit += FirIn.Node_Profit[n];
	}*/


	//删除只访问一次的节点
	for (int n = 1; n <= nc; n++)
	{
		bool is = 0;
		bool is1 = 0;
		int route = 0;
		int position = 0;
		int route1 = 0;
		int position1 = 0;
		for (int t = 0; t < nt; t++)
		{
			for (int p = 0; p < 2 * nc + 2; p++)
			{
				if (FirIn.Route_Visit_Nodes[t][p] == -1)
					break;

				if (FirIn.Route_Visit_Nodes[t][p] == n)
				{
					is = 1;
					route = t;
					position = p;
				}
				if (FirIn.Route_Visit_Nodes[t][p] == n + nc)
				{
					is1 = 1;
					route1 = t;
					position1 = p;
				}
			}
		}

		if (is == 1 && is1 == 0)
		{
			//delete 第1次
			for (int p = position; p < 2 * nc + 2; p++)
				FirIn.Route_Visit_Nodes[route][p] = FirIn.Route_Visit_Nodes[route][p + 1];
			FirIn.Route_Arrive_Time[n][route] = 0;
			Node_Number_In_Tour[route] -= 1;
		}
		if (is == 0 && is1 == 1)
		{
			//delete 第2次
			for (int p = position1; p < 2 * nc + 2; p++)
				FirIn.Route_Visit_Nodes[route1][p] = FirIn.Route_Visit_Nodes[route1][p + 1];
			FirIn.Route_Arrive_Time[n + nc][route1] = 0;
			Node_Number_In_Tour[route1] -= 1;
		}
	}
}


//第一种初始解构造方法：按照时间窗排序，依次插入节点。第二次访问节点不放在路线1，第一次访问节点不放在最后路线
float InitialSolution1(Solution&FirIn, Solution&New, Solution& Current, float Greed)
{
	for (int i = 1; i <= Node_Number; i++)//可修改i<5,对一半乱序交换即可
	{
		int j = 1;
		while (j == i)
		{
			srand(time(NULL));
			j = Random(nc - 1) + 1;//可以自己构造一个其他的函数，保证与10求余后不与原位置相同即可
		}
		//cout << "weizhi" << j << endl;
		int temp = NodeSequence[i];
		NodeSequence[i] = NodeSequence[j];
		NodeSequence[j] = temp;
	}


	All_Node_Number = Node_Number * 2;

	for (int n = 2; n <= All_Node_Number; n += 2)
	{
		for (int p = All_Node_Number; p >= n; p--)
		{
			NodeSequence[p] = NodeSequence[p - 1];
		}
		NodeSequence[n] = NodeSequence[n - 1] + Node_Number;
	}

	int n = 1;
	while (n <= All_Node_Number)//Node_Number和NodeSequence一直再更新变化着！
	{
		if (NodeSequence[n] == -1)
			break;


		bool feasible = 0;


		for (int k = 0; k < nt; k++)
		{
			if (NodeSequence[n] > Node_Number && k == 0)//第一条路线上不插入第二次访问节点
			{
				k++;
				continue;
			}
			if ((NodeSequence[n] < Node_Number) && (k == (nt - 1)))//最后一条路线上不插入第一次访问节点
			{
				k++;
				continue;
			}

			for (int i = 1; i < Node_Number_In_Tour[k]; i++)////节点对中的第一次访问节点的插入位置
			{
				int z1 = FirIn.Route_Visit_Nodes[k][i - 1];
				int z2 = NodeSequence[n];
				int z3 = FirIn.Route_Visit_Nodes[k][i];


				//计算这一对节点的开始访问时间
				float temp = 0;
				if (FirIn.Route_Arrive_Time[z1][k] + T[z1][z2] + Duration_Time[z1] <= starttimewindow[z2][k])
					temp = starttimewindow[z2][k];
				else if (FirIn.Route_Arrive_Time[z1][k] + T[z1][z2] + Duration_Time[z1] <= (endtimewindow[z2][k] - Duration_Time[z2]))
					temp = FirIn.Route_Arrive_Time[z1][k] + T[z1][z2] + Duration_Time[z1];
				else
					continue;

				/////////////如果节点不影响路线下一节点的开始访问时间，则记录
				if ((temp + T[z2][z3] + Duration_Time[z2]) <= FirIn.Route_Arrive_Time[z3][k])
				{
					//选择节点最终插入的位置的开始时间
					FirIn.Route_Arrive_Time[z2][k] = temp;

					//////为节点腾出位置
					for (int p = 2 * Max_Node + 1; p >= i + 1; p--)
						FirIn.Route_Visit_Nodes[k][p] = FirIn.Route_Visit_Nodes[k][p - 1];
					FirIn.Route_Visit_Nodes[k][i] = z2;
					//计算当前路线的长度，利于后面计算各点可插入的位置
					Node_Number_In_Tour[k] += 1;


					//把插入的节点从剩余未插入节点序列给删除掉
					for (int p = n; p <= All_Node_Number; p++)
					{
						if (NodeSequence[All_Node_Number] == NodeSequence[n])//如果是最后一个!那就直接赋值为-
							NodeSequence[All_Node_Number] = -1;
						else
							NodeSequence[p] = NodeSequence[p + 1];
					}
					All_Node_Number = All_Node_Number - 1;

					feasible = 1;
					break;
				}
			}
			if (feasible == 1)
				break;
		}
		if (feasible == 0)
			n++;
	}

	for (int i = 1; i <= nc; i++)
	{
		bool a = 1;
		for (int t = 0; t < nt; t++)
		{
			for (int t1 = 0; t1 < nt; t1++)
			{
				if (FirIn.Route_Arrive_Time[i + nc][t1] > 0 && FirIn.Route_Arrive_Time[i][t] > 0)
				{
					FirIn.Gap_Time[i] = FirIn.Route_Arrive_Time[i + nc][t1] - FirIn.Route_Arrive_Time[i][t];
					a = 0;
					break;
				}

			}
			if (a == 0)
				break;
		}
	}
	for (int i = 1; i <= nc; i++)
	{
		for (int m = 0; m < nm; m++)
		{
			if ((FirIn.Gap_Time[i] >= A_time[i][m]) && (FirIn.Gap_Time[i] < B_time[i][m]))
			{
				FirIn.Node_Profit[i] = Profit[i][m];
				break;
			}
		}
		FirIn.Profit += FirIn.Node_Profit[i];
	}

	//如果只完成一次访问，则删除
	for (int n = 1; n <= All_Node_Number; n++)
	{
		if (NodeSequence[n] == -1)
			break;

		int a = 0;
		if (NodeSequence[n] > Node_Number)
			a = NodeSequence[n] - Node_Number;
		else if (NodeSequence[n] <= Node_Number)
			a = NodeSequence[n];

		bool Is_Visited1 = 0;
		bool Is_Visited2 = 0;
		for (int k = 0; k < nt; k++)
		{
			for (int i = 1; i < Node_Number_In_Tour[k]; i++)
			{
				if (FirIn.Route_Visit_Nodes[k][i] == a)
				{
					Is_Visited1 = 1;
				}
				if (FirIn.Route_Visit_Nodes[k][i] == (a + Node_Number))
				{
					Is_Visited2 = 1;
				}
			}
		}

		if ((Is_Visited1 == 1 && Is_Visited2 == 1) || (Is_Visited1 == 0 && Is_Visited2 == 0))
			continue;


		for (int k = 0; k < nt; k++)
		{
			FirIn.Route_Arrive_Time[a][k] = 0;
			FirIn.Route_Arrive_Time[a + Node_Number][k] = 0;
			for (int i = 1; i < Node_Number_In_Tour[k]; i++)
			{
				if ((FirIn.Route_Visit_Nodes[k][i] == a)
					|| (FirIn.Route_Visit_Nodes[k][i] == (a + Node_Number)))
				{
					for (int i1 = i; i1 < Node_Number_In_Tour[k]; i1++)
					{
						FirIn.Route_Visit_Nodes[k][i1] = FirIn.Route_Visit_Nodes[k][i1 + 1];
					}
					Node_Number_In_Tour[k] -= 1;
				}
			}
		}

		if (NodeSequence[n] > Node_Number)
		{
			NodeSequence[All_Node_Number + 1] = a;
			All_Node_Number += 1;
		}
	}

	n = 1;
	while (n <= All_Node_Number)
	{
		if (NodeSequence[n] > Node_Number)
		{
			for (int n1 = n; n1 <= All_Node_Number; n1++)
			{
				if (NodeSequence[All_Node_Number] == NodeSequence[n1])
					NodeSequence[n1] = -1;
				else
					NodeSequence[n1] = NodeSequence[n1 + 1];
			}
			All_Node_Number -= 1;
		}
		else
			n++;
	}
	Node_Number = All_Node_Number;
	return FirIn.Profit;
}

//第二种初始解构造方法，为每一对节点依次选择最好的插入位置
float InitialSolution2(Solution&FirIn, Solution&New, Solution& Current, float Greed)
{	
	for (int i = 1; i <= Node_Number; i++)//可修改i<5,对一半乱序交换即可
	{
		int j = 1;
		while (j == i)
		{
			srand(time(NULL));
			j = Random(nc - 1) + 1;//可以自己构造一个其他的函数，保证与10求余后不与原位置相同即可
		}
		//cout << "weizhi" << j << endl;
		int temp = NodeSequence[i];
		NodeSequence[i] = NodeSequence[j];
		NodeSequence[j] = temp;
	}


	///////////////////////////////////////////////////////////开始求解，插入节点对
	for (int g = 0; g < 10; g++)
	{
		int n = 1;
		while (n <= Node_Number)//Node_Number和NodeSequence一直再更新变化着！
		{
			//该节点不同时间段的收益按从大到小排序
			int *Sort_Interval = new int[nm];
			for (int m = 0; m < nm; m++)
				Sort_Interval[m] = m;

			for (int m = 0; m < nm; m++)
			{
				for (int m1 = nm - 1; m1 > m; m1--)
				{
					if (Profit[NodeSequence[n]][Sort_Interval[m]] < Profit[NodeSequence[n]][Sort_Interval[m1]])
					{
						int temp = Sort_Interval[m];
						Sort_Interval[m] = Sort_Interval[m1];
						Sort_Interval[m1] = temp;
					}
				}
			}
			//for (int m = 0; m < nm; m++)
				//cout << Sort_Interval[m] << endl;
			//int temp = Profit[NodeSequence[n]][Sort_Interval[0]];

			//大于贪婪因子的节点才可以考虑插入
			if (nod[n].Grade <= 0)
			{
				//n++;
				//continue;
			}

			int FeasibleInsertion = 0;//节点对在同一路线的可插入位置个数
			int FeasibleInsertion2 = 0;//节点对在不同路线的可插入位置个数
			for (int k = 0; k < nt; k++)
			{
				for (int i = 1; i < Node_Number_In_Tour[k]; i++)////节点对中的第一次访问节点的插入位置
				{
					for (int kk = k + 1; kk < nt; kk++)
					{
						for (int j = 1; j < Node_Number_In_Tour[kk]; j++)//节点对第二次访问可插入位置
						{
							float Temp_Ratio = 0.00;//用来暂存收益

							//////为这一对节点腾出位置
							for (int p = 0; p < i; p++)
								Temp_Tour[p][k] = Tour[p][k];
							Temp_Tour[i][k] = NodeSequence[n];
							for (int p = i + 1; p < 2 * nc + 2; p++)
								Temp_Tour[p][k] = Tour[p - 1][k];

							for (int p = 0; p < j; p++)
								Temp_Tour[p][kk] = Tour[p][kk];
							Temp_Tour[j][kk] = NodeSequence[n] + nc;
							for (int p = j + 1; p < 2 * nc + 2; p++)
								Temp_Tour[p][kk] = Tour[p - 1][kk];


							//////计算这一对节点的开始访问时间
							if (FirIn.Route_Arrive_Time[Temp_Tour[i - 1][k]][k] + T[Temp_Tour[i - 1][k]][NodeSequence[n]] + Duration_Time[Temp_Tour[i - 1][k]] <= starttimewindow[NodeSequence[n]][k])
								FirIn.Route_Arrive_Time[NodeSequence[n]][k] = starttimewindow[NodeSequence[n]][k];
							else if (FirIn.Route_Arrive_Time[Temp_Tour[i - 1][k]][k] + T[Temp_Tour[i - 1][k]][NodeSequence[n]] + Duration_Time[Temp_Tour[i - 1][k]] <= (endtimewindow[NodeSequence[n]][k] - Duration_Time[NodeSequence[n]]))
								FirIn.Route_Arrive_Time[NodeSequence[n]][k] = FirIn.Route_Arrive_Time[Temp_Tour[i - 1][k]][k] + T[Temp_Tour[i - 1][k]][NodeSequence[n]] + Duration_Time[Temp_Tour[i - 1][k]];

							float b = FirIn.Route_Arrive_Time[Temp_Tour[j - 1][kk]][kk] + T[Temp_Tour[j - 1][kk]][NodeSequence[n] + nc] + Duration_Time[Temp_Tour[j - 1][kk]];
							if (b <= starttimewindow[NodeSequence[n] + nc][kk])
							{
								FirIn.Route_Arrive_Time[NodeSequence[n] + nc][kk] = starttimewindow[NodeSequence[n] + nc][kk];
								for (int m = 0; m < nm; m++)
								{
									if (starttimewindow[NodeSequence[n] + nc][kk] <= FirIn.Route_Arrive_Time[NodeSequence[n]][k] + A_time[NodeSequence[n]][Sort_Interval[m]]
										&& FirIn.Route_Arrive_Time[NodeSequence[n]][k] + A_time[NodeSequence[n]][Sort_Interval[m]] <= endtimewindow[NodeSequence[n] + nc][kk] - Duration_Time[NodeSequence[n] + nc])
									{
										FirIn.Route_Arrive_Time[NodeSequence[n] + nc][kk] = FirIn.Route_Arrive_Time[NodeSequence[n]][k] + A_time[NodeSequence[n]][Sort_Interval[m]];
										break;
									}
								}
							}
							else if (b <= (endtimewindow[NodeSequence[n] + nc][kk] - Duration_Time[NodeSequence[n] + nc]))
							{
								FirIn.Route_Arrive_Time[NodeSequence[n] + nc][kk] = b;
								for (int m = 0; m < nm; m++)
								{
									if (b <= FirIn.Route_Arrive_Time[NodeSequence[n]][k] + A_time[NodeSequence[n]][Sort_Interval[m]]
										&& FirIn.Route_Arrive_Time[NodeSequence[n]][k] + A_time[NodeSequence[n]][Sort_Interval[m]] <= endtimewindow[NodeSequence[n] + nc][kk] - Duration_Time[NodeSequence[n] + nc])
									{
										FirIn.Route_Arrive_Time[NodeSequence[n] + nc][kk] = FirIn.Route_Arrive_Time[NodeSequence[n]][k] + A_time[NodeSequence[n]][Sort_Interval[m]];
										break;
									}
								}
							}

							/////////////如果这对节点都满足时间窗约束并且不影响路线下一节点的开始访问时间，则记录，计算收益，以备比较	
							if ((FirIn.Route_Arrive_Time[NodeSequence[n]][k] >= starttimewindow[NodeSequence[n]][k]) && (FirIn.Route_Arrive_Time[NodeSequence[n]][k] <= endtimewindow[NodeSequence[n]][k] - Duration_Time[NodeSequence[n]]) && (FirIn.Route_Arrive_Time[NodeSequence[n] + nc][kk] >= starttimewindow[NodeSequence[n] + nc][kk]) && (FirIn.Route_Arrive_Time[NodeSequence[n] + nc][kk] <= endtimewindow[NodeSequence[n] + nc][kk] - Duration_Time[NodeSequence[n] + nc]))
							{
								if ((FirIn.Route_Arrive_Time[NodeSequence[n]][k] + T[NodeSequence[n]][Temp_Tour[i + 1][k]] + Duration_Time[NodeSequence[n]] <= FirIn.Route_Arrive_Time[Temp_Tour[i + 1][k]][k]) && (FirIn.Route_Arrive_Time[NodeSequence[n] + nc][kk] + T[NodeSequence[n] + nc][Temp_Tour[j + 1][kk]] + Duration_Time[NodeSequence[n] + nc] <= FirIn.Route_Arrive_Time[Temp_Tour[j + 1][kk]][kk]))
								{
									FirIn.Gap_Time[NodeSequence[n]] = FirIn.Route_Arrive_Time[NodeSequence[n] + nc][kk] - FirIn.Route_Arrive_Time[NodeSequence[n]][k];
									for (int m = 0; m < nm; m++)
									{
										if (FirIn.Gap_Time[NodeSequence[n]] >= A_time[NodeSequence[n]][m] && FirIn.Gap_Time[NodeSequence[n]] < B_time[NodeSequence[n]][m])
											Temp_Ratio = Profit[NodeSequence[n]][m];
									}
									if (Temp_Ratio == 0)
										continue;
									Insertion2[FeasibleInsertion2].NodeNO1 = NodeSequence[n];//节点编号
									Insertion2[FeasibleInsertion2].NodeNO2 = NodeSequence[n] + nc; //节点编号
									Insertion2[FeasibleInsertion2].InsertionPos1 = i;//当前点插入当前路线中的位置编号
									Insertion2[FeasibleInsertion2].InsertionPos2 = j;//当前点插入当前路线中的位置编号
									Insertion2[FeasibleInsertion2].Increase_Profit = Temp_Ratio;//产生的增加收益
									Inserted_Sequence2[FeasibleInsertion2] = FeasibleInsertion2;  //用于后面排序
									Insertion2[FeasibleInsertion2].Insertiontour1 = kk;//当前点插入的路线编号
									Insertion2[FeasibleInsertion2].Insertiontour = k;//当前点插入的路线编号
									FeasibleInsertion2++;//计数
								}
							}
							FirIn.Route_Arrive_Time[NodeSequence[n]][k] = 0;
							FirIn.Route_Arrive_Time[NodeSequence[n] + nc][kk] = 0;
							FirIn.Gap_Time[NodeSequence[n]] = -1;
						}
					}

					for (int j = i + 1; j < Node_Number_In_Tour[k] + 1; j++)//节点对第二次访问可插入位置
					{
						float Temp_Ratio = 0;//用来暂存收益
						//////为这一对节点腾出位置
						for (int p = 0; p < i; p++)
							Temp_Tour[p][k] = Tour[p][k];
						Temp_Tour[i][k] = NodeSequence[n];
						for (int p = i + 1; p < j; p++)
							Temp_Tour[p][k] = Tour[p - 1][k];
						Temp_Tour[j][k] = NodeSequence[n] + nc;
						for (int p = j + 1; p < 2 * nc + 2; p++)
							Temp_Tour[p][k] = Tour[p - 2][k];

						//////计算这一对节点的开始访问时间
						if (FirIn.Route_Arrive_Time[Temp_Tour[i - 1][k]][k] + T[Temp_Tour[i - 1][k]][NodeSequence[n]] + Duration_Time[Temp_Tour[i - 1][k]] <= starttimewindow[NodeSequence[n]][k])
							FirIn.Route_Arrive_Time[NodeSequence[n]][k] = starttimewindow[NodeSequence[n]][k];
						else if (FirIn.Route_Arrive_Time[Temp_Tour[i - 1][k]][k] + T[Temp_Tour[i - 1][k]][NodeSequence[n]] + Duration_Time[Temp_Tour[i - 1][k]] <= (endtimewindow[NodeSequence[n]][k] - Duration_Time[NodeSequence[n]]))
							FirIn.Route_Arrive_Time[NodeSequence[n]][k] = FirIn.Route_Arrive_Time[Temp_Tour[i - 1][k]][k] + T[Temp_Tour[i - 1][k]][NodeSequence[n]] + Duration_Time[Temp_Tour[i - 1][k]];

						float a = FirIn.Route_Arrive_Time[Temp_Tour[j - 1][k]][k] + T[Temp_Tour[j - 1][k]][NodeSequence[n] + nc] + Duration_Time[Temp_Tour[j - 1][k]];
						if (a <= starttimewindow[NodeSequence[n] + nc][k])
						{
							FirIn.Route_Arrive_Time[NodeSequence[n] + nc][k] = starttimewindow[NodeSequence[n] + nc][k];
							for (int m = 0; m < nm; m++)
							{
								if (starttimewindow[NodeSequence[n] + nc][k] <= FirIn.Route_Arrive_Time[NodeSequence[n]][k] + A_time[NodeSequence[n]][Sort_Interval[m]]
									&& FirIn.Route_Arrive_Time[NodeSequence[n]][k] + A_time[NodeSequence[n]][Sort_Interval[m]] <= endtimewindow[NodeSequence[n] + nc][k] - Duration_Time[NodeSequence[n] + nc])
								{
									FirIn.Route_Arrive_Time[NodeSequence[n] + nc][k] = FirIn.Route_Arrive_Time[NodeSequence[n]][k] + A_time[NodeSequence[n]][Sort_Interval[m]];
									break;
								}
							}
						}
						else if (a <= endtimewindow[NodeSequence[n] + nc][k] - Duration_Time[NodeSequence[n] + nc])
						{
							FirIn.Route_Arrive_Time[NodeSequence[n] + nc][k] = a;
							for (int m = 0; m < nm; m++)
							{
								if ((a <= FirIn.Route_Arrive_Time[NodeSequence[n]][k] + A_time[NodeSequence[n]][Sort_Interval[m]])
									&& (FirIn.Route_Arrive_Time[NodeSequence[n]][k] + A_time[NodeSequence[n]][Sort_Interval[m]] <= endtimewindow[NodeSequence[n] + nc][k] - Duration_Time[NodeSequence[n] + nc]))
								{
									FirIn.Route_Arrive_Time[NodeSequence[n] + nc][k] = FirIn.Route_Arrive_Time[NodeSequence[n]][k] + A_time[NodeSequence[n]][Sort_Interval[m]];
									break;
								}
							}
						}


						/////////////如果这对节点都满足时间窗约束并且不影响路线下一节点的开始访问时间，则记录，计算收益，以备比较	
						if (FirIn.Route_Arrive_Time[NodeSequence[n]][k] >= starttimewindow[NodeSequence[n]][k] && FirIn.Route_Arrive_Time[NodeSequence[n]][k] <= endtimewindow[NodeSequence[n]][k] - Duration_Time[NodeSequence[n]] && FirIn.Route_Arrive_Time[NodeSequence[n] + nc][k] >= starttimewindow[NodeSequence[n] + nc][k] && FirIn.Route_Arrive_Time[NodeSequence[n] + nc][k] <= endtimewindow[NodeSequence[n] + nc][k] - Duration_Time[NodeSequence[n] + nc])
						{
							if (FirIn.Route_Arrive_Time[NodeSequence[n]][k] + T[NodeSequence[n]][Temp_Tour[i + 1][k]] + Duration_Time[NodeSequence[n]] <= FirIn.Route_Arrive_Time[Temp_Tour[i + 1][k]][k] && FirIn.Route_Arrive_Time[NodeSequence[n] + nc][k] + T[NodeSequence[n] + nc][Temp_Tour[j + 1][k]] + Duration_Time[NodeSequence[n] + nc] <= FirIn.Route_Arrive_Time[Temp_Tour[j + 1][k]][k])
							{
								FirIn.Gap_Time[NodeSequence[n]] = FirIn.Route_Arrive_Time[NodeSequence[n] + nc][k] - FirIn.Route_Arrive_Time[NodeSequence[n]][k];
								for (int m = 0; m < nm; m++)
								{
									if ((FirIn.Gap_Time[NodeSequence[n]] >= A_time[NodeSequence[n]][m]) && (FirIn.Gap_Time[NodeSequence[n]] < B_time[NodeSequence[n]][m]))
										Temp_Ratio = Profit[NodeSequence[n]][m];
								}
								if (Temp_Ratio == 0)
									continue;
								Insertion[FeasibleInsertion].NodeNO1 = NodeSequence[n];//节点编号
								Insertion[FeasibleInsertion].NodeNO2 = NodeSequence[n] + nc; //节点编号
								Insertion[FeasibleInsertion].InsertionPos1 = i;//当前点插入当前路线中的位置编号
								Insertion[FeasibleInsertion].InsertionPos2 = j;//当前点插入当前路线中的位置编号
								Insertion[FeasibleInsertion].Increase_Profit = Temp_Ratio;//产生的增加收益
								Inserted_Sequence[FeasibleInsertion] = FeasibleInsertion;  //用于后面排序
								Insertion[FeasibleInsertion].Insertiontour = k;
								FeasibleInsertion++;//计数
							}
						}
						FirIn.Route_Arrive_Time[NodeSequence[n]][k] = 0;
						FirIn.Route_Arrive_Time[NodeSequence[n] + nc][k] = 0;
						FirIn.Gap_Time[NodeSequence[n]] = 0;
					}
				}
			}

			//////////所有位置已遍历结束，选择可获得最大收益的位置
			if ((FeasibleInsertion != 0) || (FeasibleInsertion2 != 0))
			{
				//把能够插入的所有位置进行排序！取最好的插入！
				sorting_Inserted_Sequence(Inserted_Sequence, FeasibleInsertion, Insertion);//非增排序
				sorting_Inserted_Sequence(Inserted_Sequence2, FeasibleInsertion2, Insertion2);//非增排序
				//插入节点，位置更换
				if ((Insertion[Inserted_Sequence[0]].Increase_Profit >= Insertion2[Inserted_Sequence2[0]].Increase_Profit || FeasibleInsertion2 == 0) && (FeasibleInsertion != 0))
				{
					int k = Insertion[Inserted_Sequence[0]].Insertiontour;
					for (int p = 0; p < Insertion[Inserted_Sequence[0]].InsertionPos1; p++)
						Current_Tour[p][k] = Tour1[p][k];
					Current_Tour[Insertion[Inserted_Sequence[0]].InsertionPos1][k] = NodeSequence[n];
					for (int p = Insertion[Inserted_Sequence[0]].InsertionPos1 + 1; p < Insertion[Inserted_Sequence[0]].InsertionPos2; p++)
						Current_Tour[p][k] = Tour1[p - 1][k];
					Current_Tour[Insertion[Inserted_Sequence[0]].InsertionPos2][k] = NodeSequence[n] + nc;
					for (int p = Insertion[Inserted_Sequence[0]].InsertionPos2 + 1; p < 2 * nc + 2; p++)
						Current_Tour[p][k] = Tour1[p - 2][k];

					/////////选择节点最终插入的位置的开始时间
					if (FirIn.Route_Arrive_Time[Current_Tour[Insertion[Inserted_Sequence[0]].InsertionPos1 - 1][k]][k] + T[Current_Tour[Insertion[Inserted_Sequence[0]].InsertionPos1 - 1][k]][NodeSequence[n]] + Duration_Time[Current_Tour[Insertion[Inserted_Sequence[0]].InsertionPos1 - 1][k]] <= starttimewindow[NodeSequence[n]][k])
						FirIn.Route_Arrive_Time[NodeSequence[n]][k] = starttimewindow[NodeSequence[n]][k];
					else if (FirIn.Route_Arrive_Time[Current_Tour[Insertion[Inserted_Sequence[0]].InsertionPos1 - 1][k]][k] + T[Current_Tour[Insertion[Inserted_Sequence[0]].InsertionPos1 - 1][k]][NodeSequence[n]] + Duration_Time[Current_Tour[Insertion[Inserted_Sequence[0]].InsertionPos1 - 1][k]] <= endtimewindow[NodeSequence[n]][k])
						FirIn.Route_Arrive_Time[NodeSequence[n]][k] = FirIn.Route_Arrive_Time[Current_Tour[Insertion[Inserted_Sequence[0]].InsertionPos1 - 1][k]][k] + T[Current_Tour[Insertion[Inserted_Sequence[0]].InsertionPos1 - 1][k]][NodeSequence[n]] + Duration_Time[Current_Tour[Insertion[Inserted_Sequence[0]].InsertionPos1 - 1][k]];

					float c = FirIn.Route_Arrive_Time[Current_Tour[Insertion[Inserted_Sequence[0]].InsertionPos2 - 1][k]][k] + T[Current_Tour[Insertion[Inserted_Sequence[0]].InsertionPos2 - 1][k]][NodeSequence[n] + nc] + Duration_Time[Current_Tour[Insertion[Inserted_Sequence[0]].InsertionPos2 - 1][k]];
					if (c <= starttimewindow[NodeSequence[n] + nc][k])
					{
						FirIn.Route_Arrive_Time[NodeSequence[n] + nc][k] = starttimewindow[NodeSequence[n] + nc][k];
						for (int m = 0; m < nm; m++)
						{
							if (starttimewindow[NodeSequence[n] + nc][k] <= FirIn.Route_Arrive_Time[NodeSequence[n]][k] + A_time[NodeSequence[n]][Sort_Interval[m]]
								&& FirIn.Route_Arrive_Time[NodeSequence[n]][k] + A_time[NodeSequence[n]][Sort_Interval[m]] <= endtimewindow[NodeSequence[n] + nc][k] - Duration_Time[NodeSequence[n] + nc])
							{
								FirIn.Route_Arrive_Time[NodeSequence[n] + nc][k] = FirIn.Route_Arrive_Time[NodeSequence[n]][k] + A_time[NodeSequence[n]][Sort_Interval[m]];
								break;
							}
						}
					}
					else if (c <= endtimewindow[NodeSequence[n] + nc][k])
					{

						FirIn.Route_Arrive_Time[NodeSequence[n] + nc][k] = c;
						for (int m = 0; m < nm; m++)
						{
							if ((c <= FirIn.Route_Arrive_Time[NodeSequence[n]][k] + A_time[NodeSequence[n]][Sort_Interval[m]])
								&& (FirIn.Route_Arrive_Time[NodeSequence[n]][k] + A_time[NodeSequence[n]][Sort_Interval[m]] <= endtimewindow[NodeSequence[n] + nc][k] - Duration_Time[NodeSequence[n] + nc]))
							{
								FirIn.Route_Arrive_Time[NodeSequence[n] + nc][k] = FirIn.Route_Arrive_Time[NodeSequence[n]][k] + A_time[NodeSequence[n]][Sort_Interval[m]];
								break;
							}
						}
					}

					//////选择各节点最终插入的位置的访问时间间隔
					FirIn.Gap_Time[NodeSequence[n]] = FirIn.Route_Arrive_Time[NodeSequence[n] + nc][k] - FirIn.Route_Arrive_Time[NodeSequence[n]][k];

					//////选择节点最终插入的位置的收益
					FirIn.Node_Profit[NodeSequence[n]] = Insertion[Inserted_Sequence[0]].Increase_Profit;


					//更新最新的位置数据，为了下一轮的继续插入	
					for (int p = 0; p < 2 * nc + 2; p++)
					{
						Temp_Tour[p][k] = Current_Tour[p][k];
						Tour[p][k] = Current_Tour[p][k];
						Tour1[p][k] = Current_Tour[p][k];
					}

					//计算当前路线的长度，利于后面计算各点可插入的位置
					Node_Number_In_Tour[k] += 2;
				}
				if ((Insertion[Inserted_Sequence[0]].Increase_Profit < Insertion2[Inserted_Sequence2[0]].Increase_Profit || FeasibleInsertion == 0) && (FeasibleInsertion2 != 0))
				{
					int k = Insertion2[Inserted_Sequence2[0]].Insertiontour;
					int k1 = Insertion2[Inserted_Sequence2[0]].Insertiontour1;
					for (int p = 0; p < Insertion2[Inserted_Sequence2[0]].InsertionPos1; p++)
						Current_Tour[p][k] = Tour1[p][k];
					Current_Tour[Insertion2[Inserted_Sequence2[0]].InsertionPos1][k] = NodeSequence[n];
					for (int p = Insertion2[Inserted_Sequence2[0]].InsertionPos1 + 1; p < 2 * nc + 2; p++)
						Current_Tour[p][k] = Tour1[p - 1][k];

					for (int p = 0; p < Insertion2[Inserted_Sequence2[0]].InsertionPos2; p++)
						Current_Tour[p][k1] = Tour1[p][k1];
					Current_Tour[Insertion2[Inserted_Sequence2[0]].InsertionPos2][k1] = NodeSequence[n] + nc;
					for (int p = Insertion2[Inserted_Sequence2[0]].InsertionPos2 + 1; p < 2 * nc + 2; p++)
						Current_Tour[p][k1] = Tour1[p - 1][k1];

					/////////选择节点最终插入的位置的开始时间
					if (FirIn.Route_Arrive_Time[Current_Tour[Insertion2[Inserted_Sequence2[0]].InsertionPos1 - 1][k]][k] + T[Current_Tour[Insertion2[Inserted_Sequence2[0]].InsertionPos1 - 1][k]][NodeSequence[n]] + Duration_Time[Current_Tour[Insertion2[Inserted_Sequence2[0]].InsertionPos1 - 1][k]] <= starttimewindow[NodeSequence[n]][k])
						FirIn.Route_Arrive_Time[NodeSequence[n]][k] = starttimewindow[NodeSequence[n]][k];
					else if (FirIn.Route_Arrive_Time[Current_Tour[Insertion2[Inserted_Sequence2[0]].InsertionPos1 - 1][k]][k] + T[Current_Tour[Insertion2[Inserted_Sequence2[0]].InsertionPos1 - 1][k]][NodeSequence[n]] + Duration_Time[Current_Tour[Insertion2[Inserted_Sequence2[0]].InsertionPos1 - 1][k]] <= endtimewindow[NodeSequence[n]][k])
						FirIn.Route_Arrive_Time[NodeSequence[n]][k] = FirIn.Route_Arrive_Time[Current_Tour[Insertion2[Inserted_Sequence2[0]].InsertionPos1 - 1][k]][k] + T[Current_Tour[Insertion2[Inserted_Sequence2[0]].InsertionPos1 - 1][k]][NodeSequence[n]] + Duration_Time[Current_Tour[Insertion2[Inserted_Sequence2[0]].InsertionPos1 - 1][k]];

					float d = FirIn.Route_Arrive_Time[Current_Tour[Insertion2[Inserted_Sequence2[0]].InsertionPos2 - 1][k1]][k1] + T[Current_Tour[Insertion2[Inserted_Sequence2[0]].InsertionPos2 - 1][k1]][NodeSequence[n] + nc] + Duration_Time[Current_Tour[Insertion2[Inserted_Sequence2[0]].InsertionPos2 - 1][k1]];
					if (d <= starttimewindow[NodeSequence[n] + nc][k1])
					{
						FirIn.Route_Arrive_Time[NodeSequence[n] + nc][k1] = starttimewindow[NodeSequence[n] + nc][k1];
						for (int m = 0; m < nm; m++)
						{
							if ((starttimewindow[NodeSequence[n] + nc][k1] <= FirIn.Route_Arrive_Time[NodeSequence[n]][k] + A_time[NodeSequence[n]][Sort_Interval[m]])
								&& (FirIn.Route_Arrive_Time[NodeSequence[n]][k] + A_time[NodeSequence[n]][Sort_Interval[m]] <= endtimewindow[NodeSequence[n] + nc][k1] - Duration_Time[NodeSequence[n] + nc]))
							{
								FirIn.Route_Arrive_Time[NodeSequence[n] + nc][k1] = FirIn.Route_Arrive_Time[NodeSequence[n]][k] + A_time[NodeSequence[n]][Sort_Interval[m]];
								break;
							}
						}
					}
					else if (d <= endtimewindow[NodeSequence[n] + nc][k1] - Duration_Time[NodeSequence[n] + nc])
					{
						FirIn.Route_Arrive_Time[NodeSequence[n] + nc][k1] = d;
						for (int m = 0; m < nm; m++)
						{
							if ((d <= (FirIn.Route_Arrive_Time[NodeSequence[n]][k] + A_time[NodeSequence[n]][Sort_Interval[m]])) &&
								((FirIn.Route_Arrive_Time[NodeSequence[n]][k] + A_time[NodeSequence[n]][Sort_Interval[m]]) <= (endtimewindow[NodeSequence[n] + nc][k1] - Duration_Time[NodeSequence[n] + nc])))
							{
								FirIn.Route_Arrive_Time[NodeSequence[n] + nc][k1] = FirIn.Route_Arrive_Time[NodeSequence[n]][k] + A_time[NodeSequence[n]][Sort_Interval[m]];
								break;
							}
						}
					}

					//////选择各节点最终插入的位置的访问时间间隔
					FirIn.Gap_Time[NodeSequence[n]] = FirIn.Route_Arrive_Time[NodeSequence[n] + nc][k1] - FirIn.Route_Arrive_Time[NodeSequence[n]][k];

					//////选择节点最终插入的位置的收益
					FirIn.Node_Profit[NodeSequence[n]] = Insertion2[Inserted_Sequence2[0]].Increase_Profit;

					//更新最新的位置数据，为了下一轮的继续插入	
					for (int p = 0; p < 2 * nc + 2; p++)
					{
						Temp_Tour[p][k] = Current_Tour[p][k];
						Tour[p][k] = Current_Tour[p][k];
						Tour1[p][k] = Current_Tour[p][k];
					}
					for (int p = 0; p < 2 * nc + 2; p++)
					{
						Temp_Tour[p][k1] = Current_Tour[p][k1];
						Tour[p][k1] = Current_Tour[p][k1];
						Tour1[p][k1] = Current_Tour[p][k1];
					}

					//计算当前路线的长度，利于后面计算各点可插入的位置
					Node_Number_In_Tour[k] += 1;
					Node_Number_In_Tour[k1] += 1;
				}

				//把插入的节点从剩余未插入节点序列给删除掉
				for (int p = n; p <= Node_Number; p++)
				{
					if (NodeSequence[Node_Number] == NodeSequence[n])//如果是最后一个!那就直接赋值为-
						NodeSequence[Node_Number] = -1;
					else
						NodeSequence[p] = NodeSequence[p + 1];
				}
				Node_Number = Node_Number - 1;
			}
			else
				n++;

				// 释放内存
				delete[] Sort_Interval;
				Sort_Interval = nullptr;
		}
	}
	//保留当前路线的顺序
	for (int k = 0; k < nt; k++)
	{
		for (int p = 0; p < 2 * nc + 2; p++)
		{
			FirIn.Route_Visit_Nodes[k][p] = Tour1[p][k];
		}
	}

	//////计算初始收益
	for (int n = 1; n <= nc; n++)
		FirIn.Profit = FirIn.Profit + FirIn.Node_Profit[n];

	//将初始路线,收益，时间，总收益赋值给当前解Current
	for (int k = 0; k < nt; k++)
	{
		for (int p = 0; p < 2 * nc + 2; p++)
			Current.Route_Visit_Nodes[k][p] = FirIn.Route_Visit_Nodes[k][p];
	}
	for (int k = 0; k < nt; k++)
	{
		for (int n = 0; n < 2 * nc + 2; n++)
			Current.Route_Arrive_Time[n][k] = FirIn.Route_Arrive_Time[n][k];
	}
	for (int n = 1; n <= nc; n++)
	{
		Current.Gap_Time[n] = FirIn.Gap_Time[n];
		Current.Node_Profit[n] = FirIn.Node_Profit[n];
	}
	Current.Profit = FirIn.Profit;

	//将初始路线,收益，时间，总收益赋值给新解New
	for (int k = 0; k < nt; k++)
	{
		for (int p = 0; p < 2 * nc + 2; p++)
			New.Route_Visit_Nodes[k][p] = Current.Route_Visit_Nodes[k][p];
	}
	for (int k = 0; k < nt; k++)
	{
		for (int n = 0; n < 2 * nc + 2; n++)
			New.Route_Arrive_Time[n][k] = Current.Route_Arrive_Time[n][k];
	}
	for (int n = 1; n <= nc; n++)
	{
		New.Gap_Time[n] = Current.Gap_Time[n];
		New.Node_Profit[n] = Current.Node_Profit[n];
	}

	return FirIn.Profit;
}

//第三种初始解构造方法，为每一对节点依次选择最好的插入位置，还尽可能多的插入节点。最复杂的，效果最好的
float InitialSolution3(Solution&FirIn, Solution&New, Solution& Current, float Greed)
{
	int All_Node_Number = nc * 2;
	for (int i = 1; i <= All_Node_Number; i++)//可修改i<5,对一半乱序交换即可
	{
		int j = 1;
		while (j == i)
		{
			srand(time(NULL));
			j = Random(All_Node_Number - 1) + 1;//可以自己构造一个其他的函数，保证与10求余后不与原位置相同即可
		}
		//cout << "weizhi" << j << endl;
		int temp = NodeSequence[i];
		NodeSequence[i] = NodeSequence[j];
		NodeSequence[j] = temp;
	}


	//标记每个节点是否完成访问
	bool* feasible = new bool[All_Node_Number+1];
	for (int i = 1; i <= All_Node_Number; i++)
		feasible[i] = 0;

	for (int g = 0; g < 10; g++)
	{
		for (int n = 1; n <= All_Node_Number; n++)//Node_Number和NodeSequence一直再更新变化着！
		{
			int z2 = NodeSequence[n];//节点编号

			if (z2 == -1)
				break;//没有可访问节点了
			if (feasible[z2] == 1)
				continue;//该节点已经被访问过了


			//大于贪婪因子的节点才可以考虑插入
			//if (nod[z2].Grade <= 0)
				//continue;
			

			//如果这是第一次访问节点，那么找到可行的位置即插入
			if (z2 <= Node_Number)
			{
				for (int k = 0; k < nt; k++)
				{
					for (int i = 1; i < Node_Number_In_Tour[k]; i++)////节点对中的第一次访问节点的插入位置
					{
						int z1 = FirIn.Route_Visit_Nodes[k][i - 1];
						int z3 = FirIn.Route_Visit_Nodes[k][i];

						//根据时间窗口，计算这一对节点的开始访问时间
						float temp = 0;
						if (FirIn.Route_Arrive_Time[z1][k] + T[z1][z2] + Duration_Time[z1] <= starttimewindow[z2][k])
							temp = starttimewindow[z2][k];
						else if (FirIn.Route_Arrive_Time[z1][k] + T[z1][z2] + Duration_Time[z1] <= (endtimewindow[z2][k] - Duration_Time[z2]))
							temp = FirIn.Route_Arrive_Time[z1][k] + T[z1][z2] + Duration_Time[z1];
						else
							continue;

						/////////////如果节点不影响路线下一节点的开始访问时间，则记录
						if ((temp + T[z2][z3] + Duration_Time[z2]) <= FirIn.Route_Arrive_Time[z3][k])
						{
							//选择节点最终插入的位置的开始时间
							FirIn.Route_Arrive_Time[z2][k] = temp;

							//////为节点腾出位置
							for (int p = 2 * Max_Node + 1; p >= i + 1; p--)
								FirIn.Route_Visit_Nodes[k][p] = FirIn.Route_Visit_Nodes[k][p - 1];
							FirIn.Route_Visit_Nodes[k][i] = z2;
							//计算当前路线的长度，利于后面计算各点可插入的位置
							Node_Number_In_Tour[k] += 1;

							feasible[z2] = 1;
							break;
						}
					}
					if (feasible[z2] == 1)
						break;
				}
			}
			//这是第二次访问节点
			else
			{
				if (feasible[z2 - Node_Number] == 0)
					continue;//该节点的前一次访问没有完成，那么第二次访问也不去执行

				//将该节点所有时间段进行从高到低排序
				int *Sort_Interval = new int[nm];
				for (int m = 0; m < nm; m++)		
					Sort_Interval[m] = m;
				
				for (int m = 0; m < nm; m++)
				{
					for (int m1 = nm - 1; m1 > m; m1--)
					{
						if (Profit[z2 - Node_Number][Sort_Interval[m]] < Profit[z2 - Node_Number][Sort_Interval[m1]])
						{
							int temp = Sort_Interval[m];
							Sort_Interval[m] = Sort_Interval[m1];
							Sort_Interval[m1] = temp;
						}
					}
				}

				//找到第一次访问节点的开始时间是多少
				float First_Time = 0.0;
				for (int k = 0; k < nt; k++)
				{
					if (FirIn.Route_Arrive_Time[z2 - Node_Number][k] > 0)
					{
						First_Time = FirIn.Route_Arrive_Time[z2 - Node_Number][k];
						break;
					}
				}

				//遍历所有时间段，寻找使得收益最大的位置
				for (int m = 0; m < nm; m++)
				{
					//在这个时间段m下，第二次访问的开始时间必是[begin，end]
					float begin = First_Time + A_time[z2 - Node_Number][Sort_Interval[m]];
					float end = First_Time + B_time[z2 - Node_Number][Sort_Interval[m]];

					if (Profit[z2 - Node_Number][Sort_Interval[m]] <= 0)
						continue;

					//遍历所有车，所有位置，寻找能够获得时间段m的收益的
					for (int k = 0; k < nt; k++)
					{
						for (int i = 1; i < Node_Number_In_Tour[k]; i++)////节点对中的第一次访问节点的插入位置
						{
							int z1 = FirIn.Route_Visit_Nodes[k][i - 1];
							int z3 = FirIn.Route_Visit_Nodes[k][i];
							//根据时间窗口，计算节点的开始访问时间
							float temp = 0;
							if (FirIn.Route_Arrive_Time[z1][k] + T[z1][z2] + Duration_Time[z1] <= starttimewindow[z2][k])
								temp = starttimewindow[z2][k];
							else if (FirIn.Route_Arrive_Time[z1][k] + T[z1][z2] + Duration_Time[z1] <= (endtimewindow[z2][k] - Duration_Time[z2]))
								temp = FirIn.Route_Arrive_Time[z1][k] + T[z1][z2] + Duration_Time[z1];
							else
								continue;

							if (temp < begin && begin <= (endtimewindow[z2][k] - Duration_Time[z2]))
								temp = begin;//取最早
							else if (temp >= begin && temp<=end&temp <= (endtimewindow[z2][k] - Duration_Time[z2]))
								temp = temp;
							else
								continue;

							////如果节点不影响路线下一节点的开始访问时间，则记录
							if ((temp + T[z2][z3] + Duration_Time[z2]) <= FirIn.Route_Arrive_Time[z3][k])
							{
								//选择节点最终插入的位置的开始时间
								FirIn.Route_Arrive_Time[z2][k] = temp;

								//////为节点腾出位置
								for (int p = 2 * Max_Node + 1; p >= i + 1; p--)
									FirIn.Route_Visit_Nodes[k][p] = FirIn.Route_Visit_Nodes[k][p - 1];
								FirIn.Route_Visit_Nodes[k][i] = z2;
								//计算当前路线的长度，利于后面计算各点可插入的位置
								Node_Number_In_Tour[k] += 1;

								feasible[z2] = 1;
								break;
							}
						}
						if (feasible[z2] == 1)
							break;
					}
					if (feasible[z2] == 1)
						break;


					//如果第一种处理方法无效，那么考虑第二种情形，替换第一次访问的那些节点
					if (feasible[z2] == 0)
					{
						//遍历所有车辆，位置
						for (int k = 0; k < nt; k++)
						{
							for (int i = 1; i < Node_Number_In_Tour[k]; i++)
							{
								int z1 = FirIn.Route_Visit_Nodes[k][i - 1];
								int z3 = FirIn.Route_Visit_Nodes[k][i];
								//计算开始访问时间
								float temp = 0;
								if (FirIn.Route_Arrive_Time[z1][k] + T[z1][z2] + Duration_Time[z1] <= starttimewindow[z2][k])
									temp = starttimewindow[z2][k];
								else if (FirIn.Route_Arrive_Time[z1][k] + T[z1][z2] + Duration_Time[z1] <= (endtimewindow[z2][k] - Duration_Time[z2]))
									temp = FirIn.Route_Arrive_Time[z1][k] + T[z1][z2] + Duration_Time[z1];
								else
									continue;

								if (temp < begin && begin < (endtimewindow[z2][k] - Duration_Time[z2]))
									temp = begin;//取最早
								else if(temp > begin && temp <= end && temp <= (endtimewindow[z2][k] - Duration_Time[z2]))
									temp = temp;
								else
									continue;

								////如果找不到不影响下一节点的，那么考虑是否替换
								if ((temp + T[z2][z3] + Duration_Time[z2]) > FirIn.Route_Arrive_Time[z3][k])
								{
									//已经是固定节点，就不动
									if(z3 <= Node_Number && feasible[z3] == 1 && feasible[z3 + Node_Number] == 1)
										continue;
									if(z3 > Node_Number)
										continue;

									int z4 = FirIn.Route_Visit_Nodes[k][i + 1];
									if ((temp + T[z2][z4] + Duration_Time[z2]) <= FirIn.Route_Arrive_Time[z4][k])
									{
										/////替换节点
										FirIn.Route_Arrive_Time[z3][k] = 0;
										FirIn.Route_Arrive_Time[z2][k] = temp;
										FirIn.Route_Visit_Nodes[k][i] = z2;
										feasible[z3] = 0;
										feasible[z2] = 1;
										break;
									}
								}
							}
							if (feasible[z2] == 1)
								break;
						}
						if (feasible[z2] == 1)
							break;
					}

					//第三种情形，选择下一段收益
					if (feasible[z2] == 0)
					{

					}
				}
			}
		}
	}

	//如果只完成一次访问，则删除
	for (int n = 1; n <= Node_Number; n++)
	{
		bool Is_Visited1 = 0;
		bool Is_Visited2 = 0;

		if (feasible[n] == 1)
			Is_Visited1 = 1;
		if (feasible[n + Node_Number] == 1)
			Is_Visited2 = 1;

		if ((Is_Visited1 == 1 && Is_Visited2 == 1) || (Is_Visited1 == 0 && Is_Visited2 == 0))
			continue;

		for (int k = 0; k < nt; k++)
		{
			FirIn.Route_Arrive_Time[n][k] = 0;
			FirIn.Route_Arrive_Time[n + Node_Number][k] = 0;
			for (int i = 1; i < Node_Number_In_Tour[k]; i++)
			{
				if ((FirIn.Route_Visit_Nodes[k][i] == n)
					|| (FirIn.Route_Visit_Nodes[k][i] == (n + Node_Number)))
				{
					for (int i1 = i; i1 < Node_Number_In_Tour[k]; i1++)
					{
						FirIn.Route_Visit_Nodes[k][i1] = FirIn.Route_Visit_Nodes[k][i1 + 1];
					}
					Node_Number_In_Tour[k] -= 1;
				}
			}
		}
	}
	//更新现在的NodeSequence[n]
	//计算未插入节点的数目和序列
	for (int p = 1; p <= nc; p++)
		NodeSequence[p] = p;

	for (int p = nc + 1; p <= 2 * nc + 1; p++)
		NodeSequence[p] = -1;

	Node_Number = nc;
	for (int n = nc; n >= 1; n--)
	{
		if (FirIn.Node_Profit[n] != 0)
		{
			for (int p = n; p <= nc; p++)
				NodeSequence[p] = NodeSequence[p + 1];
			Node_Number = Node_Number - 1;
		}
	}

	// 释放内存
	delete[] feasible;
	feasible = nullptr;

	//cout << All_Node_Number << endl;
	//for (int n = 1; n <= All_Node_Number; n++)
		//cout << setw(3) << NodeSequence[n];
	Node_Number = All_Node_Number;
	//cout << Node_Number << endl;


	//计算收益
	for (int i = 1; i <= nc; i++)
	{
		bool a = 1;
		for (int t = 0; t < nt; t++)
		{
			for (int t1 = 0; t1 < nt; t1++)
			{
				if (FirIn.Route_Arrive_Time[i + nc][t1] > 0 && FirIn.Route_Arrive_Time[i][t] > 0)
				{
					FirIn.Gap_Time[i] = FirIn.Route_Arrive_Time[i + nc][t1] - FirIn.Route_Arrive_Time[i][t];
					a = 0;
					break;
				}

			}
			if (a == 0)
				break;
		}
	}
	for (int i = 1; i <= nc; i++)
	{
		for (int m = 0; m < nm; m++)
		{
			if ((FirIn.Gap_Time[i] >= A_time[i][m]) && (FirIn.Gap_Time[i] < B_time[i][m]))
			{
				FirIn.Node_Profit[i] = Profit[i][m];
				break;
			}
		}
		FirIn.Profit += FirIn.Node_Profit[i];
	}

	//将初始路线,收益，时间，总收益赋值给当前解Current
	for (int k = 0; k < nt; k++)
	{
		for (int p = 0; p < 2 * nc + 2; p++)
			Current.Route_Visit_Nodes[k][p] = FirIn.Route_Visit_Nodes[k][p];
	}
	for (int k = 0; k < nt; k++)
	{
		for (int n = 0; n < 2 * nc + 2; n++)
			Current.Route_Arrive_Time[n][k] = FirIn.Route_Arrive_Time[n][k];
	}
	for (int n = 1; n <= nc; n++)
	{
		Current.Gap_Time[n] = FirIn.Gap_Time[n];
		Current.Node_Profit[n] = FirIn.Node_Profit[n];
	}
	Current.Profit = FirIn.Profit;

	//将初始路线,收益，时间，总收益赋值给新解New
	for (int k = 0; k < nt; k++)
	{
		for (int p = 0; p < 2 * nc + 2; p++)
			New.Route_Visit_Nodes[k][p] = Current.Route_Visit_Nodes[k][p];
	}
	for (int k = 0; k < nt; k++)
	{
		for (int n = 0; n < 2 * nc + 2; n++)
			New.Route_Arrive_Time[n][k] = Current.Route_Arrive_Time[n][k];
	}
	for (int n = 1; n <= nc; n++)
	{
		New.Gap_Time[n] = Current.Gap_Time[n];
		New.Node_Profit[n] = Current.Node_Profit[n];
	}
	return FirIn.Profit;
}

//第4种初始解构造方法
float InitialSolution4(Solution& FirIn, Solution& New, Solution& Current, float Greed)
{
	for (int i = 1; i <= Node_Number; i++)//可修改i<5,对一半乱序交换即可
	{
		int j = 1;
		while (j == i)
		{
			srand(time(NULL));
			j = Random(nc - 1) + 1;//可以自己构造一个其他的函数，保证与10求余后不与原位置相同即可
		}
		//cout << "weizhi" << j << endl;
		int temp = NodeSequence[i];
		NodeSequence[i] = NodeSequence[j];
		NodeSequence[j] = temp;
	}

	All_Node_Number = Node_Number * 2;

	///////////////////////////////////////////////////////////开始求解，插入节点对
	for (int w = 0; w < 2 * nc; w++)//要找2*nc次最好的节点插入
	{
		double max_zhibiao = -100000;//
		int optimal_NodeNO1 = 0;//节点编号
		int optimal_InsertionPos1 = 0;//当前点插入当前路线中的位置编号
		int optimal_Insertiontour = 0;//当前点插入的路线编号
		double optimal_Profit = 0;
		int optimal_time = 0;
		double optimal_Gap_time = 0;
		for (int n = 1; n <= All_Node_Number; n++)//遍历节点
		{
			if (NodeSequence[n] == -1)//已经插入完成了
				continue;

			//大于贪婪因子的节点才可以考虑插入
			if (nod[n].Grade <= 0)
			{
				//n++;
				//continue;
			}

			for (int k = 0; k < nt; k++)//遍历每辆车
			{
				for (int i = 1; i < Node_Number_In_Tour[k]; i++)//遍历每个位置
				{
					//////为这一对节点腾出位置
					for (int p = 0; p < i; p++)
						Temp_Tour[p][k] = Tour[p][k];
					Temp_Tour[i][k] = NodeSequence[n];
					for (int p = i + 1; p < 2 * nc + 2; p++)
						Temp_Tour[p][k] = Tour[p - 1][k];

					//////计算这一对节点的开始访问时间
					double Arrive_Time = 0;
					double Gap_Time = 0;
					if (FirIn.Route_Arrive_Time[Temp_Tour[i - 1][k]][k] + T[Temp_Tour[i - 1][k]][NodeSequence[n]] + Duration_Time[Temp_Tour[i - 1][k]] <= starttimewindow[NodeSequence[n]][k])
						Arrive_Time = starttimewindow[NodeSequence[n]][k];
					else if (FirIn.Route_Arrive_Time[Temp_Tour[i - 1][k]][k] + T[Temp_Tour[i - 1][k]][NodeSequence[n]] + Duration_Time[Temp_Tour[i - 1][k]] <= (endtimewindow[NodeSequence[n]][k] - Duration_Time[NodeSequence[n]]))
						Arrive_Time = FirIn.Route_Arrive_Time[Temp_Tour[i - 1][k]][k] + T[Temp_Tour[i - 1][k]][NodeSequence[n]] + Duration_Time[Temp_Tour[i - 1][k]];

					/////////////如果这对节点都满足时间窗约束并且不影响路线下一节点的开始访问时间，则记录，计算收益，以备比较	
					if (Arrive_Time >= starttimewindow[NodeSequence[n]][k] && (Arrive_Time <= endtimewindow[NodeSequence[n]][k] - Duration_Time[NodeSequence[n]]))
					{
						if (Arrive_Time + T[NodeSequence[n]][Temp_Tour[i + 1][k]] + Duration_Time[NodeSequence[n]] <= FirIn.Route_Arrive_Time[Temp_Tour[i + 1][k]][k])
						{
							double end = FirIn.Route_Arrive_Time[Temp_Tour[i + 1][k]][k] - T[NodeSequence[n]][Temp_Tour[i + 1][k]] - Duration_Time[NodeSequence[n]];
							if (end > (endtimewindow[NodeSequence[n]][k] - Duration_Time[NodeSequence[n]]))
								end = endtimewindow[NodeSequence[n]][k] - Duration_Time[NodeSequence[n]];
							for (int ti = Arrive_Time; ti <= end; ti++)
							{
								double temp_zhibiao1 = 0;
								double temp_zhibiao2 = 0;
								double tim = ti - Arrive_Time;
								if (NodeSequence[n] <= nc)
								{
									//计算新增距离
									temp_zhibiao1 -= tim - T[Temp_Tour[i - 1][k]][NodeSequence[n]] - T[NodeSequence[n]][Temp_Tour[i + 1][k]] + T[Temp_Tour[i - 1][k]][Temp_Tour[i + 1][k]];

									//计算新增收益
									double time = 0;
									for (int kk = 0; kk < nt; kk++)
									{
										if (FirIn.Route_Arrive_Time[NodeSequence[n] + nc][kk] > 0)
											time = FirIn.Route_Arrive_Time[NodeSequence[n] + nc][kk];
									}
									//if (time != 0)
										Gap_Time = ti - time;

									if (Gap_Time < 0)
										Gap_Time = time - ti;

									for (int m = 0; m < nm; m++)
									{
										if (Gap_Time >= A_time[NodeSequence[n]][m] && Gap_Time < B_time[NodeSequence[n]][m])
											temp_zhibiao2 += Profit[NodeSequence[n]][m];
									}
								}
								else
								{
									//计算新增距离
									temp_zhibiao1 -= tim - T[Temp_Tour[i - 1][k]][NodeSequence[n]] - T[NodeSequence[n]][Temp_Tour[i + 1][k]] + T[Temp_Tour[i - 1][k]][Temp_Tour[i + 1][k]];

									//计算新增收益
									double time = 0;
									for (int kk = 0; kk < nt; kk++)
									{
										if (FirIn.Route_Arrive_Time[NodeSequence[n] - nc][kk] > 0)
											time = FirIn.Route_Arrive_Time[NodeSequence[n] - nc][kk];
									}
									//if (time != 0)
										Gap_Time = ti - time;

									if (Gap_Time < 0)
										Gap_Time = time - ti;

									for (int m = 0; m < nm; m++)
									{
										if (Gap_Time >= A_time[NodeSequence[n] - nc][m] && Gap_Time < B_time[NodeSequence[n] - nc][m])
											temp_zhibiao2 += Profit[NodeSequence[n] - nc][m];
									}
								}
								temp_zhibiao2 /= 50;
								temp_zhibiao1 /= 100000;
								if ((temp_zhibiao1 + temp_zhibiao2) > max_zhibiao)
								{
									optimal_NodeNO1 = NodeSequence[n];//节点编号
									optimal_InsertionPos1 = i;//当前点插入当前路线中的位置编号
									optimal_Insertiontour = k;//当前点插入的路线编号
									optimal_Profit = temp_zhibiao2 * 50;
									optimal_time = ti;
									optimal_Gap_time = Gap_Time;
									max_zhibiao = temp_zhibiao1 + temp_zhibiao2;
								}
							}
						}
					}
				}
			}
		}
		//选指标最大的节点和车辆和位置，插入
		if (max_zhibiao != -100000)
		{
			int n = optimal_NodeNO1;
			int i = optimal_InsertionPos1;
			int k = optimal_Insertiontour;
			int ti = optimal_time;

			//////为这节点腾出位置
			for (int p = 2 * nc + 1; p > i; p--)
				FirIn.Route_Visit_Nodes[k][p] = FirIn.Route_Visit_Nodes[k][p - 1];
			FirIn.Route_Visit_Nodes[k][i] = n;


			/////////选择节点最终插入的位置的开始时间
			//if (FirIn.Route_Arrive_Time[FirIn.Route_Visit_Nodes[k][i - 1]][k] + T[FirIn.Route_Visit_Nodes[k][i - 1]][n] + Duration_Time[FirIn.Route_Visit_Nodes[k][i - 1]] <= starttimewindow[n][k])
			FirIn.Route_Arrive_Time[n][k] = ti;
			//else if (FirIn.Route_Arrive_Time[FirIn.Route_Visit_Nodes[k][i - 1]][k] + T[FirIn.Route_Visit_Nodes[k][i - 1]][n] + Duration_Time[FirIn.Route_Visit_Nodes[k][i - 1]] <= (endtimewindow[n][k] - Duration_Time[n]))
				//FirIn.Route_Arrive_Time[n][k] = FirIn.Route_Arrive_Time[FirIn.Route_Visit_Nodes[k][i - 1]][k] + T[FirIn.Route_Visit_Nodes[k][i - 1]][n] + Duration_Time[FirIn.Route_Visit_Nodes[k][i - 1]];

			FirIn.Node_Profit[n] = optimal_Profit;
			FirIn.Gap_Time[n] = optimal_Gap_time;
			//更新最新的位置数据，为了下一轮的继续插入	
			for (int p = 0; p < 2 * nc + 2; p++)
			{
				Tour[p][k] = FirIn.Route_Visit_Nodes[k][p];
			}

			//计算当前路线的长度，利于后面计算各点可插入的位置
			Node_Number_In_Tour[k] += 1;

			//把插入的节点从剩余未插入节点序列给删除掉
			for (int p = 1; p <= All_Node_Number; p++)
			{
				if (NodeSequence[p] == n)
					NodeSequence[p] = -1;
			}
		}
	}


	//把插入的节点从剩余未插入节点序列给删除掉
	for (int p = 1; p <= All_Node_Number; p++)
	{
		if (NodeSequence[p] == -1 || NodeSequence[p] > Node_Number)//如果是最后一个!那就直接赋值为-
		{
			for (int p1 = p; p1 <= All_Node_Number; p1++)
				NodeSequence[p1] = NodeSequence[p1 + 1];

		}
		if (NodeSequence[p] == -1 && NodeSequence[p] <= Node_Number)
			Node_Number = Node_Number - 1;
	}

	for (int t = 0; t < nt; t++)
	{
		for (int p = 1; p < (Node_Number_In_Tour[t] - 1); p++)
		{
			for (int tt = t; tt < nt; tt++)
			{
				int p1 = 1;
				if (tt == t)
					p1 = p + 1;
				for (int pp = p1; pp < (Node_Number_In_Tour[tt] - 1); pp++)
				{
					if (FirIn.Route_Visit_Nodes[t][p] == FirIn.Route_Visit_Nodes[tt][pp] + nc)
					{
						///////////将两个节点进行交换

						double tem = 0;
						tem = FirIn.Route_Arrive_Time[FirIn.Route_Visit_Nodes[tt][pp]][tt];
						FirIn.Route_Arrive_Time[FirIn.Route_Visit_Nodes[tt][pp]][t] = FirIn.Route_Arrive_Time[FirIn.Route_Visit_Nodes[t][p]][t];
						FirIn.Route_Arrive_Time[FirIn.Route_Visit_Nodes[t][p]][tt] = tem;
						if (tt != t)
						{
							FirIn.Route_Arrive_Time[FirIn.Route_Visit_Nodes[t][p]][t] = 0;
							FirIn.Route_Arrive_Time[FirIn.Route_Visit_Nodes[tt][pp]][tt] = 0;
						}
						int temp = 0;
						temp = FirIn.Route_Visit_Nodes[t][p];
						FirIn.Route_Visit_Nodes[t][p] = FirIn.Route_Visit_Nodes[tt][pp];
						FirIn.Route_Visit_Nodes[tt][pp] = temp;
						break;
					}
				}
			}
		}
	}

	for (int n = 1; n <= 2 * nc; n++)
	{
		if (FirIn.Node_Profit[n + nc] > 0)
		{
			FirIn.Gap_Time[n] = FirIn.Gap_Time[n + nc];
			FirIn.Node_Profit[n] = FirIn.Node_Profit[n + nc];
		}
	}

	//////计算初始收益
	for (int n = 1; n <= nc; n++)
		FirIn.Profit += FirIn.Node_Profit[n];


	//配对检查算子，若只有一次访问，则尝试插入缺失访问到使收益最大的位置，若无法合法插入，则标记为删除
	//peidui_check_Ini(New, Current, FirIn);

	//删除只访问一次的节点
	for (int n = 1; n <= nc; n++)
	{
		bool is = 0;
		bool is1 = 0;
		int route = 0;
		int position = 0;
		int route1 = 0;
		int position1 = 0;
		for (int t = 0; t < nt; t++)
		{
			for (int p = 0; p < 2 * nc + 2; p++)
			{
				if (FirIn.Route_Visit_Nodes[t][p] == -1)
					break;

				if (FirIn.Route_Visit_Nodes[t][p] == n)
				{
					is = 1;
					route = t;
					position = p;
				}
				if (FirIn.Route_Visit_Nodes[t][p] == n + nc)
				{
					is1 = 1;
					route1 = t;
					position1 = p;
				}
			}
		}

		if (is == 1 && is1 == 0)
		{
			//delete 第1次
			for (int p = position; p < 2 * nc + 2; p++)
				FirIn.Route_Visit_Nodes[route][p] = FirIn.Route_Visit_Nodes[route][p + 1];
			FirIn.Route_Arrive_Time[n][route] = 0;
			Node_Number_In_Tour[route] -= 1;
		}
		if (is == 0 && is1 == 1)
		{
			//delete 第2次
			for (int p = position1; p < 2 * nc + 2; p++)
				FirIn.Route_Visit_Nodes[route1][p] = FirIn.Route_Visit_Nodes[route1][p + 1];
			FirIn.Route_Arrive_Time[n + nc][route1] = 0;
			Node_Number_In_Tour[route1] -= 1;
		}
	}

	//计算未插入节点的数目和序列
	for (int p = 1; p <= nc; p++)
		NodeSequence[p] = p;

	for (int p = nc + 1; p <= Max_Node; p++)
		NodeSequence[p] = -1;

	Node_Number = nc;
	for (int n = nc; n >= 1; n--)
	{
		if (New.Node_Profit[n] != 0)
		{
			for (int p = n; p <= nc; p++)
				NodeSequence[p] = NodeSequence[p + 1];
			Node_Number = Node_Number - 1;
		}
	}

	//计算路线当前插入节点数
	for (int t = 0; t < nt; t++)
		Node_Number_In_Tour[t] = 0;
	for (int t = 0; t < nt; t++)
	{
		for (int i = 0; i < 2 * nc + 2; i++)
		{
			if (New.Route_Visit_Nodes[t][i] == -1)
				break;

			Node_Number_In_Tour[t]++;
		}
	}

	New.Profit = 0;
	for (int n2 = 1; n2 <= nc; n2++)
	{
		New.Profit += New.Node_Profit[n2];
	}

	//将初始路线,收益，时间，总收益赋值给当前解Current
	for (int k = 0; k < nt; k++)
	{
		for (int p = 0; p < 2 * nc + 2; p++)
			Current.Route_Visit_Nodes[k][p] = FirIn.Route_Visit_Nodes[k][p];
	}
	for (int k = 0; k < nt; k++)
	{
		for (int n = 0; n < 2 * nc + 2; n++)
			Current.Route_Arrive_Time[n][k] = FirIn.Route_Arrive_Time[n][k];
	}
	for (int n = 1; n <= nc; n++)
	{
		Current.Gap_Time[n] = FirIn.Gap_Time[n];
		Current.Node_Profit[n] = FirIn.Node_Profit[n];
	}
	Current.Profit = FirIn.Profit;

	//将初始路线,收益，时间，总收益赋值给新解New
	for (int k = 0; k < nt; k++)
	{
		for (int p = 0; p < 2 * nc + 2; p++)
			New.Route_Visit_Nodes[k][p] = Current.Route_Visit_Nodes[k][p];
	}
	for (int k = 0; k < nt; k++)
	{
		for (int n = 0; n < 2 * nc + 2; n++)
			New.Route_Arrive_Time[n][k] = Current.Route_Arrive_Time[n][k];
	}
	for (int n = 1; n <= nc; n++)
	{
		New.Gap_Time[n] = Current.Gap_Time[n];
		New.Node_Profit[n] = Current.Node_Profit[n];
	}
	return  FirIn.Profit;
}



//插入算子
void Insert_Operator(Solution& New, InsertionNode*Insertion3, InsertionNode*Insertion4)
{

	int p, t, k, i;

	for (t = 0; t < nt; t++)
	{
		for (p = 0; p < 2 * nc + 2; p++)//初始各路线各位置值为
		{
			Tour1[p][t] = New.Route_Visit_Nodes[t][p];
			Current_Tour[p][t] = New.Route_Visit_Nodes[t][p];
			Tour[p][t] = New.Route_Visit_Nodes[t][p];
			Temp_Tour[p][t] = New.Route_Visit_Nodes[t][p];
		}
	}
	

	///////////////////为剩余未插入节点寻找插入位置
	int n = 1;
	while (n <= Node_Number)//Node_Number和NodeSequence一直再更新变化着！
	{
		bool feasible = 0;

		if (NodeSequence[n] == -1)
			break;


		int FeasibleInsertion3 = 0;//节点对的可插入位置个数
		int FeasibleInsertion4 = 0;
		for (k = 0; k < nt; k++)
		{
			for (i = 1; i < Node_Number_In_Tour[k]; i++)////节点对中的第一次访问节点的插入位置
			{
				for (int kk = k + 1; kk < nt; kk++)
				{
					for (int j = 1; j < Node_Number_In_Tour[kk]; j++)//节点对第二次访问可插入位置
					{
						float Temp_Ratio = 0;//用来暂存收益

						//////为这一对节点腾出位置
						for (p = 0; p < i; p++)
							Temp_Tour[p][k] = Tour[p][k];
						Temp_Tour[i][k] = NodeSequence[n];
						for (p = i + 1; p < 2 * nc + 2; p++)
							Temp_Tour[p][k] = Tour[p - 1][k];

						for (p = 0; p < j; p++)
							Temp_Tour[p][kk] = Tour[p][kk];
						Temp_Tour[j][kk] = NodeSequence[n] + nc;
						for (p = j + 1; p < 2 * nc + 2; p++)
							Temp_Tour[p][kk] = Tour[p - 1][kk];

						//////计算这一对节点的开始访问时间
						if (New.Route_Arrive_Time[Temp_Tour[i - 1][k]][k] + T[Temp_Tour[i - 1][k]][NodeSequence[n]] + Duration_Time[Temp_Tour[i - 1][k]] <= starttimewindow[NodeSequence[n]][k])
							New.Route_Arrive_Time[NodeSequence[n]][k] = starttimewindow[NodeSequence[n]][k];
						else if (New.Route_Arrive_Time[Temp_Tour[i - 1][k]][k] + T[Temp_Tour[i - 1][k]][NodeSequence[n]] + Duration_Time[Temp_Tour[i - 1][k]] <= (endtimewindow[NodeSequence[n]][k]- Duration_Time[NodeSequence[n]]))
							New.Route_Arrive_Time[NodeSequence[n]][k] = New.Route_Arrive_Time[Temp_Tour[i - 1][k]][k] + T[Temp_Tour[i - 1][k]][NodeSequence[n]] + Duration_Time[Temp_Tour[i - 1][k]];

						float b = New.Route_Arrive_Time[Temp_Tour[j - 1][kk]][kk] + T[Temp_Tour[j - 1][kk]][NodeSequence[n] + nc] + Duration_Time[Temp_Tour[j - 1][kk]];
						if (b <= starttimewindow[NodeSequence[n] + nc][kk])
						{
							New.Route_Arrive_Time[NodeSequence[n] + nc][kk] = starttimewindow[NodeSequence[n] + nc][kk];
							/*for (int m = 0; m < nm; m++)
							{
								if (starttimewindow[NodeSequence[n] + nc][kk] <= New.Route_Arrive_Time[NodeSequence[n]][k] + A_time[NodeSequence[n]][Sort_Interval[m]]
									&& New.Route_Arrive_Time[NodeSequence[n]][k] + A_time[NodeSequence[n]][Sort_Interval[m]] <= endtimewindow[NodeSequence[n] + nc][kk] - Duration_Time[NodeSequence[n] + nc])
								{
									New.Route_Arrive_Time[NodeSequence[n] + nc][kk] = New.Route_Arrive_Time[NodeSequence[n]][k] + A_time[NodeSequence[n]][Sort_Interval[m]];
									break;
								}
							}*/
						}
						else if (b <= endtimewindow[NodeSequence[n] + nc][kk] - Duration_Time[NodeSequence[n] + nc])
						{
							New.Route_Arrive_Time[NodeSequence[n] + nc][kk] = b;
							/*for (int m = 0; m < nm; m++)
							{
								if (b <= New.Route_Arrive_Time[NodeSequence[n]][k] + A_time[NodeSequence[n]][Sort_Interval[m]] 
									&& New.Route_Arrive_Time[NodeSequence[n]][k] + A_time[NodeSequence[n]][Sort_Interval[m]] <= endtimewindow[NodeSequence[n] + nc][kk] - Duration_Time[NodeSequence[n] + nc])
								{
									New.Route_Arrive_Time[NodeSequence[n] + nc][kk] = New.Route_Arrive_Time[NodeSequence[n]][k] + A_time[NodeSequence[n]][Sort_Interval[m]];
									break;
								}
							}*/
						}

						/////////////如果这对节点都满足时间窗约束并且不影响路线下一节点的开始访问时间，则记录，计算收益，以备比较	
						if ((New.Route_Arrive_Time[NodeSequence[n]][k] >= starttimewindow[NodeSequence[n]][k]) && (New.Route_Arrive_Time[NodeSequence[n]][k] <= endtimewindow[NodeSequence[n]][k] - Duration_Time[NodeSequence[n]]) && (New.Route_Arrive_Time[NodeSequence[n] + nc][kk] >= starttimewindow[NodeSequence[n] + nc][kk]) && (New.Route_Arrive_Time[NodeSequence[n] + nc][kk] <= endtimewindow[NodeSequence[n] + nc][kk] - Duration_Time[NodeSequence[n] + nc]))
						{
							if ((New.Route_Arrive_Time[NodeSequence[n]][k] + T[NodeSequence[n]][Temp_Tour[i + 1][k]] + Duration_Time[NodeSequence[n]] <= New.Route_Arrive_Time[Temp_Tour[i + 1][k]][k]) && (New.Route_Arrive_Time[NodeSequence[n] + nc][kk] + T[NodeSequence[n] + nc][Temp_Tour[j + 1][kk]] + Duration_Time[NodeSequence[n] + nc] <= New.Route_Arrive_Time[Temp_Tour[j + 1][kk]][kk]))
							{
								New.Gap_Time[NodeSequence[n]] = New.Route_Arrive_Time[NodeSequence[n] + nc][kk] - New.Route_Arrive_Time[NodeSequence[n]][k];
								for (int m = 0; m < nm; m++)
								{
									if (New.Gap_Time[NodeSequence[n]] >= A_time[NodeSequence[n]][m] && New.Gap_Time[NodeSequence[n]] < B_time[NodeSequence[n]][m])
										Temp_Ratio = Profit[NodeSequence[n]][m];
								}

								Insertion4[FeasibleInsertion4].NodeNO1 = NodeSequence[n];//节点编号
								Insertion4[FeasibleInsertion4].NodeNO2 = NodeSequence[n] + nc; //节点编号
								Insertion4[FeasibleInsertion4].InsertionPos1 = i;//当前点插入当前路线中的位置编号
								Insertion4[FeasibleInsertion4].InsertionPos2 = j;//当前点插入当前路线中的位置编号
								Insertion4[FeasibleInsertion4].Increase_Profit = Temp_Ratio;//产生的增加收益
								Inserted_Sequence4[FeasibleInsertion4] = FeasibleInsertion4;  //用于后面排序
								Insertion4[FeasibleInsertion4].Insertiontour1 = kk;
								Insertion4[FeasibleInsertion4].Insertiontour = k;
								FeasibleInsertion4++;//计数
							}
						}
						New.Route_Arrive_Time[NodeSequence[n]][k] = 0.0;
						New.Route_Arrive_Time[NodeSequence[n] + nc][kk] = 0.0;
						New.Gap_Time[NodeSequence[n]] = -1;
					}
				}

				for (int j = i + 1; j < Node_Number_In_Tour[k] + 1; j++)//节点对第二次访问可插入位置
				{
					float Temp_Ratio = 0;//用来暂存收益
					//////为这一对节点腾出位置
					for (p = 0; p < i; p++)
						Temp_Tour[p][k] = Tour[p][k];
					Temp_Tour[i][k] = NodeSequence[n];
					for (p = i + 1; p < j; p++)
						Temp_Tour[p][k] = Tour[p - 1][k];
					Temp_Tour[j][k] = NodeSequence[n] + nc;
					for (p = j + 1; p < 2 * nc + 2; p++)
						Temp_Tour[p][k] = Tour[p - 2][k];

					//////计算这一对节点的开始访问时间
					if (New.Route_Arrive_Time[Temp_Tour[i - 1][k]][k] + T[Temp_Tour[i - 1][k]][NodeSequence[n]] + Duration_Time[Temp_Tour[i - 1][k]] <= starttimewindow[NodeSequence[n]][k])
						New.Route_Arrive_Time[NodeSequence[n]][k] = starttimewindow[NodeSequence[n]][k];
					else if (New.Route_Arrive_Time[Temp_Tour[i - 1][k]][k] + T[Temp_Tour[i - 1][k]][NodeSequence[n]] + Duration_Time[Temp_Tour[i - 1][k]] <= (endtimewindow[NodeSequence[n]][k]- Duration_Time[NodeSequence[n]]))
						New.Route_Arrive_Time[NodeSequence[n]][k] = New.Route_Arrive_Time[Temp_Tour[i - 1][k]][k] + T[Temp_Tour[i - 1][k]][NodeSequence[n]] + Duration_Time[Temp_Tour[i - 1][k]];

					float a = New.Route_Arrive_Time[Temp_Tour[j - 1][k]][k] + T[Temp_Tour[j - 1][k]][NodeSequence[n] + nc] + Duration_Time[Temp_Tour[j - 1][k]];
					if (a <= starttimewindow[NodeSequence[n] + nc][k])
					{
						New.Route_Arrive_Time[NodeSequence[n] + nc][k] = starttimewindow[NodeSequence[n] + nc][k];
						/*for (int m = 0; m < nm; m++)
						{
							if (starttimewindow[NodeSequence[n] + nc][k] <= New.Route_Arrive_Time[NodeSequence[n]][k] + A_time[NodeSequence[n]][Sort_Interval[m]]
								&& New.Route_Arrive_Time[NodeSequence[n]][k] + A_time[NodeSequence[n]][Sort_Interval[m]] <= endtimewindow[NodeSequence[n] + nc][k] - Duration_Time[NodeSequence[n] + nc]
								&& New.Route_Arrive_Time[NodeSequence[n]][k] + A_time[NodeSequence[n]][Sort_Interval[m]] + Duration_Time[NodeSequence[n]]+T[NodeSequence[n]+nc][Temp_Tour[j + 1][k]]<= New.Route_Arrive_Time[Temp_Tour[j + 1][k]][k])
							{
								New.Route_Arrive_Time[NodeSequence[n] + nc][k] = New.Route_Arrive_Time[NodeSequence[n]][k] + A_time[NodeSequence[n]][Sort_Interval[m]];
								break;
							}
						}*/					}
					else if (a <= endtimewindow[NodeSequence[n] + nc][k] - Duration_Time[NodeSequence[n] + nc])
					{
						New.Route_Arrive_Time[NodeSequence[n] + nc][k] = a;
						/*for (int m = 0; m < nm; m++)
						{
							if (a <= New.Route_Arrive_Time[NodeSequence[n]][k] + A_time[NodeSequence[n]][Sort_Interval[m]]
								&& New.Route_Arrive_Time[NodeSequence[n]][k] + A_time[NodeSequence[n]][Sort_Interval[m]] <= endtimewindow[NodeSequence[n] + nc][k] - Duration_Time[NodeSequence[n] + nc]
								&& New.Route_Arrive_Time[NodeSequence[n]][k] + A_time[NodeSequence[n]][Sort_Interval[m]] + Duration_Time[NodeSequence[n]] + T[NodeSequence[n] + nc][Temp_Tour[j + 1][k]] <= New.Route_Arrive_Time[Temp_Tour[j + 1][k]][k])

							{
								New.Route_Arrive_Time[NodeSequence[n] + nc][k] = New.Route_Arrive_Time[NodeSequence[n]][k] + A_time[NodeSequence[n]][Sort_Interval[m]];
								break;
							}
						}*/
					}

					/////////////如果这对节点都满足时间窗约束并且不影响路线下一节点的开始访问时间，则记录，计算收益，以备比较	
					if ((New.Route_Arrive_Time[NodeSequence[n]][k] >= starttimewindow[NodeSequence[n]][k]) && (New.Route_Arrive_Time[NodeSequence[n]][k] <= endtimewindow[NodeSequence[n]][k] - Duration_Time[NodeSequence[n]]) && (New.Route_Arrive_Time[NodeSequence[n] + nc][k] >= starttimewindow[NodeSequence[n] + nc][k]) && (New.Route_Arrive_Time[NodeSequence[n] + nc][k] <= endtimewindow[NodeSequence[n] + nc][k] - Duration_Time[NodeSequence[n] + nc]))
					{
						if ((New.Route_Arrive_Time[NodeSequence[n]][k] + T[NodeSequence[n]][Temp_Tour[i + 1][k]] + Duration_Time[NodeSequence[n]] <= New.Route_Arrive_Time[Temp_Tour[i + 1][k]][k]) && (New.Route_Arrive_Time[NodeSequence[n] + nc][k] + T[NodeSequence[n] + nc][Temp_Tour[j + 1][k]] + Duration_Time[NodeSequence[n] + nc] <= New.Route_Arrive_Time[Temp_Tour[j + 1][k]][k]))
						{
							New.Gap_Time[NodeSequence[n]] = New.Route_Arrive_Time[NodeSequence[n] + nc][k] - New.Route_Arrive_Time[NodeSequence[n]][k];
							for (int m = 0; m < nm; m++)
							{
								if ((New.Gap_Time[NodeSequence[n]] >= A_time[NodeSequence[n]][m]) && (New.Gap_Time[NodeSequence[n]] < B_time[NodeSequence[n]][m]))
									Temp_Ratio = Profit[NodeSequence[n]][m];
							}

							Insertion3[FeasibleInsertion3].NodeNO1 = NodeSequence[n];//节点编号
							Insertion3[FeasibleInsertion3].NodeNO2 = NodeSequence[n] + nc; //节点编号
							Insertion3[FeasibleInsertion3].InsertionPos1 = i;//当前点插入当前路线中的位置编号
							Insertion3[FeasibleInsertion3].InsertionPos2 = j;//当前点插入当前路线中的位置编号
							Insertion3[FeasibleInsertion3].Increase_Profit = Temp_Ratio;//产生的增加收益
							Inserted_Sequence3[FeasibleInsertion3] = FeasibleInsertion3;  //用于后面排序
							Insertion3[FeasibleInsertion3].Insertiontour = k;
							FeasibleInsertion3++;//计数
						}
					}
					New.Route_Arrive_Time[NodeSequence[n]][k] = 0;
					New.Route_Arrive_Time[NodeSequence[n] + nc][k] = 0;
					New.Gap_Time[NodeSequence[n]] = 0;
				}
			}
		}
		//////////所有位置已遍历结束

	//函数：将该节点插入路线;将该节点插入到最好的插入位置中，然后更新被剩余节点列表、开始访问时间
		if ((FeasibleInsertion3 != 0) || (FeasibleInsertion4 != 0))
		{
			//把能够插入的所有位置进行排序！取最好的插入！
			sorting_Inserted_Sequence(Inserted_Sequence3, FeasibleInsertion3, Insertion3);//非增排序
			sorting_Inserted_Sequence(Inserted_Sequence4, FeasibleInsertion4, Insertion4);//非增排序

			//插入节点，位置更换
			if ((Insertion3[Inserted_Sequence3[0]].Increase_Profit >= Insertion4[Inserted_Sequence4[0]].Increase_Profit || FeasibleInsertion4 == 0) && (FeasibleInsertion3 != 0))
			{
				int k0 = Insertion3[Inserted_Sequence3[0]].Insertiontour;
				for (p = 0; p < Insertion3[Inserted_Sequence3[0]].InsertionPos1; p++)
					Current_Tour[p][k0] = Tour1[p][k0];
				Current_Tour[Insertion3[Inserted_Sequence3[0]].InsertionPos1][k0] = NodeSequence[n];
				for (p = Insertion3[Inserted_Sequence3[0]].InsertionPos1 + 1; p < Insertion3[Inserted_Sequence3[0]].InsertionPos2; p++)
					Current_Tour[p][k0] = Tour1[p - 1][k0];
				Current_Tour[Insertion3[Inserted_Sequence3[0]].InsertionPos2][k0] = NodeSequence[n] + nc;
				for (p = Insertion3[Inserted_Sequence3[0]].InsertionPos2 + 1; p < 2 * nc + 2; p++)
					Current_Tour[p][k0] = Tour1[p - 2][k0];

				/////////选择节点最终插入的位置的开始时间
				if (New.Route_Arrive_Time[Current_Tour[Insertion3[Inserted_Sequence3[0]].InsertionPos1 - 1][k0]][k0] + T[Current_Tour[Insertion3[Inserted_Sequence3[0]].InsertionPos1 - 1][k0]][NodeSequence[n]] + Duration_Time[Current_Tour[Insertion3[Inserted_Sequence3[0]].InsertionPos1 - 1][k0]] <= starttimewindow[NodeSequence[n]][k0])
					New.Route_Arrive_Time[NodeSequence[n]][k0] = starttimewindow[NodeSequence[n]][k0];
				else if (New.Route_Arrive_Time[Current_Tour[Insertion3[Inserted_Sequence3[0]].InsertionPos1 - 1][k0]][k0] + T[Current_Tour[Insertion3[Inserted_Sequence3[0]].InsertionPos1 - 1][k0]][NodeSequence[n]] + Duration_Time[Current_Tour[Insertion3[Inserted_Sequence3[0]].InsertionPos1 - 1][k0]] <= endtimewindow[NodeSequence[n]][k0] - Duration_Time[NodeSequence[n]])
					New.Route_Arrive_Time[NodeSequence[n]][k0] = New.Route_Arrive_Time[Current_Tour[Insertion3[Inserted_Sequence3[0]].InsertionPos1 - 1][k0]][k0] + T[Current_Tour[Insertion3[Inserted_Sequence3[0]].InsertionPos1 - 1][k0]][NodeSequence[n]] + Duration_Time[Current_Tour[Insertion3[Inserted_Sequence3[0]].InsertionPos1 - 1][k0]];

				float c = New.Route_Arrive_Time[Current_Tour[Insertion3[Inserted_Sequence3[0]].InsertionPos2 - 1][k0]][k0] + T[Current_Tour[Insertion3[Inserted_Sequence3[0]].InsertionPos2 - 1][k0]][NodeSequence[n] + nc] + Duration_Time[Current_Tour[Insertion3[Inserted_Sequence3[0]].InsertionPos2 - 1][k0]];
				if (c <= starttimewindow[NodeSequence[n] + nc][k0])
				{
					New.Route_Arrive_Time[NodeSequence[n] + nc][k0] = starttimewindow[NodeSequence[n] + nc][k0];
					/*for (int m = 0; m < nm; m++)
					{
						if (starttimewindow[NodeSequence[n] + nc][k0] <= New.Route_Arrive_Time[NodeSequence[n]][k0] + A_time[NodeSequence[n]][Sort_Interval[m]]
							&& New.Route_Arrive_Time[NodeSequence[n]][k0] + A_time[NodeSequence[n]][Sort_Interval[m]] <= endtimewindow[NodeSequence[n] + nc][k0] - Duration_Time[NodeSequence[n] + nc])
						{
							New.Route_Arrive_Time[NodeSequence[n] + nc][k0] = New.Route_Arrive_Time[NodeSequence[n]][k0] + A_time[NodeSequence[n]][Sort_Interval[m]];
							break;
						}
					}*/
				}
				else if (c <= endtimewindow[NodeSequence[n] + nc][k0] - Duration_Time[NodeSequence[n] + nc])
				{
					New.Route_Arrive_Time[NodeSequence[n] + nc][k0] = c;
					/*for (int m = 0; m < nm; m++)
					{
						if (c <= New.Route_Arrive_Time[NodeSequence[n]][k0] + A_time[NodeSequence[n]][Sort_Interval[m]] 
							&& New.Route_Arrive_Time[NodeSequence[n]][k0] + A_time[NodeSequence[n]][Sort_Interval[m]] <= endtimewindow[NodeSequence[n] + nc][k0] - Duration_Time[NodeSequence[n] + nc])
						{
							New.Route_Arrive_Time[NodeSequence[n] + nc][k0] = New.Route_Arrive_Time[NodeSequence[n]][k0] + A_time[NodeSequence[n]][Sort_Interval[m]];
							break;
						}
					}*/
				}
			

				//////选择各节点最终插入的位置的访问时间间隔
				New.Gap_Time[NodeSequence[n]] = New.Route_Arrive_Time[NodeSequence[n] + nc][k0] - New.Route_Arrive_Time[NodeSequence[n]][k0];

				//////选择节点最终插入的位置的收益
				New.Node_Profit[NodeSequence[n]] = Insertion3[Inserted_Sequence3[0]].Increase_Profit;

				//更新最新的位置数据，为了下一轮的继续插入	
				for (p = 0; p < 2 * nc + 2; p++)
				{
					Temp_Tour[p][k0] = Current_Tour[p][k0];
					Tour[p][k0] = Current_Tour[p][k0];
					Tour1[p][k0] = Current_Tour[p][k0];
				}

				//计算当前路线的长度，利于后面计算各点可插入的位置
				Node_Number_In_Tour[k0] += 2;
			}
			if ((Insertion3[Inserted_Sequence3[0]].Increase_Profit < Insertion4[Inserted_Sequence4[0]].Increase_Profit || FeasibleInsertion3 == 0) && (FeasibleInsertion4 != 0))
			{
				int k0 = Insertion4[Inserted_Sequence4[0]].Insertiontour;
				int k1 = Insertion4[Inserted_Sequence4[0]].Insertiontour1;
				for (p = 0; p < Insertion4[Inserted_Sequence4[0]].InsertionPos1; p++)
					Current_Tour[p][k0] = Tour1[p][k0];
				Current_Tour[Insertion4[Inserted_Sequence4[0]].InsertionPos1][k0] = NodeSequence[n];
				for (p = Insertion4[Inserted_Sequence4[0]].InsertionPos1 + 1; p < 2 * nc + 2; p++)
					Current_Tour[p][k0] = Tour1[p - 1][k0];

				for (p = 0; p < Insertion4[Inserted_Sequence4[0]].InsertionPos2; p++)
					Current_Tour[p][k1] = Tour1[p][k1];
				Current_Tour[Insertion4[Inserted_Sequence4[0]].InsertionPos2][k1] = NodeSequence[n] + nc;
				for (p = Insertion4[Inserted_Sequence4[0]].InsertionPos2 + 1; p < 2 * nc + 2; p++)
					Current_Tour[p][k1] = Tour1[p - 1][k1];

				/////////选择节点最终插入的位置的开始时间
				if (New.Route_Arrive_Time[Current_Tour[Insertion4[Inserted_Sequence4[0]].InsertionPos1 - 1][k0]][k0] + T[Current_Tour[Insertion4[Inserted_Sequence4[0]].InsertionPos1 - 1][k0]][NodeSequence[n]] + Duration_Time[Current_Tour[Insertion4[Inserted_Sequence4[0]].InsertionPos1 - 1][k0]] <= starttimewindow[NodeSequence[n]][k0])
					New.Route_Arrive_Time[NodeSequence[n]][k0] = starttimewindow[NodeSequence[n]][k0];
				else if (New.Route_Arrive_Time[Current_Tour[Insertion4[Inserted_Sequence4[0]].InsertionPos1 - 1][k0]][k0] + T[Current_Tour[Insertion4[Inserted_Sequence4[0]].InsertionPos1 - 1][k0]][NodeSequence[n]] + Duration_Time[Current_Tour[Insertion4[Inserted_Sequence4[0]].InsertionPos1 - 1][k0]] <= (endtimewindow[NodeSequence[n]][k0] - Duration_Time[NodeSequence[n]]))
					New.Route_Arrive_Time[NodeSequence[n]][k0] = New.Route_Arrive_Time[Current_Tour[Insertion4[Inserted_Sequence4[0]].InsertionPos1 - 1][k0]][k0] + T[Current_Tour[Insertion4[Inserted_Sequence4[0]].InsertionPos1 - 1][k0]][NodeSequence[n]] + Duration_Time[Current_Tour[Insertion4[Inserted_Sequence4[0]].InsertionPos1 - 1][k0]];

				float d = New.Route_Arrive_Time[Current_Tour[Insertion4[Inserted_Sequence4[0]].InsertionPos2 - 1][k1]][k1] + T[Current_Tour[Insertion4[Inserted_Sequence4[0]].InsertionPos2 - 1][k1]][NodeSequence[n] + nc] + Duration_Time[Current_Tour[Insertion4[Inserted_Sequence4[0]].InsertionPos2 - 1][k1]];
				if (d <= starttimewindow[NodeSequence[n] + nc][k1])
				{
					New.Route_Arrive_Time[NodeSequence[n] + nc][k1] = starttimewindow[NodeSequence[n] + nc][k1];
					/*for (int m = 0; m < nm; m++)
					{
						if (starttimewindow[NodeSequence[n] + nc][k1] <= New.Route_Arrive_Time[NodeSequence[n]][k0] + A_time[NodeSequence[n]][Sort_Interval[m]]
							&& New.Route_Arrive_Time[NodeSequence[n]][k0] + A_time[NodeSequence[n]][Sort_Interval[m]] <= endtimewindow[NodeSequence[n] + nc][k1] - Duration_Time[NodeSequence[n] + nc]
							&& New.Route_Arrive_Time[NodeSequence[n]][k0] + A_time[NodeSequence[n]][Sort_Interval[m]] + Duration_Time[NodeSequence[n]] + T[NodeSequence[n] + nc][Current_Tour[Insertion4[Inserted_Sequence4[0]].InsertionPos1 - 1][k1]] <= New.Route_Arrive_Time[Current_Tour[Insertion4[Inserted_Sequence4[0]].InsertionPos1 - 1][k1]][k1])
						{
							New.Route_Arrive_Time[NodeSequence[n] + nc][k1] = New.Route_Arrive_Time[NodeSequence[n]][k0] + A_time[NodeSequence[n]][Sort_Interval[m]];
							break;
						}
					}*/
				}
				else if (d <= (endtimewindow[NodeSequence[n] + nc][k1] - Duration_Time[NodeSequence[n] + nc]))
				{
					New.Route_Arrive_Time[NodeSequence[n] + nc][k1] = d;
					/*for (int m = 0; m < nm; m++)
					{
						if (d <= New.Route_Arrive_Time[NodeSequence[n]][k0] + A_time[NodeSequence[n]][Sort_Interval[m]] 
							&& New.Route_Arrive_Time[NodeSequence[n]][k0] + A_time[NodeSequence[n]][Sort_Interval[m]] <= endtimewindow[NodeSequence[n] + nc][k1] - Duration_Time[NodeSequence[n] + nc]
							&& New.Route_Arrive_Time[NodeSequence[n]][k0] + A_time[NodeSequence[n]][Sort_Interval[m]] + Duration_Time[NodeSequence[n]] + T[NodeSequence[n] + nc][Current_Tour[Insertion4[Inserted_Sequence4[0]].InsertionPos1 - 1][k1]] <= New.Route_Arrive_Time[Current_Tour[Insertion4[Inserted_Sequence4[0]].InsertionPos1 - 1][k1]][k1])

						{
							New.Route_Arrive_Time[NodeSequence[n] + nc][k1] = New.Route_Arrive_Time[NodeSequence[n]][k0] + A_time[NodeSequence[n]][Sort_Interval[m]];
							break;
						}
					}*/
				}

				//////选择各节点最终插入的位置的访问时间间隔
				New.Gap_Time[NodeSequence[n]] = New.Route_Arrive_Time[NodeSequence[n] + nc][k1] - New.Route_Arrive_Time[NodeSequence[n]][k0];

				//////选择节点最终插入的位置的收益
				New.Node_Profit[NodeSequence[n]] = Insertion4[Inserted_Sequence4[0]].Increase_Profit;

				//更新最新的位置数据，为了下一轮的继续插入	
				for (p = 0; p < 2 * nc + 2; p++)
				{
					Temp_Tour[p][k0] = Current_Tour[p][k0];
					Tour[p][k0] = Current_Tour[p][k0];
					Tour1[p][k0] = Current_Tour[p][k0];
				}
				for (p = 0; p < 2 * nc + 2; p++)
				{
					Temp_Tour[p][k1] = Current_Tour[p][k1];
					Tour[p][k1] = Current_Tour[p][k1];
					Tour1[p][k1] = Current_Tour[p][k1];
				}
				//计算当前路线的长度，利于后面计算各点可插入的位置
				Node_Number_In_Tour[k0] += 1;
				Node_Number_In_Tour[k1] += 1;
			}

			//把插入的节点给删除掉
			for (p = n; p <= Node_Number; p++)
			{
				if (NodeSequence[Node_Number] == NodeSequence[n])//如果是最后一个!那就直接赋值为-
					NodeSequence[Node_Number] = -1;
				else
					NodeSequence[p] = NodeSequence[p + 1];
			}
			Node_Number = Node_Number - 1;

			feasible = 1;
		}
		if (feasible == 0)
			n++;
	}


	//保留当前路线的顺序
	for (t = 0; t < nt; t++)
	{
		for (p = 0; p < 2 * nc + 2; p++)
		{
			New.Route_Visit_Nodes[t][p] = Tour1[p][t];
		}
	}

	//////计算总收益
	New.Profit = 0;
	for (n = 1; n <= nc; n++)
	{
		New.Profit = New.Profit + New.Node_Profit[n];
	}

	/////////////////////////显示结果
	///显示路径结果
	/*cout << "插入算子后解为：" << endl;
	for (t = 0; t < nt; t++)
	{
		cout << "第" << t + 1 << "条路线的次序如下：" << endl;
		for (p = 0; p < 2 * nc + 2; p++)
		{
			if (New.Route_Visit_Nodes[t][p] == -1)
				break;
			cout << New.Route_Visit_Nodes[t][p] << setw(8);
		}
		cout << endl;
	}
	cout << "各节点的开始访问时间:" << endl;
	for (t = 0; t < nt; t++)
	{
		cout << "第" << t + 1 << "条路线的开始访问时间如下：" << endl;
		for (n = 0; n < 2 * nc + 2; n++)
		{
			cout << New.Route_Arrive_Time[n][t] << setw(8);
		}
		cout << endl;
	}
	cout << endl;
	cout << "各节点两次访问的间隔时间:" << endl;
	for (n = 1; n <= nc; n++)
	{
		cout << New.Gap_Time[n] << setw(8);
	}
	cout << endl;
	cout << "各节点两次访问的收益：" << endl;
	for (n = 1; n <= nc; n++)
	{
		cout << New.Node_Profit[n] << setw(8);
	}
	cout << endl;
	cout << "最大收益=" << New.Profit << endl;
	*/
}


//第一个算子•remove：单节点移除算子后修复
void Remove(InsertionNode*Insertion3, InsertionNode*Insertion4)
{
	for (int t = 0; t < nt; t++)
	{
		for (int p = 0; p < 2 * nc + 2; p++)
		{
			Tour1[p][t] = New.Route_Visit_Nodes[t][p];
			Current_Tour[p][t] = New.Route_Visit_Nodes[t][p];
		}
	}

	//在当前解，节点序列中删除那些被移除的节点，清空时间、收益等信息
	for (int t = 0; t < nt; t++)
	{
		for (int pp = 2 * nc; pp >= 1; pp--) ///依次判断访问节点是否被删除
		{
			if (New.Route_Visit_Nodes[t][pp] == -1 || New.Route_Visit_Nodes[t][pp] == 2 * nc + 1)
				continue;
			//cout << New.Route_Visit_Nodes[t][pp] << endl;
			if (isChoose_Remove[New.Route_Visit_Nodes[t][pp]] == 1) /////当前节点被移除,该路径后续节点均向前移动一位；
			{
				////若移除该位置，更新开始时间，间隔时间，收益
				for (int p = 0; p < pp; p++)
					Current_Tour[p][t] = Tour1[p][t];
				for (int p = pp + 1; p < 2 * nc + 2; p++)
					Current_Tour[p - 1][t] = Tour1[p][t];

				//计算当前路线的长度，利于后面计算各点可插入的位置
				Node_Number_In_Tour[t] -= 1;

				///更新位置
				for (int p = 0; p < 2 * nc + 2; p++)
					Tour1[p][t] = Current_Tour[p][t];

			}
		}
	}
	///////更新节点序列
	for (int t = 0; t < nt; t++)
		for (int p = 0; p < 2 * nc + 2; p++)
			New.Route_Visit_Nodes[t][p] = Tour1[p][t];


	for (int n = 1; n <= 2 * nc; n++)
	{
		if (isChoose_Remove[n] == 1)
		{
			if (n <= nc)
			{
				New.Gap_Time[n] = -1;
				New.Node_Profit[n] = 0;
				for (int k = 0; k < nt; k++)
					New.Route_Arrive_Time[n][k] = 0.0;
			}
			else
			{
				New.Gap_Time[n - nc] = -1;
				New.Node_Profit[n - nc] = 0;
				for (int k = 0; k < nt; k++)
					New.Route_Arrive_Time[n][k] = 0.0;
			}
		}
	}


	//重新插入移除节点
	for (int n = nc + 1; n <= 2 * nc; n++)
	{
		if (isChoose_Remove[n] == 1)
		{
			int tempt = -1;
			int tempk = -1;
			int tempj = -1;
			float profi = 0;
			int tempm = -1;
			int temp_time = 0;//第一次访问的节点

			//遍历所有的车辆和位置和时间段，选择收益最大的，记录下来
			for (int k = nt - 1; k > 0; k--)
			{
				for (int j = 1; j < Node_Number_In_Tour[k]; j++)//节点可插入位置
				{
					int z1 = New.Route_Visit_Nodes[k][j - 1];
					int z3 = New.Route_Visit_Nodes[k][j];
					if (z1 == -1)
						continue;

					//////计算这个节点可选的最早开始时间
					float b = New.Route_Arrive_Time[z1][k] + T[z1][n] + Duration_Time[z1];
					int Feasible_starttime = 0;
					if (b <= starttimewindow[n][k])
						Feasible_starttime = ceil(starttimewindow[n][k]);
					else if (b < endtimewindow[n][k] - Duration_Time[n])
						Feasible_starttime = ceil(b);
					else
						Feasible_starttime = 0;


					/////////////如果这对节点都满足时间窗约束并且不影响路线下一节点的开始访问时间，则记录，计算收益
					if (Feasible_starttime != 0)
					{
						//////计算这个节点可选的最晚开始时间
						int Feasible_endtime = min(floor(New.Route_Arrive_Time[z3][k] - Duration_Time[n] - T[n][z3]), (endtimewindow[n][k] - Duration_Time[n]));

						//在这个区间内，遍历所有时刻，找收益最大的位置
						if (Feasible_starttime <= Feasible_endtime)
						{
							for (int k1 = nt - 1; k1 >= 0; k1--)
							{
								if (New.Route_Arrive_Time[n - nc][k1] > 0)
									temp_time = New.Route_Arrive_Time[n - nc][k1];
							}

							for (int tm = Feasible_starttime; tm <= Feasible_endtime; tm++)
							{
								for (int m = 0; m < nm; m++)
								{
									if ((tm - temp_time) >= A_time[n - nc][m]
										&& (tm - temp_time) <= B_time[n - nc][m])
									{
										if (Profit[n - nc][m] > profi)
										{
											tempk = k;
											tempj = j;
											tempm = m;
											profi = Profit[n - nc][m];
											tempt = tm;
										}
									}
								}
							}
						}
					}
				}
			}

			//在收益最大的位置，插入该节点
			if (tempm != -1)
			{
				for (int p = 2 * Max_Node + 1; p >= tempj + 1; p--)
					New.Route_Visit_Nodes[tempk][p] = New.Route_Visit_Nodes[tempk][p - 1];
				New.Route_Visit_Nodes[tempk][tempj] = n;

				New.Route_Arrive_Time[n][tempk] = tempt;
				New.Gap_Time[n - nc] = New.Route_Arrive_Time[n][tempk] - (float)temp_time;
				New.Node_Profit[n - nc] = Profit[n - nc][tempm];

				//计算当前路线的长度，利于后面计算各点可插入的位置
				Node_Number_In_Tour[tempk] += 1;
			}
		}
	}

	//删除只访问一次的节点
	for (int n = 1; n <= nc; n++)
	{
		bool is = 0;
		bool is1 = 0;
		int route = 0;
		int position = 0;
		int route1 = 0;
		int position1 = 0;
		for (int t = 0; t < nt; t++)
		{
			for (int p = 0; p < 2 * nc + 2; p++)
			{
				if (New.Route_Visit_Nodes[t][p] == -1)
					break;

				if (New.Route_Visit_Nodes[t][p] == n)
				{
					is = 1;
					route = t;
					position = p;
				}
				if (New.Route_Visit_Nodes[t][p] == n + nc)
				{
					is1 = 1;
					route1 = t;
					position1 = p;
				}
			}
		}

		if (is == 1 && is1 == 0)
		{
			//delete 第1次
			for (int p = position; p < 2 * nc + 2; p++)
				New.Route_Visit_Nodes[route][p] = New.Route_Visit_Nodes[route][p + 1];
			New.Route_Arrive_Time[n][route] = 0;
			Node_Number_In_Tour[route] -= 1;
		}
		if (is == 0 && is1 == 1)
		{
			//delete 第2次
			for (int p = position1; p < 2 * nc + 2; p++)
				New.Route_Visit_Nodes[route1][p] = New.Route_Visit_Nodes[route1][p + 1];
			New.Route_Arrive_Time[n + nc][route1] = 0;
			Node_Number_In_Tour[route1] -= 1;
		}
	}

	//计算未插入节点的数目和序列
	for (int p = 1; p <= nc; p++)
		NodeSequence[p] = p;

	for (int p = nc + 1; p <= Max_Node; p++)
		NodeSequence[p] = -1;

	Node_Number = nc;
	for (int n = nc; n >= 1; n--)
	{
		if (New.Node_Profit[n] != 0)
		{
			for (int p = n; p <= nc; p++)
				NodeSequence[p] = NodeSequence[p + 1];
			Node_Number = Node_Number - 1;
		}
	}

	//计算路线当前插入节点数
	for (int t = 0; t < nt; t++)
		Node_Number_In_Tour[t] = 0;
	for (int t = 0; t < nt; t++)
	{
		for (int i = 0; i < 2 * nc + 2; i++)
		{
			if (New.Route_Visit_Nodes[t][i] == -1)
				break;

			Node_Number_In_Tour[t]++;
		}
	}
	New.Profit = 0;
	for (int n2 = 1; n2 <= nc; n2++)
	{
		New.Profit += New.Node_Profit[n2];
	}
	//插入算子(选择最好的位置继续插入）
	Insert_Operator(New, Insertion3, Insertion4);


	//////更新局部最优解
	if (Final.Profit < New.Profit)
	{
		//////更新当前解
		for (int t = 0; t < nt; t++)
		{
			for (int i = 0; i < 2 * nc + 2; i++)
			{
				Final.Route_Visit_Nodes[t][i] = New.Route_Visit_Nodes[t][i];

			}
		}
		for (int t = 0; t < nt; t++)
		{
			for (int n1 = 0; n1 < 2 * nc + 2; n1++)
			{
				Final.Route_Arrive_Time[n1][t] = New.Route_Arrive_Time[n1][t];
			}
		}
		for (int n2 = 1; n2 <= nc; n2++)
		{
			Final.Gap_Time[n2] = New.Gap_Time[n2];
			Final.Node_Profit[n2] = New.Node_Profit[n2];
		}
		Final.Profit = New.Profit;

	}


	//恢复旧解，继续进行局部搜索
	for (int t = 0; t < nt; t++)
		for (int i = 0; i < 2 * nc + 2; i++)
			New.Route_Visit_Nodes[t][i] = Current.Route_Visit_Nodes[t][i];

	for (int t = 0; t < nt; t++)
		for (int n1 = 0; n1 < 2 * nc + 2; n1++)
			New.Route_Arrive_Time[n1][t] = Current.Route_Arrive_Time[n1][t];

	for (int n2 = 1; n2 <= nc; n2++)
	{
		New.Gap_Time[n2] = Current.Gap_Time[n2];
		New.Node_Profit[n2] = Current.Node_Profit[n2];
	}
	New.Profit = Current.Profit;


	//计算未插入节点的数目和序列
	for (int p = 1; p <= nc; p++)
		NodeSequence[p] = p;

	for (int p = nc + 1; p <= 2*nc+1; p++)
		NodeSequence[p] = -1;

	Node_Number = nc;
	for (int n = nc; n >= 1; n--)
	{
		if (New.Node_Profit[n] != 0)
		{
			for (int p = n; p <= nc; p++)
				NodeSequence[p] = NodeSequence[p + 1];
			Node_Number = Node_Number - 1;
		}
	}

	//计算路线当前插入节点数
	for (int t = 0; t < nt; t++)
		Node_Number_In_Tour[t] = 0;
	for (int t = 0; t < nt; t++)
	{
		for (int i = 0; i < 2 * nc + 2; i++)
		{
			if (New.Route_Visit_Nodes[t][i] == -1)
				break;

			Node_Number_In_Tour[t]++;
		}
	}
}
void Update_Solution_Remove_One(Solution& New, Solution&Final, InsertionNode*Insertion3, InsertionNode*Insertion4,Solution& Current)
{
	//选择移除哪些第二次访问节点
	for (int t1 = 0; t1 < nt; t1++)
	{
		for (int p1 = 1; p1 < 2 * nc + 2; p1++)
		{
			if (New.Route_Visit_Nodes[t1][p1] == 2 * nc + 1 || New.Route_Visit_Nodes[t1][p1] == -1)
				break;

			if (New.Route_Visit_Nodes[t1][p1] <= nc)
				continue;

			for (int i = 0; i <= 2 * nc; i++)
				isChoose_Remove[i] = 0;

			isChoose_Remove[New.Route_Visit_Nodes[t1][p1]] = 1;
			Remove(Insertion3, Insertion4);

			for (int t2 = t1; t2 < nt; t2++)
			{
				//从哪个位置开始
				int temp = 1;
				if (t2 == t1)
					temp = p1 + 1;

				for (int p2 = temp; p2 < 2 * nc + 2; p2++)
				{
					if (New.Route_Visit_Nodes[t2][p2] == 2 * nc + 1 || New.Route_Visit_Nodes[t2][p2] == -1)
						break;
					if (New.Route_Visit_Nodes[t2][p2] <= nc)
						continue;


					for (int i = 0; i <= 2 * nc; i++)
						isChoose_Remove[i] = 0;
					isChoose_Remove[New.Route_Visit_Nodes[t1][p1]] = 1;
					isChoose_Remove[New.Route_Visit_Nodes[t2][p2]] = 1;
					Remove(Insertion3, Insertion4);


					for (int t3 = t2; t3 < nt; t3++)
					{
						//从哪个位置开始
						int temp1 = 1;
						if (t3 == t2)
							temp1 = p2 + 1;

						for (int p3 = temp1; p3 < 2 * nc + 2; p3++)
						{
							if (New.Route_Visit_Nodes[t3][p3] == 2 * nc + 1 || New.Route_Visit_Nodes[t3][p3] == -1)
								break;
							if (New.Route_Visit_Nodes[t3][p3] <= nc)
								continue;

							for (int i = 0; i <= 2 * nc; i++)
								isChoose_Remove[i] = 0;
							isChoose_Remove[New.Route_Visit_Nodes[t1][p1]] = 1;
							isChoose_Remove[New.Route_Visit_Nodes[t2][p2]] = 1;
							isChoose_Remove[New.Route_Visit_Nodes[t3][p3]] = 1;

							//cout << New.Route_Visit_Nodes[t1][p1] << "  " << New.Route_Visit_Nodes[t2][p2] << endl;
							Remove(Insertion3, Insertion4);


							/*for (int t4 = t3; t4 < nt; t4++)
							{
								//从哪个位置开始
								int temp1 = 1;
								if (t4 == t3)
									temp1 = p3 + 1;

								for (int p4 = temp1; p4 < 2 * nc + 2; p4++)
								{
									if (New.Route_Visit_Nodes[t4][p4] == 2 * nc + 1)
										break;
									if (New.Route_Visit_Nodes[t4][p4] <= nc)
										continue;

									for (int i = 1; i <= 2 * nc; i++)
										isChoose_Remove[i] = 0;
									isChoose_Remove[New.Route_Visit_Nodes[t1][p1]] = 1;
									isChoose_Remove[New.Route_Visit_Nodes[t2][p2]] = 1;
									isChoose_Remove[New.Route_Visit_Nodes[t3][p3]] = 1;
									isChoose_Remove[New.Route_Visit_Nodes[t4][p4]] = 1;
									//cout << New.Route_Visit_Nodes[t1][p1] << "  " << New.Route_Visit_Nodes[t2][p2] << endl;
									Remove(Insertion3, Insertion4);

								}
							}*/
						}
					}
				}
			}
		}
	}
}


//第二个算子•remove-pair：双节点移除算子后修复
void Update_Route(Solution& New, Solution& Current)
{
	int p; int n; int t; int tt; int m;//p是位置，n是节点，t,tt是路线, m是时间段

	for (t = 0; t < nt; t++)
	{
		for (p = 0; p < 2 * nc + 2; p++)//赋值
		{
			Tour1[p][t] = New.Route_Visit_Nodes[t][p];
			Current_Tour[p][t] = New.Route_Visit_Nodes[t][p];
		}
	}

	///////更新当前路径
	for (t = 0; t < nt; t++)
	{
		int a = Node_Number_In_Tour[t] - 1;
		for (int p1 = a; p1 >= 1; p1--) ///依次判断访问节点是否被移除    ??????
		{
			if(New.Route_Visit_Nodes[t][p1]==-1|| New.Route_Visit_Nodes[t][p1]==2*nc+1)
				continue;

			if (isChoose_Remove[New.Route_Visit_Nodes[t][p1]] == 1) /////当前节点被移除,该路径后续节点均向前移动一位；
			{
				////若移除该位置，更新开始时间，间隔时间，收益
				for (p = 0; p < p1; p++)
					Current_Tour[p][t] = Tour1[p][t];
				for (p = p1 + 1; p < 2*nc+2; p++)
					Current_Tour[p - 1][t] = Tour1[p][t];

				//计算当前路线的长度，利于后面计算各点可插入的位置
				Node_Number_In_Tour[t] -= 1;

				//更新位置
				for (p = 0; p < 2 * nc + 2; p++)
				{
					Tour1[p][t] = Current_Tour[p][t];
				}
			}
		}
	}

	///////更新访问节点序列
	for (t = 0; t < nt; t++)
	{
		for (p = 0; p < 2 * nc + 2; p++)
		{
			New.Route_Visit_Nodes[t][p] = Tour1[p][t];
		}
	}


	///移除节点对的下列值为0
	for (n = 1; n <= nc; n++)
	{
		if (isChoose_Remove[n] == 1)
		{
			New.Gap_Time[n] = -1;
			New.Node_Profit[n] = 0;
			for (t = 0; t < nt; t++)
			{
				New.Route_Arrive_Time[n][t] = 0;
				New.Route_Arrive_Time[n + nc][t] = 0;
			}
		}
	}

	///////更新开始时间
	/*for (t = 0; t < nt; t++)
	{
		for (int p2 = 1; p2 < Node_Number_In_Tour[t] - 1; p2++)
		{
			int J0 = New.Route_Visit_Nodes[t][p2 - 1];
			int J = New.Route_Visit_Nodes[t][p2];

			//////计算一次访问节点的开始访问时间
			if (J <= nc && J >= 1)
			{
				if (New.Route_Arrive_Time[J0][t] + T[J0][J] + Duration_Time[J0] <= starttimewindow[J][t])
					New.Route_Arrive_Time[J][t] = starttimewindow[J][t];
				else if (New.Route_Arrive_Time[J0][t] + T[J0][J] + Duration_Time[J0] <= (endtimewindow[J][t] - Duration_Time[J]))
					New.Route_Arrive_Time[J][t] = New.Route_Arrive_Time[J0][t] + T[J0][J] + Duration_Time[J0];
			}

			//////计算二次访问节点的开始访问时间
			if (J <= 2 * nc && J >= nc + 1)
			{
				float b = New.Route_Arrive_Time[J0][t] + T[J0][J] + Duration_Time[J0];
				if (b <= starttimewindow[J][t])
				{
					New.Route_Arrive_Time[J][t] = starttimewindow[J][t];
					/*if (starttimewindow[J][t] <= New.Route_Arrive_Time[J - nc][t] + A_time[J - nc][0] && New.Route_Arrive_Time[J - nc][t] + A_time[J - nc][0] <= endtimewindow[J][t] - Duration_Time[J])
						New.Route_Arrive_Time[J][t] = New.Route_Arrive_Time[J - nc][t] + A_time[J - nc][0];
					if (starttimewindow[J][t] <= New.Route_Arrive_Time[J - nc][t] + A_time[J - nc][7] && New.Route_Arrive_Time[J - nc][t] + A_time[J - nc][7] <= endtimewindow[J][t] - Duration_Time[J])
						New.Route_Arrive_Time[J][t] = New.Route_Arrive_Time[J - nc][t] + A_time[J - nc][7];
					if (starttimewindow[J][t] <= New.Route_Arrive_Time[J - nc][t] + A_time[J - nc][1] && New.Route_Arrive_Time[J - nc][t] + A_time[J - nc][1] <= endtimewindow[J][t] - Duration_Time[J])
						New.Route_Arrive_Time[J][t] = New.Route_Arrive_Time[J - nc][t] + A_time[J - nc][1];
					if (starttimewindow[J][t] <= New.Route_Arrive_Time[J - nc][t] + A_time[J - nc][6] && New.Route_Arrive_Time[J - nc][t] + A_time[J - nc][6] <= endtimewindow[J][t] - Duration_Time[J])
						New.Route_Arrive_Time[J][t] = New.Route_Arrive_Time[J - nc][t] + A_time[J - nc][6];
					if (starttimewindow[J][t] <= New.Route_Arrive_Time[J - nc][t] + A_time[J - nc][2] && New.Route_Arrive_Time[J - nc][t] + A_time[J - nc][2] <= endtimewindow[J][t] - Duration_Time[J])
						New.Route_Arrive_Time[J][t] = New.Route_Arrive_Time[J - nc][t] + A_time[J - nc][2];
					if (starttimewindow[J][t] <= New.Route_Arrive_Time[J - nc][t] + A_time[J - nc][5] && New.Route_Arrive_Time[J - nc][t] + A_time[J - nc][5] <= endtimewindow[J][t] - Duration_Time[J])
						New.Route_Arrive_Time[J][t] = New.Route_Arrive_Time[J - nc][t] + A_time[J - nc][5];
					if (starttimewindow[J][t] <= New.Route_Arrive_Time[J - nc][t] + A_time[J - nc][4] && New.Route_Arrive_Time[J - nc][t] + A_time[J - nc][4] <= endtimewindow[J][t] - Duration_Time[J])
						New.Route_Arrive_Time[J][t] = New.Route_Arrive_Time[J - nc][t] + A_time[J - nc][4];
					if (starttimewindow[J][t] <= New.Route_Arrive_Time[J - nc][t] + A_time[J - nc][3] && New.Route_Arrive_Time[J - nc][t] + A_time[J - nc][3] <= endtimewindow[J][t] - Duration_Time[J])
						New.Route_Arrive_Time[J][t] = New.Route_Arrive_Time[J - nc][t] + A_time[J - nc][3];
				
				}
				else if (b <= endtimewindow[J][t] - Duration_Time[J])
				{
					New.Route_Arrive_Time[J][t] = b;
					/*if (b <= New.Route_Arrive_Time[J - nc][t] + A_time[J - nc][0] && New.Route_Arrive_Time[J - nc][t] + A_time[J - nc][0] <= endtimewindow[J][t] - Duration_Time[J])
						New.Route_Arrive_Time[J][t] = New.Route_Arrive_Time[J - nc][t] + A_time[J - nc][0];
					if (b <= New.Route_Arrive_Time[J - nc][t] + A_time[J - nc][7] && New.Route_Arrive_Time[J - nc][t] + A_time[J - nc][7] <= endtimewindow[J][t] - Duration_Time[J])
						New.Route_Arrive_Time[J][t] = New.Route_Arrive_Time[J - nc][t] + A_time[J - nc][7];
					if (b <= New.Route_Arrive_Time[J - nc][t] + A_time[J - nc][1] && New.Route_Arrive_Time[J - nc][t] + A_time[J - nc][1] <= endtimewindow[J][t] - Duration_Time[J])
						New.Route_Arrive_Time[J][t] = New.Route_Arrive_Time[J - nc][t] + A_time[J - nc][1];
					if (b <= New.Route_Arrive_Time[J - nc][t] + A_time[J - nc][6] && New.Route_Arrive_Time[J - nc][t] + A_time[J - nc][6] <= endtimewindow[J][t] - Duration_Time[J])
						New.Route_Arrive_Time[J][t] = New.Route_Arrive_Time[J - nc][t] + A_time[J - nc][6];
					if (b <= New.Route_Arrive_Time[J - nc][t] + A_time[J - nc][2] && New.Route_Arrive_Time[J - nc][t] + A_time[J - nc][2] <= endtimewindow[J][t] - Duration_Time[J])
						New.Route_Arrive_Time[J][t] = New.Route_Arrive_Time[J - nc][t] + A_time[J - nc][2];
					if (b <= New.Route_Arrive_Time[J - nc][t] + A_time[J - nc][5] && New.Route_Arrive_Time[J - nc][t] + A_time[J - nc][5] <= endtimewindow[J][t] - Duration_Time[J])
						New.Route_Arrive_Time[J][t] = New.Route_Arrive_Time[J - nc][t] + A_time[J - nc][5];
					if (b <= New.Route_Arrive_Time[J - nc][t] + A_time[J - nc][4] && New.Route_Arrive_Time[J - nc][t] + A_time[J - nc][4] <= endtimewindow[J][t] - Duration_Time[J])
						New.Route_Arrive_Time[J][t] = New.Route_Arrive_Time[J - nc][t] + A_time[J - nc][4];
					if (b <= New.Route_Arrive_Time[J - nc][t] + A_time[J - nc][3] && New.Route_Arrive_Time[J - nc][t] + A_time[J - nc][3] <= endtimewindow[J][t] - Duration_Time[J])
						New.Route_Arrive_Time[J][t] = New.Route_Arrive_Time[J - nc][t] + A_time[J - nc][3];
				
				}
			}
		}
	}*/


	//计算
	/*for (n = 1; n <= nc; n++)
	{
		New.Node_Profit[n] = 0;
		for (t = 0; t < nt; t++)
		{
			for (tt = t; tt < nt; tt++)
			{
				if (New.Route_Arrive_Time[n][t] > 0 && New.Route_Arrive_Time[n + nc][tt] > 0)
				{
					New.Gap_Time[n] = New.Route_Arrive_Time[n + nc][tt] - New.Route_Arrive_Time[n][t];
					for (m = 0; m < nm; m++)
					{
						if (New.Gap_Time[n] >= A_time[n][m] && New.Gap_Time[n] < B_time[n][m])
							New.Node_Profit[n] = Profit[n][m];
					}
				}
			}
		}
	}*/




	//把上面移除的节点给恢复到剩余未插入节点序列
	for (n = 1; n <= nc; n++)
	{
		if (isChoose_Remove[n] == 1)
		{
			Node_Number = Node_Number + 1;
			NodeSequence[Node_Number] = n;
		}
	}

	///显示路径结果
	/*cout << "移除算子后的解为：" << endl;
	for (t = 0; t < nt; t++)
	{
		cout << "第" << t + 1 << "条路线的次序如下：" << endl;
		for (p = 0; p < 2 * nc + 2; p++)
		{
			if (New.Route_Visit_Nodes[t][p] == -1)
				break;
			cout << New.Route_Visit_Nodes[t][p] << setw(8);
		}
		cout << endl;
	}
	cout << endl;
	for (t = 0; t < nt; t++)
	{
		cout << "第" << t + 1 << "条路线的开始访问时间如下：" << endl;
		for (n = 0; n < 2 * nc + 2; n++)
		{
			cout << New.Route_Arrive_Time[n][t] << setw(8);
		}
		cout << endl;
	}
	cout << endl;
	cout << "间隔时间如下" << endl;
	for (n = 1; n <= nc; n++)
	{
		cout << New.Gap_Time[n] << setw(8);
	}
	cout << endl;
	cout << "间隔收益如下" << endl;
	for (n = 1; n <= nc; n++)
	{
		cout << New.Node_Profit[n] << setw(8);
	}
	cout << endl;
	*/
}//函数：移除节点后贪婪修复
void Double_Remove(InsertionNode*Insertion3, InsertionNode*Insertion4)
{
	//删除移除的节点
	Update_Route(New, Current);


	//删除只访问一次的节点
	for (int n = 1; n <= nc; n++)
	{
		bool is = 0;
		bool is1 = 0;
		int route = 0;
		int position = 0;
		int route1 = 0;
		int position1 = 0;
		for (int t = 0; t < nt; t++)
		{
			for (int p = 0; p < 2 * nc + 2; p++)
			{
				if (New.Route_Visit_Nodes[t][p] == -1)
					break;

				if (New.Route_Visit_Nodes[t][p] == n)
				{
					is = 1;
					route = t;
					position = p;
				}
				if (New.Route_Visit_Nodes[t][p] == n + nc)
				{
					is1 = 1;
					route1 = t;
					position1 = p;
				}
			}
		}

		if (is == 1 && is1 == 0)
		{
			//delete 第1次
			for (int p = position; p < 2 * nc + 2; p++)
				New.Route_Visit_Nodes[route][p] = New.Route_Visit_Nodes[route][p + 1];
			New.Route_Arrive_Time[n][route] = 0;
			Node_Number_In_Tour[route] -= 1;
		}
		if (is == 0 && is1 == 1)
		{
			//delete 第2次
			for (int p = position1; p < 2 * nc + 2; p++)
				New.Route_Visit_Nodes[route1][p] = New.Route_Visit_Nodes[route1][p + 1];
			New.Route_Arrive_Time[n + nc][route1] = 0;
			Node_Number_In_Tour[route1] -= 1;
		}
	}

	//计算未插入节点的数目和序列
	for (int p = 1; p <= nc; p++)
		NodeSequence[p] = p;

	for (int p = nc + 1; p <= Max_Node; p++)
		NodeSequence[p] = -1;

	Node_Number = nc;
	for (int n = nc; n >= 1; n--)
	{
		if (New.Node_Profit[n] != 0)
		{
			for (int p = n; p <= nc; p++)
				NodeSequence[p] = NodeSequence[p + 1];
			Node_Number = Node_Number - 1;
		}
	}

	//计算路线当前插入节点数
	for (int t = 0; t < nt; t++)
		Node_Number_In_Tour[t] = 0;
	for (int t = 0; t < nt; t++)
	{
		for (int i = 0; i < 2 * nc + 2; i++)
		{
			if (New.Route_Visit_Nodes[t][i] == -1)
				break;

			Node_Number_In_Tour[t]++;
		}
	}

	New.Profit = 0;
	for (int n2 = 1; n2 <= nc; n2++)
	{
		New.Profit += New.Node_Profit[n2];
	}

	//插入算子(选择最好的位置继续插入）
	Insert_Operator(New, Insertion3, Insertion4);


	//////比较新解与当前解，判断是否更新解
	if (Final.Profit < New.Profit)
	{
		for (int t = 0; t < nt; t++)
			for (int i = 0; i < 2 * nc + 2; i++)
				Final.Route_Visit_Nodes[t][i] = New.Route_Visit_Nodes[t][i];

		for (int t = 0; t < nt; t++)
			for (int n1 = 0; n1 < 2 * nc + 2; n1++)
				Final.Route_Arrive_Time[n1][t] = New.Route_Arrive_Time[n1][t];

		for (int n2 = 1; n2 <= nc; n2++)
		{
			Final.Gap_Time[n2] = New.Gap_Time[n2];
			Final.Node_Profit[n2] = New.Node_Profit[n2];
		}
		Final.Profit = New.Profit;
	}

	for (int t = 0; t < nt; t++)
		for (int i = 0; i < 2 * nc + 2; i++)
			New.Route_Visit_Nodes[t][i] = Current.Route_Visit_Nodes[t][i];
	for (int t = 0; t < nt; t++)
		for (int n1 = 0; n1 < 2 * nc + 2; n1++)
			New.Route_Arrive_Time[n1][t] = Current.Route_Arrive_Time[n1][t];

	for (int n2 = 1; n2 <= nc; n2++)
	{
		New.Gap_Time[n2] = Current.Gap_Time[n2];
		New.Node_Profit[n2] = Current.Node_Profit[n2];
	}
	New.Profit = Current.Profit;

	//计算未插入节点的数目和序列
	for (int p = 1; p <= nc; p++)
		NodeSequence[p] = p;

	for (int p = nc + 1; p <= 2*nc+1; p++)
		NodeSequence[p] = -1;

	Node_Number = nc;
	for (int n = nc; n >= 1; n--)
	{
		if (Current.Node_Profit[n] != 0)
		{
			for (int p = n; p <= nc; p++)
			{
				NodeSequence[p] = NodeSequence[p + 1];
			}
			Node_Number = Node_Number - 1;
		}
	}

	//计算路线当前插入节点数
	for (int t = 0; t < nt; t++)
		Node_Number_In_Tour[t] = 0;
	for (int t = 0; t < nt; t++)
	{
		for (int i = 0; i < 2 * nc + 2; i++)
		{
			if (Current.Route_Visit_Nodes[t][i] == -1)
				break;

			Node_Number_In_Tour[t]++;
		}
	}
}
void Update_Solution_Remove(Solution& New, Solution &Final, InsertionNode*Insertion3, InsertionNode*Insertion4, Solution& Current)
{
	for (int t1 = 0; t1 < nt; t1++)
	{
		for (int p1 = 1; p1 < 2 * nc + 2; p1++)
		{
			if (New.Route_Visit_Nodes[t1][p1] == 2 * nc + 1|| New.Route_Visit_Nodes[t1][p1] == -1)
				break;

			if (New.Route_Visit_Nodes[t1][p1] > nc)
				continue;
			for (int n = 0; n <= 2 * nc; n++)
				isChoose_Remove[n] = 0;


			isChoose_Remove[New.Route_Visit_Nodes[t1][p1]] = 1;
			isChoose_Remove[New.Route_Visit_Nodes[t1][p1] + nc] = 1;
			Double_Remove(Insertion3, Insertion4);

			for (int t2 = t1; t2 < nt; t2++)
			{
				int temp = 1;
				if (t2 == t1)
					temp = p1 + 1;
				for (int p2 = temp; p2 < 2 * nc + 2; p2++)
				{
					if (New.Route_Visit_Nodes[t2][p2] == 2 * nc + 1 || New.Route_Visit_Nodes[t2][p2] == -1)
						break;

					if (New.Route_Visit_Nodes[t2][p2] > nc)
						continue;

					for (int n = 1; n <= 2 * nc; n++)
						isChoose_Remove[n] = 0;
					isChoose_Remove[New.Route_Visit_Nodes[t1][p1]] = 1;
					isChoose_Remove[New.Route_Visit_Nodes[t1][p1] + nc] = 1;
					isChoose_Remove[New.Route_Visit_Nodes[t2][p2]] = 1;
					isChoose_Remove[New.Route_Visit_Nodes[t2][p2] + nc] = 1;
					Double_Remove(Insertion3, Insertion4);


					/*for (int t3 = t2; t3 < nt; t3++)
					{
						int temp = 1;
						if (t3 == t2)
							temp = p2 + 1;
						for (int p3 = temp; p3 < 2 * nc + 2; p3++)
						{
							if (New.Route_Visit_Nodes[t3][p3] == 2 * nc + 1)
								break;
							if (New.Route_Visit_Nodes[t3][p3] > nc)
								continue;

							for (int n = 1; n <= 2 * nc; n++)
								isChoose_Remove[n] = 0;
							isChoose_Remove[New.Route_Visit_Nodes[t1][p1]] = 1;
							isChoose_Remove[New.Route_Visit_Nodes[t1][p1] + nc] = 1;
							isChoose_Remove[New.Route_Visit_Nodes[t2][p2]] = 1;
							isChoose_Remove[New.Route_Visit_Nodes[t2][p2] + nc] = 1;
							isChoose_Remove[New.Route_Visit_Nodes[t3][p3]] = 1;
							isChoose_Remove[New.Route_Visit_Nodes[t3][p3] + nc] = 1;
							Double_Remove(Insertion3, Insertion4);

						}
					}*/
				}
			}
		}
	}
}



//更新局部最优解，跳到局部最优解的位置
void Update_Loacl_Solution()
{
	Final.Profit = 0;
	for (int n = 1; n <= nc; n++)
	{
		Final.Node_Profit[n] = 0;
		for (int t = 0; t < nt; t++)
		{
			for (int tt = t; tt < nt; tt++)
			{
				if (Final.Route_Arrive_Time[n][t] > 0 && Final.Route_Arrive_Time[n + nc][tt] > 0)
				{
					Final.Gap_Time[n] = Final.Route_Arrive_Time[n + nc][tt] - Final.Route_Arrive_Time[n][t];
					for (int m = 0; m < nm; m++)
					{
						if (Final.Gap_Time[n] >= A_time[n][m] && Final.Gap_Time[n] < B_time[n][m])
							Final.Node_Profit[n] = Profit[n][m];
					}
				}
			}
		}
		Final.Profit += Final.Node_Profit[n];
	}

	for (int t = 0; t < nt; t++)
	{
		for (int i = 0; i < 2 * nc + 2; i++)
		{
			New.Route_Visit_Nodes[t][i] = Final.Route_Visit_Nodes[t][i];

		}
	}
	for (int t = 0; t < nt; t++)
	{
		for (int n1 = 0; n1 < 2 * nc + 2; n1++)
		{
			New.Route_Arrive_Time[n1][t] = Final.Route_Arrive_Time[n1][t];
		}
	}
	for (int n2 = 1; n2 <= nc; n2++)
	{
		New.Gap_Time[n2] = Final.Gap_Time[n2];
		New.Node_Profit[n2] = Final.Node_Profit[n2];
	}
	New.Profit = Final.Profit;
	for (int t = 0; t < nt; t++)
	{
		for (int i = 0; i < 2 * nc + 2; i++)
		{
			Current.Route_Visit_Nodes[t][i] = Final.Route_Visit_Nodes[t][i];

		}
	}
	for (int t = 0; t < nt; t++)
	{
		for (int n1 = 0; n1 < 2 * nc + 2; n1++)
		{
			Current.Route_Arrive_Time[n1][t] = Final.Route_Arrive_Time[n1][t];
		}
	}
	for (int n2 = 1; n2 <= nc; n2++)
	{
		Current.Gap_Time[n2] = Final.Gap_Time[n2];
		Current.Node_Profit[n2] = Final.Node_Profit[n2];
	}
	Current.Profit = Final.Profit;


	//计算未插入节点的数目和序列
	for (int p = 1; p <= nc; p++)
		NodeSequence[p] = p;

	for (int p = nc + 1; p <= 2*nc+1; p++)
		NodeSequence[p] = -1;

	Node_Number = nc;
	for (int n = nc; n >= 1; n--)
	{
		if (Final.Node_Profit[n] != 0)
		{
			for (int p = n; p <= nc; p++)
				NodeSequence[p] = NodeSequence[p + 1];
			Node_Number = Node_Number - 1;
		}
	}

	//计算路线当前插入节点数
	for (int t = 0; t < nt; t++)
		Node_Number_In_Tour[t] = 0;
	for (int t = 0; t < nt; t++)
	{
		for (int i = 0; i < 2 * nc + 2; i++)
		{
			if (Final.Route_Visit_Nodes[t][i] == -1)
				break;

			Node_Number_In_Tour[t]++;
		}
	}
}


void improve(Solution& New, Solution& Current, Solution& Final, InsertionNode* Insertion3, InsertionNode* Insertion4)
{
	//针对分段收益特性，将当前路径中低收益节点访问，尝试移动到高收益区间，提高分段收益函数下的整体利润
	for (int n = 1; n <= nc; n++)
	{
		//判读是否已取得最大收益
		int max_profit = 0;
		for (int m = 0; m < nm; m++)
		{
			if (Profit[n][m] > max_profit)
				max_profit = Profit[n][m];
		}
		if (Final.Node_Profit[n] == max_profit)
			continue;


		//如果没有拿到最大收益，同一个窗口调整时间
		bool is = 0;
		bool is1 = 0;
		int route = -1;
		int position = -1;
		int route1 = -1;
		int position1 = -1;
		//找到两次访问所在的车辆和路径位置
		for (int t = 0; t < nt; t++)
		{
			for (int p = 1; p < 2 * nc + 2; p++)
			{
				if (Final.Route_Visit_Nodes[t][p] == 2*nc+1)
					break;
				if (Final.Route_Visit_Nodes[t][p] == 0)
					continue;
				if (Final.Route_Visit_Nodes[t][p] == n)
				{
					is = 1;
					route = t;
					position = p;
				}
				if (Final.Route_Visit_Nodes[t][p] == n + nc)
				{
					is1 = 1;
					route1 = t;
					position1 = p;
				}
			}
		}


		//找最好的位置，调整时间差
		if (position == -1 || position1 == -1)
			continue;

		double max_zhibiao = Final.Node_Profit[n];
		double optimal_Profit = 0;
		int optimal_time0 = 0;
		int optimal_time = 0;
		double optimal_Gap_time = 0;

		double Gap_Time = 0;
		//第一次访问的可取时间
		int k0 = route;
		int i0 = position;
		int next_node0 = Final.Route_Visit_Nodes[k0][i0 + 1];
		double end0 = Final.Route_Arrive_Time[next_node0][k0] - T[n][next_node0] - Duration_Time[n];
		if (end0 > (endtimewindow[n][k0] - Duration_Time[n]))
			end0 = endtimewindow[n][k0] - Duration_Time[n];

		//第二次访问的可取时间
		int k = route1;
		int i = position1;
		int next_node = Final.Route_Visit_Nodes[k][i + 1];
		float end = Final.Route_Arrive_Time[next_node][k] - T[n + nc][next_node] - Duration_Time[n + nc];
		if (end > (endtimewindow[n + nc][k] - Duration_Time[n + nc]))
			end = endtimewindow[n + nc][k] - Duration_Time[n + nc];

		//计算所有取值，可得的收益值
		for (int t0 = ceil(Final.Route_Arrive_Time[n][k0]); t0 <= floor(end0); t0++)
		{
			for (int ti = ceil(Final.Route_Arrive_Time[n + nc][k]); ti <= floor(end); ti++)
			{
				double temp_zhibiao2 = 0;

				//计算新增收益
				Gap_Time = ti - t0;
				for (int m = 0; m < nm; m++)
				{
					if (Gap_Time >= A_time[n][m] && Gap_Time < B_time[n][m])
						temp_zhibiao2 = Profit[n][m];
				}
				if (temp_zhibiao2 > max_zhibiao)
				{
					optimal_Profit = temp_zhibiao2;
					optimal_time0 = t0;
					optimal_time = ti;
					optimal_Gap_time = Gap_Time;
					max_zhibiao = temp_zhibiao2;
				}
			}
		}

		//选指标最大的时间，插入
		if (max_zhibiao != Final.Node_Profit[n])
		{
			Final.Route_Arrive_Time[n][k0] = optimal_time0;
			Final.Route_Arrive_Time[n+nc][k] = optimal_time;
			Final.Node_Profit[n] = optimal_Profit;
			Final.Gap_Time[n] = optimal_Gap_time;
		}
	}
	Final.Profit = 0;
	for (int n = 1; n <= nc; n++)
		Final.Profit += Final.Node_Profit[n];
}


void improve2(Solution& New, Solution& Current, Solution& Final, InsertionNode* Insertion3, InsertionNode* Insertion4)
{
	//节点访问受时间窗限制，可以尝试把访问移动到收益高的时间窗内
	for (int n = 1; n <= nc; n++)
	{
		//判读是否已取得最大收益
		int max_profit = 0;
		for (int m = 0; m < nm; m++)
		{
			if (Profit[n][m] > max_profit)
				max_profit = Profit[n][m];
		}
		if (Final.Node_Profit[n] == max_profit)
			continue;


		//如果没有拿到最大收益，同一个窗口调整时间
		int route = -1;
		int route1 = -1;
		int position = -1;
		int position1 = -1;
		//找到两次访问所在的车辆和路径位置
		for (int t = 0; t < nt; t++)
		{
			for (int p = 1; p < 2 * nc + 2; p++)
			{
				if (Final.Route_Visit_Nodes[t][p] == 2 * nc + 1)
					break;

				if (Final.Route_Visit_Nodes[t][p] == n)
				{
					route = t;
					position = p;
				}
				if (Final.Route_Visit_Nodes[t][p] == n + nc)
				{
					route1 = t;
					position1 = p;
				}
			}
		}
		if (position == -1 || position1 == -1)
			continue;

		//找最好的位置，调整时间差
		int max_zhibiao = Final.Node_Profit[n];
		int optimal_InsertionPos1 = 0;//当前点插入当前路线中的位置编号
		int optimal_Insertiontour = 0;//当前点插入的路线编号
		double optimal_Profit = 0;
		int optimal_time = 0;
		double optimal_Gap_time = 0;
		double Gap_Time = 0;

		for (int k = route; k < nt; k++)
		{
			//不找自己本来在的这个车辆了
			if (k == route1)
				continue;

			if (k != route)
				position = 0;

			for (int i = position + 1; i < 2 * nc + 2; i++)
			{
				if (Final.Route_Visit_Nodes[k][i] == 2 * nc + 1 || Final.Route_Visit_Nodes[k][i] == -1)
					continue;

				int last_node = Final.Route_Visit_Nodes[k][i];
				int next_node = Final.Route_Visit_Nodes[k][i + 1];

				//取小的
				float end = Final.Route_Arrive_Time[next_node][k] - T[n + nc][next_node] - Duration_Time[n + nc];
				if (end > (endtimewindow[n + nc][k] - Duration_Time[n + nc]))
					end = endtimewindow[n + nc][k] - Duration_Time[n + nc];

				int a = max(ceil(Final.Route_Arrive_Time[last_node][k] + Duration_Time[last_node] + T[last_node][n + nc]), starttimewindow[n + nc][k]);
				if (a > end)
					continue;
				for (int ti = a; ti <= floor(end); ti++)
				{
					double temp_zhibiao2 = 0;

					//计算新增收益
					Gap_Time = ti - Final.Route_Arrive_Time[n][route];
					for (int m = 0; m < nm; m++)
					{
						if (Gap_Time >= A_time[n][m] && Gap_Time < B_time[n][m])
							temp_zhibiao2 = Profit[n][m];
					}
					if (temp_zhibiao2 > max_zhibiao)
					{
						optimal_InsertionPos1 = i+1;//当前点插入当前路线中的位置编号
						optimal_Insertiontour = k;//当前点插入的路线编号
						optimal_Profit = temp_zhibiao2;
						optimal_time = ti;
						optimal_Gap_time = Gap_Time;
						max_zhibiao = temp_zhibiao2;
					}
				}
			}
		}
		//选指标最大的时间，插入
		if (max_zhibiao > Final.Node_Profit[n])
		{
			int i = optimal_InsertionPos1;
			int k = optimal_Insertiontour;
			int ti = optimal_time;

			//////为这节点腾出位置
			for (int p = position1; p < 2 * nc + 2; p++)
				Final.Route_Visit_Nodes[route1][p] = Final.Route_Visit_Nodes[route1][p + 1];

			for (int p = 2 * nc + 1; p > i; p--)
				Final.Route_Visit_Nodes[k][p] = Final.Route_Visit_Nodes[k][p - 1];
			Final.Route_Visit_Nodes[k][i] = n + nc;



			/////////选择节点最终插入的位置的开始时间
			Final.Route_Arrive_Time[n + nc][route1] = 0;
			Final.Route_Arrive_Time[n + nc][k] = ti;

			Final.Node_Profit[n] = optimal_Profit;
			Final.Gap_Time[n] = optimal_Gap_time;

			//计算当前路线的长度，利于后面计算各点可插入的位置
			Node_Number_In_Tour[route1] -= 1;
			Node_Number_In_Tour[k] += 1;
		}
	}
	Final.Profit = 0;
	for (int n = 1; n <= nc; n++)
		Final.Profit += Final.Node_Profit[n];
}


//生成初始解前，对每个节点分别打分：节点可取得的收益，最大到最小的落差
void Heuristic_Selection(Node* nod)
{
	float* grade1 = new float[2*nc+1];//评分指标：时间窗长度
	float* grade2 = new float[2*nc+1];//评分指标：收益损失值
	for (int i = 0; i <= 2 * nc; i++)
	{
		grade1[i] = 0;
		grade2[i] = 0;
	}

	for (int i = 1; i <= 2*nc; i++)
	{
		grade1[i] = endtimewindow[i][nt - 1] - starttimewindow[i][0];
	}

	for (int i = 1; i <= nc; i++)
	{
		for (int m = 0; m < nm; m++)
		{
			if ((endtimewindow[i][nt - 1] - starttimewindow[i][0]) >= A_time[i][m])
				if (grade2[i] < Profit[i][m])
					grade2[i] = Profit[i][m];

		}
		grade2[i] -= Profit[i][0];
	}

	//计算评估值
	for (int i = 1; i <= nc; i++)
	{
		nod[i].Grade = grade2[i];// grade1[i] / 30000 + grade2[i];
		nod[i + nc].Grade = nod[i].Grade;
	}
	

	//从大到小排序
	for (int i = 1; i <= nc; i++)
		NodeSequence[i] = i;
	for (int i = 1; i <= nc; i++)
	{
		for (int i1 = nc; i1 > i; i1--)
		{
			if (nod[NodeSequence[i]].Grade < nod[NodeSequence[i1]].Grade)
			{
				int temp = NodeSequence[i];
				NodeSequence[i] = NodeSequence[i1];
				NodeSequence[i1] = temp;
			}
		}
	}

	//找到最大值
	/*float temp = 0;
	for (int i = 1; i <= nc; i++)
		if (nod[i].Grade >= temp)
			temp = nod[i].Grade;

	//归一化
	for (int i = 1; i <= nc; i++)
		nod[i].Grade = nod[i].Grade / temp;*/

		// 释放内存
	delete[] grade1;
	grade1 = nullptr;
	delete[] grade2;
	grade2 = nullptr;
}



//UCB(Upper Confidence Bound)策略，由两部分组成，一部分为每个赌博机目前位置的平均收益，另外一部分为赌博机尝试次数反比例函数
int algorithm_selection(int rounds, float Greed, int K, int* num, double* mu)
{
	K = 2;
	int chosen = -1;
	// 随机数引擎（只初始化一次）
	random_device rd;
	mt19937 gen(rd());
	bool found_untried = false;

		// 先保证每个臂至少被试验一次
		for (int j = 0; j < K; ++j) 
		{
			if (num[j] == 0) 
			{
				chosen = j;
				found_untried = true;
				break;
			}
		}

		// 若都已试过，按 UCB 选择
		if (!found_untried) 
		{
			// 初始化当前最优的 UCB 值为负无穷
			double bestUCB = -numeric_limits<double>::infinity();
			// 使用 t-1 作为已观测轮次（保证 log 参数 >= 1）
			for (int j = 0; j < K; j++)
			{
				// mu[j] 表示第 j 个臂的平均奖励
                // num[j] 表示第 j 个臂已被选择的次数
                // rounds 表示当前是第几轮（或总的轮次）
				double ucb = mu[j] + sqrt((2.0 * log(double(rounds - 1))) / double(num[j]));
				if (ucb > bestUCB) 
				{
					bestUCB = ucb;
					chosen = j;
				}
			}
		}

		// 拉取 chosen 臂，获得奖励
		double reward = 0;
		if (chosen == 0)
			reward = InitialSolution4(FirIn, New, Current, Greed);
		//if (chosen == 1)
			//reward = InitialSolution2(FirIn, New, Current, Greed);
		if (chosen == 1)
			reward = InitialSolution3(FirIn, New, Current, Greed);


		// 更新平均奖励与计数
		mu[chosen] = (num[chosen] * mu[chosen] + reward) / double(num[chosen] + 1);
		num[chosen] += 1;


	// 输出结果
	/*cout << fixed << setprecision(4);
	cout << "Final estimated means and pull counts:\n";
	for (int j = 0; j < K; ++j) {
		cout << "Arm " << j << ": mu = " << mu[j]
			<< ", pulls = " << num[j] << '\n';
	}

	cout << "选中的算法是：" << chosen << endl;
	cout << "奖励：" << reward << endl;
	cout << endl;*/

	return 0;
}

void FreeMemory()
{
	delete[] isChoose_Remove;
	isChoose_Remove = nullptr;

	delete[] isChoose_Change;
	isChoose_Change = nullptr;

	delete[] NodeSequence;
	NodeSequence = nullptr;

	delete[] Node_Number_In_Tour;
	Node_Number_In_Tour = nullptr;

	delete[] Insertion3;
	Insertion3 = nullptr;

	delete[] Insertion4;
	Insertion4 = nullptr;

	delete[] Inserted_Sequence3;
	Inserted_Sequence3 = nullptr;

	delete[] Inserted_Sequence4;
	Inserted_Sequence4 = nullptr;


	for (int p = 0; p < 2 * nc + 2; p++) {
		delete[] Current_Tour[p];
		delete[] Temp_Tour[p];
		delete[] Tour[p];
		delete[] Tour1[p];
	}
	delete[] Current_Tour;
	delete[] Temp_Tour;
	delete[] Tour;
	delete[] Tour1;

	Current_Tour = nullptr;
	Temp_Tour = nullptr;
	Tour = nullptr;
	Tour1 = nullptr;


}
void delete_memory()
{
	// 1. 释放节点数组（一维）
	delete[] nod;
	nod = nullptr;

	// 2. 释放距离矩阵 D（二维）
	for (int i = 0; i < 2 * nc + 2; i++) {
		delete[] D[i];
	}
	delete[] D;
	D = nullptr;

	// 3. 释放时间矩阵 T（二维）
	for (int i = 0; i < 2 * nc + 2; i++) {
		delete[] T[i];
	}
	delete[] T;
	T = nullptr;

	// 4. 释放收益矩阵 Profit（二维）
	for (int i = 0; i <= nc; i++) {
		delete[] Profit[i];
	}
	delete[] Profit;
	Profit = nullptr;

	// 5. 释放时间窗 starttimewindow（二维）
	for (int i = 0; i < 2 * nc + 2; i++) {
		delete[] starttimewindow[i];
	}
	delete[] starttimewindow;
	starttimewindow = nullptr;

	// 6. 释放时间窗 endtimewindow（二维）
	for (int i = 0; i < 2 * nc + 2; i++) {
		delete[] endtimewindow[i];
	}
	delete[] endtimewindow;
	endtimewindow = nullptr;

	// 7. 释放任务执行时间 Duration_Time（一维）
	delete[] Duration_Time;
	Duration_Time = nullptr;

	// 8. 释放时间段 A_time（二维）
	for (int i = 0; i < nc + 1; i++) {
		delete[] A_time[i];
	}
	delete[] A_time;
	A_time = nullptr;

	// 9. 释放时间段 B_time（二维）
	for (int i = 0; i < nc + 1; i++) {
		delete[] B_time[i];
	}
	delete[] B_time;
	B_time = nullptr;
}

//算法求解
void Solve(string strSolutionFile, vector<double>& bestCurve) 
{
	isChoose_Remove = new int[Max_Node+1];
	isChoose_Change = new int[Max_Node+1];
	NodeSequence = new int[Max_Node + 1];


	//每条路线的现有节点数，初始为0	
	Node_Number_In_Tour = new int[Max_Tour];


	InsertionNode*Insertion3;//定义插入节点类的一个对象
	Insertion3 = new InsertionNode[1000000];
	InsertionNode*Insertion4;
	Insertion4 = new InsertionNode[1000000];

	//节点插入后，在各位置收益的增加按大到小排序
	Inserted_Sequence3 = new int[1000000];
	Inserted_Sequence4 = new int[1000000];


	clock_t startTime, endTime, endTime1;//定义计时
	startTime = clock();//开始计时
	Final.Profit = 0;

	//贪婪随机自适应迭代局部搜索算法
	//第一步，Heuristic Selection
	Heuristic_Selection(nod);
	//for (int i = 1; i <= nc; i++)
		//cout << setw(3)<< NodeSequence[i] << "  ";


	//UCB策略初始化
	int rounds = 1;       // 总轮数
	int K = 3;
	int* num = new int[K];        // 每个臂被拉的次数
	double *mu = new double [K];  // 每个臂的样本平均奖励
	for (int nu = 0; nu < K; nu++)
	{
		num[nu] = 0;
		mu[nu] = 0;
	}


	//外循环
	float Greed = 0.89;//算法控制贪婪因子参数 Greed(Greed ∈ (0, 1)) 的值
	while (Greed > (0.89 - 0.3))
	{
		//初始化参数
		Initial_Parameter(FirIn, New, Current, Insertion3, Insertion4);
		for (int i = nc * Greed + 1; i <= nc; i++)
		{
			nod[NodeSequence[i]].Grade = 0;
			nod[NodeSequence[i] + nc].Grade = 0;
		}



		//选择初始解算法
		srand(time(NULL));
		/*int chosen = std::rand() % 2;      // 结果为 0、1 或 2
		int reward = 0;
		if (chosen == 0)
			reward = InitialSolution4(FirIn, New, Current, Greed);
		else if (chosen == 1)
			reward = InitialSolution2(FirIn, New, Current, Greed);
		//else if (chosen == 2)
			//reward = InitialSolution3(FirIn, New, Current, Greed);
			*/

		algorithm_selection(rounds, Greed, K, num, mu);
		rounds += 1;
		Show_InitialSolution();
		for (int t = 0; t < nt; t++)
		{
			for (int i = 0; i < 2 * nc + 2; i++)
			{
				Final.Route_Visit_Nodes[t][i] = New.Route_Visit_Nodes[t][i];

			}
		}
		for (int t = 0; t < nt; t++)
		{
			for (int n1 = 0; n1 < 2 * nc + 2; n1++)
			{
				Final.Route_Arrive_Time[n1][t] = New.Route_Arrive_Time[n1][t];
			}
		}
		Final.Profit = 0;
		for (int n2 = 1; n2 <= nc; n2++)
		{
			Final.Gap_Time[n2] = New.Gap_Time[n2];
			Final.Node_Profit[n2] = New.Node_Profit[n2];
			Final.Profit += Final.Node_Profit[n2];
		}


		endTime1 = clock();//结束计时
		//cout << "初始解：" << FirIn.Profit << endl;//显示目标值
		//cout << "初始解求解时间：" << (double)(endTime1 - startTime) / CLOCKS_PER_SEC << "s" << endl;//显示求解时间

		float Obj = 0;

		///局部搜索算法求解
		int global_Iteration1 = 0;
		int global_Iteration = 0;
		while (global_Iteration < 2)
		{
			//单节点移除算子，移除后贪婪修复算子
			Update_Solution_Remove_One(New, Final, Insertion3, Insertion4, Current);
			Update_Loacl_Solution();


			//节点对移除算子，移除后贪婪修复算子
			Update_Solution_Remove(New, Final, Insertion3, Insertion4, Current);
			Update_Loacl_Solution();

			//Update_Solution_Relocate(New, Final, Current);
			//Update_Loacl_Solution();

			//单节点交换算子，交换后贪婪修复算子
			//Update_Solution_Change(New, Current, Final, Insertion3, Insertion4, AT);
			//Update_Loacl_Solution();
			//Update_Solution_Swap(New, Final, Current);

			//Update_Solution_SwapPair(New, Final, Current);

			//节点对交换算子，交换后贪婪修复算子
			//Update_Solution_Change2(New, Current, Final, Insertion3, Insertion4, AT);
			//Update_Loacl_Solution();



			peidui_check(New, Current, Final);
			improve(New, Current, Final, Insertion3, Insertion4);
			improve2(New, Current, Final, Insertion3, Insertion4);
			Update_Loacl_Solution();

			/*if (Final.Profit > tishengqian)
			{
				cout << "tishenghou " << Final.Profit << endl;
				Show_optimalSolution();
				cout << endl;
			}*/


			if (Final.Profit > Obj)
			{
				Obj = Final.Profit;
				global_Iteration = 0;
			}
			else
				global_Iteration++;


			//cout << "本次迭代最好的解" << Final.Profit << endl;//显示目标值

			global_Iteration1++;

			// ⭐ 每一轮都记录
			int a=max(Final.Profit, Obj, Final0.Profit);
			bestCurve.push_back(a);


			if (global_Iteration1 > 50)
				break;

		}
		if (Final.Profit > Final0.Profit)
		{
			for (int t = 0; t < nt; t++)
			{
				for (int i = 0; i < 2 * nc + 2; i++)
				{
					Final0.Route_Visit_Nodes[t][i] = Final.Route_Visit_Nodes[t][i];

				}
			}
			for (int t = 0; t < nt; t++)
			{
				for (int n1 = 0; n1 < 2 * nc + 2; n1++)
				{
					Final0.Route_Arrive_Time[n1][t] = Final.Route_Arrive_Time[n1][t];
				}
			}
			Final0.Profit = 0;
			for (int n2 = 1; n2 <= nc; n2++)
			{
				Final0.Gap_Time[n2] = Final.Gap_Time[n2];
				Final0.Node_Profit[n2] = Final.Node_Profit[n2];
				Final0.Profit += Final0.Node_Profit[n2];
			}
		}
		//cout << "最好的解:" << Final0.Profit << endl;
		Greed -= 0.05;   // 步长GreedDecrease
	}

	// 使用完后释放
	delete[] num;
	delete[] mu;
	FreeMemory();

	endTime = clock();//结束计时

	cout << "最终解" << Final0.Profit << endl;//显示目标值
	cout << "运行时间:" << '\t' << endTime - startTime << endl;//总耗时

	TIME = endTime - startTime;

}

//输出并存储结果
void Output(string strSolutionFile)
{

	//////定义result数据来源的路径，用来存储运行时间、目标值等结果	
	ofstream outFile(strSolutionFile.c_str());
	outFile << "初始解=" << FirIn.Profit << endl;//目标值
	outFile << "算法优化解=" << Final0.Profit << endl;//目标值
	outFile << "运行时间:" << '\t' << TIME << endl;//总耗时

	outFile << endl;//换行

	//路线解
	for (int t = 0; t < nt; t++)
	{
		for (int i = 0; i < 2 * nc + 2; i++)
		{
			outFile << Final0.Route_Visit_Nodes[t][i] << "  ";
		}
		outFile << endl;
	}
	outFile << endl;//换行

//开始时间解
	outFile << "S[i][t]" << endl;//换行
	for (int i = 1; i < 2 * nc + 1; i++)
	{
		for (int t = 0; t < nt; t++)
		{
			outFile << Final0.Route_Arrive_Time[i][t] << "           ";
		}
		outFile << endl;
	}

	outFile.close();
}
void CheckSolution()
{
	cout << "\n========== CHECK SOLUTION ==========\n";

	vector<int> visitCount(nc + 1, 0);
	double recomputedTotal = 0;

	// ================= 路径检查 =================
	for (int t = 0; t < nt; t++)
	{
		cout << "\n[Route " << t + 1 << "]\n";

		for (int i = 0; i < 2 * nc + 2; i++)
		{
			int u = Final0.Route_Visit_Nodes[t][i];
			float st = Final0.Route_Arrive_Time[u][t];

			if (u == 0 || u == -1) continue; // 空节点

			// ===== 时间窗检查 =====
			if (st < starttimewindow[u][t] - 1e-6 ||
				st > endtimewindow[u][t] + 1e-6)
			{
				cout << "❌ 时间窗违规: node " << u
					<< " time=" << st
					<< " TW=[" << starttimewindow[u][t]
					<< "," << endtimewindow[u][t] << "]\n";
			}

			// ===== 时间衔接检查 =====
			if (i > 0)
			{
				int prev = Final0.Route_Visit_Nodes[t][i - 1];
				if (prev != 0)
				{
					float expected =
						Final0.Route_Arrive_Time[prev][t]
						+ Duration_Time[prev]
						+ T[prev][u];

					if (st < expected - 1)
					{
						cout << "❌ 时间衔接错误: "
							<< prev << " -> " << u
							<< " expected=" << expected
							<< " actual=" << st << "\n";
					}
				}
			}

			// ===== 访问计数 =====
			if (u >= 1 && u <= nc)
				visitCount[u]++;
			if (u > nc && u <= 2 * nc)
				visitCount[u - nc]++;
		}
	}

	// ================= 流平衡检查 =================
	cout << "\n[Flow Check]\n";
	for (int c = 1; c <= nc; c++)
	{
		if (visitCount[c] != 0 && visitCount[c] != 2)
		{
			cout << "❌ 客户 " << c
				<< " 访问次数错误: "
				<< visitCount[c] << " (应为0或2)\n";
		}
	}

	// ================= 收益检查 =================
	cout << "\n[Profit Check]\n";

	for (int c = 1; c <= nc; c++)
	{
		float t1 = -1, t2 = -1;

		for (int t = 0; t < nt; t++)
		{
			for (int i = 0; i < 2 * nc + 2; i++)
			{
				int u = Final0.Route_Visit_Nodes[t][i];

				if (u == c)
					t1 = Final0.Route_Arrive_Time[u][t];

				if (u == c + nc)
					t2 = Final0.Route_Arrive_Time[u][t];
			}
		}

		if (t1 < 0 && t2 < 0) continue; // 没服务

		if (t1 < 0 || t2 < 0)
		{
			cout << "❌ 客户 " << c << " 配对不完整\n";
			continue;
		}

		float iv = t2 - t1;

		bool found = false;
		double pf = 0;

		for (int m = 0; m < nm; m++)
		{
			if (iv >= A_time[c][m] && iv < B_time[c][m])
			{
				pf = Profit[c][m];
				found = true;
				break;
			}
		}

		if (!found)
		{
			cout << "❌ 客户 " << c
				<< " interval 无匹配区间 iv=" << iv << "\n";
		}

		recomputedTotal += pf;
	}

	cout << "\n[Total Profit]\n";
	cout << "算法结果: " << Final0.Profit << endl;
	cout << "重算结果: " << recomputedTotal << endl;

	if (fabs(Final0.Profit - recomputedTotal) > 1e-6)
		cout << "❌ 收益不一致！\n";
	else
		cout << "✅ 收益正确\n";

	cout << "====================================\n";
}



void RunExperimentToSummary(ofstream& summary,
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
		for (int t = 0; t < nt; t++)
		{
			for (int i = 0; i < 2 * nc + 2; i++)
			{
				Final0.Route_Visit_Nodes[t][i] = 0;

			}
		}
		for (int t = 0; t < nt; t++)
		{
			for (int n1 = 0; n1 < 2 * nc + 2; n1++)
			{
				Final0.Route_Arrive_Time[n1][t] = 0;
			}
		}
		Final0.Profit = 0;
		for (int n2 = 1; n2 <= nc; n2++)
		{
			Final0.Gap_Time[n2] = 0;
			Final0.Node_Profit[n2] = 0;
			Final0.Profit += Final0.Node_Profit[n2];
		}

		clock_t start = clock();
		vector<double> bestCurve;

		Solve(instanceFile, bestCurve);
		allBestCurves.push_back(bestCurve);
		//Show_optimalSolution();
		CheckSolution();
		clock_t end = clock();

		double t = (double)(end - start) / CLOCKS_PER_SEC;
		double obj = Final0.Profit;

		objs.push_back(obj);
		times.push_back(t);

		// 写每一行
		summary << instanceName << ","
			<< r + 1 << ","
			<< obj << ","
			<< t << "\n";
	}
	//释放内存
	delete_memory();

	int T = allBestCurves[0].size();

	ofstream fcurve("convergence_ALNS_avg.csv");
	fcurve << "Iter,BestAvg\n";

	for (int i = 0; i < T; i++) {

		double sum = 0;

		for (int r = 0; r < allBestCurves.size(); r++)
			sum += allBestCurves[r][i];

		fcurve << i + 1 << "," << sum / allBestCurves.size() << "\n";
	}

	fcurve.close();

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


	// 写 mean
	summary << instanceName << ",Mean,"
		<< meanObj << ","
		<< stdObj << ","
		<< meanTime << ","
		<< stdTime << "\n";


	// 写 best
	summary << instanceName << ",Best,"
		<< bestObj << ",-\n";
}


// ================= MAIN =================
int main(int argc, char** argv)
{
	srand(time(NULL));

	ofstream summary("summary.csv");

	// 表头
	summary << "Instance,Run,Obj,Time\n";
	summary << "Instance,Stat,MeanObj,StdObj,MeanTime,StdTime\n";


	char ss[100];

	for (int i = 50; i <= 50; i += 10)
	{
		for (int k = 6; k <= 6; k += 2)
		{
			for (int p = 1; p <= 1; p++)
			{
				sprintf_s(ss, "%d_%d_%d", i, k, p);
				string instanceName = ss;

				sprintf_s(ss, "Problem Instances\\%d_%d_%d.txt", i, k, p);
				//sprintf_s(ss, "三种算例\\%d_%d_%d.txt", i, k, p);
				string instanceFile = ss;

				cout << "\n=============================\n";
				cout << "Instance: " << instanceName << endl;
				cout << "=============================\n";

				RunExperimentToSummary(summary, instanceName, instanceFile, 10);
			}
		}
	}

	summary.close();

	cout << "\n✅ 所有算例已输出到 summary.csv\n";

	return 0;
}
