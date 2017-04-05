#include "config.h"
#include "define.h"
#include <iostream>
#include <vector>
#include <memory.h>
#include <time.h>
#include <algorithm>
#include <math.h>

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
bool comp3(const Map_imProbability &a,const Map_imProbability &b)	//�����ú���
{
    return a.vProbability<b.vProbability;
}
bool comp2(const float &a,const float &b)	//�����ú���
{
    return a>b;
}
bool comp4(const float &a,const float &b)	//�����ú���
{
    return a<b;
}
inline int fnRndBoundary(int iBegin, int iEnd)	//�����������±�
{

	return rand()%(iEnd-iBegin) + iBegin;
}



typedef struct Immune_global_t
{
  int im_number;        //��ʾδ֪������
  int im_maxserver;
	int im_minserver;
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
  immune(CUSTOMER customer, uint_16 node_num, uint_16 cost, vector<uint_32> capacity, uint_32 consume);    //OK���캯����������������vector���飩
  ~immune();
//  bool imReceiveAfinity();    //��ȡ��С�����������������׺Ͷ���Ϣ,������·���� �� ������

  bool imCreateGene();        //������ʼ��Ⱥ
  bool imGeneChoose();        //����ѡ��
  bool imGeneMix();           //���򽻲�
  bool imGeneAberrance(Feedback feedback);     //��������
  bool imGeneSort();			//������ֳ������
  bool im_memoryGeneSort();	//���¼�����
	bool im_RETGen_Gene();		//�����µ���Ⱥ
  bool im_updateGen_Gene(Feedback const &fb);

  bool imAfinity(Feedback feedback);           //�������л����׺Ͷ�
  bool imDensity();           //������Ⱥ�ܶ�/Ũ��
  int imDensityOne(T &Gene);    //����ĳһ������ͬ�������������ƶ�
  bool imBreedProbability();    //��ֳ����

  void imPrintInfo(vector<T> species);

  vector<T> im_Gen_Gene;  //��Ⱥ��Ϣ
  vector<T> im_memory;  //����ϸ��
  vector<T> im_Gen_Gene_Father;   //������Ⱥ

  F im_Aber_Pro; //aberrance probability of each Gene
  F im_Probability;
  F im_memoryProbability;
  F im_affinity;
  F im_density;

  Immune_global ginfo;  //ȫ����Ϣ
  bool imGeneAberranceOne(int index,Feedback feedback);   //����ĳ������
  //bool imWheel(T Gene);     //���̶�
};

immune::immune()
{
}


immune::~immune()
{
}

immune::immune(CUSTOMER customer, uint_16 node_num, uint_16 cost, vector<uint_32> capacity, uint_32 consume)
{//��ʼ��ȫ�ֱ���
  //���ؿ��н����ڵ�һ��
  int c_num = customer.size();
  int n_num = node_num;
  ginfo.im_number = n_num;
  ginfo.im_scost = cost;
  ginfo.im_maxserver = c_num;
	ginfo.im_minserver = 0;
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
  for(j = 0;j < c_num;j++)
  {
		//printf("customer[%d][1]=%d\n",j,customer[j][1]);
		im_Gen_Gene[0][customer[j][1]] = 1;
  }
	//comment
	long cnt = 0;
	for(vector<uint_32>::iterator iter=capacity.begin();iter<capacity.end();iter++)
	{
		cnt += (*iter);	//calculate sum of capacity
	}
	double beta = _Aberr/(*max_element(capacity.begin(), capacity.end())*1.0/cnt); //_Aberr = 0.7
	for(vector<uint_32>::iterator iter=capacity.begin();iter<capacity.end();iter++)
	{
		int temp = 1-beta*(*iter)/cnt;
		im_Aber_Pro.push_back(temp);
	}
	sort(capacity.begin(), capacity.end());
	int accumulate=0;
	for(vector<uint_32>::iterator iter=capacity.end()-1;iter>=capacity.begin();iter--)
	{
		accumulate += (*iter);
		ginfo.im_minserver++;
		if(accumulate > consume)	//calculate the min_total_server that satisfy the need of consumer
		{
			break;
		}
	}
}

//�������ã���ʼ����Ⱥ
//ʹ�õ���ȫ����Ϣ��
//�����������������ޡ���MIN_SOLUTION
//�ڵ���������������NODECAP_Probability
//�ڵ����������������NODECAP
bool immune::imCreateGene()
{
	int i, j, temp, cnt=0, rand_count, size;
	srand(time(NULL));
	double pick;
	T first=im_Gen_Gene[0];
	T *second;
	size=first.size();
	T::iterator it;
	cnt = count(first.begin(), first.end(), 1);
	for(i=1;i<_GENERATION_AMOUNT;i++)
	{
		second=&im_Gen_Gene[i];
		rand_count=rand()%(cnt-ginfo.im_minserver)+ginfo.im_minserver;
		for(j=0;j<rand_count;j++)
		{
			temp=rand()%size;
			if((*second)[temp]==1)
			{
				j--;
				continue;
			}
			else
			{
                                //pick���������ڿ��Բ���һ������ĸ���ֵ������������������ǰ�ڵ��Ӧ�ı�����ʽϴ���ô���0����1�Ŀ����Ի�ܸߡ�
				pick = rand()/(double)(RAND_MAX);
				if(pick < (1-im_Aber_Pro[temp]))
				{
					(*second)[temp]=1;
				}
				else
				{
					j--;
					continue;
				}
			}
		}
	}
  return true;
}

bool immune::im_memoryGeneSort()
{
	int i;
	Map_imProbability *p = new Map_imProbability[_GENERATION_AMOUNT];
	for(i=0;i<_GENERATION_AMOUNT;i++)
	{
		p[i].i = i;
		p[i].vProbability = im_memoryProbability[i];
	}

	sort(p, p+_GENERATION_AMOUNT, comp1);

	vector<T> temp(_GENERATION_AMOUNT);
	for(i=0;i<_GENERATION_AMOUNT;i++)
	{
		temp[i] = im_memory[p[i].i];
	}
	for(i=0;i<_GENERATION_AMOUNT;i++)
	{
		im_memory[i] = temp[i];
	}
	delete[] p;
	p = NULL;



	return true;
}

bool immune::imGeneChoose()
{
		int i,j,size;
		imGeneSort();

//		im_Gen_Gene_Father.clear();
//		for(i=0;i<im_N;i++)
//		{
//			im_Gen_Gene_Father.push_back(im_Gen_Gene[i]);
//		}

		if(im_memory.empty())
		{
			im_memoryProbability.clear();
			for(i=0;i<im_m;i++)
			{
				im_memory.push_back(im_Gen_Gene[i]);
				im_memoryProbability.push_back(im_Probability[i]);
			}

		}
		else
		{
			if((_GENERATION_AMOUNT-im_memory.size()) <= im_m)
			{
				for(i = 0,j= _GENERATION_AMOUNT-im_m-1;i<im_m && j<_GENERATION_AMOUNT; i++,j++)
				{
					im_memory[j] = im_Gen_Gene[i];
					im_memoryProbability[j] = im_Probability[i];
				}
			}
			else
			{
				for(i = 0;i<im_m; i++)
				{
					im_memory.push_back(im_Gen_Gene[i]);
					im_memoryProbability.push_back(im_Probability[i]);
				}
			}
			im_memoryGeneSort();

			sort(im_memoryProbability.begin(),im_memoryProbability.end(),comp2);
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
	sort(p,p+_GENERATION_AMOUNT,comp1);
	vector<T> temp(_GENERATION_AMOUNT);			//����vector
	for(i=0;i<_GENERATION_AMOUNT;i++)
	{
		temp[i]=im_Gen_Gene[p[i].i];
	}
	for(i=0;i<_GENERATION_AMOUNT;i++)	//������������Ⱥ���Ƶ���ʼ��Ⱥ��
	{
		im_Gen_Gene[i] = temp[i];
	}
	delete[] p;
	p = NULL;
	sort(im_Probability.begin(),im_Probability.end(),comp2);	//����Ӧ����Ⱥ�������ʽ�������
  return true;
}

bool immune::imGeneMix()
{
//	printf("mix 前\n");
//    imPrintInfo(im_Gen_Gene);
	srand(time(NULL));
    T Child1(ginfo.im_number), Child2(ginfo.im_number);

    im_Gen_Gene_Father.clear();
    for( int i = 0, j = 0 ; i < _GENERATION_AMOUNT; i++)
    {
        if(i < im_N)
        {
            im_Gen_Gene_Father.push_back(im_Gen_Gene[i]);
        }
        else
        {
			im_Gen_Gene_Father.push_back(im_memory[j++]);
        }
    }

	vector<T> temp(_GENERATION_AMOUNT, T(ginfo.im_number, 0));

    for( int i = 0; i < _GENERATION_AMOUNT/2; i=i+2)
    {

        Child1.clear();
        Child2.clear();

        for( int j = 0; j < ginfo.im_number; j++)
        {
//        	double rand_num = rand()*1.0/RAND_MAX;
//
//        	if( rand_num > ginfo.pcross)
//            {
//                Child1.push_back(im_Gen_Gene_Father[i][j]);
//                Child2.push_back(im_Gen_Gene_Father[i+1][j]);
//            }
//            else
//            {
//                Child1.push_back(im_Gen_Gene_Father[i+1][j]);
//                Child2.push_back(im_Gen_Gene_Father[i][j]);
//            }
        }

        temp[i] = Child1;
        temp[i+1] = Child2;
    }
//    if (count(Child1.begin(), Child1.end() ,1) == 0)
//    {
//    	cout<<"error!"<<endl;
//
//    }
//    for (int i = 0; i < _GENERATION_AMOUNT; i++)
//    {
//        im_Gen_Gene[i] = temp[i];
//    }
//    printf("mix 后\n");
//    imPrintInfo(im_Gen_Gene);


	return true;
}

bool immune::imGeneAberrance(Feedback feedback)
{

	srand(time(NULL));
	int i;
	double RVariation; //�����ı�������
	im_RETGen_Gene();
	for(i=0; i<_GENERATION_AMOUNT; i++)
	{
			RVariation = rand()/(double)(RAND_MAX);
			if(RVariation < ginfo.pmutation*2/3) //���pmutationԽС��������ڷ������죿����
			{
				imGeneAberranceOne(i,feedback);
			}else if(RVariation < ginfo.pmutation){
				int count = rand()% (ginfo.im_number);
				for(int j=0;j<count;j++)
				{
					int k = rand()%ginfo.im_number;
					im_Gen_Gene[i][k] ^= (uint_16)1;
				}
			}
	}

	return true;
}

bool immune::imGeneAberranceOne(int index, Feedback feedback)  //����ĳ������
{
	srand(time(NULL));
	int size = im_Gen_Gene[0].size();
	double RVariation;
	RVariation = rand()/(double)(RAND_MAX);
	if(feedback[index][1] == 0)//im_Gen_Gene[index] satisfys
	{//kexingjie liebian huo jiandian
		for(int i = 0; i<size; i++)
		{
			float pro = im_Aber_Pro[i]; //probability of 1->0
			if(im_Gen_Gene[index][i] == 1 && RVariation < pro)
			{
				im_Gen_Gene[index][i] ^= (uint_16)1;
				break;
			}
		}
	}
//	else//im_Gen_Gene[index] is not satisfied
//	{//bukexingjie yiwei huo jiadian
//		for(int i = 0; i<size; i++)
//		{
//			float pro = 1-im_Aber_Pro[i]; //probability of 1->0
//			if(im_Gen_Gene[index][i] == 0 && RVariation < pro)
//			{
//				im_Gen_Gene[index][i] ^= (uint_16)1;
//				break;
//			}
//		}
//	}
	/*
	for(int i = 0; i<size; i++)
	{
		RVariation = rand()/(double)(RAND_MAX);
		float pro = im_Aber_Pro[i]; //probability of 1->0
		if(im_Gen_Gene[index][i] == 0)
		{
			pro = 1-pro;	//probability of 0->1
		}
		if(RVariation < pro)
		{
			im_Gen_Gene[index][i] ^= (uint_16)1;
		}
	}*/
	return true;
}

bool immune::im_RETGen_Gene()  //�����µ���Ⱥ�����ݽ����������ĸ����ͼ������е�ǰm����������
{
	im_Gen_Gene_Father = im_Gen_Gene;
	im_Gen_Gene.clear();

	for(int i=0,j=0; i<_GENERATION_AMOUNT; i++)
	{
		if( i < im_N)
		{
			im_Gen_Gene.push_back(im_Gen_Gene_Father[i]);
		}
		else
		{
			im_Gen_Gene.push_back(im_memory[j++]);
		}
	}
  return true;
}

bool immune::imAfinity(Feedback feedback)          //�������л����׺Ͷ�
{
  uint_16 i;
  float probability;
  for(i = 0;i < _GENERATION_AMOUNT; i++)
  {
    // probability = 1.0*count(im_Gen_Gene[i].begin(),im_Gen_Gene[i].end(),1)*ginfo.im_scost + feedback[i][0] + C*feedback[i][1];
	probability = 1.0 * feedback[i][0] + C*feedback[i][1];
    probability =1/(probability);
    im_affinity[i] = probability;
  }
  return true;
}

bool immune::imDensity()           //������Ⱥ�ܶ�/Ũ��
{
  int i;
  im_density.clear();
  for(i = 0;i < _GENERATION_AMOUNT; i++)
  {
    im_density.push_back((ginfo.deno-imDensityOne(im_Gen_Gene[i]))*1.0/ginfo.deno);
  }
  return true;
}

int immune::imDensityOne(T &Gene)    //����ĳһ������ͬ�������������ƶ�
{
  int i, j;
  int count = 0;
  for(i = 0;i < _GENERATION_AMOUNT; i++){
    for(j = 0;j < ginfo.im_number; j++){
      count += (Gene[j]^im_Gen_Gene[i][j]);

    }
  }
  return count;
}

bool immune::imBreedProbability()
{
  int i;
  float sum_afinity=0, sum_dencity=0;
  for(i = 0;i < _GENERATION_AMOUNT;i++)
  {
    sum_afinity += im_affinity[i];
    sum_dencity += im_density[i];
  }
  float aff, den;
  for(i = 0;i < _GENERATION_AMOUNT;i++)
  {
    aff = im_affinity[i]/sum_afinity;
    den = im_density[i]/sum_dencity;
    im_Probability[i] = ALPHA*(aff-den)+den;    //define�ж���ALPHA
  }
  return true;
}

void immune::imPrintInfo(vector<T> species)
{//��ӡ��
  int i, j;
  cout<<"Output Species Info"<<endl;
  for(i = 0;i < species.size();i++)
  {
    for(j = 0;j < species[i].size();j++)//ginfo.im_number
		{
      cout<<species[i][j]<<" ";
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
