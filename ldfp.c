#include <stdio.h>
#include <stdlib.h>
#include "ldfp.h"

int GetNthBit(void* p,int N)
{
    char ch=( (char*)p )[N/8];
    char n=1;
    return (ch&(n<<(N%8)))!=0;
}

int SetNthBit(void* p,int N)
{
    char* q=&( (char*)p )[N/8];
    char n=1;
    (*q)=(*q)|(n<<(N%8));
    return ((*q)&(n<<(N%8)))!=0;
}

int ResetNthBit(void* p,int N)
{
    char* q=&( (char*)p )[N/8];
    char n=1;
    (*q)=(*q)&(~(n<<(N%8)));
    return ((*q)&(n<<(N%8)))==0;//ok

}

long GetNValue(void* p)
{
    long res=0;
    int i=126;
    while(i>=112)
    {
        res=2*res;
        if(GetNthBit(p,i))
        {
            res++;
        }
        i--;
    }
    res=res-16383;
    return res;
}

int GetSign(void* p)
{
    if(GetNthBit(p,127))
        return 1;
    else
        return 0;
}

int BiggerM(char* p,char* q)
{
    for(int i=113;i>=0;i--)
    {
        if(p[i]>q[i])
        {
            return 1;//p is bigger than q
        }else if(p[i]<q[i])
        {
            return -1;//p is smaller than q
        }
    }
    return 0;//equal
}

char* GetM(void* p)//remeber to free M //left smaller
{
    char* res=(char*) malloc(sizeof(char)*114);//res[113] 是保留进位的
    for(int i=0;i<113;i++)
        res[i]=0;
    for(int i=0;i<112;i++)
    {
        if(GetNthBit(p,i))
        {
            res[i]=1;
        }else
        {
            res[i]=0;
        }
    }
    res[112]=1;

    return res;
}

void MoveN(char* p,int n)
{
    for(int i=n;i<=113;i++)
    {
        p[i-n]=p[i];
    }
    for(int i=113-n+1;i<=113;i++)
    {
        p[i]=0;
    }
}

void SetN(void* p,long N)
{
    N+=16383;
    int i=112;
    while(i<=126)
    {
        if(N%2==1)
        {
            SetNthBit(p,i);
        }else
        {
            ResetNthBit(p,i);
        }
        N=N/2;
        i++;
    }

}
//        SetM(r,R);
void SetM(void* r,char* R)//p to q
{
    int i;
    if(R[113]==0)//略过隐藏1
    {
        i=111;
    }else
    {
        i=112;
    }

    int j=111;
    while(j>=0)
    {
        if(R[i]==0)
        {
            ResetNthBit(r,j);
        }else
        {
            SetNthBit(r,j);
        }
        j--;
        i--;
    }

}

void Minis(char* M1,char* M2,char* R)//0->113
{
    if(BiggerM(M1,M2)==0)
    {
        for(int i=0;i<=113;i++)
        {
            R[i]=0;
        }
        return ;
    }if(BiggerM(M1,M2)==-1)//M1 is maller of M2
    {
        Minis(M2,M1,R);
        return ;
    }else//now M1 is bigger than M2
    {
        for(int i=0;i<114;i++)
        {
            R[i]=M1[i]-M2[i];
        }
        for(int i=0;i<113;i++)
        {
            while(R[i]<0)
            {
                R[i]+=2;
                R[i+1]-=1;//
            }
        }
    }
}

long_double FP_add(long_double op1, long_double op2) {
    void* p1=(void*)(&op1);
    void* p2=(void*)(&op2);
    long_double res;
    void* r=(void*)(&res);
    for(int i=0;i<128;i++)
    {
        ResetNthBit(r,i);
    }
    if(GetSign(p1)==GetSign(p2))//same sign
    {

        char* M1=GetM(p1);
        char* M2=GetM(p2);
        long N1=GetNValue(p1);
        long N2=GetNValue(p2);
        if(N1>N2)//对齐
        {
            MoveN(M2,N1-N2);
        }else
        {
            MoveN(M1,N2-N1);
        }
        char* R=(char*)malloc(sizeof(char)*114);

        for(int i=0;i<114;i++)
        {
            R[i]=M1[i]+M2[i];
        }

        int add=0;
        for(int i=0;i<113;i++)
        {
            R[i]+=add;
            add=R[i]/2;
            R[i]%=2;
        }
        R[113]=add;
        long N=(N1>N2?N1:N2);
        if(R[113]==1)
            N++;
        SetN(r,N);
        SetM(r,R);

        free(M1);
        free(M2);
        free(R);
        if(GetSign(p1))//set sign
        {
            SetNthBit(r,127);
        }else
        {
            ResetNthBit(r,127);
        }
    }else//different sign
    {
        char* M1=GetM(p1);
        char* M2=GetM(p2);
        long N1=GetNValue(p1);
        long N2=GetNValue(p2);
        if(N1>N2)//对齐
        {
            MoveN(M2,N1-N2);
        }else
        {
            MoveN(M1,N2-N1);
        }
        char* R=(char*)malloc(sizeof(char)*114);


        //R is the answer of the bigger minis the smaller
        Minis(M1,M2,R);

        int i=113;
        while(i>=0&&R[i]==0)
        {
            i--;
        }
        if(i<0)
        {
            for(int i=0;i<128;i++)
            {
                ResetNthBit(r,i);
            }
            free(M1);
            free(M2);
            free(R);
            return res;
        }
        //set N
        long N=(N1>N2?N1:N2);
        N=N-(112-i);
        SetN(r,N);

        i--;
        int j=111;
        while(i>=0)
        {
            if(R[i]==0)
            {
                ResetNthBit(r,j);
            }else
            {
                SetNthBit(r,j);
            }
            i--;
            j--;
        }
        while(j>=0)
        {
            ResetNthBit(r,j);
            j--;
        }
        if(N1>N2)
        {
            if(GetNthBit(p1,127))
            {
                SetNthBit(r,127);
            }else
            {
                ResetNthBit(r,127);
            }
        }else if(N1<N2)
        {
            if(GetNthBit(p2,127))
            {
                SetNthBit(r,127);
            }else
            {
                ResetNthBit(r,127);
            }
        }else
        {
            if(BiggerM(M1,M2)==1)
            {
                if(GetNthBit(p1,127))
                {
                    SetNthBit(r,127);
                }else
                {
                    ResetNthBit(r,127);
                }
            }else if(BiggerM(M1,M2)==-1)
            {
                if(GetNthBit(p2,127))
                {
                    SetNthBit(r,127);

                }else
                {
                    ResetNthBit(r,127);
                }
            }
        }

        free(M1);
        free(M2);
        free(R);
    }


    return res;
}
long_double double_to_long_double(double op) {
    long_double ret = { {0x0}, };
    // unsigned long *ulCasted;

	/* you can manipulate bits in *ulCasted from now */
	// ulCasted = (unsigned long*)(&op);

	/* ---- write your own code here! ---- */
    void *ld=&ret;
    void *d=&op;
    for(int i=0;i<128;i++)//reset ld
    {
        ResetNthBit(ld,i);
    }


    //special
    int all0=1;
    for(int i=63;i>=0;i--)
    {
        if(GetNthBit(d,i)!=0)
        {
            all0=0;
            break;
        }
    }
    if(all0)
    {
        if(GetNthBit(d,63))
        {
            SetNthBit(ld,127);
        }
        return ret;
    }

    all0=1;
    int all1=1;
    for(int i=63;i>=52;i--)
    {
        if(GetNthBit(d,i)!=1)
        {
            all1=0;
            break;
        }
    }
    if(all1==1)
    {
        for(int i=52;i>=0;i--)
        {
            if(GetNthBit(d,i)!=0)
            {
                all0=0;
                break;
            }
        }
        if(all0==1)
        {
            if(GetNthBit(d,63))
            {
                SetNthBit(ld,127);
            }
            for(int i=126;i>=112;i--)
            {
                SetNthBit(ld,i);
            }
            return ret;
        }
    }

    int sign=GetNthBit(d,63);
    if(sign==1)
    {
        SetNthBit(ld,127);
    }
    long N=0;
    for(int i=62;i>=52;i--)
    {
        N=N*2;
        if(GetNthBit(d,i))
        {
            N=N+1;
        }
    }

    N=N-1023;
    N=N+16383;
    int j=112;

    while(j<=126)
    {
        if(N%2==1)
        {
            SetNthBit(ld,j);
        }else
        {
            ResetNthBit(ld,j);
        }
        j++;
        N=N/2;
    }
    for(int i=51,j=111;i>=0;i--,j--)
    {
        if(GetNthBit(d,i))
        {
            SetNthBit(ld,j);
        }else
        {
            ResetNthBit(ld,j);
        }
    }

	return ret;
}

long_double FP_mul(long_double op1, long_double op2) {

	/* ---- write your own code here! ---- */
    int neg=0;
    void* p1=&op1;
    void* p2=&op2;
    long_double res;
    void* r=&res;
    for(int i=0;i<127;i++)
    {
        ResetNthBit(r,i);
    }
    if(GetNthBit(p1,127))
    {
        neg++;
    }
    if(GetNthBit(p2,127))
    {
        neg++;
    }
    if(neg%2==1)
    {
        SetNthBit(r,127);//符号
    }


    long N1=0;
    for(int i=126;i>=112;i--)
    {
        N1=N1*2;
        if(GetNthBit(p1,i))
        {
            N1++;
        }
    }
    long N2=0;
    for(int i=126;i>=112;i--)
    {
        N2=N2*2;
        if(GetNthBit(p2,i))
        {
            N2++;
        }
    }
    long N=N1+N2-16383;
    int j=112;

    char* mul=(char*)malloc(sizeof(char)*226);
    for(int i=0;i<226;i++)
    {
        mul[i]=0;
    }

    for(int i=0;i<=112;i++)
    {
        int num0=0;
        if(i==112)
        {
            num0=1;
        }else
        {
            num0=GetNthBit(p1,i);
        }
        for(int j=0;j<=112;j++)
        {
            int num1;
            if(j==112)
            {
                num1=1;
            }else
            {
                num1=GetNthBit(p2,j);
            }
            if(num1&&num0)
            {
                mul[j+i]++;
            }
        }
    }

    int add=0;
    int i=0;
    while(i<226)
    {
        mul[i]+=add;
        add=mul[i]/2;
        mul[i]%=2;
        i++;
    }

    j=225;
    while(mul[j]!=1)
    {
        j--;
    }
    j--;

    /*//225 必定是首位的1*/
    for(int i=111;i>=0;i--,j--)
    {
        if(mul[j]==0)
        {
            ResetNthBit(r,i);
        }else
        {
            SetNthBit(r,i);
        }
    }

    j=112;
    if(mul[225]==1)
    {
        N++;
    }
    do{
        if(N%2==1)
        {
            SetNthBit(r,j);
        }else
        {
            ResetNthBit(r,j);
        }
        j++;
        N=N/2;
    }while(N);


    free(mul);
	return res;
}




char *long_double_print_bitseq(long_double op) {
	char *str = (char*)malloc(MAXLEN); // this memory needs to be freed from caller

	/* ---- write your own code here! ---- */
        /* PRINT OUT from MSB in op.data[15] to LSB in op.data[0] */
    for(int i=0;i<MAXLEN;i++)
    {
        str[i]=0;
    }

    void* ld=&op;

    for(int i=127,j=0;i>=0;i--,j++)
    {
        if((i+1)%8==0)
        {
            str[j]=' ';
            j++;
        }
        if(GetNthBit(ld,i))
        {
            str[j]='1';
        }else
        {
            str[j]='0';
        }
    }

	return str;
}

char *double_print_bitseq(double op) {
	char *str = (char*)malloc(MAXLEN); // this memory needs to be freed from caller

	/* ---- write your own code here! ---- */
        /* PRINT OUT from MSB in op.data[15] to LSB in op.data[0] */
    for(int i=0;i<MAXLEN;i++)
    {
        str[i]=0;
    }

    void* ld=&op;

    for(int i=63,j=0;i>=0;i--,j++)
    {
        if((i+1)%8==0)
        {
            str[j]=' ';
            j++;
        }
        if(GetNthBit(ld,i))
        {
            str[j]='1';
        }else
        {
            str[j]='0';
        }
    }

	return str;
}
char *long_double_print_normalized(long_double op) {
	char *str = (char*)malloc(MAXLEN); // this memory needs to be freed from caller

	/* ---- write your own code here! ---- */
    int all0=1;
    for(int i=0;i<128;i++)
    {
        if(GetNthBit(&op,i)==1)
        {
            all0=0;
            break;
        }
    }
    for(int i=0;i<MAXLEN;i++)
    {
        str[i]=0;
    }

    void* ld=&op;
    int j=0;
    int i=127;
    if(GetNthBit(ld,i)!=1)
    {
        str[j];
    }else
    {
        str[j]='-';
        j++;
    }
    i--;

    if(all0==0)
        str[j]='1';
    else
        str[j]='0';
    str[j+1]='.';
    j=j+2;

    for(int i=111;i>=0;i--,j++)
    {
        if(GetNthBit(ld,i))
        {
            str[j]='1';
        }else
        {
            str[j]='0';
        }
    }
    str[j++]=' ';
    str[j++]='X';
    str[j++]=' ';
    str[j++]='2';
    str[j++]='^';

    long N=0;
    for(int i=126;i>=112;i--)
    {
        N=N*2;
        if(GetNthBit(ld,i)==1)
        {
            N=N+1;
        }
    }
    N=N-16383;
    if(all0==1)
    {
        N=-16382;
    }
    if(N<0)
    {
        str[j]='-';
        j++;
        N=-N;
    }
    int s=j;
    do{
        int rmn=N%10;
        N=N/10;
        str[j]=rmn+'0';
        j++;
    }while(N!=0);
    int e=j-1;
    while(s<e)
    {
        char tmp=str[s];
        str[s]=str[e];
        str[e]=tmp;
        s++;
        e--;
    }

	return str;
}

char *double_print_normalized(double op) {
    char *str = (char*)malloc(MAXLEN); // this memory needs to be freed from caller

    /* ---- write your own code here! ---- */
    for(int i=0;i<MAXLEN;i++)
    {
        str[i]=0;

    }

    void* ld=&op;
    int j=0;
    int i=63;
    if(GetNthBit(ld,i)!=1)
    {
        str[j];

    }else
    {
        str[j]='-';
        j++;

    }
    i--;

    str[j]='1';
    str[j+1]='.';
    j=j+2;

    for(int i=51;i>=0;i--,j++)
    {
        if(GetNthBit(ld,i))
        {
            str[j]='1';

        }else
        {
            str[j]='0';

        }

    }
    str[j++]=' ';
    str[j++]='X';
    str[j++]=' ';
    str[j++]='2';
    str[j++]='^';

    long N=0;
    for(int i=62;i>=52;i--)
    {
        N=N*2;
        if(GetNthBit(ld,i)==1)
        {
            N=N+1;
        }

    }
    N=N-1023;
    if(N<0)
    {
        str[j]='-';
        j++;
        N=-N;
    }
    do{
        int rmn=N%10;
        N=N/10;
        str[j]=rmn+'0';
        j++;

    }while(N!=0);

    return str;

}

