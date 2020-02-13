// NICOLAS TEDORI
// TAYLOR SERIES CODE for exp(x)

#include <iostream>
#include <iomanip>
#include <cmath>

using namespace std;

// FUNCTION DECLARATIONS
template <typename numType>
numType factorial(numType fact);
template <typename numType>
numType expTerms(numType x, int termNum);
template <typename numType>
numType eulerSeries(numType x, int minTerms);
void testFunctions(int minTerms);


int main() {
  int minTerms;

  // get user input
  cout << "e^x Taylor series calculation" << endl;
  cout << "Min terms: ";
  cin >> minTerms;

  // set input to 10 if input is too small
  if (minTerms < 10) {
    cout << "Too few terms. Setting minTerms to 10." << endl;
    minTerms = 10;
  }

  float x[3];
  float y[3];
  // values to test for x
  x[0] = 0.3325;
  x[1] = 37.21;
  x[2] = 23.789;

  for (int i = 0; i < 3; i++) {
    y[i] = eulerSeries <float> (x[i], minTerms);
    cout << "For x = " << x[i] << ", y = " << y[i] << endl;
  }
}

// find a Factorial
template <typename numType>
numType factorial(numType fact) {
  // base case
  if (fact == 1) {
    return fact;
  }

  return fact * factorial(fact - 1);
}

// find the value of the taylor series term for e^x
template <typename numType>
numType expTerms(numType x, int termNum) {
  cout << "Calculating term "  << termNum << ": " << "(" << x << "^" << termNum 
    << ")/(" << termNum << "!) = ";

  // base case
  if (termNum < 1) {
    cout << 1 << endl;
    return (numType) 1;
  }

  numType test = (pow(x, (numType) termNum) / (factorial <numType> (termNum)));
  cout << test << endl;

  // term value if termNum >= 1
  return (pow(x, (numType) termNum) / (factorial <numType> (termNum)));
}

// find the taylor series for e^x
template <typename numType>
numType eulerSeries(numType x, int minTerms) {
  if (minTerms < 0) {
    return (numType) 0;
  }
  //cout << "minTerms = " << minTerms << endl;
  return ( expTerms <numType> (x, minTerms) + eulerSeries(x, (minTerms - 1)) );
}

// test the functions
void testFunctions(int minTerms) {
  // factorial test
  int factTest = factorial(minTerms);
  cout << "Factorial result = " << factTest << endl;

  // expTerm test
  float termResult = expTerms <float> ((float) 5, 0);
  cout << "expTerm = " << termResult << endl;
  
  // euler series test
  float series = eulerSeries <float> ((float) 3, (minTerms - 1));
  cout << "e^x = ~" << series << endl;
  cout << "Size of series: " << sizeof(series) * 8  << " bits" << endl;
}