#ifndef FILTERS_H
#define FILTERS_H

#include <vector>

namespace optfir
{
  class FirFilter {
  public:
    FirFilter(unsigned int N, const double coeffs[] = 0);
    unsigned int getOrder() const;
    unsigned int getRank() const;
    virtual double& operator[](unsigned int i);
    const   double& operator[](unsigned int i) const;
    void operator=(const double* h);
    void operator=(const std::vector<double>& h);
    double getAmplitudeResponse(double w) const;
  protected:
    unsigned int m_order;
    unsigned int m_rank;
    std::vector<double> m_h;
  };

  class SymmetricFirFilter : public FirFilter {
  public:
    SymmetricFirFilter(unsigned int N, const double coeffs[] = 0);
    virtual double& operator[](unsigned int i);
  };

  class SymmetricFirFilterForcedDc : public FirFilter {
  public:
    SymmetricFirFilterForcedDc(double dcGain, unsigned int N, const double coeffs[] = 0);
    virtual double& operator[](unsigned int i);
  private:
    const double m_dcGain;
    double m_tempForReturn;
  };
}

#endif
