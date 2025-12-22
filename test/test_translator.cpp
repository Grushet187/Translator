#include <gtest.h>
#include <stdexcept>
#include <cmath>
#include <string>

#include "translator.h"

class TranslatorTest : public ::testing::Test {
protected:
    Translator tr;

    static void ExpectNear(double actual, double expected, double eps = 1e-12) {
        EXPECT_NEAR(actual, expected, eps);
    }
};
TEST_F(TranslatorTest, Arithmetic_Add) { EXPECT_DOUBLE_EQ(tr.calculate("2+2"), 4); }
TEST_F(TranslatorTest, Arithmetic_Sub) { EXPECT_DOUBLE_EQ(tr.calculate("10-3"), 7); }
TEST_F(TranslatorTest, Arithmetic_Mul) { EXPECT_DOUBLE_EQ(tr.calculate("6*7"), 42); }
TEST_F(TranslatorTest, Arithmetic_Div) { ExpectNear(tr.calculate("7/2"), 3.5); }

TEST_F(TranslatorTest, Arithmetic_ZeroRules) {
    EXPECT_DOUBLE_EQ(tr.calculate("0+0"), 0);
    EXPECT_DOUBLE_EQ(tr.calculate("0*123"), 0);
    EXPECT_DOUBLE_EQ(tr.calculate("123*0"), 0);
    EXPECT_DOUBLE_EQ(tr.calculate("0-5"), -5);
    EXPECT_DOUBLE_EQ(tr.calculate("5-0"), 5);
    EXPECT_THROW(tr.calculate("1/0"), std::runtime_error);
}
TEST_F(TranslatorTest, Precedence_AddMul) {
    EXPECT_DOUBLE_EQ(tr.calculate("2+3*4"), 14);
    EXPECT_DOUBLE_EQ(tr.calculate("2*3+4"), 10);
}

TEST_F(TranslatorTest, Precedence_MulDivLeftToRight) {
    ExpectNear(tr.calculate("8/2*3"), 12.0);
    ExpectNear(tr.calculate("8/(2*3)"), 8.0 / 6.0);
    ExpectNear(tr.calculate("8/2/2"), 2.0);
}

TEST_F(TranslatorTest, Precedence_MixMany) {
   
    ExpectNear(tr.calculate("5+2*3-4/2"), 9.0);
  
    ExpectNear(tr.calculate("2+3*4-5/5"), 13.0);
}
TEST_F(TranslatorTest, Parentheses_Simple) {
    EXPECT_DOUBLE_EQ(tr.calculate("(2+3)"), 5);
    EXPECT_DOUBLE_EQ(tr.calculate("(2+3)*4"), 20);
}

TEST_F(TranslatorTest, Parentheses_Nesting) {
    EXPECT_DOUBLE_EQ(tr.calculate("((1+2)*3)"), 9);
    ExpectNear(tr.calculate("(2+(3*(4+5)))/5"), (2.0 + (3.0 * 9.0)) / 5.0);
    ExpectNear(tr.calculate("((((1+2)*3)+4)/5)"), 2.6);
}

TEST_F(TranslatorTest, Parentheses_WithSpaces) {
    EXPECT_DOUBLE_EQ(tr.calculate("   2   *  ( 3 + 1 )  "), 8);
    EXPECT_DOUBLE_EQ(tr.calculate("\t(\n1 + 2\t)\n* 3\r"), 9);
}

TEST_F(TranslatorTest, Parentheses_ManyRedundant) {
    EXPECT_DOUBLE_EQ(tr.calculate("((((3))))"), 3);
    EXPECT_DOUBLE_EQ(tr.calculate("(((((42)))))"), 42);
}
TEST_F(TranslatorTest, Unary_Simple) {
    EXPECT_DOUBLE_EQ(tr.calculate("-5"), -5);
    EXPECT_DOUBLE_EQ(tr.calculate("--3"), 3);
    EXPECT_DOUBLE_EQ(tr.calculate("-(-2)"), 2);
}

TEST_F(TranslatorTest, Unary_WithBinary) {
    EXPECT_DOUBLE_EQ(tr.calculate("5*-3"), -15);
    EXPECT_DOUBLE_EQ(tr.calculate("(-2)*(-3)"), 6);
    EXPECT_DOUBLE_EQ(tr.calculate("-(3+2)"), -5);
    EXPECT_DOUBLE_EQ(tr.calculate("-(2+3)*4"), -20);
}

TEST_F(TranslatorTest, Unary_Stacking) {
    EXPECT_DOUBLE_EQ(tr.calculate("---2"), -2);
    EXPECT_DOUBLE_EQ(tr.calculate("----2"), 2);
    EXPECT_DOUBLE_EQ(tr.calculate("--(5) + -(-2)"), 7);
}

TEST_F(TranslatorTest, Unary_BindingVsMulDiv) {
   
    EXPECT_DOUBLE_EQ(tr.calculate("-3*2"), -6);
    
    EXPECT_DOUBLE_EQ(tr.calculate("6/-3"), -2);
   
    EXPECT_DOUBLE_EQ(tr.calculate("-6/-3"), 2);
}
TEST_F(TranslatorTest, Float_Decimals) {
    ExpectNear(tr.calculate("0.5+0.25"), 0.75);
    ExpectNear(tr.calculate(".5 + .25"), 0.75);
    ExpectNear(tr.calculate("1.2000 + 0.0300"), 1.23);
}

TEST_F(TranslatorTest, Float_MoreCases) {
    ExpectNear(tr.calculate("2.5*2"), 5.0);
    ExpectNear(tr.calculate("5/2"), 2.5);
    ExpectNear(tr.calculate("0.1+0.2"), 0.3, 1e-9); 
    ExpectNear(tr.calculate("123456.789 + 0.001"), 123456.79, 1e-9);
}
TEST_F(TranslatorTest, Float_NegativeDecimal) {
    ExpectNear(tr.calculate("-0.5"), -0.5);
    ExpectNear(tr.calculate("-(.5)"), -0.5);
    ExpectNear(tr.calculate("-(.5 + .25)"), -0.75);
}
TEST_F(TranslatorTest, Complex_KnownRpnExample) {
    ExpectNear(tr.calculate("3 + 4 * 2 / (1 - 5)"), 1.0);
}

TEST_F(TranslatorTest, Complex_ManyOperatorsAndParens) {
    ExpectNear(tr.calculate("((2+3)*(4+5)-6)/(1+2)"), 13.0);
    EXPECT_DOUBLE_EQ(tr.calculate("-((3+2)*(1+1))"), -10);
}

TEST_F(TranslatorTest, Complex_SignAndFractions) {
    ExpectNear(tr.calculate("10/(2+3) + 7*(1-3)"), -12.0);
}
TEST_F(TranslatorTest, Errors_EmptyOrSpaces) {
    EXPECT_THROW(tr.calculate(""), std::runtime_error);
    EXPECT_THROW(tr.calculate("   "), std::runtime_error);
    EXPECT_THROW(tr.calculate("\t\n\r"), std::runtime_error);
}

TEST_F(TranslatorTest, Errors_UnknownCharacter) {
    EXPECT_THROW(tr.calculate("2 & 3"), std::runtime_error);
    EXPECT_THROW(tr.calculate("2a+3"), std::runtime_error);
    EXPECT_THROW(tr.calculate("12a3"), std::runtime_error);
}

TEST_F(TranslatorTest, Errors_InvalidNumberFormat) {
    EXPECT_THROW(tr.calculate("1..2"), std::runtime_error);
    EXPECT_THROW(tr.calculate("..2"), std::runtime_error);
}

TEST_F(TranslatorTest, Errors_DoubleOperator) {
    EXPECT_THROW(tr.calculate("5**2"), std::runtime_error);
    EXPECT_THROW(tr.calculate("5//2"), std::runtime_error);
    EXPECT_THROW(tr.calculate("5+-*2"), std::runtime_error);
}

TEST_F(TranslatorTest, Errors_TrailingOperator) {
    EXPECT_THROW(tr.calculate("2+3-"), std::runtime_error);
    EXPECT_THROW(tr.calculate("2*"), std::runtime_error);
    EXPECT_THROW(tr.calculate("2/"), std::runtime_error);
}

TEST_F(TranslatorTest, Errors_LeadingBinaryOperator) {
    EXPECT_THROW(tr.calculate("+3"), std::runtime_error);
    EXPECT_THROW(tr.calculate("*2"), std::runtime_error);
    EXPECT_THROW(tr.calculate("/2"), std::runtime_error);
}

TEST_F(TranslatorTest, Errors_TooManyNumbersNoOperator) {
    EXPECT_THROW(tr.calculate("2 3"), std::runtime_error);
    EXPECT_THROW(tr.calculate("1 2 3"), std::runtime_error);
    EXPECT_THROW(tr.calculate("1(2+3)"), std::runtime_error);
    EXPECT_THROW(tr.calculate(")(1+2)("), std::runtime_error);
}

TEST_F(TranslatorTest, Errors_Parentheses) {
    EXPECT_THROW(tr.calculate("()"), std::runtime_error);
    EXPECT_THROW(tr.calculate("(2+3"), std::runtime_error);
    EXPECT_THROW(tr.calculate("2+3)"), std::runtime_error);
    EXPECT_THROW(tr.calculate("(1+2))"), std::runtime_error);
    EXPECT_THROW(tr.calculate("((1+2)"), std::runtime_error);
    EXPECT_THROW(tr.calculate("(*)"), std::runtime_error);
    EXPECT_THROW(tr.calculate("(-)"), std::runtime_error);
}

TEST_F(TranslatorTest, Errors_MissingOperandAroundOperator) {
    EXPECT_THROW(tr.calculate("2 + "), std::runtime_error);
    EXPECT_THROW(tr.calculate("2 / ( )"), std::runtime_error);
    EXPECT_THROW(tr.calculate("2 / ()"), std::runtime_error);
    EXPECT_THROW(tr.calculate("(1+)"), std::runtime_error);
    EXPECT_THROW(tr.calculate("(*2)"), std::runtime_error);
}

TEST_F(TranslatorTest, Errors_DivideByZeroVariants) {
    EXPECT_THROW(tr.calculate("5/0"), std::runtime_error);
    EXPECT_THROW(tr.calculate("5/(3-3)"), std::runtime_error);
    EXPECT_THROW(tr.calculate("0/0"), std::runtime_error);
}

TEST_F(TranslatorTest, Edge_LongExpression) {
    ExpectNear(tr.calculate("1+2+3+4+5+6+7+8+9+10"), 55.0);
}

TEST_F(TranslatorTest, Edge_DeepNestingUnary) {
    ExpectNear(tr.calculate("-(-(-(-1)))"), 1.0); 
    ExpectNear(tr.calculate("-(-(-(-(-1))))"), -1.0);
}

TEST_F(TranslatorTest, Edge_ManySpacesEverywhere) {
    ExpectNear(tr.calculate("  (  (  2  +  3 )  * (  4 + 5 )  -  6 )  /  ( 1 + 2 )  "), 13.0);
}