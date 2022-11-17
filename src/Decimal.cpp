/*
 * Decimal - Arbitrary-Precision Float Arithmetic Library
 * Copyright (C) 2015 Gianfranco Mariotti
 * Copyright (C) 2022 ChainWorks Industries
 */

//XXX gcc -E -dM -x c <(true)
//XXX To get compiler constants

#include "types/Decimal.h"
#include <stdexcept>
#include <limits.h>
#include <float.h>
#include <locale>
#include <algorithm>

/**
 * Locale-independent version of std::to_string
 */
    template <typename T>
std::string ToString(const T& t)
{
    std::ostringstream oss;
    oss.imbue(std::locale::classic());
    oss << t;
    return oss.str();
}

//------------------------Private Methods--------------------------------

Decimal Decimal::FromHex(const std::string& hex) {
    Decimal a = 0_D, _16 = 16_D, boost = 1_D;
    DecimalIterations its;
    its.decimals = 0;
    a.iterations = its;
    a.sign = '+';
    _16.iterations = its;

    Decimal _16boost = 1_D(its);
    for (auto it = hex.rbegin(); it != hex.rend(); it++) {
        if (*it == '-') {
            a.sign = '-';
            continue;
        }
        else if (*it == '+') {
            a.sign = '+'; // redundant
            continue;
        }
        switch (*it) {
            case '0':
            break;
            case '1':
                a += 1_D(its) * _16boost;
            break;
            case '2':
                a += 2_D(its) * _16boost;
            break;
            case '3':
                a += 3_D(its) * _16boost;
            break;
            case '4':
                a += 4_D(its) * _16boost;
            break;
            case '5':
                a += 5_D(its) * _16boost;
            break;
            case '6':
                a += 6_D(its) * _16boost;
            break;
            case '7':
                a += 7_D(its) * _16boost;
            break;
            case '8':
                a += 8_D(its) * _16boost;
            break;
            case '9':
                a += 9_D(its) * _16boost;
            break;
            case 'a':
            case 'A':
                a += 10_D(its) * _16boost;
            break;
            case 'b':
            case 'B':
                a += 11_D(its) * _16boost;
            break;
            case 'c':
            case 'C':
                a += 12_D(its) * _16boost;
            break;
            case 'd':
            case 'D':
                a += 13_D(its) * _16boost;
            break;
            case 'e':
            case 'E':
                a += 14_D(its) * _16boost;
            break;
            case 'f':
            case 'F':
                a += 15_D(its) * _16boost;
            break;
            default:
                throw DecimalIllegalOperation("Invalid hex character");
        }
        _16boost *= _16;
    }
    return a;
}

//Comparator without sign, utilized by Comparators and Operations
int Decimal::CompareNum(const Decimal& left, const Decimal& right)
{
    if( (left.number.size() - left.decimals) > (right.number.size() - right.decimals) )
        return 1;
    else if( (left.number.size() - left.decimals) < (right.number.size() - right.decimals) )
        return 2;

    if(left.decimals>right.decimals)
    {
        Decimal tmp;
        tmp=right;
        while (left.decimals>tmp.decimals)
        {
            tmp.decimals++;
            tmp.number.push_front('0');
        }

        for (int i= left.number.size() - 1 ; i>=0; i--)
        {
            if(left.number[i]>tmp.number[i])
                return 1;
            else if(left.number[i]<tmp.number[i])
                return 2;
        }
        return 0;
    }
    else if(left.decimals<right.decimals)
    {
        Decimal tmp;
        tmp=left;
        while (tmp.decimals<right.decimals)
        {
            tmp.decimals++;
            tmp.number.push_front('0');
        }

        for (int i= tmp.number.size() - 1 ; i>=0; i--)
        {
            if(tmp.number[i]>right.number[i])
                return 1;
            else if(tmp.number[i]<right.number[i])
                return 2;
        }
        return 0;
    }
    else
    {
        for (int i = left.number.size() - 1 ; i>=0; i--)
        {
            if(left.number[i]>right.number[i])
                return 1;
            else if(left.number[i]<right.number[i])
                return 2;
        }
        return 0;
    }
};

//Operations without sign and decimals, utilized by Operations
Decimal Decimal::Sum(const Decimal& left, const Decimal& right)
{
    Decimal tmp(left.iterations);
    tmp.type = Decimal::NumType::_NORMAL;
    int carry=0;
    size_t loopsize = (left.number.size()>right.number.size()) ? left.number.size() : right.number.size();

    for (size_t i=0; i<loopsize; ++i)
    {
        int val1, val2;
        val1 = ( i > left.number.size() -1) ? 0 : CharToInt(left.number[i]);
        val2 = ( i > right.number.size() -1 ) ? 0 : CharToInt(right.number[i]);

        int aus= val1+val2+carry;

        carry=0;
        if(aus>9)
        {
            carry = 1;
            aus = aus-10;
        }

        tmp.number.push_back(IntToChar(aus));
    }
    if(carry!=0)
        tmp.number.push_back(IntToChar(carry));

    return tmp;
};

Decimal Decimal::Subtract(const Decimal& left, const Decimal& right)
{
    Decimal tmp(left.iterations);
    tmp.type = NumType::_NORMAL;
    int carry=0;
    int aus;

    for (size_t i=0; i<left.number.size(); ++i)
    {
        int val1, val2;
        val1 = CharToInt(left.number[i]);
        val2 = ( i > right.number.size() -1 ) ? 0 : CharToInt(right.number[i]);

        val1 -= carry;

        if(val1<val2)
        {
            aus= 10 + val1 - val2;
            carry = 1;
        }
        else
        {
            aus = val1 - val2;
            carry = 0;
        }

        tmp.number.push_back(IntToChar(aus));
    }

    return tmp;
};


Decimal Decimal::Multiply(const Decimal& left, const Decimal& right)
{
    Decimal ris(left.iterations);
    Decimal tmp(left.iterations);
    tmp.type = Decimal::NumType::_NORMAL;
    ris.number.push_back('0');
    int carry=0;

    for (size_t i=0; i<right.number.size(); ++i)
    {
        for (size_t k=0; k<i ; ++k)
        {
            tmp.number.push_front('0');
        }

        for (size_t j=0; j<left.number.size() ; ++j)
        {
            int aus= CharToInt(right.number[i]) * CharToInt(left.number[j]) + carry;
            carry=0;
            if(aus>9)
            {
                while(aus>9)
                {
                    carry++;
                    aus-=10;
                }
            }
            tmp.number.push_back(IntToChar(aus));
        }

        if(carry!=0)
            tmp.number.push_back(IntToChar(carry));
        carry=0;

        ris= Sum(ris,tmp);
        tmp.number.clear();
    }

    return ris;
};

//------------------------Public Methods--------------------------------

//Assignment operators
Decimal& Decimal::operator=(const char* strNum)
{
    type = NumType::_NORMAL;
    number.clear();
    iterations = DecimalIterations();
    sign='\0';
    decimals = 0;

    int count=0;
    bool start_dec = false;

    if(strNum[count]=='+')
    {
        sign='+';
        count++;
    }
    else if(strNum[count]=='-')
    {
        sign='-';
        count++;
    }
    else if(isdigit(strNum[count]))
        sign='+';
    else if (strNum[count]=='.') {
        sign='+';
        number.push_front('0');
        start_dec = true;
        count++;
    }
    else
    {
        throw DecimalIllegalOperation("Bad input string");
    }

    while(strNum[count]!='\0')
    {
        if(!start_dec)
            if(strNum[count]=='.')
            {
                if(number.empty())
                {
                    // Ensure leading 0
                    number.push_front('0');
                }
                start_dec = true;
                count++;
            }

        if(isdigit(strNum[count]))
        {
            number.push_front(strNum[count]);
            count++;
            if(start_dec)
                decimals++;
        }
        else
        {
            throw DecimalIllegalOperation("Bad input string");
        }
    }
    this->LeadTrim();
    if (decimals > iterations.decimals) {
        iterations.decimals = decimals;
    }
    return *this;
};

Decimal& Decimal::operator=(std::string strNum)
{
    *this = strNum.c_str();
    return *this;
};

Decimal& Decimal::operator=(char Num)
{
    *this = ::ToString(Num);
    return *this;
};

Decimal& Decimal::operator=(unsigned char Num)
{
    *this = ::ToString(Num);
    return *this;
};

Decimal& Decimal::operator=(short Num)
{
    *this = ::ToString(Num);
    return *this;
};

Decimal& Decimal::operator=(unsigned short Num)
{
    *this = ::ToString(Num);
    return *this;
};

Decimal& Decimal::operator=(int Num)
{
    *this = ::ToString(Num);
    return *this;
};

Decimal& Decimal::operator=(unsigned int Num)
{
    *this = ::ToString(Num);
    return *this;
};

Decimal& Decimal::operator=(long Num)
{
    *this = ::ToString(Num);
    return *this;
};

Decimal& Decimal::operator=(unsigned long Num)
{
    *this = ::ToString(Num);
    return *this;
};

Decimal& Decimal::operator=(long long Num)
{
    *this = ::ToString(Num);
    return *this;
};

Decimal& Decimal::operator=(unsigned long long Num)
{
    *this = ::ToString(Num);
    return *this;
};

Decimal& Decimal::operator=(float Num)
{
    *this = std::to_string(Num);
    return *this;
};

Decimal& Decimal::operator=(double Num)
{
    *this = std::to_string(Num);
    return *this;
};

Decimal& Decimal::operator=(long double Num)
{
    *this = std::to_string(Num);
    return *this;
};

//Operations
Decimal operator+ ( const Decimal& left_, const Decimal& right_ )
{
    Decimal tmp(left_.iterations);
    tmp.type = Decimal::NumType::_NORMAL;
    tmp.iterations.throw_on_error = left_.iterations.TOE() || right_.iterations.TOE();

    Decimal left,right;
    left=left_;
    right=right_;

    if (left.IsNaN() || right.IsNaN()) {
        if (left.iterations.TOE() || right.iterations.TOE()) {
            throw DecimalIllegalOperation("IEE754 special number arithmetic is disabled");
        }
        tmp.SpecialClear();
        tmp.type = Decimal::NumType::_NAN;
        return tmp;
    }
    else if (left.IsInf() || right.IsInf()) {
        if (left.sign != right.sign) {
            if (left.iterations.TOE() || right.iterations.TOE()) {
                throw DecimalIllegalOperation("IEE754 special number arithmetic is disabled");
            }
            tmp.SpecialClear();
            tmp.type = Decimal::NumType::_NAN;
            return tmp;
        }
        return left; // Or equivalently, right
    }

    if(left.decimals>right.decimals) {
        while(left.decimals>right.decimals)
        {
            right.decimals++;
            right.number.push_front('0');
        }
        right.iterations.decimals = left.iterations.decimals;
    }
    else if    (left.decimals<right.decimals) {
        while(left.decimals<right.decimals)
        {
            left.decimals++;
            left.number.push_front('0');
        }
        left.iterations.decimals = right.iterations.decimals;
    }
        

    if( (left.sign=='+')&& (right.sign=='-'))
    {
        int check= Decimal::CompareNum(left,right);
        if(check==0)
        {
            tmp = 0;
            return tmp;
        }
        if(check==1)
        {
            tmp=Decimal::Subtract(left,right);
            tmp.sign='+';
            tmp.decimals=left.decimals;
            tmp.iterations.decimals=left.iterations.decimals;
            tmp.LeadTrim();
            return tmp;
        }
        if(check==2)
        {
            tmp=Decimal::Subtract(right,left);
            tmp.sign='-';
            tmp.decimals=left.decimals;
            tmp.iterations.decimals=left.iterations.decimals;
            tmp.LeadTrim();
            return tmp;
        }
    }
    if( (left.sign=='-')&& (right.sign=='+'))
    {
        int check= Decimal::CompareNum(left,right);
        if(check==0)
        {
            tmp = 0;
            return tmp;
        }
        if(check==1)
        {
            tmp=Decimal::Subtract(left,right);
            tmp.sign='-';
            tmp.decimals=left.decimals;
            tmp.iterations.decimals=left.iterations.decimals;
            tmp.LeadTrim();
            return tmp;
        }
        if(check==2)
        {
            tmp=Decimal::Subtract(right,left);
            tmp.sign='+';
            tmp.decimals=left.decimals;
            tmp.iterations.decimals=left.iterations.decimals;
            tmp.LeadTrim();
            return tmp;
        }
    }

    if( (left.sign=='+')&& (right.sign=='+'))
    {
        tmp=Decimal::Sum(left,right);
        tmp.sign='+';
        tmp.decimals=left.decimals;
        tmp.iterations.decimals=left.iterations.decimals;
        return tmp;
    }
    if( (left.sign=='-')&& (right.sign=='-'))
    {
        tmp=Decimal::Sum(left,right);
        tmp.sign='-';
        tmp.decimals=left.decimals;
        tmp.iterations.decimals=left.iterations.decimals;
        return tmp;
    }
    // Should not get here
    throw DecimalIllegalOperation("Decimal assertion failure");
};

Decimal operator- ( const Decimal& left_, const Decimal& right_ )
{
    Decimal tmp(left_.iterations);
    tmp.type = Decimal::NumType::_NORMAL;
    tmp.iterations.throw_on_error = left_.iterations.TOE() || right_.iterations.TOE();

    Decimal left,right;
    left=left_;
    right=right_;

    if (left.IsNaN() || right.IsNaN()) {
        if (left.iterations.TOE() || right.iterations.TOE()) {
            throw DecimalIllegalOperation("IEE754 special number arithmetic is disabled");
        }
        tmp.SpecialClear();
        tmp.type = Decimal::NumType::_NAN;
        return tmp;
    }
    else if (left.IsInf() && right.IsInf()) {
        if (left.sign == right.sign) {
            if (left.iterations.TOE() || right.iterations.TOE()) {
                throw DecimalIllegalOperation("IEE754 special number arithmetic is disabled");
            }
            tmp.SpecialClear();
            tmp.type = Decimal::NumType::_NAN;
            return tmp;
        }
    }

    if (left.IsInf()) {
        if (left.iterations.TOE() || right.iterations.TOE()) {
            throw DecimalIllegalOperation("IEE754 special number arithmetic is disabled");
        }
        return left;
    }
    else if (right.IsInf()) {
        if (left.iterations.TOE() || right.iterations.TOE()) {
            throw DecimalIllegalOperation("IEE754 special number arithmetic is disabled");
        }
        // Invert the sign
        return -right;
    }


    if(left.decimals>right.decimals) {
        while(left.decimals>right.decimals)
        {
            right.decimals++;
            right.number.push_front('0');
        }
        right.iterations.decimals = left.iterations.decimals;
    }
    else if (left.decimals<right.decimals) {
        while(left.decimals<right.decimals)
        {
            left.decimals++;
            left.number.push_front('0');
        }
        left.iterations.decimals = right.iterations.decimals;
    }

    if( (left.sign=='+')&& (right.sign=='-'))
    {
        tmp=Decimal::Sum(left,right);
        tmp.sign='+';
        tmp.decimals=left.decimals;
        tmp.iterations.decimals=left.iterations.decimals;
        return tmp;
    }
    if( (left.sign=='-')&& (right.sign=='+'))
    {
        tmp=Decimal::Sum(left,right);
        tmp.sign='-';
        tmp.decimals=left.decimals;
        tmp.iterations.decimals=left.iterations.decimals;
        return tmp;
    }

    if( (left.sign=='+')&& (right.sign=='+'))
    {
        int check= Decimal::CompareNum(left,right);
        if(check==0)
        {
            tmp=0;
            return tmp;
        }
        if(check==1)
        {
            tmp=Decimal::Subtract(left,right);
            tmp.sign='+';
            tmp.decimals=left.decimals;
            tmp.iterations.decimals=left.iterations.decimals;
            tmp.LeadTrim();
            return tmp;
        }
        if(check==2)
        {
            tmp=Decimal::Subtract(right,left);
            tmp.sign='-';
            tmp.decimals=left.decimals;
            tmp.iterations.decimals=left.iterations.decimals;
            tmp.LeadTrim();
            return tmp;
        }
    }

    if( (left.sign=='-')&& (right.sign=='-'))
    {
        int check= Decimal::CompareNum(left,right);
        if(check==0)
        {
            tmp = 0;
            return tmp;
        }
        if(check==1)
        {
            tmp=Decimal::Subtract(left,right);
            tmp.sign='-';
            tmp.decimals=left.decimals;
            tmp.iterations.decimals=left.iterations.decimals;
            tmp.LeadTrim();
            return tmp;
        }
        if(check==2)
        {
            tmp=Decimal::Subtract(right,left);
            tmp.sign='+';
            tmp.decimals=left.decimals;
            tmp.iterations.decimals=left.iterations.decimals;
            tmp.LeadTrim();
            return tmp;
        }
    }
    // Should not get here
    throw DecimalIllegalOperation("Decimal assertion failure");
};

Decimal operator*(const Decimal& left, const Decimal& right)
{
    Decimal tmp(left.iterations);
    tmp.type = Decimal::NumType::_NORMAL;
    tmp.iterations.throw_on_error = left.iterations.TOE() || right.iterations.TOE();

    if (left.IsNaN() || right.IsNaN()) {
        if (left.iterations.TOE() || right.iterations.TOE()) {
            throw DecimalIllegalOperation("IEE754 special number arithmetic is disabled");
        }
        tmp.SpecialClear();
        tmp.type = Decimal::NumType::_NAN;
        return tmp;
    }
    else if (left.IsInf() && right.IsInf()) {
        if (left.iterations.TOE() || right.iterations.TOE()) {
            throw DecimalIllegalOperation("IEE754 special number arithmetic is disabled");
        }
        if (left.sign != right.sign) {
            tmp.SpecialClear();
            tmp.type = Decimal::NumType::_NAN;
            return tmp;
        }
        return left; // or equivalently right
    }

    tmp=Decimal::Multiply(left,right);
    if( ((left.sign=='-')&& (right.sign=='-')) || ((left.sign=='+')&& (right.sign=='+')) )
        tmp.sign='+';
    else
        tmp.sign='-';

    tmp.decimals=left.decimals+right.decimals;
    if (left.iterations.decimals >= right.iterations.decimals) {
        tmp.iterations.decimals = left.iterations.decimals;
    }
    else {
        tmp.iterations.decimals = right.iterations.decimals;
    }
    tmp.LeadTrim();
    tmp.TrailTrim();

    return tmp;
};


Decimal Decimal::Divide(const Decimal& left, const Decimal& right)
{
    Decimal tmp(left.iterations);
    tmp.type = Decimal::NumType::_NORMAL;
    tmp.iterations.throw_on_error = left.iterations.TOE() || right.iterations.TOE();

    Decimal Q(left.iterations) , R(left.iterations) , D(left.iterations) ,
            N(left.iterations),  zero(left.iterations) ;
    Q.type = Decimal::NumType::_NORMAL;
    R.type = Decimal::NumType::_NORMAL;
    D.type = Decimal::NumType::_NORMAL;
    N.type = Decimal::NumType::_NORMAL;
    zero = 0;


    N=left;
    D=right;
    R.sign='+';

    int Ndiff = N.iterations.decimals - N.decimals;
    int Ddiff = D.iterations.decimals - D.decimals;
    while( (N.decimals!=0) || (D.decimals!=0) )
    {
        if(N.decimals==0)
            N.number.push_front('0');
        else
            N.decimals--;

        if(D.decimals==0)
            D.number.push_front('0');
        else
            D.decimals--;
    }
    for (int i = 0; i < Ndiff; i++) {
        N.number.push_front('0');
        N.decimals++;
    }
    for (int i = 0; i < Ddiff; i++) {
        D.number.push_front('0');
        D.decimals++;
    }

    N.LeadTrim();
    D.LeadTrim();

    //Increase Precision to highest decimal quote
    int div_precision = (left.iterations.decimals>right.iterations.decimals) ? (left.iterations.decimals) : (right.iterations.decimals) ;
    int div_true_precision = (left.decimals>right.decimals) ? (left.decimals) : (right.decimals) ;
    // int div_precision = div_true_precision;
    for (size_t i=0; i < (size_t) div_true_precision ; i++)
        N.number.push_front('0');

    int check= Decimal::CompareNum(xFD::Abs(N),xFD::Abs(D));

    if(check==0)
    {
        tmp.number.push_front('1');
    }
    if(check==2)
    {
        // It looks like this algo cannot handle when numerator < denominator,
        // so this is a little "hack" to force it to work. 
        if (N < 0_D && D < 0_D) {
            N = xFD::Abs(N);
            D = xFD::Abs(D);
        }

        if (N < 0_D) {
            return Decimal::Divide(N-D, D) + 1_D;
        }
        else if (D < 0_D) {
            D = -D;
            return -(Decimal::Divide(N+D, D) - 1_D);
        }
        else {
            return Decimal::Divide(N+D, D) - 1_D;
        }
    }
    else
    {
        N= xFD::Abs(N);
        D= xFD::Abs(D);
        while(!N.number.empty())
        {
            R.number.push_front(*(N.number.rbegin()));
            N.number.pop_back();

            bool is_zero=true;
            std::deque<char>::const_iterator zero_iter = R.number.begin();
            for(;zero_iter!= R.number.end();++zero_iter)
                if(*zero_iter!='0')
                    is_zero=false;

            if((R>=D) && (!is_zero))
            {
                int Q_sub=0;
                int min=0;
                int max = 9;

                while( R >= D)
                {
                    int avg = max-min;
                    int mod_avg = avg / 2;
                    avg= (avg - mod_avg * 2) ? (mod_avg + 1) : (mod_avg);

                    int div_check = Decimal::CompareNum ( R, D*avg );

                    if (div_check == 2)
                    {
                        max = avg;
                    }
                    else
                    {
                        Q_sub = Q_sub + avg;
                        R = R - D * avg;

                        max = 9;
                    }
                }

		int modnum = Q_sub / 10;
		std::deque<char> staging;
		while (modnum > 0) {
			staging.push_front(modnum % 10);
			modnum /= 10;
		}
		for (auto it = staging.rbegin(); it != staging.rend(); it++) {
			Q.number.push_front(Decimal::IntToChar(*it));
		}
                Q.number.push_front(Decimal::IntToChar(Q_sub % 10));

                bool is_zero=true;
                std::deque<char>::const_iterator zero_iter = R.number.begin();
                for(;zero_iter!= R.number.end();++zero_iter)
                    if(*zero_iter!='0')
                        is_zero=false;
                if(is_zero)
                    R.number.clear();

            }
            else
            {
                Q.number.push_front('0');
            }
        }
        tmp = Q;
    }

    if( ((left.sign=='-')&& (right.sign=='-')) || ((left.sign=='+')&& (right.sign=='+')) )
        tmp.sign='+';
    else
        tmp.sign='-';

    tmp.decimals = div_precision;
    tmp.iterations.decimals = div_precision;
    tmp.LeadTrim();
    tmp.TrailTrim();

    return tmp;
};

Decimal operator/(const Decimal& left, const Decimal& right) {
    Decimal tmp(left.iterations);
    if (left.IsNaN() || right.IsNaN() ||  (left == 0_D && right == 0_D) || (left.IsInf() && right.IsInf())) {
        if (left.iterations.TOE() || right.iterations.TOE()) {
            throw DecimalIllegalOperation("IEE754 special number arithmetic is disabled");
        }
        tmp.SpecialClear();
        tmp.type = Decimal::NumType::_NAN;
        return tmp;
    }
    else if (right.IsInf()) {
        if (left.iterations.TOE() || right.iterations.TOE()) {
            throw DecimalIllegalOperation("IEE754 special number arithmetic is disabled");
        }
        tmp = 0;
        tmp.type = Decimal::NumType::_NORMAL;
        return tmp;
    }
    else if (right == 0_D)
    {
        if (tmp.iterations.TOE()) {
            throw DecimalIllegalOperation("Division by 0");
        }
        else {
            tmp.SpecialClear();
            tmp.type = Decimal::NumType::_INFINITY;
            return tmp;
        }
    }
    else if (left == 0_D) {
        return 0_D;
    }

    if (right.iterations.decimals > 0) {
        Decimal X = Decimal::Divide(1_D, right);

        // The output from the "Divide" method is almost accurate
        // but is in rare cases, several decimals off-precision.
        // Newton-Rhapson iteraton will set all the integers right.
        //
        // Keep trimming the decimal places, so that it doesn't grow
        // monstrously.
        for (int i = 0; i < right.iterations.div; i++) {
            X = X*(2_D - right*X);
            while (X.decimals > right.iterations.decimals) {
                X.decimals--;
                X.number.pop_front();
            }
        }
        X.TrailTrim();
        X = (X.iterations.trunc_not_round) ? xFD::Floor(X) : 
            xFD::Round(X, -right.iterations.decimals);

        Decimal res = left*X;
        res.TrailTrim();
        return res;
    }
    else {
        return Decimal::Divide(left, right);
    }
}

Decimal operator%(const Decimal& left, const Decimal& right)
{
    Decimal tmp(left.iterations);
    tmp.type = Decimal::NumType::_NORMAL;
    tmp.iterations.throw_on_error = left.iterations.TOE() || right.iterations.TOE();

    if( (left.decimals!=0) || (right.decimals!=0) )
    {
        throw DecimalIllegalOperation("Modulus between non-integers");
    }

    if (left.IsNaN() || right.IsNaN() || (left == 0_D && right == 0_D) || (left.IsInf() && right.IsInf())) {
        if (left.iterations.TOE() || right.iterations.TOE()) {
            throw DecimalIllegalOperation("IEE754 special number arithmetic is disabled");
        }
        tmp.SpecialClear();
        tmp.type = Decimal::NumType::_NAN;
        return tmp;
    }


    if (right == 0_D)
    {
        if (tmp.iterations.throw_on_error) {
            throw DecimalIllegalOperation("Modulus by 0");
        }
        else {
            tmp.SpecialClear();
            tmp.type = Decimal::NumType::_NAN;
            return tmp;
        }
    }

    Decimal left_ = left, right_ = right;
    DecimalIterations its;
    left_.iterations = its;
    right_.iterations = its;

    // 416984806968863648079 % 16
    // is broken, check in debugger.
    // check hex 458479643868196418248935325987194
    Decimal Q = left / right;
    // Obtain the fractional part and multiply it by `right`
    // to get the mod number.
    // The result should be an integer or ridiculously
    // close to one.
    Decimal res = xFD::Round((Q - xFD::Floor(Q)) * right);
    res.TrailTrim();
    res.iterations = left.iterations;
    return res;
}

// UNSAFE. INTERNAL USE ONLY.
Decimal Decimal::Mod(const Decimal& left, const Decimal& right)
{
    Decimal tmp(left.iterations);
    tmp.type = Decimal::NumType::_NORMAL;
    tmp.iterations.throw_on_error = left.iterations.TOE() || right.iterations.TOE();
    if( (left.decimals!=0) || (right.decimals!=0) )
    {
        throw DecimalIllegalOperation("Modulus between non-integers");
    }

    Decimal Q(left.iterations) , R(left.iterations) , D(left.iterations) , N(left.iterations), 
            zero(left.iterations), ret(left.iterations);
    Q.type = Decimal::NumType::_NORMAL;
    R.type = Decimal::NumType::_NORMAL;
    D.type = Decimal::NumType::_NORMAL;
    N.type = Decimal::NumType::_NORMAL;
    ret.type = Decimal::NumType::_NORMAL;
    zero = 0;
    if (left.IsNaN() || right.IsNaN() || (left == 0_D && right == 0_D) || (left.IsInf() && right.IsInf())) {
        if (left.iterations.TOE() || right.iterations.TOE()) {
            throw DecimalIllegalOperation("IEE754 special number arithmetic is disabled");
        }
        tmp.SpecialClear();
        tmp.type = Decimal::NumType::_NAN;
        return tmp;
    }


    if (right == zero)
    if (right == 0_D)
    {
        if (tmp.iterations.throw_on_error) {
            throw DecimalIllegalOperation("Modulus by 0");
        }
        else {
            tmp.SpecialClear();
            tmp.type = Decimal::NumType::_NAN;
            return tmp;
        }
    }

    N= ( left>zero ) ? (left) : (left * (-1)) ;
    D= ( right>zero ) ? (right) : (right* (-1)) ;
    R.sign='+';

    int check= Decimal::CompareNum(N,D);

    if(check==0)
    {
        return zero;
    }
    if(check==2)
    {
        return left;
    }
    else
    {
        while(!N.number.empty())
        {
            R.number.push_front(*(N.number.rbegin()));
            N.number.pop_back();

            bool is_zero=true;
            std::deque<char>::const_iterator zero_iter = R.number.begin();
            for(;zero_iter!= R.number.end();++zero_iter)
                if(*zero_iter!='0')
                    is_zero=false;

            if((R>=D) && (!is_zero))
            {
                int Q_sub=0;
                int min=0;
                int max = 9;

                while( R >= D)
                {
                    int avg = max-min;
                    int mod_avg = avg / 2;
                    avg= (avg - mod_avg * 2) ? (mod_avg + 1) : (mod_avg);

                    int div_check = Decimal::CompareNum ( R, D*avg );

                    if (div_check == 2)
                    {
                        max = avg;
                    }
                    else
                    {
                        Q_sub = Q_sub + avg;
                        R = R - D * avg;

                        max = 9;
                    }
                }

		int modnum = Q_sub / 10;
		std::deque<char> staging;
		while (modnum > 0) {
			staging.push_front(modnum % 10);
			modnum /= 10;
		}
		for (auto it = staging.rbegin(); it != staging.rend(); it++) {
			Q.number.push_front(Decimal::IntToChar(*it));
		}
                Q.number.push_front(Decimal::IntToChar(Q_sub % 10));
                ret = R;

                bool is_zero=true;
                std::deque<char>::const_iterator zero_iter = R.number.begin();
                for(;zero_iter!= R.number.end();++zero_iter)
                    if(*zero_iter!='0')
                        is_zero=false;
                if(is_zero)
                    R.number.clear();
            }
            else
            {
                ret = R;
                Q.number.push_front('0');
            }
        }

        Q.LeadTrim();
        ret.LeadTrim();
        tmp = ret;
    }

    if( ((left.sign=='-')&& (right.sign=='-')) || ((left.sign=='+')&& (right.sign=='+')) )
        tmp.sign='+';
    else
        tmp.sign='-';

    if(!Decimal::CompareNum(tmp,zero))
        tmp.sign='+';

    return tmp;
}


Decimal Decimal::Factorial(const Decimal& x) {
    if (x.IsNaN() || x.IsInf()) {
        if (x.iterations.TOE()) {
            throw DecimalIllegalOperation("IEE754 special number arithmetic is disabled");
        }
    }
    if (x.decimals > 0 || x < 0) {
        throw DecimalIllegalOperation("Factorial is only allowed for positive integers");
    }
    Decimal r(1), i(1);
    while (i < x) {
        r *= i;
        i++;
    }
    return r;

}

Decimal Decimal::Floor(const Decimal& x) {
    auto y = x;
    // Shed all decimals numbers and then replace them with zeroes.
    for (int d = x.decimals; d > 0; d--) {
        y.number.pop_front();
    }
    for (int d = 0; d < x.decimals; d++) {
        y.number.push_front('0');
    }
    y.TrailTrim();
    return y;
}

Decimal Decimal::Round(const Decimal& x, int places) {
    --places; // To get the true rounding behavior.
    auto y = x;
    y.TrailTrim();
    y.LeadTrim();
    int add_ints = 0;
    int add_decs = 0;
    int chop_limit = places + y.decimals;
    int force_zero = false;
    if (chop_limit < 0) {
        return x;
    }
    else if (static_cast<size_t>(chop_limit) >= y.number.size()) {
        return 0_D; // An n-digit number cannot possibly have >=5
                    // in the n+1st decimal place.
    }
    else if (static_cast<size_t>(chop_limit) == y.number.size()-1 ||
            (y.Ints() == 1 && y.number[chop_limit] == '0' && static_cast<size_t>(chop_limit) == y.number.size()-2)) {
        int chop_index = chop_limit;
        if (y.Ints() == 1 && y.number[chop_limit] == '0') {
            force_zero = true;
        }
        if (y.number[chop_index] >= '5') {
            y.number.clear();
            if (force_zero) {
                y.number.push_front('0');
            }
            y.number.push_front('1');
            for (int i = 0; i <= chop_index; i++) {
                y.number.push_front('0');
            }
            y.TrailTrim();
            return y;
        }
        else {
            return 0_D;
        }
    }

    // We are asked to round something within range.
    // Chop off everything below the limit
    for (int i = 0; i < chop_limit; i++) {
        y.number.pop_front();
        if (y.decimals > 0) {
            --y.decimals;
            ++add_decs;
        }
        else {
            ++add_ints;
        }
    }


    bool add1 = false;
    if (y.number[0] >= '5') {
        add1 = true;
    }
    
    y.number.pop_front();
    if (y.decimals > 0) {
        --y.decimals;
        ++add_decs;
    }
    else {
        ++add_ints;
    }

    if (add1) {
        if (y.number[0] > '9') {
            if (y.number.size() > 0) {
                ++y.number[1];
            }
            else {
                y.number.push_back('1');
            }
        }
        else {
            ++y.number[0];
        }
        add1 = false;
    }



    for (int i = 0; i < add_ints; i++) {
        y.number.push_front('0');
    }
    if (places < 0) {
        y.decimals = -(places+1);
    }
    else {
        y.decimals = 0;
    }

    for (size_t i = 0; i < y.number.size(); i++) {
        if (y.number[i] > '9') {
            if (i+1 < y.number.size()) {
                ++y.number[i+1];
            }
            else {
                y.number.push_back('1');
            }
            y.number[i] = '0';
        }
        else {
            break;
        }
    }
     
    y.TrailTrim();
    return y;
}

Decimal Decimal::Inc() {
    Decimal x = *this;
    if (x < 0) {
        return -(-x).Dec();
    }
    ++x.number[0];
    for (size_t i = 0; i < x.number.size(); i++) {
        if (x.number[i] > '9') {
            if (i+1 < x.number.size()) {
                ++x.number[i+1];
            }
            else {
                x.number.push_back('1');
            }
            x.number[i] = '0';
        }
        else {
            break;
        }
    }
    x.TrailTrim();
    return x;
};

Decimal Decimal::Dec() {
    Decimal x = *this;
    if (x <= 0) {
        return -xFD::Abs(x).Inc();
    }
    --x.number[0];
    x.LeadTrim();
    for (size_t i = 0; i < x.number.size(); i++) {
        if (x.number[i] < '0') {
            // It follows that since x > 0 and we
            // trimmed the leading zeros of X, there
            // is at least 1 digit in the number
            // greater than zero - at the very least,
            // the most significant digit is > 0.
                if (i+1 < x.number.size()) {
                    --x.number[i+1];
                }
                else {
                    throw DecimalIllegalOperation("Assertion failed, should not get here");
                }
            x.number[i] = '9';
        }
        else {
            break;
        }
    }
    x.TrailTrim();
    return x;
};

Decimal DecimalConstants::ImprovisedSqrt(const Decimal& a) const {
    Decimal x = 1_D;
    for (int i = 1; i <= iterations.sqrt; i++) {
        x = (x + a/x) / 2_D;
    }
    return x;

}

void DecimalConstants::GenE() {
    Decimal e = 1_D; // i = 0
    int i = 1;
    Decimal fact = 1_D;
    while (i < iterations.E) {
        e += 1_D / fact;
        i += 1;
        fact *= i;
    }
    pE = e;
}

void DecimalConstants::Gen_1Pi() {
    Decimal n1 = "13591409"_D;
    Decimal n2 = "545140134"_D;
    Decimal d1 = "640320"_D;
    Decimal sqd1 = ImprovisedSqrt(d1);
    Decimal _3d1 = d1*d1*d1;
    auto ipi = n1;
    int i = 1;
    Decimal _ifacti = 1_D;
    Decimal _3ifacti = 6_D;
    Decimal _6ifacti = 720_D;
    Decimal sign = -1_D;
    while (i < iterations.Pi) {
        Decimal fi3 = _ifacti*_ifacti*_ifacti;
        Decimal n = sign *_6ifacti * (n1 + n2*i);
        Decimal d = _3ifacti * fi3 * _3d1 * sqd1 * d1;
        ipi += n/d;
        i += 1;
        sign *= -1_D;
        _ifacti *= i;
        // Multiply by (3*i) * (3*(i-1)) * (3*(i-2))
        _3ifacti *= 27_D*i*(i-1)*(i-2);
        // Multiply by (6*i) * (6*(i-1)) * (6*(i-2)) * (6*(i-3)) * (6*(i-4)) * (6*(i-5))
        _6ifacti *= 46656_D*i*(i-1)*(i-2)*(i-3)*(i-4)*(i-5);
    }
    ipi *= 12_D;
    p_1Pi = ipi;
}

Decimal Decimal::nPr(const Decimal& n, const Decimal& k) {
    if (n.IsNaN() || k.IsNaN() || n.IsInf() || k.IsInf()) {
        if (n.iterations.TOE() || n.iterations.TOE()) {
            throw DecimalIllegalOperation("IEE754 special number arithmetic is disabled");
        }
    }
    if (!n.IsInt() || !k.IsInt()) {
        return Decimal(0);
    }
    Decimal den = 1_D;
    Decimal num = 1_D;
    for (Decimal i = k+1_D; i <= n; i++) {
        den *= i;
    }
    for (Decimal i = n; i > n-k; i--) {
        num *= i;
    }
    return num/den;
}

Decimal Decimal::nCr(const Decimal& n, const Decimal& k) {
    if (n.IsNaN() || k.IsNaN() || n.IsInf() || k.IsInf()) {
        if (n.iterations.TOE() || k.iterations.TOE()) {
            throw DecimalIllegalOperation("IEE754 special number arithmetic is disabled");
        }
    }
    if (!n.IsInt() || !k.IsInt()) {
        return Decimal(0);
    }
    Decimal den1 = 1_D;
    Decimal den2 = 1_D;
    Decimal num = 1_D;
    for (Decimal i = 0_D; i <= k; i++) {
        den1 *= i;
    }
    for (Decimal i = k+1_D; i <= n; i++) {
        den2 *= i;
    }
    for (Decimal i = n; i > n-k; i--) {
        num *= i;
    }
    return num/(den1*den2);
}

Decimal Decimal::Binomial(const Decimal& x, const Decimal& y, const Decimal& n) {
    if (x.IsNaN() || y.IsNaN() || n.IsNaN() || x.IsInf() || y.IsInf() || n.IsInf()) {
        //FIXME too, for sinh - this is too long and unwieldly!
        if (x.iterations.TOE() || y.iterations.TOE() || n.iterations.TOE()) {
            throw DecimalIllegalOperation("IEE754 special number arithmetic is disabled");
        }
    }
    if (!n.IsInt()) {
        throw DecimalIllegalOperation("Binomial power must be an integer");
    }
    Decimal s = 0_D;
    Decimal py = 1_D;
    Decimal px = 2_D;
    for (Decimal i = 0_D; i < n; i++) {
        px *= x;
    }
    for (Decimal k = 0_D; k < n+1; k++) {
        s += Decimal::nCr(n, k)*py*px;
        py *= y;
        px /= x;
    }
    return s;
}

Decimal SeqBernoulli::pTerm(const Decimal& n) const {
    if (n == 0_D) {
        return 1_D;
    }
    else if (n == 1_D) {
        return -0.5_D;
    }
    else if (n % 2_D == 1_D) {
        return 0_D;
    }
    // N is even >= 2
    auto PI = xFDCon::Pi();
    Decimal _2ni = 1_D; 
    Decimal _2n = 2_D; 
    Decimal _nfacti = 1_D; 
    Decimal _pini = 1_D; 
    Decimal _pin = PI; 
    for (Decimal i = 1_D; i < n+1; i++) {
        _2ni *= _2n;
        _pini *= _pin;
        _nfacti *= i;
    }
    auto phic = 2_D * (_2ni-1)*_nfacti / (_2ni/2_D  * _pini);
    Decimal s = 0_D;
    for (Decimal k = 1_D; k < xFD::Ceil(1.5_D*n); k++) {
        Decimal _kni = 1_D;
        for (Decimal kk = 0_D; kk < n; kk++) {
            _kni *= k;
        }
        _kni = 1_D/_kni;
        s += _kni;
    }
    auto phi = xFD::Floor(phic * s);
    auto term = (1_D+phi)/2_D*(_2ni-1_D);
    if (n % 4_D == 0_D) {
        term = -term;
    }
     return term;
}

Decimal Decimal::Sinh(const Decimal& x) {
    if (x.IsNaN() || x.IsInf()) {
        if (x.iterations.TOE()) {
            throw DecimalIllegalOperation("IEE754 special number arithmetic is disabled");
        }
    }
    return (xFD::Pow(x) - xFD::Pow(-x)) / 2_D;
}

Decimal Decimal::Cosh(const Decimal& x) {
    if (x.IsNaN() || x.IsInf()) {
        if (x.iterations.TOE()) {
            throw DecimalIllegalOperation("IEE754 special number arithmetic is disabled");
        }
    }
    return (xFD::Pow(x) + xFD::Pow(-x)) / 2_D;
}

// Required to compute e^x
Decimal Decimal::Tanh(const Decimal& x) {
    if (x.IsNaN() || x.IsInf()) {
        if (x.iterations.TOE()) {
            throw DecimalIllegalOperation("IEE754 special number arithmetic is disabled");
        }
    }
    if (x.IsInf() || x.IsNaN()) return NaN();
    Decimal i = 1_D;
    Decimal T = 0_D;
    Decimal _22n = 4_D;
    Decimal _22ni = _22n;
    Decimal _x2nm1 = x;
    Decimal _x2nm12 = x*x;
    Decimal _x2nm1i = _x2nm1;
    Decimal fact = 2_D;
    while (i < x.iterations.tanh) {
        Decimal n = _22ni * (_22ni-1_D) * SeqBernoulli::Term(2_D*i) * _x2nm1i;
        T += n/fact;
        _22ni *= _x2nm12;
        _x2nm1i *= _x2nm12;
        i += 1_D;
        fact *= i*2_D;
        fact *= i*2_D - 1_D;
    }
    return T;
}

Decimal Decimal::Coth(const Decimal& x) {
    if (x.IsNaN() || x.IsInf()) {
        if (x.iterations.TOE()) {
            throw DecimalIllegalOperation("IEE754 special number arithmetic is disabled");
        }
    }
    if (x == 0_D) {
        if (x.iterations.throw_on_error) {
            throw DecimalIllegalOperation("Hyperbolic Cot is undefined at x = 0");
        }
        else {
            return NaN(); // It's 0/0
        }
    }
    return 1_D/xFD::Tanh(x);
}

Decimal Decimal::Sech(const Decimal& x) {
    if (x.IsNaN() || x.IsInf()) {
        if (x.iterations.TOE()) {
            throw DecimalIllegalOperation("IEE754 special number arithmetic is disabled");
        }
    }
    return 1_D/xFD::Cosh(x);
}

Decimal Decimal::Csch(const Decimal& x) {
    if (x.IsNaN() || x.IsInf()) {
        if (x.iterations.TOE()) {
            throw DecimalIllegalOperation("IEE754 special number arithmetic is disabled");
        }
    }
    return 1_D/xFD::Sinh(x);
}

// And we get the inverse hyperbolic functions for free,
// courtesy of logarithms and sqrts.

Decimal Decimal::Asinh(const Decimal& x) {
    if (x.IsNaN() || x.IsInf()) {
        if (x.iterations.TOE()) {
            throw DecimalIllegalOperation("IEE754 special number arithmetic is disabled");
        }
    }
    return xFD::Ln(x + xFD::Sqrt(x*x + 1));
}

Decimal Decimal::Acosh(const Decimal& x) {
    if (x.IsNaN() || x.IsInf()) {
        if (x.iterations.TOE()) {
            throw DecimalIllegalOperation("IEE754 special number arithmetic is disabled");
        }
    }
    return xFD::Ln(x + xFD::Sqrt(x*x - 1));
}

Decimal Decimal::Atanh(const Decimal& x) {
    if (x.IsNaN() || x.IsInf()) {
        if (x.iterations.TOE()) {
            throw DecimalIllegalOperation("IEE754 special number arithmetic is disabled");
        }
    }
    return xFD::Ln((1_D+x)/(1_D-x))/2_D;
}

Decimal Decimal::Acoth(const Decimal& x) {
    if (x.IsNaN() || x.IsInf()) {
        if (x.iterations.TOE()) {
            throw DecimalIllegalOperation("IEE754 special number arithmetic is disabled");
        }
    }
    return xFD::Ln((x+1_D)/(x-1_D))/2_D;
}

Decimal Decimal::Asech(const Decimal& x) {
    if (x.IsNaN() || x.IsInf()) {
        if (x.iterations.TOE()) {
            throw DecimalIllegalOperation("IEE754 special number arithmetic is disabled");
        }
    }
    return xFD::Ln((1_D/x) + xFD::Sqrt(1_D/x*x - 1_D));
}

Decimal Decimal::Acsch(const Decimal& x) {
    if (x.IsNaN() || x.IsInf()) {
        if (x.iterations.TOE()) {
            throw DecimalIllegalOperation("IEE754 special number arithmetic is disabled");
        }
    }
    return xFD::Ln((1_D/x) + xFD::Sqrt(1_D/x*x + 1_D));
}


Decimal Decimal::Erf(const Decimal& x) {
    if (x.IsNaN() || x.IsInf()) {
        if (x.iterations.TOE()) {
            throw DecimalIllegalOperation("IEE754 special number arithmetic is disabled");
        }
    }
    Decimal term = x;
    Decimal n = 1_D;
    Decimal _2n = 2_D;
    Decimal fact = 1_D;
    Decimal _x2 = x*x;
    Decimal _xp = _x2*x;
    Decimal _22ni = 4_D;
    Decimal _22n = 4_D;
    Decimal sign = -1_D;

    for (int i = 1; i <= x.iterations.trig; i++) {
        term += sign/(fact*_22ni) * _xp/(_2n+1_D);
        _xp *= _x2;
        fact *= n+1_D;
        n += 1_D;
        _2n += 2_D;
        _22ni *= _22n;
    }
    return term*xFDCon::_2SqrtPi();
}

//Gamma, beta, and bessel will have to wait for another day.

// Computes e^x.
Decimal Decimal::Pow(const Decimal& x) {
    if (x.IsNaN() || x.IsInf()) {
        if (x.iterations.TOE()) {
            throw DecimalIllegalOperation("IEE754 special number arithmetic is disabled");
        }
    }
    Decimal xi = xFD::Floor(x);
    Decimal xf = x - xi;
    Decimal txf_2 = xFD::Tanh(xf/2_D);
    Decimal exf = 1_D + 2_D*txf_2 / (1_D-txf_2);
    Decimal exi;
    if (xi == 0_D) {
        return exf;
    }
    auto E = xFDCon::E();
    // Make a 2's compliment bitset starting at the highest bit.
    bool take_reciprocal = false;
    if (x < 0) {
        take_reciprocal = true;
    }
    Decimal i = 1_D;
    Decimal j = 1_D;
    while (i < x) {
        i *= 2;
        j += 1;
    }
    if (i == x) {
        // Optimization when the power is a single 2's bit. 
        exi = E;
        for (Decimal jj = 1_D; jj < j; jj++) {
            exi *= exi;
        }
    }
    else {
        // i Greater than x
        std::vector<Decimal> bit_set;
        Decimal max_bit = j-1_D;
        Decimal xmod = x;
        // Decompose the integral exponent into
        // two's compliment values and record the indices
        // i.e. 19 becomes 2^4 + 2^2 + 2^1
        while (xmod != 0_D) {
            xmod %= i;
            i /= 2;
            j-= 1;
            if (xmod - (xmod % i) != 0_D) {
                bit_set.push_back(j);
            }
        }
        exi = 1_D;
        // Start at E^(2^0) = E^1
        Decimal extmp = E;
        // Evaluate the exponent of the two's compliment
        // representation to get the answer - faster performance
        // e.g. instead of e^19 evaluate e^(2^4 + 2^2 + 2^1)
        // = e^(2^4) * e^(2^2) * e^(2^1)
        for (Decimal k = 1_D; k <= max_bit; k++) {
            if (std::count(bit_set.begin(), bit_set.end(), k) > 0) {
                // Perform the exponent multiplication depicted above.
                exi *= extmp;
            }
            // "Increments" to (evaluates) e^(2^1), e^(2^2), ...
            extmp *= extmp;
        }
    }

    if (take_reciprocal) {
        exi = 1_D/exi;
    }

    // a^(int+frac) = a^int * a^frac
    return exi * exf;
}

Decimal Decimal::Pow(const Decimal& x, const Decimal& y) {
    if (x.IsNaN() || x.IsInf() || y.IsNaN() || y.IsInf()) {
        if (x.iterations.TOE() || y.iterations.TOE()) {
            throw DecimalIllegalOperation("IEE754 special number arithmetic is disabled");
        }
    }
    return Pow(x*Ln(y));
}


//TODO there's an Ln approximation - use that instead?
Decimal Decimal::Ln(const Decimal& x) {
    if (x.IsNaN() || x.IsInf()) {
        if (x.iterations.TOE()) {
            throw DecimalIllegalOperation("IEE754 special number arithmetic is disabled");
        }
    }
    if (x < 0_D) {
        throw DecimalIllegalOperation("Ln is undefined for negative numbers");
    }
    Decimal L = 1_D;
    for (Decimal i = 1_D; i < x.iterations.ln; i++) {
        Decimal eL = xFD::Pow(L);
        L += 2_D * (x - eL) / (x + eL);
    }
    return L;
}

Decimal Decimal::Log(const Decimal &x, const Decimal &base) {
    if (x.IsNaN() || x.IsInf()) {
        if (x.iterations.TOE()) {
            throw DecimalIllegalOperation("IEE754 special number arithmetic is disabled");
        }
    }
    return Ln(x)/Ln(base);
}

Decimal Decimal::Log10(const Decimal &x) {
    if (x.IsNaN() || x.IsInf()) {
        if (x.iterations.TOE()) {
            throw DecimalIllegalOperation("IEE754 special number arithmetic is disabled");
        }
    }
    return Ln(x)/xFDCon::Ln10();
}

Decimal Decimal::Log2(const Decimal &x) {
    if (x.IsNaN() || x.IsInf()) {
        if (x.iterations.TOE()) {
            throw DecimalIllegalOperation("IEE754 special number arithmetic is disabled");
        }
    }
    return Ln(x)/xFDCon::Ln2();
}



Decimal Decimal::Sin(const Decimal& x) {
    if (x.IsNaN() || x.IsInf()) {
        if (x.iterations.TOE()) {
            throw DecimalIllegalOperation("IEE754 special number arithmetic is disabled");
        }
    }
    Decimal term = x;
    Decimal n = 3_D;
    Decimal fact = 6_D;
    Decimal _x2 = x*x;
    Decimal _xp = _x2*x;
    Decimal sign = -1_D;
    for (int i = 1; i <= x.iterations.trig; i++) {
        term += sign * _xp / fact;
        _xp *= _x2;
        sign *= -1_D;
        fact *= (n-1_D) * (n-2_D);
        n += 2_D;
    }
    return term;
}

Decimal Decimal::Cos(const Decimal& x) {
    if (x.IsNaN() || x.IsInf()) {
        if (x.iterations.TOE()) {
            throw DecimalIllegalOperation("IEE754 special number arithmetic is disabled");
        }
    }
    Decimal term = 1_D;
    Decimal n = 2_D;
    Decimal fact = 2_D;
    Decimal _x2 = x*x;
    Decimal _xp = _x2;
    Decimal sign = -1_D;
    for (int i = 1; i <= x.iterations.trig; i++) {
        term += sign * _xp / fact;
        _xp *= _x2;
        sign *= -1_D;
        fact *= (n-1_D) * (n-2_D);
        n += 2_D;
    }
    return term;
}

Decimal Decimal::Tan(const Decimal& x) {
    if (x.IsNaN() || x.IsInf()) {
        if (x.iterations.TOE()) {
            throw DecimalIllegalOperation("IEE754 special number arithmetic is disabled");
        }
    }
    // There is no Taylor series for tangent!!
    // Fortunately we have an elemntary formula
    // at our disposal.
    Decimal sin = xFD::Sin(x);
    if (sin == 0_D) {
        if (x.iterations.TOE()) {
            throw DecimalIllegalOperation("Tan is not defined at the location \"Pi/2\" in the period");
        }
        else {
            // At this point, there is a vertical asymptote, so it is positive
            // and negative infinity at this point. Do not make any assumption
            // the sign of this return value, positive was chosen arbitrarily
            // but it could've been equally negative. My advise is to take the
            // absolute value of any Infinity values in your tangent calculations.
            return xFD::Inf();
        }
    }
    Decimal cos = xFD::Cos(x);
    return cos/sin;
}

Decimal Decimal::Cot(const Decimal& x) {
    if (x.IsNaN() || x.IsInf()) {
        if (x.iterations.TOE()) {
            throw DecimalIllegalOperation("IEE754 special number arithmetic is disabled");
        }
    }
    return 1_D/xFD::Tan(x);
}

Decimal Decimal::Sec(const Decimal& x) {
    if (x.IsNaN() || x.IsInf()) {
        if (x.iterations.TOE()) {
            throw DecimalIllegalOperation("IEE754 special number arithmetic is disabled");
        }
    }
    return 1_D/xFD::Cos(x);
}

Decimal Decimal::Csc(const Decimal& x) {
    if (x.IsNaN() || x.IsInf()) {
        if (x.iterations.TOE()) {
            throw DecimalIllegalOperation("IEE754 special number arithmetic is disabled");
        }
    }
    return 1_D/xFD::Sin(x);
}


Decimal Decimal::Asin(const Decimal& x) {
    if (x.IsNaN() || x.IsInf()) {
        if (x.iterations.TOE()) {
            throw DecimalIllegalOperation("IEE754 special number arithmetic is disabled");
        }
    }
    if (xFD::Abs(x) > 1_D) {
        throw DecimalIllegalOperation("Inverse sine is only defined for -1 <= x <= 1");
    }

    Decimal term = x;
    Decimal n = 1_D;
    Decimal _2n = 2_D;
    // This is nCr(2n,n) i.e. (2n)!/((n)!(2n-n)!)
    // or simply (2n)!/((n!)^2)
    Decimal ncr = 2_D;
    Decimal fact2 = 2_D;
    Decimal fact1 = 1_D;
    Decimal _x2 = x*x;
    Decimal _xp = _x2*x;
    Decimal _22ni = 4_D;
    Decimal _22n = 4_D;

    for (int i = 1; i <= x.iterations.trig; i++) {
        term += ncr/_22ni * _xp/(_2n+1_D);
        _xp *= _x2;
        fact2 *= (_2n+1_D) * (_2n*2_D);
        fact1 *= n+1_D;
        ncr = fact2/(fact1*fact1);
        n += 1_D;
        _2n += 2_D;
        _22ni *= _22n;
    }
    return term;
}

Decimal Decimal::Acos(const Decimal& x) {
    if (x.IsNaN() || x.IsInf()) {
        if (x.iterations.TOE()) {
            throw DecimalIllegalOperation("IEE754 special number arithmetic is disabled");
        }
    }
    if (xFD::Abs(x) > 1_D) {
        throw DecimalIllegalOperation("Inverse cosine is only defined for -1 <= x <= 1");
    }


    Decimal PI2 = xFDCon::Pi2();
    return PI2 - Asin(x);
}

Decimal Decimal::Atan(const Decimal& x) {
    if (x.IsNaN() || x.IsInf()) {
        if (x.iterations.TOE()) {
            throw DecimalIllegalOperation("IEE754 special number arithmetic is disabled");
        }
    }
    if (xFD::Abs(x) < 1_D) {
        Decimal term = x;
        Decimal n = 3_D;
        Decimal _x2 = x*x;
        Decimal _xp = _x2*x;
        Decimal sign = -1_D;
        
        for (int i = 1; i <= x.iterations.trig; i++) {
            term += sign*_xp/n;
            _xp *= _x2;
            n += 2_D;
            sign *= -1_D;
        }
        return term;
    }
    else {
        Decimal PI2 = xFDCon::Pi2();
        Decimal term = PI2 * xFD::Sign(x);
        Decimal n = 3_D;
        Decimal _x2 = x*x;
        Decimal _xp = _x2*x;
        Decimal sign = -1_D;

        for (int i = 1; i <= x.iterations.trig; i++) {
            term += sign/(n*_xp);
            _xp *= _x2;
            n += 2_D;
            sign *= -1_D;
        }
        return term;

    }
}

Decimal Decimal::Atan2(const Decimal& x, const Decimal& y) {
    if (x.IsNaN() || x.IsInf()) {
        if (x.iterations.TOE()) {
            throw DecimalIllegalOperation("IEE754 special number arithmetic is disabled");
        }
    }
    Decimal PI2 = xFDCon::Pi2();
    if (y == 0_D) {
        if (x.iterations.throw_on_error || y.iterations.throw_on_error) {
            throw DecimalIllegalOperation("Inverse tangent on any angle on the same period as Pi/2 is undefined");
        }
        else {
            if (x == 0_D) {
                return xFD::NaN();
            }
            else if (x > 0_D) {
                return PI2;
            }
            else {
                return xFD::TrigPhaseCorrect(PI2);
            }
        }
    }

    Decimal term = Atan(y/x);

    // Now fix the quadrant because the result is between 0 and PI/2
    // x = cos, y = sin
    // Sine is positive in second quadrant
    // Sine and cosine are both negative in 3rd quadrant
    // Cosine is positive in 4th quadrant
    // Code uses https://en.wikipedia.org/wiki/Atan2#Definition_and_computation
    if (x < 0_D && y >= 0_D) {
        return xFD::TrigPhaseCorrect(term + 2_D*PI2);
    }
    else if (x < 0_D && y < 0_D) {
        return xFD::TrigPhaseCorrect(term - 2_D*PI2);
    }
    else { // x > 0_D
        return term;
    }
}

Decimal Decimal::Acot(const Decimal& x) {
    if (x.IsNaN() || x.IsInf()) {
        if (x.iterations.TOE()) {
            throw DecimalIllegalOperation("IEE754 special number arithmetic is disabled");
        }
    }
    Decimal PI2 = xFDCon::Pi2();
    return PI2 - Atan(x);
}

Decimal Decimal::Asec(const Decimal& x) {
    if (x.IsNaN() || x.IsInf()) {
        if (x.iterations.TOE()) {
            throw DecimalIllegalOperation("IEE754 special number arithmetic is disabled");
        }
    }
    return Acos(1_D/x);
}

Decimal Decimal::Acsc(const Decimal &x) {
    if (x.IsNaN() || x.IsInf()) {
        if (x.iterations.TOE()) {
            throw DecimalIllegalOperation("IEE754 special number arithmetic is disabled");
        }
    }
    return Asin(1_D/x);
}

//Comparators
bool Decimal::operator== (const Decimal& right) const
{
    int check = CompareNum(*this,right);
    if ((check == 0)&&(sign==right.sign))
        return true;
    return false;
};

bool Decimal::operator!= (const Decimal& right) const
{
    return !(*this==right);
};

bool Decimal::operator> (const Decimal& right) const
{
    if( ((sign=='+')&& (right.sign=='+')) )
    {
        int check = CompareNum(*this,right);
        if (check == 1)
            return true;
        return false;
    }
    if( ((sign=='-')&& (right.sign=='-')) )
    {
        int check = CompareNum(*this,right);
        if (check == 2)
            return true;
        return false;
    }
    if( ((sign=='-')&& (right.sign=='+')) )
        return false;
    if( ((sign=='+')&& (right.sign=='-')) )
        return true;
    // Should not get here
    throw DecimalIllegalOperation("Decimal assertion failure");
};

bool Decimal::operator>= (const Decimal& right) const
{
    return !(*this < right);
};

bool Decimal::operator< (const Decimal& right) const
{
    if( ((sign=='+')&& (right.sign=='+')) )
    {
        int check = CompareNum(*this,right);
        if (check == 2)
            return true;
        return false;
    }
    if( ((sign=='-')&& (right.sign=='-')) )
    {
        int check = CompareNum(*this,right);
        if (check == 1)
            return true;
        return false;
    }
    if( ((sign=='-')&& (right.sign=='+')) )
        return true;
    if( ((sign=='+')&& (right.sign=='-')) )
        return false;
    // Should not get here
    throw DecimalIllegalOperation("Decimal assertion failure");
};

bool Decimal::operator<= (const Decimal& right) const
{
    return !(*this > right);
};

//Stream Operators
std::ostream& operator<<(std::ostream& out, const Decimal& right)
{
    if(right.type == Decimal::NumType::_NAN)
    {
        out<<"NaN";
        return out;
    }

    else if(right.type == Decimal::NumType::_INFINITY)
    {
        out<<right.sign;
        out << "INF";
        return out;
    }

    out<<right.sign;
    for (int i=right.number.size()-1; i>=0; --i)
    {
        out << right.number[i];
        if((i==right.decimals)&&(i!=0))
            out <<'.';
    }
    return out;

};

std::istream& operator>>(std::istream& in, Decimal& right)
{
    std::string c;
    in >> c;
    right = c;
    return in;
};

//Transformation Methods
bool Decimal::FitsChar8() const {
    if (type != Decimal::NumType::_NORMAL) return false;
    if (decimals > 0) return false;

    Decimal a(std::to_string(SCHAR_MIN));
    Decimal b(std::to_string(SCHAR_MAX));
    if (*this < a) return false;
    if (*this > b) return false;

    return true;
}

bool Decimal::FitsUChar8() const {
    if (type != Decimal::NumType::_NORMAL) return false;
    if (decimals > 0) return false;

    Decimal b(std::to_string(UCHAR_MAX));
    if (*this < 0) return false;
    if (*this > b) return false;

    return true;
}

bool Decimal::FitsShort16() const {
    if (type != Decimal::NumType::_NORMAL) return false;
    if (decimals > 0) return false;

    Decimal a(std::to_string(SHRT_MIN));
    Decimal b(std::to_string(SHRT_MAX));
    if (*this < a) return false;
    if (*this > b) return false;

    return true;
}

bool Decimal::FitsUShort16() const {
    if (type != Decimal::NumType::_NORMAL) return false;
    if (decimals > 0) return false;

    Decimal b(std::to_string(USHRT_MAX));
    if (*this < 0) return false;
    if (*this > b) return false;

    return true;
}

bool Decimal::FitsInt32() const {
    if (type != Decimal::NumType::_NORMAL) return false;
    if (decimals > 0) return false;

    Decimal a(std::to_string(INT_MIN));
    Decimal b(std::to_string(INT_MAX));
    if (*this < a) return false;
    if (*this > b) return false;

    return true;
}

bool Decimal::FitsUInt32() const {
    if (type != Decimal::NumType::_NORMAL) return false;
    if (decimals > 0) return false;

    Decimal b(std::to_string(UINT_MAX));
    if (*this < 0) return false;
    if (*this > b) return false;

    return true;
}

bool Decimal::FitsLong64() const {
    if (type != Decimal::NumType::_NORMAL) return false;
    if (decimals > 0) return false;

    Decimal a(std::to_string(LONG_MIN));
    Decimal b(std::to_string(LONG_MAX));
    if (*this < a) return false;
    if (*this > b) return false;

    return true;
}

bool Decimal::FitsULong64() const {
    if (type != Decimal::NumType::_NORMAL) return false;
    if (decimals > 0) return false;

    Decimal b(std::to_string(ULONG_MAX));
    if (*this < 0) return false;
    if (*this > b) return false;

    return true;
}

bool Decimal::FitsLongLong64() const {
    if (type != Decimal::NumType::_NORMAL) return false;
    if (decimals > 0) return false;

    Decimal a(std::to_string(LONG_LONG_MIN));
    Decimal b(std::to_string(LONG_LONG_MAX));
    if (*this < a) return false;
    if (*this > b) return false;

    return true;
}

bool Decimal::FitsULongLong64() const {
    if (type != Decimal::NumType::_NORMAL) return false;
    if (decimals > 0) return false;

    Decimal b(std::to_string(ULONG_LONG_MAX));
    if (*this < 0) return false;
    if (*this > b) return false;

    return true;
}

bool Decimal::FitsFloat() const {
    // IEEE-754 special numbers support for floating-points
    // is unreliable so don't use it
    if (type != Decimal::NumType::_NORMAL) return false;

    // Ensure the number is within bounds
    Decimal a(std::to_string(FLT_MAX));
    if (xFD::Abs(*this) > a) return false;

    // Ensure the number is within precision
    a = Decimal(std::to_string(FLT_MIN));
    if (xFD::Abs(*this) < a) return false;

    // Ensure the digits of the number fit inside
    // the mantisa (that group of digits in the floating-point
    // type that holds the actual decimal numbers), throwing
    // away any leading zeroes.
    // This is very important because C/C++ silently lets the
    // error pass by throwing away precision.
    int sig = 0;
    bool passed_initial_zero = false;
    int delta = 1;
    for (int i = number.size()-1; i >= 0; i--) {
        if (number[i] == 0 && !passed_initial_zero) continue;
        else if (number[i] != 0 && !passed_initial_zero) passed_initial_zero = true;

        // Checks for trailing zeroes.
        if (number[i] != 0) {
            sig += delta;
            delta = 1;
        }
        else {
            ++delta;
        }
        if (sig > FLT_DIG) return false;
    }
    return true;
}

bool Decimal::FitsDouble() const {
    // IEEE-754 special numbers support for floating-points
    // is unreliable so don't use it
    if (type != Decimal::NumType::_NORMAL) return false;

    // Ensure the number is within bounds
    Decimal a(std::to_string(DBL_MAX));
    if (xFD::Abs(*this) > a) return false;

    // Ensure the number is within precision
    a = Decimal(std::to_string(DBL_MIN));
    if (xFD::Abs(*this) < a) return false;

    // Ensure the digits of the number fit inside
    // the mantisa (that group of digits in the floating-point
    // type that holds the actual decimal numbers), throwing
    // away any leading zeroes.
    // This is very important because C/C++ silently lets the
    // error pass by throwing away precision.
    int sig = 0;
    bool passed_initial_zero = false;
    int delta = 1;
    for (int i = number.size()-1; i >= 0; i--) {
        if (number[i] == 0 && !passed_initial_zero) continue;
        else if (number[i] != 0 && !passed_initial_zero) passed_initial_zero = true;

        // Checks for trailing zeroes.
        if (number[i] != 0) {
            sig += delta;
            delta = 1;
        }
        else {
            ++delta;
        }
        if (sig > DBL_DIG) return false;
    }
    return true;
}

bool Decimal::FitsLongDouble() const {
    // IEEE-754 special numbers support for floating-points
    // is unreliable so don't use it
    if (type != Decimal::NumType::_NORMAL) return false;

    // Ensure the number is within bounds
    Decimal a(std::to_string(LDBL_MAX));
    if (xFD::Abs(*this) > a) return false;

    // Ensure the number is within precision
    a = Decimal(std::to_string(LDBL_MIN));
    if (xFD::Abs(*this) < a) return false;

    // Ensure the digits of the number fit inside
    // the mantisa (that group of digits in the floating-point
    // type that holds the actual decimal numbers), throwing
    // away any leading zeroes.
    // This is very important because C/C++ silently lets the
    // error pass by throwing away precision.
    int sig = 0;
    bool passed_initial_zero = false;
    int delta = 1;
    for (int i = number.size()-1; i >= 0; i--) {
        if (number[i] == 0 && !passed_initial_zero) continue;
        else if (number[i] != 0 && !passed_initial_zero) passed_initial_zero = true;

        // Checks for trailing zeroes.
        if (number[i] != 0) {
            sig += delta;
            delta = 1;
        }
        else {
            ++delta;
        }
        if (sig > LDBL_DIG) return false;
    }
    return true;
}


char Decimal::ToChar8() const
{
    char var = 0;

    if(!this->FitsChar8())
    {
        throw DecimalIllegalOperation("Decimal cannot be converted to Char8");
    }

    int dec = 1;

    for(size_t i = 0 ; i< number.size(); i++)
    {
        var += CharToInt(number[i])*dec;
        dec*=10;
    }

    if (sign=='-')
        var *= -1;

    return var;
};

unsigned char Decimal::ToUChar8() const
{
    unsigned char var = 0;

    if(!this->FitsUChar8())
    {
        throw DecimalIllegalOperation("Decimal cannot be converted to UChar8");
    }

    int dec = 1;

    for(size_t i = 0 ; i< number.size(); i++)
    {
        var += CharToInt(number[i])*dec;
        dec*=10;
    }

    return var;
};

short Decimal::ToShort16() const
{
    short var = 0;

    if(!this->FitsShort16())
    {
        throw DecimalIllegalOperation("Decimal cannot be converted to Short16");
    }

    int dec = 1;

    for(size_t i = 0 ; i< number.size(); i++)
    {
        var += CharToInt(number[i])*dec;
        dec*=10;
    }

    if (sign=='-')
        var *= -1;

    return var;
};

unsigned short Decimal::ToUShort16() const
{
    unsigned short var = 0;

    if(!this->FitsUShort16())
    {
        throw DecimalIllegalOperation("Decimal cannot be converted to UShort16");
    }

    int dec = 1;

    for(size_t i = 0 ; i< number.size(); i++)
    {
        var += CharToInt(number[i])*dec;
        dec*=10;
    }

    return var;
};

int Decimal::ToInt32() const
{
    int var = 0;

    if(!this->FitsInt32())
    {
        throw DecimalIllegalOperation("Decimal cannot be converted to Int32");
    }

    int dec = 1;

    for(size_t i = 0 ; i< number.size(); i++)
    {
        var += CharToInt(number[i])*dec;
        dec*=10;
    }

    if (sign=='-')
        var *= -1;

    return var;
};

unsigned int Decimal::ToUInt32() const
{
    unsigned int var = 0;

    if(!this->FitsUInt32())
    {
        throw DecimalIllegalOperation("Decimal cannot be converted to UInt32");
    }

    unsigned int dec = 1;

    for(size_t i = 0 ; i< number.size(); i++)
    {
        var += CharToInt(number[i])*dec;
        dec*=10;
    }

    return var;
};

long Decimal::ToLong64() const
{
    long var = 0;

    if(!this->FitsLong64())
    {
        throw DecimalIllegalOperation("Decimal cannot be converted to Long64");
    }

    long dec = 1;

    for(size_t i = 0 ; i< number.size(); i++)
    {
        var += CharToInt(number[i])*dec;
        dec*=10;
    }

    if (sign=='-')
        var *= -1;

    return var;
};

unsigned long Decimal::ToULong64() const
{
    unsigned long var = 0;

    if(!this->FitsULong64())
    {
        throw DecimalIllegalOperation("Decimal cannot be converted to ULong64");
    }

    unsigned long dec = 1;

    for(size_t i = 0 ; i< number.size(); i++)
    {
        var += CharToInt(number[i])*dec;
        dec*=10;
    }

    return var;
};

long long Decimal::ToLongLong64() const
{
    long long var = 0;

    if(!this->FitsLongLong64())
    {
        throw DecimalIllegalOperation("Decimal cannot be converted to LongLong64");
    }

    long long dec = 1;

    for(size_t i = 0 ; i< number.size(); i++)
    {
        var += CharToInt(number[i])*dec;
        dec*=10;
    }

    if (sign=='-')
        var *= -1;

    return var;
};

unsigned long long Decimal::ToULongLong64() const
{
    unsigned long long var = 0;

    if(!this->FitsULongLong64())
    {
        throw DecimalIllegalOperation("Decimal cannot be converted to ULongLong64");
    }

    unsigned long long dec = 1;

    for(size_t i = 0 ; i< number.size(); i++)
    {
        var += CharToInt(number[i])*dec;
        dec*=10;
    }

    return var;
};

float Decimal::ToFloat() const
{
    float var = 0;

    if(!this->FitsFloat())
    {
        //        var=std::nan("");
        //        return var;
        throw DecimalIllegalOperation("Decimal cannot be converted to Float");
    }

    float dec = 1;
    if(decimals>0)
    {
        int aus = decimals;
        while( aus != 0)
        {
            dec /= 10;
            aus--;
        }
    }

    for(size_t i = 0 ; i< number.size(); i++)
    {
        var += CharToInt(number[i])*dec;
        dec*=10;
    }

    // Cut all unnecessary precision by rounding
    // to the decimal place.
    float exponent = std::pow(10, decimals);
    var = std::roundf(var * exponent) / exponent;

    if (sign=='-')
        var *= -1;

    return var;
};

double Decimal::ToDouble() const
{
    double var = 0;

    if(!this->FitsDouble())
    {
        //        var=std::nan("");
        //        return var;
        throw DecimalIllegalOperation("Decimal cannot be converted to Double");
    }

    double dec = 1;
    if(decimals>0)
    {
        int aus = decimals;
        while( aus != 0)
        {
            dec /= 10;
            aus--;
        }
    }

    for(size_t i = 0 ; i< number.size(); i++)
    {
        var += CharToInt(number[i])*dec;
        dec*=10;
    }

    // Cut all unnecessary precision by rounding
    // to the decimal place.
    double exponent = std::pow(10, decimals);
    var = std::round(var * exponent) / exponent;

    if (sign=='-')
        var *= -1;

    return var;
};


long double Decimal::ToLongDouble() const
{
    long double var = 0;

    if(!this->FitsLongDouble())
    {
        //        var=std::nan("");
        //        return var;
        throw DecimalIllegalOperation("Decimal cannot be converted to LongDouble");
    }

    long double dec = 1;
    if(decimals>0)
    {
        int aus = decimals;
        while( aus != 0)
        {
            dec /= 10;
            aus--;
        }
    }

    for(size_t i = 0 ; i< number.size(); i++)
    {
        var += CharToInt(number[i])*dec;
        dec*=10;
    }

    // Cut all unnecessary precision by rounding
    // to the decimal place.
    long double exponent = std::pow(10, decimals);
    var = std::roundl(var * exponent) / exponent;

    if (sign=='-')
        var *= -1;

    return var;
};

std::string Decimal::ToString() const
{
    std::string var;
    std::stringstream ss;

    if(type == Decimal::NumType::_NAN)
    {
        return "NaN";
    }
    else if (type == Decimal::NumType::_INFINITY)
    {
        if (sign=='-') {
            return "-INF";
        }
        else {
            return "INF";
        }
    }

    if(sign=='-')
    {
        ss<<sign;
    }
    for(int i = static_cast<int>(number.size())-1 ; i>=0 ; i--)
    {
        ss<<number[i];
        if( (i==decimals)&&(i!=0) )
            ss<<".";
    }
    ss>>var;
    return var;
};


std::string Decimal::ToFixedString() const
{
    std::string var;
    std::stringstream ss;

    if(type == Decimal::NumType::_NAN)
    {
        return "NaN";
    }
    else if (type == Decimal::NumType::_INFINITY)
    {
        if (sign=='-') {
                return "-INF";
        }
        else {
            return "INF";
        }
    }

    ss<<sign;
    for(int i = number.size()-1 ; i>=0 ; i--)
    {
        ss<<number[i];
        if( (i==decimals)&&(i!=0) )
            ss<<".";
    }
    ss>>var;
    return var;
};

std::string Decimal::ToHex(bool lowercase) const {
    if (IsNaN() || IsInf() || !IsInt()) {
        throw DecimalIllegalOperation("can only convert integers to hex");
    }
    if (*this == 0_D) {
        return "00";
    }
    std::string out, scratch;
    if (sign == '-') {
        out += "-";
    }
    Decimal q = xFD::Floor(*this);
    Decimal r = q;
    Decimal _16 = 16_D;

    while (q > 0_D) {
        r = q % _16;
        q = xFD::Floor(q /_16);
        if (r == 0_D) {
            scratch += "0";
        }
        else if (r == 1_D) {
            scratch += "1";
        }
        else if (r == 2_D) {
            scratch += "2";
        }
        else if (r == 3_D) {
            scratch += "3";
        }
        else if (r == 4_D) {
            scratch += "4";
        }
        else if (r == 5_D) {
            scratch += "5";
        }
        else if (r == 6_D) {
            scratch += "6";
        }
        else if (r == 7_D) {
            scratch += "7";
        }
        else if (r == 8_D) {
            scratch += "8";
        }
        else if (r == 9_D) {
            scratch += "9";
        }
        else if (r == 10_D) {
            scratch += (lowercase) ? "a" : "A";
        }
        else if (r == 11_D) {
            scratch += (lowercase) ? "b" : "B";
        }
        else if (r == 12_D) {
            scratch += (lowercase) ? "c" : "C";
        }
        else if (r == 13_D) {
            scratch += (lowercase) ? "d" : "D";
        }
        else if (r == 14_D) {
            scratch += (lowercase) ? "e" : "E";
        }
        else if (r == 15_D) {
            scratch += (lowercase) ? "f" : "F";
        }
        else {
            throw DecimalIllegalOperation("Invalid number");
        }
    }
    if (scratch.length() % 2 != 0) {
        scratch += "0";
    }
    for (auto it = scratch.rbegin(); it != scratch.rend(); it++) {
        out += *it;
    }
    return out;
}

// Normalizes numbers between 0 and 2*Pi.
Decimal Decimal::TrigPhaseCorrect(const Decimal& x) {
    Decimal _2PI = xFDCon::_2Pi();
    Decimal delta = xFD::Floor(x/_2PI);
    if (delta < 0_D) {
        return x + _2PI*delta;
    }
    else if (delta > 0_D) {
        return x - _2PI*delta;
    }
    else {
        return x;
    }
}

void Decimal::SetPrecision(int prec)
{
    if(prec<0)
        return;
    if(this->decimals<prec)
    {
        while (this->decimals<prec)
        {
            this->decimals++;
            this->number.push_front('0');
        }
    }
    else if(this->decimals>prec)
    {
        char last;
        while (this->decimals>prec)
        {
            last=number[0];
            this->decimals--;
            this->number.pop_front();
        }
        if(CharToInt(last)>=5)
        {
            if(prec==0)
                (*this)++;
            else
            {
                std::stringstream ss;
                ss<<"0.";
                for( int i=1; i<prec; i++)
                    ss<<"0";
                ss<<"1";
                Decimal aus(ss.str());
                *this = *this + aus;
            }
        }
    }
    if (iterations.decimals < decimals) {
        iterations.decimals = decimals;
    }
};

//Remove leading zeros of numbers, utilized by Operations without sign
void Decimal::LeadTrim()
{
    for (int i = number.size()-1 ; i > decimals ; --i)
    {
        if(number[i]=='0')
            number.pop_back();
        else
            break;
    }
};

//Remove non significant trailing zeros
void Decimal::TrailTrim()
{
    while((decimals > 0) && (number[0]=='0'))
    {
        number.pop_front();
        decimals--;
    }
};

//Math/Scientific Methods
Decimal Decimal::Abs(const Decimal& x) {
    if (x.IsNaN() || x.IsInf()) {
        if (x.iterations.TOE()) {
            throw DecimalIllegalOperation("IEE754 special number arithmetic is disabled");
        }
    }
    Decimal a = x;
    a.sign = '+';
    return a;
}

Decimal Decimal::Sign(const Decimal& x) {
    if (x.IsNaN() || x.IsInf()) {
        if (x.iterations.TOE()) {
            throw DecimalIllegalOperation("IEE754 special number arithmetic is disabled");
        }
    }
    if (x.sign == '+') {
        return 1_D;
    }
    else if (x.sign == '-') {
        return -1_D;
    }
    else {
        return 0_D;
    }
}

Decimal Decimal::operator-() const {
    Decimal a = *this;
    if (a.IsNaN() || a.IsInf()) {
        if (a.iterations.TOE()) {
            throw DecimalIllegalOperation("IEE754 special number arithmetic is disabled");
        }
    }
    if (a.sign == '+') {
        a.sign = '-';
    }
    else if (a.sign == '-') {
        a.sign = '+';
    }
    return a;
}

//Miscellaneous Methods
std::string Decimal::Exp() const
{
    std::stringstream out;
    if(type == Decimal::NumType::_NAN)
    {
        return "NaN";
    }
    else if (type == Decimal::NumType::_INFINITY)
    {
        if (sign=='-') {
            return "-INF";
        }
        else {
            return "INF";
        }
    }

    int check = Decimal::CompareNum(*this,Decimal(1));
    if( check == 0)
    {
        out<<sign;
        out<<"1e+0";
    }
    if( check == 1)
    {
        out<<sign;
        int i = number.size()-1;
        out<< number[i];
        i--;
        if(i>0)
        {
            out<<'.';
            for (; (i>=(int)number.size()-6) && (i>=0) ; --i)
            {
                out << number[i];
            }
        }
        out<<"e+"<<Ints()-1;
    }
    if( check == 2)
    {
        int exp = 0;
        int count = number.size()-1;
        while (number[count]=='0' && count>0 )
        {
            count--;
            exp++;
        }
        if(count==0)
            if(number[count]=='0')
                out<<"+0";
            else
                out<<sign<<number[count]<<"e-"<<exp;
        else
        {
            out<<sign<<number[count]<<'.';
            for (int i=count-1; (i>=count-5) && (i>=0) ; --i)
            {
                out << number[i];
            }
            out<<"e-"<<exp;
        }
    }
    return out.str();
};

