#pragma once
#include <math.h>
#include <errno.h>
/*
edited by Mkdir
2014.3.3
*/

inline int Round(const double& z, const int& decimal_length = 0){
	if( decimal_length == 0 )
		return (int)(z + 0.5);
	else if( decimal_length == 1 )
		return (int)(z * 10 + 0.5);
	else if( decimal_length == 2 )
		return (int)(z * 100 + 0.5);
	else if( decimal_length == 3 )
		return (int)(z * 1000 + 0.5);
	else if( decimal_length == 4 )
		return (int)(z * 10000 + 0.5);
	else 
		return (int)(z * pow((double)10, (double)decimal_length) + 0.5);
}
const double PI = 3.1415926;

/***************************************************************/  
/* ���ر�׼��̬�ֲ����ۻ������
/***************************************************************/
inline double Normal(double z)  
{  
	double temp;  
	temp=exp((-1)*z*z/2)/sqrt(2*PI);  
	return temp;
};

/***************************************************************/  
/* ���ر�׼��̬�ֲ����ۻ���÷ֲ���ƽ��ֵΪ 0����׼ƫ��Ϊ 1��                           */  
/***************************************************************/ 
inline double NormSDist(const double z)  
{  
	// this guards against overflow  
	if(z > 6) return 1;  
	if(z < -6) return 0;   

	static const double gamma =  0.231641900,  
		a1  =  0.319381530,  
		a2  = -0.356563782,  
		a3  =  1.781477973,  
		a4  = -1.821255978,  
		a5  =  1.330274429;   

	double k = 1.0 / (1 + fabs(z) * gamma);  
	double n = k * (a1 + k * (a2 + k * (a3 + k * (a4 + k * a5))));  
	n = 1 - Normal(z) * n;  
	if(z < 0)  
		return 1.0 - n;   

	return n;  
};

/***************************************************************/  
/* ���ر�׼��̬�ֲ��ۻ�����溯��÷ֲ���ƽ��ֵΪ 0����׼ƫ��Ϊ 1��                    */  
/***************************************************************/   
inline double NormSinV(const double p)  
{  
	static const double LOW  = 0.02425;  
	static const double HIGH = 0.97575;  

	/* Coefficients in rational approximations. */  
	static const double a[] =  
	{  
		-3.969683028665376e+01,  
		2.209460984245205e+02,  
		-2.759285104469687e+02,  
		1.383577518672690e+02,  
		-3.066479806614716e+01,  
		2.506628277459239e+00  
	};   

	static const double b[] =  
	{  
		-5.447609879822406e+01,  
		1.615858368580409e+02,  
		-1.556989798598866e+02,  
		6.680131188771972e+01,  
		-1.328068155288572e+01  
	};   

	static const double c[] =  
	{  
		-7.784894002430293e-03,  
		-3.223964580411365e-01,  
		-2.400758277161838e+00,  
		-2.549732539343734e+00,  
		4.374664141464968e+00,  
		2.938163982698783e+00  
	};   

	static const double d[] =  
	{  
		7.784695709041462e-03,  
		3.224671290700398e-01,  
		2.445134137142996e+00,  
		3.754408661907416e+00  
	};   

	double q, r;   

	errno = 0;   

	if (p < 0 || p > 1)  
	{  
		errno = EDOM;  
		return 0.0;  
	}  
	else if (p == 0)  
	{  
		errno = ERANGE;  
		return -HUGE_VAL /* minus "infinity" */;  
	}  
	else if (p == 1)  
	{  
		errno = ERANGE;  
		return HUGE_VAL /* "infinity" */;  
	}  
	else if (p < LOW)  
	{  
		/* Rational approximation for lower region */  
		q = sqrt(-2*log(p));  
		return (((((c[0]*q+c[1])*q+c[2])*q+c[3])*q+c[4])*q+c[5]) /  
			((((d[0]*q+d[1])*q+d[2])*q+d[3])*q+1);  
	}  
	else if (p > HIGH)  
	{  
		/* Rational approximation for upper region */  
		q  = sqrt(-2*log(1-p));  
		return -(((((c[0]*q+c[1])*q+c[2])*q+c[3])*q+c[4])*q+c[5]) /  
			((((d[0]*q+d[1])*q+d[2])*q+d[3])*q+1);  
	}  
	else  
	{  
		/* Rational approximation for central region */  
		q = p - 0.5;  
		r = q*q;  
		return (((((a[0]*r+a[1])*r+a[2])*r+a[3])*r+a[4])*r+a[5])*q /  
			(((((b[0]*r+b[1])*r+b[2])*r+b[3])*r+b[4])*r+1);  
	}  
};

