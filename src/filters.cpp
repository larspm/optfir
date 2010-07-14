#include <cmath>
#include <vector>
#include <string>
#include <complex>
#include <numeric>

#include "filters.h"

namespace optfir
{
  double signum(double x)
  {
    if (x<0) return -1;
    if (x>0) return 1;
    return 0;
  }

  FirFilter::FirFilter(unsigned int N, const double* coeffs)
    : m_order(N)
    , m_rank(N)
    , m_h(m_rank, 0)
  {
    if (coeffs != 0)
    {
      for (unsigned int i = 0; i < m_rank; i++)
      {
        m_h[i] = coeffs[i];
      }
    }
  }

  unsigned int FirFilter::getOrder() const
  {
    return m_order;
  }

  unsigned int FirFilter::getRank() const
  {
    return m_rank;
  }

  double& FirFilter::operator[](unsigned int i)
  {
    if (i >= m_order)
    {
      throw std::string("index too large");
    }

    return m_h[i];
  }

  const double& FirFilter::operator[](unsigned int i) const
  {
    // Cast to mutable so we get the correct operator,
    // which is ok since we return const reference.
    return const_cast<FirFilter*>(this)->operator[](i);
  }

  void FirFilter::operator=(const double* h)
  {
    for (unsigned int i = 0; i < m_rank; i++)
    {
      m_h[i] = h[i];
    }
  }

  void FirFilter::operator=(const std::vector<double>& h)
  {
    if (m_h.size() != h.size())
    {
      throw std::string("size mismatch");
    }
    m_h = h;
  }

  double FirFilter::getAmplitudeResponse(double w) const
  {
    std::complex<double> res(0);

    const std::complex<double> i(0.0, 1.0);

    for (unsigned int n = 0; n < m_order; n++)
    {
      res += operator[](n) * std::exp(-i * w * (double)(n));
    }

    return std::abs(res);
  }

  SymmetricFirFilter::SymmetricFirFilter(unsigned int N, const double* coeffs)
    : FirFilter((unsigned int)ceil((double)N/2), coeffs)
  {
    m_order = N;
  }

  double& SymmetricFirFilter::operator[](unsigned int i)
  {
    if (i >= m_order)
    {
      throw std::string("index too large");
    }

    if (i < m_rank)
    {
      return m_h[i];
    }
    else
    {
      return m_h[m_order - i - 1];
    }
  }

  SymmetricFirFilterForcedDc::SymmetricFirFilterForcedDc(double dcGain, unsigned int N, const double* coeffs)
    : FirFilter((unsigned int)floor((double)N/2), coeffs)
    , m_dcGain(dcGain)
  {
    if ( (N & 0x1) == 0)
    {
      throw std::string("SymmetricFirFilterForcedDc not implemented for N even");
    }
    m_order = N;
  }

  double& SymmetricFirFilterForcedDc::operator[](unsigned int i)
  {
    if (i >= m_order)
    {
      throw std::string("index too large");
    }

    if (i < m_rank)
    {
      return m_h[i];
    }
    else if (i == m_rank)
    {
      m_tempForReturn = m_dcGain - 2 * std::accumulate(m_h.begin(), m_h.end(), 0.0);
      return m_tempForReturn;
    }
    else
    {
      return m_h[m_order - i - 1];
    }
  }
}
