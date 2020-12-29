#include "pch.h"
#include "DoubleHelper.h"
#include <numeric>

bool DoubleHelper::Compare(const double& left, const double& right)
{
	return fabs(left - right) < std::numeric_limits<double>::epsilon();
}
