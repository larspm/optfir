#ifndef FILTERS_H
#define FILTERS_H

#include <vector>

class FirFilter {
public:
  FirFilter(unsigned int N, double coeffs[] = 0);
  unsigned int getOrder();
  unsigned int getRank();
  virtual double& operator[](unsigned int i);
  void operator=(const double* h);
  double getAmplitudeResponse(double w);
  virtual std::vector<double> getGradient(double w);
protected:
  unsigned int m_order;
  unsigned int m_rank;
  std::vector<double> m_h;
};

class SymmetricFirFilter : public FirFilter {
public:
  SymmetricFirFilter(unsigned int N, double coeffs[] = 0);
  virtual double& operator[](unsigned int i);
  virtual std::vector<double> getGradient(double w);
};

#endif
