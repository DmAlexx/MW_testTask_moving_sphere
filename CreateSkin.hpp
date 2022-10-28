#ifndef CREATESKIN_HPP
#define CREATESKIN_HPP

#include "mwTPoint3d.hpp"
#include "mwDiscreteFunction.hpp"
#include "PointsCloud.hpp"			
#include "CreateFile.hpp"
#include <iostream>
#include <fstream>
#include <thread>
#include <vector>

void CreateSkin( const cadcam::mwTPoint3d<double> refPoint, 
				const unsigned long nx, const unsigned long ny, 
				const unsigned long nz, const double sphereRad, 
				mwDiscreteFunction &func, const double deltaT, 
				const double delta, const std::string &skinFileName);

void checkIntersectionBySphere(const unsigned long nx, const unsigned long ny,
				const unsigned long nz, const double sphereRad, double t, double end,
				mwDiscreteFunction& func, PointsCloud& cloudCoord, const double delta);

void checkIntersectionByCylinder(const unsigned long nx, const unsigned long ny,
				const unsigned long nz, const double sphereRad, double t, double end,
				mwDiscreteFunction& func, PointsCloud& cloudCoord, const double delta);

void const CheckTopPointsOfCloud(const unsigned long nx, const unsigned long ny, const unsigned long nz,
				PointsCloud& cloudCoord, CreateFile& result);

void threadsForIntersectionPoints(const unsigned long nx, const unsigned long ny,
				const unsigned long nz, const double sphereRad, mwDiscreteFunction& func,
				const double deltaT, PointsCloud& cloudCoord, const double delta, const int THREADS_COUNT);

#endif /* CREATESKIN_HPP */
