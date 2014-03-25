#ifndef __IMAGE_CACHE_H__
#define __IMAGE_CACHE_H__

#include "image.hpp"

class ImageCache {
public:
	static Image *acquire_image(std::string &name);
	static void release_image(Image *img);
};

#endif
