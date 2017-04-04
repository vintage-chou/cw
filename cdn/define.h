#ifndef __DEFINE_H__
#define __DEFINE_H__


#define _GENERATION_AMOUNT 28
#define _PCROSS 0.5
#define _PMUTATION 0.12


#define im_m (_GENERATION_AMOUNT/4)				//TODO:����ÿ�ζ�Ҫ���㲻������ÿ�η����������ĸ�������
#define im_N (_GENERATION_AMOUNT/2+_GENERATION_AMOUNT/4)				//TODO��ÿһ������Ⱥ���ĸ���
#define _ITEMP rand()%(im_N)	//TODO���������±�
#define	_P_GENE_MIX (im_N-1)/2  		//TODO���ӽ�����
#define _PONISH_FACTOR 10
#define _ALPHA 0.7
#endif
