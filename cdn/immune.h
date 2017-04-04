#include "config.h"
#include "define.h"
#include <iostream>
#include <vector>
#include <memory.h>
#include <time.h>
#include <algorithm>
using namespace std;
//#define _GENERATION_AMOUNT 20
//����ȫ�ֳ���
const int C = _PONISH_FACTOR; //ponishment
const float ALPHA = _ALPHA; //im_Probability=ALPHA*im_affinity+(1-ALPHA)*im_density

typedef vector<uint_16> T;
typedef vector<float> F;
//typedef vector<int> P;
//typedef vector<float> F;
typedef struct _Map_im_Probability		//����ӳ����
{
	int i;
	float vProbability;
}Map_imProbability;

bool comp1(const Map_imProbability &a,const Map_imProbability &b)	//�����ú���
{
    return a.vProbability>b.vProbability;
}

bool comp2(const float &a,const float &b)	//�����ú���
{
    return a>b;
}

inline int fnRndBoundary(int iBegin, int iEnd)	//�����������±�
{

	return rand()%(iEnd-iBegin) + iBegin;
}



typedef struct Immune_global_t
{
  int im_number;        //��ʾδ֪������
  int im_maxserver;
  int im_scost;
  int deno;             //��ʾim_number*_GENERATION_AMOUNT
  float pcross;
  float pmutation;
}Immune_global;
//��Ҫ����T,P,STAT
//T��Ⱥ��������P����Ӧ��, F��ʾ��������
class immune
{
public:
  immune();
  immune(CUSTOMER customer, uint_16 node_num, uint_16 cost);    //OK���캯����������������vector���飩
  ~immune();

  //bool imGetCost(const int cost); //��ȡÿ̨�������ɱ�
  //vector<T> imSendSpecies();       //�Ѳ�����������Ⱥ���͸���С����������
  bool imReceiveAfinity();    //��ȡ��С�����������������׺Ͷ���Ϣ,������·���� �� ������

  bool imCreateGene();        //������ʼ��Ⱥ
  bool imGeneChoose();        //����ѡ��
  bool imGeneMix();           //���򽻲�
  bool imGeneAberrance();     //��������
  bool imGeneSort();			//������ֳ������
  bool im_memoryGeneSort();	//���¼�����
	bool im_RETGen_Gene();		//�����µ���Ⱥ
  bool im_updateGen_Gene(Feedback const &fb);

  //bool imEvalAll();           //�������л�����Ӧ��
  bool imAfinity(T link_cost, T flow_err);           //�������л����׺Ͷ�
  //bool imAfinityOne(T &Gene);    //����ĳһ�������׺Ͷ�
  bool imDensity();           //������Ⱥ�ܶ�/Ũ��
  int imDensityOne(T &Gene);    //����ĳһ������ͬ�������������ƶ�
  bool imBreedProbability();    //��ֳ����

  //bool imPutGenetoMemory(T Gene);   //��һ��������������ϸ����
  //bool imGetGenefromMemory();   //�Ӽ���ϸ���л�ȡ����

  void imPrintInfo();

  vector<T> im_Gen_Gene;  //��Ⱥ��Ϣ
  vector<T> im_memory;  //����ϸ��
  vector<T> im_Gen_Gene_Father;   //������Ⱥ

  F im_Probability;
  F im_memoryProbability;
  F im_affinity;
  F im_density;
  //float im_Probability[_GENERATION_AMOUNT];     //P����ѡ������
  //float im_memory_Probability[_GENERATION_AMOUNT];  //����������ѡ������
  //float im_affinity[_GENERATION_AMOUNT];        //F�׺Ͷ�
  //float im_density[_GENERATION_AMOUNT];         //FŨ��

  Immune_global ginfo;  //ȫ����Ϣ
private:
  bool imGeneAberranceOne(int index);   //����ĳ������
  //bool imWheel(T Gene);     //���̶�
};

immune::immune()
{
}


immune::~immune()
{
}

immune::immune(CUSTOMER customer, uint_16 node_num, uint_16 cost)
{//��ʼ��ȫ�ֱ���
  //���ؿ��н����ڵ�һ��
  int c_num = customer.size();
  int n_num = node_num;
  ginfo.im_number = n_num;
  ginfo.im_scost = cost;
  ginfo.im_maxserver = c_num;
  ginfo.deno = ginfo.im_number * _GENERATION_AMOUNT;   //���Է��ڹ��캯�����Ż�
  ginfo.pcross = _PCROSS;		//��������
  ginfo.pmutation = _PMUTATION;	//��������
  //��im_Gen_Gene��im_memory��ʼ��Ϊ0

  im_Gen_Gene.reserve(_GENERATION_AMOUNT);  //��Ⱥ��Ϣ
  im_memory.reserve(_GENERATION_AMOUNT);  //����ϸ��
  im_Gen_Gene_Father.reserve(_GENERATION_AMOUNT);   //������Ⱥ
  im_Probability.reserve(_GENERATION_AMOUNT);
  im_memoryProbability.reserve(_GENERATION_AMOUNT);
  im_affinity.reserve(_GENERATION_AMOUNT);
  im_density.reserve(_GENERATION_AMOUNT);
  int i, j;
  T temp(ginfo.im_number, 0);
  for(i = 0;i < _GENERATION_AMOUNT;i++)
  {
    im_Gen_Gene.push_back(temp);
    im_Probability.push_back(0);
    im_affinity.push_back(0);
    im_density.push_back(0);
  }

	//comment
	/*for(j = 0;j < ginfo.im_number;j++)
	{
		printf("im_Gen_Gene[0][%d]=%d\n",j,im_Gen_Gene[0][j]);
	}*/

  //im_Gen_Gene[i].assign(flsize, 0);
  for(j = 0;j < c_num;j++)
  {
		//printf("customer[%d][1]=%d\n",j,customer[j][1]);
		im_Gen_Gene[0][customer[j][1]] = 1;
  }
	//comment
	/*for(j = 0;j < ginfo.im_number;j++)
	{
		printf("im_Gen_Gene[0][%d]=%d\n",j,im_Gen_Gene[0][j]);
	}*/
}

bool immune::im_updateGen_Gene(Feedback const &fb)
{
	srand(time(NULL));
	int vaild_count = 0;
	int i, j, size, rand_index, tem, MIN_SOLUTION = ginfo.im_maxserver/2;
	size = im_Gen_Gene[0].size();
	vector<T> temp;
	T *it_v;
	temp.reserve(_GENERATION_AMOUNT);
	temp.clear();
	for(i = 0; i<_GENERATION_AMOUNT; i++)
	{
		if(fb[i][0] == 1)
		{
			vaild_count++;
			temp.push_back(im_Gen_Gene[i]);
		}
	}
	im_Gen_Gene.clear();
	for(i = 0; i<vaild_count; i++)
	{
		im_Gen_Gene[i] = temp[i];
	}
	for( i; i<_GENERATION_AMOUNT; i++)
	{
		it_v = &im_Gen_Gene[i];
		rand_index=rand()%(ginfo.im_maxserver-MIN_SOLUTION)+MIN_SOLUTION;
		for(j = 0; j<rand_index; j++)
		{
			tem=rand()%size;
			if((*it_v)[tem]==1)
			{
				j--;
				continue;
			}
			else
				(*it_v)[tem]=1;
		}
	}
	return true;
}

bool immune::imCreateGene()        //����������ʼ��Ⱥ
{
	int i,j,temp,cnt=0,rand_count,size;
	srand(time(NULL));
	T first=im_Gen_Gene[0];
	T *second;
	size=first.size();
	T::iterator it;
  cnt = count(first.begin(), first.end(), 1);
	/*for(it=first.begin();it!=first.end();it++)
	{
		if((*it)==1)
			count++;
	}*/
	int MIN_SOLUTION = cnt/2;
	for(i=1;i<_GENERATION_AMOUNT;i++)
	{
		second=&im_Gen_Gene[i];
		//im_Gen_Gene[i].assign(size,0);					//�Ƚ�����Ⱥ������ʼ������
		rand_count=rand()%(cnt-MIN_SOLUTION)+MIN_SOLUTION;
		for(j=0;j<rand_count;j++)
		{
			temp=rand()%size;
			if((*second)[temp]==1)
			{
				j--;
				continue;
			}
			else
				(*second)[temp]=1;
		}
	}
  return true;
}

bool immune::im_memoryGeneSort()
{
	int i;
	Map_imProbability *p = new Map_imProbability[_GENERATION_AMOUNT];
	printf("memory1\n");
	for(i=0;i<_GENERATION_AMOUNT;i++)
	{
		p[i].i = i;
		p[i].vProbability = im_memoryProbability[i];
	}
	printf("memory2\n");
	sort(p,p+_GENERATION_AMOUNT,comp1);
	printf("memory2.1\n");
	vector<T> temp(_GENERATION_AMOUNT);			//����vector
	printf("memory3\n");
	for(i=0;i<_GENERATION_AMOUNT;i++)
	{
		//temp[i].assign(im_memory[p[i].i].begin(),im_memory[p[i].i].end());
		temp[i] = im_memory[p[i].i];
	}
	printf("memory4\n");
	for(i=0;i<_GENERATION_AMOUNT;i++)	//���������ļ���������������������
	{
		//im_memory[i].assign(temp[i].begin(),temp[i].end());
		im_memory[i] = temp[i];
	}
	printf("memory5\n");
	delete p;
	p = NULL;
}

bool immune::imGeneChoose()        //����ѡ��
{
printf("choose0\n");
		int i,j,size;
		imGeneSort();		//�ȶԵ�ǰ����Ⱥ����һ��������������������Ϊ������ֳ����

		im_Gen_Gene_Father.clear();
		for(i=0;i<im_N;i++)	//ȡǰim_N����Ϊ����Ⱥ��
		{
			im_Gen_Gene_Father.push_back(im_Gen_Gene[i]);
		}
		printf("choose1\n");

		if(im_memory.empty())
		{
		printf("choose2\n");
			im_memory.clear();
			im_memoryProbability.clear();
			for(i=0;i<im_m;i++)	//ȡǰim_m�����뵽��������
			{
				im_memory.push_back(im_Gen_Gene[i]);
				im_memoryProbability.push_back(im_Probability[i]);
			}
		}
		else					//�������ǿգ���ʱ��Ҫ���������¼�����
		{
			size = im_memory.size();
			if((_GENERATION_AMOUNT-size)<im_m)
			{
			printf("choose3\n");
				for(i = 0,j= _GENERATION_AMOUNT-im_m-1;i<im_m && j<_GENERATION_AMOUNT; i++,j++)
				{
					im_memory[j] = im_Gen_Gene[i];
					im_memoryProbability[j] = im_Probability[i];
				}
			}
			else
			{
			printf("choose4\n");
				for(i = 0;i<im_m; i++)
				{
					im_memory.push_back(im_Gen_Gene[i]);
					im_memoryProbability.push_back(im_Probability[i]);
				}
			printf("choose5\n");
			}

			im_memoryGeneSort();		//���¼�����
			printf("choose6\n");
			sort(im_memoryProbability.begin(),im_memoryProbability.end(),comp2);	//���¼�������Ӧ�ĸ���
		}
    return true;
}

bool immune::imGeneSort()								//��Ⱥ���򣬰���������ֳ����
{
	int i;
	Map_imProbability *p = new Map_imProbability[_GENERATION_AMOUNT];
	for(i=0;i<_GENERATION_AMOUNT;i++)
	{
		p[i].i = i;
		p[i].vProbability = im_Probability[i];
	}
	printf("sort1\n");
	sort(p,p+_GENERATION_AMOUNT,comp1);
	printf("sort2\n");
	vector<T> temp(_GENERATION_AMOUNT);			//����vector
	printf("2.1\n");
	for(i=0;i<_GENERATION_AMOUNT;i++)
	{
		printf("p[i].i = %d\t temp.size = %d\t im_Gen_Gene.size = %d\n",p[i].i, temp.size(), im_Gen_Gene.size() );
		temp[i]=im_Gen_Gene[p[i].i];
	}
	printf("sort3\n");
	for(i=0;i<_GENERATION_AMOUNT;i++)	//������������Ⱥ���Ƶ���ʼ��Ⱥ��
	{
		im_Gen_Gene[i] = temp[i];
	}
	printf("sort4\n");
	delete p;
	p = NULL;
	sort(im_Probability.begin(),im_Probability.end(),comp2);	//����Ӧ����Ⱥ�������ʽ�������
	printf("sort5\n");
  return true;
}



bool immune::imGeneMix()
{
	srand(time(NULL));
	printf("mix1\n");
  	vector<T> temp(im_N,T(ginfo.im_number,0));			//�����ĸ�����Ⱥ
	double pick;							//���������Ľ�������
	int i, j, size, cnt;
	int iFather;                         //���׵Ĵ���
	int iMother;                         //ĸ�׵Ĵ���
	T FatherBK(ginfo.im_number),MotherBK(ginfo.im_number);				//��ĸ�Ļ���
	T Child1, Child2;                     //������ĸ���ӽ�������Ů�Ļ���
	T::iterator V_iter;
	int rand_num = ginfo.im_maxserver;
	int rand_index[rand_num];
	int limit_server_num;
	int rand_delete_index;
	size = im_Gen_Gene_Father[0].size();
	printf("mix2\n");
	for(i = 0; i < _P_GENE_MIX; i++)
	{
		// printf("i=%d  temp.size()=%d\n",i,temp.size());
		printf("_P_GENE_MIX=%d\n",_P_GENE_MIX);
		pick = rand()/(double)(RAND_MAX);
		if(pick>ginfo.pcross)
			continue;
		iFather=_ITEMP;
		do
		{
			iMother = _ITEMP;
		}while(iMother == iFather);
		Child1.reserve(size);         //��ʼ����Ů�ļ�����
		Child2.reserve(size);
		Child1.clear();
		Child2.clear();
		printf("mix3\n");
		for(int k = 0; k<rand_num; k++)		//��������rand_num���±�
		{
			rand_index[k] = rand()%size;
			for(j = 0; j<k; j++)
			{
				if(rand_index[j] == rand_index[k])
				{
						k--;
						break;
				}
			}
		}
		printf("mix4\n");
		sort(rand_index, rand_index+rand_num);
		printf("mix4.1\n");
		FatherBK = im_Gen_Gene_Father[iFather];
		MotherBK = im_Gen_Gene_Father[iMother];
		printf("mix4.2\n");
		for(int k = 0; k<rand_num; k++)
		{
		printf("mix4.3\n");
				Child1.push_back(FatherBK[rand_index[k]]);
				Child2.push_back(MotherBK[rand_index[k]]);
		}
		printf("mix5\n");
		for(int k = 0; k<size;k++)
		{
				for(j = 0; j < rand_num; j++)
				{
						if(rand_index[j] == k)
							continue;
				}
				Child1.push_back(MotherBK[k]);
				Child2.push_back(FatherBK[k]);
		}
		printf("mix6\n");
		srand(time(NULL));
		if((limit_server_num = count(Child1.begin(), Child1.end(), 1)) >= ginfo.im_maxserver)
		{
		printf("mix6.1\n");
				cnt = limit_server_num - ginfo.im_maxserver;
				printf("mix6.1.1\n");
				if(cnt >= 2)
					cnt = cnt + rand()%(cnt/2);
				while(cnt>0)
				{
				printf("mix6.2\n");
						rand_delete_index = rand()%size;
						if(Child1[rand_delete_index] == 1)
						{
								Child1[rand_delete_index] = 0;
								cnt--;
						}
				}
				printf("mix6.3\n");
		}
		printf("mix7\n");
		if((limit_server_num = count(Child2.begin(), Child2.end(), 1)) >= ginfo.im_maxserver)
		{
				cnt = limit_server_num - ginfo.im_maxserver;
				if(cnt >= 2)
					cnt = cnt + rand()%(cnt/2);
				while(cnt>0)
				{
						rand_delete_index = rand()%size;
						if(Child2[rand_delete_index] == 1)
						{
								Child2[rand_delete_index] = 0;
								cnt--;
						}
				}
		}
		printf("mix8\n");

		printf("i=%d  temp.size()=%d\n",i,temp.size());
		temp[i] = Child1;
		printf("mix8.1\n");
		printf("i=%d  temp.size()=%d\n",i,temp.size());
		temp[i+1] = Child2;
		i++;
		printf("for done!\n");
	}
	for(i=0; i<im_N; i++)
	{
		// printf("_GENERATION_AMOUNT%d\n", _GENERATION_AMOUNT);
		// _GENERATION_AMOUNT = 28
		im_Gen_Gene_Father[i] = temp[i];
	}
	printf("mix9\n");
}

bool immune::imGeneAberrance()     //��������
{
	srand(time(NULL));
	int i;
	double RVariation;						//�����ı�������
	for(i=0; i<im_N; i++)
	{
		RVariation = rand()/(double)(RAND_MAX);
		if(RVariation > ginfo.pmutation)		//��������
		{
			imGeneAberranceOne(i);
		}
	}
	im_RETGen_Gene();					//������һ����Ⱥ
  return true;
}

bool immune::im_RETGen_Gene()  //�����µ���Ⱥ�����ݽ����������ĸ����ͼ������е�ǰm����������
{
	int i,j;
	im_Gen_Gene.clear();

	for(i=0,j=0; i<_GENERATION_AMOUNT; i++)
	{
		printf("RET1\n" );
		if(i<im_N)
		{
			im_Gen_Gene[i] = im_Gen_Gene_Father[i];
		}
		else
		{
			im_Gen_Gene[i] = im_memory[j];
			j++;
		}
	}
  return true;
}

bool immune::imGeneAberranceOne(int index)  //����ĳ������
{
	srand(time(NULL));
	int temp;
	int size = im_Gen_Gene_Father[0].size();
	T::iterator V_it;
	//int Low_limit = 0;
	//int High_limit = size-1;
	temp = rand()%size;
	while(temp == size)
		temp = rand()%size;
	int num = count(im_Gen_Gene_Father[index].begin(),im_Gen_Gene_Father[index].end(),1);
	if(im_Gen_Gene_Father[index][temp] == 1)
		im_Gen_Gene_Father[index][temp] = 0;
	else
	{
		if(num>=ginfo.im_maxserver)
		{
			while(im_Gen_Gene_Father[index][temp] == 0)
			{
				temp = rand()%size;
				while(temp == size)
					temp = rand()%size;
			}
			im_Gen_Gene_Father[index][temp] = 0;
		}
		else
			im_Gen_Gene_Father[index][temp] = 1;
	}
}

bool immune::imAfinity(T link_cost, T flow_err)          //�������л����׺Ͷ�
{//�׺Ͷ�����С���������������� ��·�ܳɱ� �� �Ƿ��ǿ��н� �����ó�
  int i;
  float probability;
  //����ÿ���������׺Ͷ�
  for(i = 0;i < _GENERATION_AMOUNT; i++)
  {
    probability = count(im_Gen_Gene[i].begin(),im_Gen_Gene[i].end(),1)*ginfo.im_scost\
                + link_cost[i] - C*flow_err[i]; //����C
    probability = (probability>0) ? (1/probability):0;
    im_affinity[i] = probability;
  }
  return true;
}

bool immune::imDensity()           //������Ⱥ�ܶ�/Ũ��
{
  int i;
  for(i = 0;i < _GENERATION_AMOUNT; i++)
  {
    im_density[i] = (ginfo.deno-imDensityOne(im_Gen_Gene[i]))/ginfo.deno;
  }
  return true;
}

int immune::imDensityOne(T &Gene)    //����ĳһ������ͬ�������������ƶ�
{//ͨ�����������жϻ���֮�������ƶ�
  int i, j;
  int count = 0;
  for(i = 0;i < _GENERATION_AMOUNT; i++)
    for(j = 0;j < ginfo.im_number;j++)
      count += (Gene[j]^im_Gen_Gene[i][j]);
  return count;
}

bool immune::imBreedProbability()
{
	printf("breed1\n");
  int i;
  float sum_afinity=0, sum_dencity=0;
  for(i = 0;i < _GENERATION_AMOUNT;i++)
  {
    sum_afinity += im_affinity[i];
    sum_dencity += im_density[i];
  }
  	printf("breed2\n");
  float aff, den;
  for(i = 0;i < _GENERATION_AMOUNT;i++)
  {
    aff = im_affinity[i]/sum_afinity;
    den = im_density[i]/sum_dencity;
    im_Probability[i] = ALPHA*(aff-den)+den;    //define�ж���ALPHA
  }
  	printf("breed3\n");
  return true;
}

void immune::imPrintInfo()
{//��ӡ��
  int i, j;
  cout<<"Output Species Info"<<endl;
  for(i = 0;i < _GENERATION_AMOUNT;i++)
  {
		printf("im_Gen_Gene[%d]\n",i);
    for(j = 0;j < ginfo.im_number;j++)
		{
      cout<<im_Gen_Gene[i][j]<<" ";
		}
    cout<<endl;
  }/*
  cout<<"Output Affinity Info"<<endl;
  for(i = 0;i < ginfo.im_number;i++)
    cout<<im_affinity[i]<<" ";
  cout<<endl;
  cout<<"Output Density Info"<<endl;
  for(i = 0;i < ginfo.im_number;i++)
    cout<<im_density[i]<<" ";
  cout<<endl;
  cout<<"Output Probability Info"<<endl;
  for(i = 0;i < ginfo.im_number;i++)
    cout<<im_Probability[i]<<" ";
  cout<<endl;*/
}
