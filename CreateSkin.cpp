#include "CreateSkin.hpp"

namespace
{
	//On my laptop the best performance with 4 threads
	//Without exception. You have to know characteristics of your computer
	const int THREADS_COUNT = 4;
}

void CreateSkin(const cadcam::mwTPoint3d<double> refPoint,
				const unsigned long nx, const unsigned long ny,
				const unsigned long nz, const double sphereRad,
				mwDiscreteFunction& func, const double deltaT,
				const double delta, const std::string& skinFileName)
{
	//Creating objects of classes
	PointsCloud cloudCoord(nx, ny, nz);
	CreateFile result(skinFileName);

	//Uncomment line 21, 25-27 to check time of performance threadsForIntersectionPoints function 
	//auto start = std::chrono::high_resolution_clock::now();
	
	threadsForIntersectionPoints(nx, ny, nz, sphereRad, func, deltaT, cloudCoord, delta, THREADS_COUNT);
	
	//auto end = std::chrono::high_resolution_clock::now();
	//std::chrono::duration<float> duration = end - start;
	//std::cout << "Duration threadsForIntersectionPoints " << duration.count() << std::endl;

	//Uncomment line 30, 34-36 to check time of performance CheckTopPointsOfCloud function 
	//auto start1 = std::chrono::high_resolution_clock::now();

	CheckTopPointsOfCloud(nx, ny, nz, cloudCoord, result);
	
	//auto end1 = std::chrono::high_resolution_clock::now();
	//std::chrono::duration<float> duration1 = end1 - start1;
	//std::cout << "Duration CheckTopPointsOfCloud " << duration1.count() << std::endl;
}

//Function that compares points of sphere with points of cloud to check their intersection
void checkIntersectionBySphere(const unsigned long nx, const unsigned long ny,
	const unsigned long nz, const double sphereRad, double t, double end,
	mwDiscreteFunction& func, PointsCloud& cloudCoord, const double delta)
{
	unsigned long counter = 0;
	double sphereRadSqr = sphereRad * sphereRad;
	for (t, end; t <= end; t += delta)
	{
		mwDiscreteFunction::point3d centerOfSphere = func.Evaluate(t);
		double a = centerOfSphere.x();
		double b = centerOfSphere.y();
		double c = centerOfSphere.z();
		counter++;
		for (unsigned long z = floor(c - 5), z_end = ceil(c + 5); z <= z_end; ++z)
		{
			for (unsigned long y = floor(b - 5), y_end = ceil(c + 5); y <= y_end; ++y)
			{
				for (unsigned long x = floor(a - 5), x_end = ceil(c + 5); x <= x_end; ++x)
				{
					if (x >= 0 && x <= nx && y >= 0 && y <= ny && z >= 0 && z <= nz && !cloudCoord.getCoord(x, y, z) &&
						((x - a) * (x - a) + (y - b) * (y - b) + (z - c) * (z - c)) <= sphereRadSqr)
					{
						cloudCoord.setCloud(x, y, z, true);
					}
				}
			}
		}
	}
}

//Function that compares points of cylinder with points of cloud to check their intersection
void checkIntersectionByCylinder(const unsigned long nx, const unsigned long ny, const unsigned long nz, const double sphereRad, double t, double end, mwDiscreteFunction& func, PointsCloud& cloudCoord, const double delta)
{
	double sphereRadSqr = sphereRad * sphereRad;
	for (t, end; t <= end-delta; t += delta)
	{
		mwDiscreteFunction::point3d pointFirst = func.Evaluate(t);
		mwDiscreteFunction::point3d pointNext = func.Evaluate(t + delta);
		mwDiscreteFunction::point3d vectorFirstNext = pointNext - pointFirst;
		mwDiscreteFunction::point3d testPoint;
		mwDiscreteFunction::point3d vectorFirst_testPoint;
		double DiagonalCyl = sqrt(4 * sphereRadSqr + ~vectorFirstNext * ~vectorFirstNext);
		double lengthVectorfirst_testPointProjection = 0;
		
		for (unsigned long z = floor(pointFirst.z() - DiagonalCyl), z_end = ceil(pointFirst.z() + DiagonalCyl); z <= z_end; ++z)
		{
			for (unsigned long y = floor(pointFirst.y() - DiagonalCyl), y_end = ceil(pointFirst.y() + DiagonalCyl); y <= y_end; ++y)
			{
				for (unsigned long x = floor(pointFirst.x() - DiagonalCyl), x_end = ceil(pointFirst.x() + DiagonalCyl); x <= x_end; ++x)
				{
					testPoint.x(x);
					testPoint.y(y);
					testPoint.z(z);
					vectorFirst_testPoint = testPoint - pointFirst;
					lengthVectorfirst_testPointProjection = vectorFirst_testPoint * vectorFirstNext / ~vectorFirstNext;
										
					if (x >= 0 && x <= nx && y >= 0 && y <= ny && z >= 0 && z <= nz && !cloudCoord.getCoord(x, y, z) && lengthVectorfirst_testPointProjection <= ~vectorFirstNext
						&& ~vectorFirst_testPoint * ~vectorFirst_testPoint - lengthVectorfirst_testPointProjection * lengthVectorfirst_testPointProjection <= sphereRadSqr)
					{
						cloudCoord.setCloud(x, y, z, true);
					}
				}
			}
		}
	}
}

//Function that checks undeleted top rows of point
void const CheckTopPointsOfCloud(const unsigned long nx, const unsigned long ny, const unsigned long nz, 
								 PointsCloud& cloudCoord, CreateFile& result)
{
	char buffer[64];
	
	for (unsigned long x = 0; x < nx; ++x)
	{
		for (unsigned long y = 0; y < ny; ++y)
		{
			for (unsigned long z = nz - 1; z >= 0; --z)
			{
				if (!cloudCoord.getCoord(x, y, z))
				{
					sprintf(buffer, "%d %d %d\n", x, y, z);
					result.writeToFile(buffer);
					break;
				}
			}
		}
	}
}

//Function that creats threads to check intersection mooving sphere
void threadsForIntersectionPoints(const unsigned long nx, const unsigned long ny,
					const unsigned long nz, const double sphereRad, mwDiscreteFunction& func, 
					const double deltaT, PointsCloud& cloudCoord, const double delta, const int THREADS_COUNT)
{
	double count_delta = deltaT / delta;
	double t = func.GetBeginParameter();
	double midBegin = 0.;
	double midEnd = 0.;
	double end = func.GetEndParameter();
	
	std::vector<std::thread> threads;
	threads.reserve(THREADS_COUNT);

	for (int i = 0; i < THREADS_COUNT; ++i) 
	{
		if (i == 0 && THREADS_COUNT != 1)
		{
			midEnd = func.GetEndParameter() / THREADS_COUNT;
			threads.emplace_back([nx, ny, nz, sphereRad, t, midEnd, &func, &cloudCoord, count_delta]()
			{
					checkIntersectionByCylinder(nx, ny, nz, sphereRad, t, midEnd, func, cloudCoord, count_delta);
					checkIntersectionBySphere(nx, ny, nz, sphereRad, t, midEnd, func, cloudCoord, count_delta);
				
			});
		}
		if (i == THREADS_COUNT - 1)
		{
			midBegin = i * func.GetEndParameter() / THREADS_COUNT;
			threads.emplace_back([nx, ny, nz, sphereRad, midBegin, end, &func, &cloudCoord, count_delta]()
			{
					checkIntersectionByCylinder(nx, ny, nz, sphereRad, midBegin, end, func, cloudCoord, count_delta);
					checkIntersectionBySphere(nx, ny, nz, sphereRad, midBegin, end, func, cloudCoord, count_delta);
			});
		}
		if (i != THREADS_COUNT - 1 && i != 0)
		{
			midBegin = i * func.GetEndParameter() / THREADS_COUNT;
			midEnd = (i + 1) * func.GetEndParameter() / THREADS_COUNT;
			threads.emplace_back([nx, ny, nz, sphereRad, midBegin, midEnd, &func, &cloudCoord, count_delta]()
			{
					checkIntersectionByCylinder(nx, ny, nz, sphereRad, midBegin, midEnd, func, cloudCoord, count_delta);
					checkIntersectionBySphere(nx, ny, nz, sphereRad, midBegin, midEnd, func, cloudCoord, count_delta);
			});
		}
	}
	
	for (auto& thread : threads) 
	{
		thread.join();
	}	
}