
#include <map>

#include "image_cache.hpp"


typedef std::pair<int,Image*> MapImg;
typedef std::map<std::string, MapImg> Map;
typedef Map::iterator MapItr;

static Map images;


Image*
ImageCache::acquire_image(std::string &name)
{
	MapItr it = images.find(name);
	if (it == images.end())
	{
		Image *img = new Image();
		img->loadPng(name);
		if (img->width() == 0 || img->height() == 0)
		{
			delete img;
			return 0;
		}

		MapImg item(1, img);
		images[name] = item;
		return img;
	}
	else
	{
		MapImg *img = &it->second;
		img->first += 1;
		return img->second;
	}
}


void
ImageCache::release_image(Image *img)
{
	bool remove = false;
	MapImg *item;
	MapItr it;
	for (it = images.begin(); it != images.end(); ++it)
	{
		item = &it->second;
		if (item->second == img)
		{
			item->first -= 1;
			if (item->first == 0)
				remove = true;
			break;
		}
	}

	if (remove)
	{
		delete item->second;
		images.erase(it);
	}
}
