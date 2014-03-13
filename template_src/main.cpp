
#include <sys/time.h>
#include <cstdio>
#include "tracer.hpp"
#include "job.hpp"

int main(int nargs, char **args)
{
	Image img(1000,500,3);
	RayTracer rt(&img);
	for (int i = 1; i < 20; ++i)
	{
		JobFactory jf(&rt, i);
		struct timeval t1, t2;
		gettimeofday(&t1, NULL);
		jf.run();
		gettimeofday(&t2, NULL);
		printf("%d, Time: %d\n", i, (t2.tv_sec-t1.tv_sec)*1000000 +
				(t2.tv_usec-t1.tv_usec));
	}
	return 0;
}
