#pragma once
#include <iostream>

//Class that creat a 3D Euclidian point cloud 
class PointsCloud
{
private:
	bool*** m_Cloud = nullptr;
	unsigned long mX_length;
	unsigned long mY_length;
	unsigned long mZ_length;
				
public:
	PointsCloud(const unsigned long nx, const unsigned long ny, const unsigned long nz);
	~PointsCloud();
	inline bool getCoord(unsigned long x, unsigned long y, unsigned long z)
	{
		return m_Cloud[x][y][z];
	}

	inline void setCloud(unsigned long x_coord, unsigned long y_coord, unsigned long z_coord, bool state)
	{
		m_Cloud[x_coord][y_coord][z_coord] = state;
	}
};

