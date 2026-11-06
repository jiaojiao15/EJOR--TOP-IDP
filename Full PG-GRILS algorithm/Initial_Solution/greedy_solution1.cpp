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



using  std::cout;
using  std::endl;
using  std::string;
using  std::ostringstream;
using  std::vector;

using namespace std;


//############################################以下是求解所需的函数      
//选择插入节点的序列（按插入后增加的收益，非增排序）
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

void sorting_Inserted_Sequence_t1(int *NodeSequence, float **starttimewindow, int Node_Number)
{
	int Temp;
	for (int i = 1; i <= Node_Number; i++)
		for (int j = Node_Number; i < j; j--)
			if (starttimewindow[i][0] > starttimewindow[j][0])// 
			{
				Temp = NodeSequence[i];
				NodeSequence[i] = NodeSequence[j];
				NodeSequence[j] = Temp;
			}
}



void InitialSolution1(First_Inition_Solution&FirIn, New_Solution&New, Current_Solution& Current, int &Node_Number, int *NodeSequence, int *&Node_Number_In_Tour, float **D, float **T, float *Duration_Time, int nm, int nc, int nt, float**starttimewindow, float**endtimewindow, int * Fixed_Profit, int**Profit, float**A_time, float**B_time)
//初始解（初始解指针，距离矩阵，时间矩阵，节点类的子节点对象，任务执行时间，时间段数，节点数，路线数，开始时间窗，结束时间窗，剩余未插入节点数指针，收益，时间段的开始时间，时间段的结束时间，间隔时间）
{

	int OK = 0;
	///////////以下四个为路线，用来记录插入节点，四个交换使用，不断更新，最后的”-1"来判断线路的结束
	int **Current_Tour = new int*[2 * nc + 2];//暂存路线，用于交换
	int **Temp_Tour = new int*[2 * nc + 2];//暂存路线，用于交换
	int **Tour = new int*[2 * nc + 2];//开辟新路线（只包含起点和终点），与Temp_Tour交换，确定当前路线各位置插入节点情况
	int **Tour1 = new int*[2 * nc + 2];//最终路线，与Current_Tour交换，确定最终路线，并将值赋给初始解保存
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

	InsertionNode*Insertion;//定义插入节点类的一个对象
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

	InsertionNode*Insertion2;//定义插入节点类的一个对象
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

	//开辟新路线，新路线只包含起点和终点
	for (int k = 0; k < nt; k++)
	{
		Tour[0][k] = 0;
		Tour[1][k] = 2 * nc + 1;
		Tour1[0][k] = 0;
		Tour1[1][k] = 2 * nc + 1;
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

	//节点插入后，在各位置收益的增加按大到小排序
	int *Inserted_Sequence = new int[1000000];
	for (int p = 0; p < 1000000; p++)
		Inserted_Sequence[p] = -1;
	int *Inserted_Sequence2 = new int[1000000];
	for (int p = 0; p < 1000000; p++)
		Inserted_Sequence2[p] = -1;

	for (int k = 0; k < nt; k++)
	{
		FirIn.Route_Arrive_Time[0][k] = 0;//起点的开始访问时间
		FirIn.Route_Arrive_Time[2 * nc + 1][k] = 86400;//终点的开始访问时间
	}

	for (int i = 1; i <= Node_Number; i++)//可修改i<5,对一半乱序交换即可
	{
		int j = 1;
		while (j == i)
		{
			srand(time(NULL));
			j = Random(nc - 1) + 1;//可以自己构造一个其他的函数，保证与10求余后不与原位置相同即可
		}
		cout << "weizhi" << j << endl;
		int temp = NodeSequence[i];
		NodeSequence[i] = NodeSequence[j];
		NodeSequence[j] = temp;
	}
	cout << Node_Number << endl;
	for (int n = 1; n <= Node_Number; n++)
		cout << setw(4) << NodeSequence[n];
	cout << endl;

	///////////////////////////////////////////////////////////开始求解，插入节点对
	for (int n = 1; n <= Node_Number; n++)//Node_Number和NodeSequence一直再更新变化着！
	{
		//区间排序
		int *Sort_Interval = new int[nm];
		for (int m = 0; m < nm; m++)
		{
			Sort_Interval[m] = m;
		}
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
							FirIn.Route_Arrive_Time[NodeSequence[n] + nc][kk] = starttimewindow[NodeSequence[n] + nc][k];
							if (starttimewindow[NodeSequence[n] + nc][kk] <= FirIn.Route_Arrive_Time[NodeSequence[n]][k] + A_time[NodeSequence[n]][3] && FirIn.Route_Arrive_Time[NodeSequence[n]][k] + A_time[NodeSequence[n]][3] <= endtimewindow[NodeSequence[n] + nc][kk] - Duration_Time[NodeSequence[n] + nc])
								FirIn.Route_Arrive_Time[NodeSequence[n] + nc][kk] = FirIn.Route_Arrive_Time[NodeSequence[n]][k] + A_time[NodeSequence[n]][3];
							if (starttimewindow[NodeSequence[n] + nc][kk] <= FirIn.Route_Arrive_Time[NodeSequence[n]][k] + A_time[NodeSequence[n]][2] && FirIn.Route_Arrive_Time[NodeSequence[n]][k] + A_time[NodeSequence[n]][2] <= endtimewindow[NodeSequence[n] + nc][kk] - Duration_Time[NodeSequence[n] + nc])
								FirIn.Route_Arrive_Time[NodeSequence[n] + nc][kk] = FirIn.Route_Arrive_Time[NodeSequence[n]][k] + A_time[NodeSequence[n]][2];
							if (starttimewindow[NodeSequence[n] + nc][kk] <= FirIn.Route_Arrive_Time[NodeSequence[n]][k] + A_time[NodeSequence[n]][1] && FirIn.Route_Arrive_Time[NodeSequence[n]][k] + A_time[NodeSequence[n]][1] <= endtimewindow[NodeSequence[n] + nc][kk] - Duration_Time[NodeSequence[n] + nc])
								FirIn.Route_Arrive_Time[NodeSequence[n] + nc][kk] = FirIn.Route_Arrive_Time[NodeSequence[n]][k] + A_time[NodeSequence[n]][1];
							if (starttimewindow[NodeSequence[n] + nc][kk] <= FirIn.Route_Arrive_Time[NodeSequence[n]][k] + A_time[NodeSequence[n]][0] && FirIn.Route_Arrive_Time[NodeSequence[n]][k] + A_time[NodeSequence[n]][0] <= endtimewindow[NodeSequence[n] + nc][kk] - Duration_Time[NodeSequence[n] + nc])
								FirIn.Route_Arrive_Time[NodeSequence[n] + nc][kk] = FirIn.Route_Arrive_Time[NodeSequence[n]][k] + A_time[NodeSequence[n]][0];
						}
						else if (b <= (endtimewindow[NodeSequence[n] + nc][kk] - Duration_Time[NodeSequence[n] + nc]))
						{
							FirIn.Route_Arrive_Time[NodeSequence[n] + nc][kk] = b;
							if (b <= FirIn.Route_Arrive_Time[NodeSequence[n]][k] + A_time[NodeSequence[n]][3] && FirIn.Route_Arrive_Time[NodeSequence[n]][k] + A_time[NodeSequence[n]][3] <= endtimewindow[NodeSequence[n] + nc][kk] - Duration_Time[NodeSequence[n] + nc])
								FirIn.Route_Arrive_Time[NodeSequence[n] + nc][kk] = FirIn.Route_Arrive_Time[NodeSequence[n]][k] + A_time[NodeSequence[n]][3];
							if (b <= FirIn.Route_Arrive_Time[NodeSequence[n]][k] + A_time[NodeSequence[n]][2] && FirIn.Route_Arrive_Time[NodeSequence[n]][k] + A_time[NodeSequence[n]][2] <= endtimewindow[NodeSequence[n] + nc][kk] - Duration_Time[NodeSequence[n] + nc])
								FirIn.Route_Arrive_Time[NodeSequence[n] + nc][kk] = FirIn.Route_Arrive_Time[NodeSequence[n]][k] + A_time[NodeSequence[n]][2];
							if (b <= FirIn.Route_Arrive_Time[NodeSequence[n]][k] + A_time[NodeSequence[n]][1] && FirIn.Route_Arrive_Time[NodeSequence[n]][k] + A_time[NodeSequence[n]][1] <= endtimewindow[NodeSequence[n] + nc][kk] - Duration_Time[NodeSequence[n] + nc])
								FirIn.Route_Arrive_Time[NodeSequence[n] + nc][kk] = FirIn.Route_Arrive_Time[NodeSequence[n]][k] + A_time[NodeSequence[n]][1];
							if (b <= FirIn.Route_Arrive_Time[NodeSequence[n]][k] + A_time[NodeSequence[n]][0] && FirIn.Route_Arrive_Time[NodeSequence[n]][k] + A_time[NodeSequence[n]][0] <= endtimewindow[NodeSequence[n] + nc][kk] - Duration_Time[NodeSequence[n] + nc])
								FirIn.Route_Arrive_Time[NodeSequence[n] + nc][kk] = FirIn.Route_Arrive_Time[NodeSequence[n]][k] + A_time[NodeSequence[n]][0];

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

			OK = NodeSequence[n];//记录当前插入节点序号

			//把插入的节点从剩余未插入节点序列给删除掉
			for (int p = n; p <= Node_Number; p++)
			{
				if (NodeSequence[Node_Number] == NodeSequence[n])//如果是最后一个!那就直接赋值为-
					NodeSequence[Node_Number] = -1;
				else
					NodeSequence[p] = NodeSequence[p + 1];
			}
			Node_Number = Node_Number - 1;


			///将这些节点按照与当前路线访问节点的距离进行排序；
			int c;
			for (int i4 = 1; i4 <= Node_Number; i4++)
			{
				for (int j4 = Node_Number; j4 >= 1; j4--)
				{
					if (D[OK][NodeSequence[i4]] > D[OK][NodeSequence[j4]])
					{
						c = NodeSequence[j4];
						NodeSequence[j4] = NodeSequence[i4];
						NodeSequence[i4] = c;
					}
				}
			}
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
	{
		FirIn.Profit = FirIn.Profit + FirIn.Node_Profit[n];
	}

	/////////////////////////////////////////////////////显示结果
	///显示路径结果
	cout << "初始解为：" << endl;
	for (int k = 0; k < nt; k++)
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
			cout << setw(8) << FirIn.Route_Arrive_Time[n][k];
		}
		cout << endl;
	}
	cout << endl;
	cout << "各节点两次访问的间隔时间:" << endl;
	for (int n = 1; n <= nc; n++)
	{
		cout << setw(8) << FirIn.Gap_Time[n];
	}
	cout << endl;
	cout << "各节点两次访问的收益：" << endl;
	for (int n = 1; n <= nc; n++)
	{
		cout << setw(8) << FirIn.Node_Profit[n];
	}
	cout << endl;
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

