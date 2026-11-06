#pragma once
#include <iostream>
#include"math.h"
#include <windows.h>
#include<string>


#define Max_Tour 10 //最多的路线
#define Max_Node 200 //最多的节点数

int nc, nt, nm;
//节点信息数据
class Node
{
public:
	int Node_No;
	float Coordinate_X;
	float Coordinate_Y;
	float Grade;
public:
	Node()
	{
		Node_No = -1;
		Coordinate_X = 0.0;
		Coordinate_Y = 0.0;
		Grade = 0.00;
	}
};
Node *nod;//定义了一个Nod新对象
float **D;
float **T;
int *Fixed_Profit;
float **Profit;
float ** starttimewindow;
float ** endtimewindow;
float * Duration_Time;
float ** A_time;
float ** B_time;



//调整开始访问时间
class Arrival_Time_Adjust
{
public:
	float Node_FA[Max_Tour][2 * Max_Node + 2];  /////表示路线中各节点开始访问时间能够减少的大小；
	float Node_BA[Max_Tour][2 * Max_Node + 2];////表示路线中各节点开始访问时间能够增加的大小；
	float Position_FA[Max_Tour][2 * Max_Node + 2];////当前路线第i个节点前（不含I） 可向前调整的时间
	float Position_BA[Max_Tour][2 * Max_Node + 2];////当前路线第i个节点后（含I） 可向后调整的时间
	int isUnfeasible[Max_Tour];////当前 路线 不可行
public:
	Arrival_Time_Adjust()
	{
		for (int r = 0; r < Max_Tour; r++)
		{
			for (int i = 0; i < 2 * Max_Node + 2; i++)
			{
				Node_FA[r][i] = 100;
				Node_BA[r][i] = 100;
				Position_FA[r][i] = 100;
				Position_BA[r][i] = 100;
			}
			isUnfeasible[r] = 0;
		}
	}
	void Initial()//将下列变量初始化
	{
		for (int r = 0; r < Max_Tour; r++)
		{
			for (int i = 0; i < 2 * Max_Node + 2; i++)
			{
				Node_FA[r][i] = 100;
				Node_BA[r][i] = 100;
				Position_FA[r][i] = 100;
				Position_BA[r][i] = 100;
			}
			isUnfeasible[r] = 0;
		}
	}
};
/*
class Insert_Information
{
public:
	float Need_ForwardT[2 * Max_Node + 2][Max_Tour][2 * Max_Node + 2];
	float Need_BackwardT[2 * Max_Node + 2][Max_Tour][2 * Max_Node + 2];

public:
	Insert_Information()
	{
		for (int n = 0; n < 2 * Max_Node + 2; n++)
		{
			for (int p = 0; p < Max_Tour; p++)
			{
				for (int q = 0; q < 2 * Max_Node + 2; q++)
				{
					Need_ForwardT[n][p][q] = 0;
					Need_BackwardT[n][p][q] = 0;
				}
			}
		}
	}
};
*/
///初始解 用于记录初始解，即四个决策变量（）
class Solution
{
public:
	int Route_Visit_Nodes[Max_Tour][2 * Max_Node + 2]; ///每路线访问的节点序列
	float Route_Arrive_Time[2 * Max_Node + 2][Max_Tour]; ///每路线的开始访问时间
	float Gap_Time[Max_Node+1];///每节点两次访问的间隔时间
	float Node_Profit[Max_Node+1];///每节点两次访问获得的收益
	float Profit;//总收益
public:
	Solution()
	{
		for (int k = 0; k < Max_Tour; k++)
		{
			for (int n = 0; n < 2 * Max_Node + 2; n++)
				Route_Visit_Nodes[k][n] = -1;
		}
		for (int n = 0; n < 2 * Max_Node + 2; n++)
		{
			for (int k = 0; k < Max_Tour; k++)
				Route_Arrive_Time[n][k] = 0.00;
		}
		for (int n = 0; n <= Max_Node; n++)
			Gap_Time[n] = -1;
		for (int n = 0; n <= Max_Node; n++)
			Node_Profit[n] = 0.00;
		Profit = 0.00;
	}
};


///插入节点对
class InsertionNode //每对节点遍历插入当前路线中各个位置时，记录以下各变量值
{
public:
	int NodeNO1; //一次访问节点编号
	int InsertionPos1; //一次访问节点插入当前路线中的位置编号
	int NodeNO2; //二次访问节点编号
	int InsertionPos2; //二次访问节点插入当前路线中的位置编号
	float Increase_Profit; //两次访问产生的收益
	int Insertiontour;//一次访问路线编号
	int Insertiontour1;//二次访问路线编号
public:
	InsertionNode()
	{
		NodeNO1 = -1;
		InsertionPos1 = -1;
		NodeNO2 = -1;
		InsertionPos2 = -1;
		Increase_Profit = 0.00;
		Insertiontour = -1;
		Insertiontour1 = -1;
	}
};

	int* isChoose_Remove;/////判断客户是否被选中移除；
	int*isChoose_Change;/////判断客户是否被选中交换；
	int Node_Number;
	int All_Node_Number;
	int *NodeSequence;
	int *Node_Number_In_Tour;
	int *Inserted_Sequence3;
	int *Inserted_Sequence4;
	int *Inserted_Sequence;
	int *Inserted_Sequence2;
	int QUIT;

	int **Current_Tour;
	int **Temp_Tour;
	int **Tour;
	int **Tour1;
	InsertionNode*Insertion;//定义插入节点类的一个对象
	InsertionNode*Insertion2;
	InsertionNode* Insertion3;//定义插入节点类的一个对象
	InsertionNode* Insertion4;

	////定义解
	Solution FirIn;//初始解
	Solution New;//新解
	Solution Current;//当前解
	Solution Final;//最终解
	Solution Final0;//最终解
	double TIME;