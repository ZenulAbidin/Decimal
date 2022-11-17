#include <iostream>
#include <sstream>
#include <string>
#include <string.h>
#include <vector>
#include "types/Decimal.h"

#include <limits.h>
#include <float.h>

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE Topaz_Decimal

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE( Test_Suite_Decimal );

BOOST_AUTO_TEST_CASE(NaN)
{
    //Test
    Decimal a,b,c;

    //NaN test
    BOOST_CHECK_THROW(a.ToChar8(), DecimalIllegalOperation);
    BOOST_CHECK_THROW(a.ToUChar8(), DecimalIllegalOperation);
    BOOST_CHECK_THROW(a.ToShort16(), DecimalIllegalOperation);
    BOOST_CHECK_THROW(a.ToUShort16(), DecimalIllegalOperation);
    BOOST_CHECK_THROW(a.ToInt32(), DecimalIllegalOperation);
    BOOST_CHECK_THROW(a.ToUInt32(), DecimalIllegalOperation);
    BOOST_CHECK_THROW(a.ToLong64(), DecimalIllegalOperation);
    BOOST_CHECK_THROW(a.ToULong64(), DecimalIllegalOperation);
    BOOST_CHECK_THROW(a.ToFloat(), DecimalIllegalOperation);
    BOOST_CHECK_THROW(a.ToDouble(), DecimalIllegalOperation);
    BOOST_CHECK_THROW(a.ToLongDouble(), DecimalIllegalOperation);
    BOOST_CHECK_EQUAL(a.ToFixedString(), "NaN");

    //Error Codes test
    a=11.5_D;
    BOOST_CHECK_THROW(a%2_D, DecimalIllegalOperation);
    b=15_D;
    BOOST_CHECK_THROW(b/0_D, DecimalIllegalOperation);
}

BOOST_AUTO_TEST_CASE(Precision)
{
    //Assignation,SetPrecision,Trim test
    Decimal d(11.5_D);
    BOOST_CHECK_EQUAL(d.ToFixedString(), "+11.500000");
    d=".1"_D;
    BOOST_CHECK_EQUAL(d.ToFixedString(), "+0.1");
    d="-.1"_D;
    BOOST_CHECK_EQUAL(d.ToFixedString(), "-0.1");
    d=-14.34434340000;
    BOOST_CHECK_EQUAL(d.ToFixedString(), "-14.344343");
    d="000.645343400000"_D;
    BOOST_CHECK_EQUAL(d.ToFixedString(), "+0.645343400000");
    BOOST_CHECK_EQUAL(d.Decimals(), 12);
    d.TrailTrim();
    d.LeadTrim();
    BOOST_CHECK_EQUAL(d.ToFixedString(), "+0.6453434");
    d.SetPrecision(2);
    BOOST_CHECK_EQUAL(d.ToFixedString(), "+0.65");
    d.SetPrecision(0);
    BOOST_CHECK_EQUAL(d.ToFixedString(), "+1");
    d="-1000.12323"_D;
    BOOST_CHECK_EQUAL(d.Exp(), "-1.00012e+3");
    d="-1.1223222"_D;
    BOOST_CHECK_EQUAL(d.Exp(), "-1.12232e+0");
    d="-1";
    BOOST_CHECK_EQUAL(d.Exp(), "-1e+0");
    d="-0.0000123453434"_D;
    BOOST_CHECK_EQUAL(d.Exp(), "-1.23453e-5");
    d="-0.0000000000000"_D;
    BOOST_CHECK_EQUAL(d.Exp(), "+0");
}

BOOST_AUTO_TEST_CASE(Convert_Limits) {
    Decimal a = "1"_D;
    BOOST_CHECK_EQUAL(a.ToChar8(),(char) 1);
    BOOST_CHECK_EQUAL(a.ToUChar8(), (unsigned char) 1);
    BOOST_CHECK_EQUAL(a.ToShort16(), (short) 1);
    BOOST_CHECK_EQUAL(a.ToUShort16(), (unsigned short) 1);
    BOOST_CHECK_EQUAL(a.ToInt32(), (int) 1);
    BOOST_CHECK_EQUAL(a.ToUInt32(), (unsigned int) 1);
    BOOST_CHECK_EQUAL(a.ToLong64(), (long) 1);
    BOOST_CHECK_EQUAL(a.ToULong64(), (unsigned long) 1);
    BOOST_CHECK_EQUAL(a.ToFloat(), (float) 1);
    BOOST_CHECK_EQUAL(a.ToDouble(), (double) 1);
    BOOST_CHECK_EQUAL(a.ToLongDouble(), (long double) 1);
    BOOST_CHECK_EQUAL(a.ToString(), "1");

    a = "-1"_D;
    BOOST_CHECK_EQUAL(a.ToChar8(),(char) -1);
    BOOST_CHECK_THROW(a.ToUChar8(), DecimalIllegalOperation);
    BOOST_CHECK_EQUAL(a.ToShort16(), (short) -1);
    BOOST_CHECK_THROW(a.ToUShort16(), DecimalIllegalOperation);
    BOOST_CHECK_EQUAL(a.ToInt32(), (int) -1);
    BOOST_CHECK_THROW(a.ToUInt32(), DecimalIllegalOperation);
    BOOST_CHECK_EQUAL(a.ToLong64(), (long) -1);
    BOOST_CHECK_THROW(a.ToULong64(), DecimalIllegalOperation);
    BOOST_CHECK_EQUAL(a.ToFloat(), (float) -1);
    BOOST_CHECK_EQUAL(a.ToDouble(), (double) -1);
    BOOST_CHECK_EQUAL(a.ToLongDouble(), (long double) -1);
    BOOST_CHECK_EQUAL(a.ToString(), "-1");

    a = "1.3"_D;
    BOOST_CHECK_THROW(a.ToChar8(), DecimalIllegalOperation);
    BOOST_CHECK_THROW(a.ToUChar8(), DecimalIllegalOperation);
    BOOST_CHECK_THROW(a.ToShort16(), DecimalIllegalOperation);
    BOOST_CHECK_THROW(a.ToUShort16(), DecimalIllegalOperation);
    BOOST_CHECK_THROW(a.ToInt32(), DecimalIllegalOperation);
    BOOST_CHECK_THROW(a.ToUInt32(), DecimalIllegalOperation);
    BOOST_CHECK_THROW(a.ToLong64(), DecimalIllegalOperation);
    BOOST_CHECK_THROW(a.ToULong64(), DecimalIllegalOperation);
    BOOST_CHECK_EQUAL(a.ToString(), "1.3");

    a = Decimal(SCHAR_MAX) + 1_D;
    BOOST_CHECK_THROW(a.ToChar8(), DecimalIllegalOperation);
    BOOST_CHECK_EQUAL(a.ToUChar8(), (unsigned char) (SCHAR_MAX + 1));
    BOOST_CHECK_EQUAL(a.ToShort16(), (short) (SCHAR_MAX + 1));
    BOOST_CHECK_EQUAL(a.ToUShort16(), (unsigned short) (SCHAR_MAX + 1));
    BOOST_CHECK_EQUAL(a.ToInt32(), (int) (SCHAR_MAX + 1));
    BOOST_CHECK_EQUAL(a.ToUInt32(), (unsigned int) (SCHAR_MAX + 1));
    BOOST_CHECK_EQUAL(a.ToLong64(), (long) (SCHAR_MAX + 1));
    BOOST_CHECK_EQUAL(a.ToULong64(), (unsigned long) (SCHAR_MAX + 1));

    a = Decimal(UCHAR_MAX) + 1_D;
    BOOST_CHECK_THROW(a.ToChar8(), DecimalIllegalOperation);
    BOOST_CHECK_THROW(a.ToUChar8(), DecimalIllegalOperation);
    BOOST_CHECK_EQUAL(a.ToShort16(), (short) (UCHAR_MAX + 1));
    BOOST_CHECK_EQUAL(a.ToUShort16(), (unsigned short) (UCHAR_MAX + 1));
    BOOST_CHECK_EQUAL(a.ToInt32(), (int) (UCHAR_MAX + 1));
    BOOST_CHECK_EQUAL(a.ToUInt32(), (unsigned int) (UCHAR_MAX + 1));
    BOOST_CHECK_EQUAL(a.ToLong64(), (long) (UCHAR_MAX + 1));
    BOOST_CHECK_EQUAL(a.ToULong64(), (unsigned long) (UCHAR_MAX + 1));

    a = Decimal(SHRT_MAX) + 1_D;
    BOOST_CHECK_THROW(a.ToChar8(), DecimalIllegalOperation);
    BOOST_CHECK_THROW(a.ToUChar8(), DecimalIllegalOperation);
    BOOST_CHECK_THROW(a.ToShort16(), DecimalIllegalOperation);
    BOOST_CHECK_EQUAL(a.ToUShort16(), (unsigned short) (SHRT_MAX + 1));
    BOOST_CHECK_EQUAL(a.ToInt32(), (int) (SHRT_MAX + 1));
    BOOST_CHECK_EQUAL(a.ToUInt32(), (unsigned int) (SHRT_MAX + 1));
    BOOST_CHECK_EQUAL(a.ToLong64(), (long) (SHRT_MAX + 1));
    BOOST_CHECK_EQUAL(a.ToULong64(), (unsigned long) (SHRT_MAX + 1));

    a = Decimal(USHRT_MAX) + 1_D;
    BOOST_CHECK_THROW(a.ToChar8(), DecimalIllegalOperation);
    BOOST_CHECK_THROW(a.ToUChar8(), DecimalIllegalOperation);
    BOOST_CHECK_THROW(a.ToShort16(), DecimalIllegalOperation);
    BOOST_CHECK_THROW(a.ToUShort16(), DecimalIllegalOperation);
    BOOST_CHECK_EQUAL(a.ToInt32(), (int) (USHRT_MAX + 1));
    BOOST_CHECK_EQUAL(a.ToUInt32(), (unsigned int) (USHRT_MAX + 1));
    BOOST_CHECK_EQUAL(a.ToLong64(), (long) (USHRT_MAX + 1));
    BOOST_CHECK_EQUAL(a.ToULong64(), (unsigned long) (USHRT_MAX + 1));

    a = Decimal(INT_MAX) + 1_D;
    BOOST_CHECK_THROW(a.ToChar8(), DecimalIllegalOperation);
    BOOST_CHECK_THROW(a.ToUChar8(), DecimalIllegalOperation);
    BOOST_CHECK_THROW(a.ToShort16(), DecimalIllegalOperation);
    BOOST_CHECK_THROW(a.ToUShort16(), DecimalIllegalOperation);
    BOOST_CHECK_THROW(a.ToInt32(), DecimalIllegalOperation);
    BOOST_CHECK_EQUAL(a.ToUInt32(), (unsigned int) (INT_MAX) + 1);
    BOOST_CHECK_EQUAL(a.ToLong64(), (long) (INT_MAX) + 1);
    BOOST_CHECK_EQUAL(a.ToULong64(), (unsigned long) (INT_MAX) + 1);

    a = Decimal(UINT_MAX) + 1_D;
    BOOST_CHECK_THROW(a.ToChar8(), DecimalIllegalOperation);
    BOOST_CHECK_THROW(a.ToUChar8(), DecimalIllegalOperation);
    BOOST_CHECK_THROW(a.ToShort16(), DecimalIllegalOperation);
    BOOST_CHECK_THROW(a.ToUShort16(), DecimalIllegalOperation);
    BOOST_CHECK_THROW(a.ToInt32(), DecimalIllegalOperation);
    BOOST_CHECK_THROW(a.ToUInt32(), DecimalIllegalOperation);
    BOOST_CHECK_EQUAL(a.ToLong64(), (long) (UINT_MAX) + 1);
    BOOST_CHECK_EQUAL(a.ToULong64(), (unsigned long) (UINT_MAX) + 1);

    a = Decimal(LONG_MAX) + 1_D;
    BOOST_CHECK_THROW(a.ToChar8(), DecimalIllegalOperation);
    BOOST_CHECK_THROW(a.ToUChar8(), DecimalIllegalOperation);
    BOOST_CHECK_THROW(a.ToShort16(), DecimalIllegalOperation);
    BOOST_CHECK_THROW(a.ToUShort16(), DecimalIllegalOperation);
    BOOST_CHECK_THROW(a.ToInt32(), DecimalIllegalOperation);
    BOOST_CHECK_THROW(a.ToUInt32(), DecimalIllegalOperation);
    BOOST_CHECK_THROW(a.ToLong64(), DecimalIllegalOperation);
    BOOST_CHECK_EQUAL(a.ToULong64(), (unsigned long) (LONG_MAX) + 1);

    a = Decimal(ULONG_MAX) + 1_D;
    BOOST_CHECK_THROW(a.ToChar8(), DecimalIllegalOperation);
    BOOST_CHECK_THROW(a.ToUChar8(), DecimalIllegalOperation);
    BOOST_CHECK_THROW(a.ToShort16(), DecimalIllegalOperation);
    BOOST_CHECK_THROW(a.ToUShort16(), DecimalIllegalOperation);
    BOOST_CHECK_THROW(a.ToInt32(), DecimalIllegalOperation);
    BOOST_CHECK_THROW(a.ToUInt32(), DecimalIllegalOperation);
    BOOST_CHECK_THROW(a.ToLong64(), DecimalIllegalOperation);
    BOOST_CHECK_THROW(a.ToULong64(), DecimalIllegalOperation);

    a = Decimal(SCHAR_MIN) - 1_D;
    BOOST_CHECK_THROW(a.ToChar8(), DecimalIllegalOperation);
    BOOST_CHECK_EQUAL(a.ToShort16(), (short) (SCHAR_MIN) - 1);
    BOOST_CHECK_EQUAL(a.ToInt32(), (int) (SCHAR_MIN) - 1);
    BOOST_CHECK_EQUAL(a.ToLong64(), (long) (SCHAR_MIN) - 1);

    a = Decimal(SHRT_MIN) - 1_D;
    BOOST_CHECK_THROW(a.ToChar8(), DecimalIllegalOperation);
    BOOST_CHECK_THROW(a.ToShort16(), DecimalIllegalOperation);
    BOOST_CHECK_EQUAL(a.ToInt32(), (int) (SHRT_MIN) - 1);
    BOOST_CHECK_EQUAL(a.ToLong64(), (long) (SHRT_MIN) - 1);

    a = Decimal(INT_MIN) - 1_D;
    BOOST_CHECK_THROW(a.ToChar8(), DecimalIllegalOperation);
    BOOST_CHECK_THROW(a.ToShort16(), DecimalIllegalOperation);
    BOOST_CHECK_THROW(a.ToInt32(), DecimalIllegalOperation);
    BOOST_CHECK_EQUAL(a.ToLong64(), (long) (INT_MIN) - 1);

    a = Decimal(LONG_MIN) - 1_D;
    BOOST_CHECK_THROW(a.ToChar8(), DecimalIllegalOperation);
    BOOST_CHECK_THROW(a.ToShort16(), DecimalIllegalOperation);
    BOOST_CHECK_THROW(a.ToInt32(), DecimalIllegalOperation);
    BOOST_CHECK_THROW(a.ToLong64(), DecimalIllegalOperation);
}

BOOST_AUTO_TEST_CASE(Convert_FloatMax) {
    Decimal a = Decimal(FLT_MAX) + 1_D;
    BOOST_CHECK_THROW(a.ToFloat(), DecimalIllegalOperation);
    // Following fill fail on GCC (AMD64) due to excessive precision
    // in the constants. But this behavior cannot be guarrenteed on
    // other platforms, so a test cannot be made out of this.
    //BOOST_CHECK_EQUAL(a.ToDouble(), (double) (FLT_MAX) + 1);
    //BOOST_CHECK_EQUAL(a.ToLongDouble(), (long double) (FLT_MAX) + 1);

    a = Decimal(DBL_MAX) + 1_D;
    BOOST_CHECK_THROW(a.ToFloat(), DecimalIllegalOperation);
    BOOST_CHECK_THROW(a.ToDouble(), DecimalIllegalOperation);
    //BOOST_CHECK_EQUAL(a.ToLongDouble(), (long double) (DBL_MAX) + 1);

    a = Decimal(LDBL_MAX) + 1_D;
    BOOST_CHECK_THROW(a.ToFloat(), DecimalIllegalOperation);
    BOOST_CHECK_THROW(a.ToDouble(), DecimalIllegalOperation);
    BOOST_CHECK_THROW(a.ToLongDouble(), DecimalIllegalOperation);
}

BOOST_AUTO_TEST_CASE(Convert_FloatPrec) {
    Decimal a;

    std::string s = "2.";
    for (int i = 0; i < FLT_DIG; i++) {
        s += "2";
    }
    a = s;
    BOOST_CHECK_THROW(a.ToFloat(), DecimalIllegalOperation);
    BOOST_CHECK_EQUAL(a.ToDouble(), std::stod(s));
    BOOST_CHECK_EQUAL(a.ToLongDouble(), std::stold(s));

    s = "2.";
    for (int i = 0; i < DBL_DIG; i++) {
        s += "2";
    }
    a = s;
    BOOST_CHECK_THROW(a.ToFloat(), DecimalIllegalOperation);
    BOOST_CHECK_THROW(a.ToDouble(), DecimalIllegalOperation);
    BOOST_CHECK_EQUAL(a.ToLongDouble(), std::stold(s));

    s = "2.";
    for (int i = 0; i < LDBL_DIG; i++) {
        s += "2";
    }
    a = s;
    BOOST_CHECK_THROW(a.ToFloat(), DecimalIllegalOperation);
    BOOST_CHECK_THROW(a.ToDouble(), DecimalIllegalOperation);
    BOOST_CHECK_THROW(a.ToLongDouble(), DecimalIllegalOperation);
}

BOOST_AUTO_TEST_CASE(Arithmetic) {
    //Test
    Decimal a = "1", b = "2", c;
    BOOST_CHECK_EQUAL(a.ToFixedString(), "+1");
    BOOST_CHECK_EQUAL(b.ToFixedString(), "+2");

    //Operations +,-,*,/ Test
    c=a+b;
    BOOST_CHECK_EQUAL(c.ToFixedString(), "+3");

    c=a-b;
    BOOST_CHECK_EQUAL(c.ToFixedString(), "-1");

    c=a*b;
    BOOST_CHECK_EQUAL(c.ToFixedString(), "+2");

    c=a/b;
    BOOST_CHECK_EQUAL(c.ToFixedString(), "+0.5");

    c /= 8_D;
    BOOST_CHECK_EQUAL(c.ToFixedString(), "+0.0625");

    c=a%b;
    BOOST_CHECK_EQUAL(c.ToFixedString(), "+1");

    //Comparation Test
    BOOST_CHECK_EQUAL(a==b, false);
    BOOST_CHECK_EQUAL(a>b, false);
    BOOST_CHECK_EQUAL(a<b, true);
    BOOST_CHECK_EQUAL(a>=b, false);
    BOOST_CHECK_EQUAL(a<=b, true);

    BOOST_CHECK_EQUAL(b==a, false);
    BOOST_CHECK_EQUAL(b>a, true);
    BOOST_CHECK_EQUAL(b<a, false);
    BOOST_CHECK_EQUAL(b>=a, true);
    BOOST_CHECK_EQUAL(b<=a, false);

    BOOST_CHECK_EQUAL(a==a, true);
    BOOST_CHECK_EQUAL(b!=a, true);

    //++,-- Test
    ++a;
    BOOST_CHECK_EQUAL(a.ToFixedString(), "+2");
    a++;
    BOOST_CHECK_EQUAL(a.ToFixedString(), "+3");
    --a;
    BOOST_CHECK_EQUAL(a.ToFixedString(), "+2");
    a--;
    BOOST_CHECK_EQUAL(a.ToFixedString(), "+1");

    // Rounding and increment, decrement test
    
    Decimal d = "20.02"_D;
    BOOST_CHECK_EQUAL(d.Inc().ToFixedString(), "+20.03");
    BOOST_CHECK_EQUAL(d.Dec().ToFixedString(), "+20.01");

    d = -"20.02"_D;
    BOOST_CHECK_EQUAL(d.Inc().ToFixedString(), "-20.01");
    BOOST_CHECK_EQUAL(d.Dec().ToFixedString(), "-20.03");

    d = "95.045"_D;
    BOOST_CHECK_EQUAL(xFD::Round(d, -2).ToFixedString(), "+95.05");
    BOOST_CHECK_EQUAL(xFD::Round(d, -1).ToFixedString(), "+95");
    BOOST_CHECK_EQUAL(xFD::Round(d, 2).ToFixedString(), "+100");

    // Regressions
    BOOST_CHECK_EQUAL("416984806968863648079"_D % 16_D, 15_D);

    // Hex tests
    BOOST_CHECK_EQUAL(255_D().ToHex(), "FF");
    BOOST_CHECK_EQUAL(4096_D().ToHex(), "1000");
    BOOST_CHECK_EQUAL(1_D().ToHex(), "01");
    BOOST_CHECK_EQUAL("458479643868196418248935325987194"_D.ToHex(),
            "169AD3A0A871F2694F8BCBDA717A");
    BOOST_CHECK_EQUAL("86844066927987146567678238756515930889628173209306178286953872356138621120753"_D.ToHex(),
            "BFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF0C0325AD0376782CCFDDC6E99C28B0F1");
    BOOST_CHECK_EQUAL(Decimal::FromHex("FF"), 255_D);
    BOOST_CHECK_EQUAL(Decimal::FromHex("1000"), 4096_D);
    BOOST_CHECK_EQUAL(Decimal::FromHex("1"), 1_D);
    BOOST_CHECK_EQUAL(Decimal::FromHex("169AD3A0A871F2694F8BCBDA717A"),
            "458479643868196418248935325987194"_D);
    BOOST_CHECK_EQUAL(Decimal::FromHex("BFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF0C0325AD0376782CCFDDC6E99C28B0F1"),
            "86844066927987146567678238756515930889628173209306178286953872356138621120753"_D);
}

BOOST_AUTO_TEST_SUITE_END();
