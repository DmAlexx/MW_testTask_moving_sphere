#include "PointsCloud.hpp"



PointsCloud::PointsCloud(const unsigned long nx, const unsigned long ny, const unsigned long nz)
	: mX_length(nx)
	, mY_length(ny)
	, mZ_length(nz)
{
	m_Cloud = new bool** [mX_length];
	for (unsigned long i = 0; i < mX_length; ++i)
	{
		m_Cloud[i] = new bool* [mY_length];
		for (unsigned long j = 0; j < mY_length; ++j)
		{
			m_Cloud[i][j] = new bool[mZ_length];
			std::memset(m_Cloud[i][j], 0, mZ_length);
		}
	}
}

PointsCloud::~PointsCloud()
{
	for (unsigned long i = 0; i < mX_length; ++i)
	{
		for (unsigned long j = 0; j < mY_length; ++j)
		{
			delete[] m_Cloud[i][j];
		}
		delete[] m_Cloud[i];
	}
	delete[] m_Cloud;
}