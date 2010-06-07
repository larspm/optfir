#ifndef OPTEXCEPT_H
#define OPTEXCEPT_H

#include <string>
#include <stdexcept>

class OptfirException : public std::runtime_error
{
public:
  OptfirException(const std::string& what) : std::runtime_error(what) {}
};

#endif
