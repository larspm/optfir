#include <cmath>
#include <vector>
#include <string>
#include <complex>

#include "filters.h"

double signum(double x)
{
  if (x<0) return -1;
  if (x>0) return 1;
  return 0;
}

FirFilter::FirFilter(unsigned int N, double* coeffs)
  : m_order(N)
  , m_rank(N)
  , m_h(m_rank, 0)
{
  if (coeffs != 0)
  {
    for (int i = 0; i < m_rank; i++)
    {
      m_h[i] = coeffs[i];
    }
  }
}

unsigned int FirFilter::getOrder()
{
  return m_order;
}

unsigned int FirFilter::getRank()
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

void FirFilter::operator=(const double* h)
{
  for (int i = 0; i < m_rank; i++)
  {
    m_h[i] = h[i];
  }
}

double FirFilter::getAmplitudeResponse(double w)
{
  std::complex<double> res(0);

  const std::complex<double> i(0.0, 1.0);

  for (int n = 0; n < m_order; n++)
  {
    res += operator[](n) * std::exp(-i * w * (double)(n));
  }

  return std::abs(res);
}

std::vector<double> FirFilter::getGradient(double w)
{
  std::vector<double> grad(m_rank, 0);
  throw std::string("index too large");
  return grad;
}

SymmetricFirFilter::SymmetricFirFilter(unsigned int N, double* coeffs)
  : FirFilter(ceil((double)N/2), coeffs)
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

std::vector<double> SymmetricFirFilter::getGradient(double w)
{
  std::vector<double> grad(m_rank, 0);

  double temp = m_h[m_rank - 1];
  for (int i = 1; i < m_rank; i++)
  {
    temp += 2 * m_h[m_rank - 1 - i] * cos(w * i);
  }
  temp = signum(temp);

  grad[m_rank - 1] = -temp;
  for(int i = 0; i < (m_rank - 1); i++)
  {
    grad[i] = temp * 2 * cos(w * (m_rank - 1 - i));
  }

  return grad;
}
