
#include <sys/time.h>
#include <cstdio>
#include "tracer.hpp"
#include "job.hpp"

#include <iostream>
#include "lua_api.hpp"

int main(int nargs, char **args)
{
/*	Image img(1000,500,3);
	RayTracer rt(NULL, &img);
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
*/

	// TODO - parse the command

	std::string filename = "test.lua";

	if (!run_lua(filename))
	{
		std::cerr << "Could not open " << filename << std::endl;
		return 1;
	}
	return 0;
}
