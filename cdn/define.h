#ifndef __DEFINE_H__
#define __DEFINE_H__


#define _GENERATION_AMOUNT 100
#define _PCROSS 0.9
#define _PMUTATION 1
#define _Aberr 0.7


#define im_m (_GENERATION_AMOUNT/50)				//TODO:����ÿ�ζ�Ҫ���㲻������ÿ�η����������ĸ�������
#define im_N (_GENERATION_AMOUNT-im_m)				//TODO��ÿһ������Ⱥ���ĸ���
#define _ITEMP rand()%(im_N)	//TODO���������±�
#define	_P_GENE_MIX (im_N-1)/2  		//TODO���ӽ�����
#define _PONISH_FACTOR 10000
#define _ALPHA 0.7
#endif
