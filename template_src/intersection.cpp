
#include <vector>
#include "job.hpp"

#include "intersection.hpp"


IntersectionStrategy *get_strategy(IntersectionStrategyParams &params)
{
	if (params.type == BRUTE_FORCE)
		return new BruteForceStrategy();

	return 0;
}

////////////////////////////////////////////////
////////////////////////////////////////////////
///////////////// Intersection /////////////////
////////////////////////////////////////////////


Intersection::Intersection(JobData *data,
		double eps) :
	m_found(false),
	m_cache(data->cache),
	m_vec(&data->intersections),
	m_eps(eps)
{
	if (m_cache)
		m_cache->increment_cache_version();
	// TODO - possibly increment the cache in here.
}


void
Intersection::get_intersections(const Ray *ray, SceneObject *object,
			std::vector<IntersectionData> &intersections)
{
	if (!m_cache || m_cache->test_id(object->get_id()))
	{
		object->intersection(ray, this, intersections);
	}
}

void
Intersection::update_intersection(IntersectionData &i)
{
	if (i.t > m_eps && (!m_found || i.t < m_intersection.t))
	{
		m_found = true;
		m_intersection = i;
	}
}

IntersectionData&
Intersection::get_data()
{
	return m_intersection;
}

bool
Intersection::intersects() const
{
	return m_found;
}


IntersectionCache*
Intersection::swap_cache(IntersectionCache *cache)
{
	IntersectionCache *tmp = m_cache;
	m_cache = cache;
	return tmp;
}

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
///////////////// Intersection Cache /////////////////
//////////////////////////////////////////////////////

IntersectionCache::IntersectionCache(int cache_size) :
	m_cache_size(cache_size),
	m_cache_version(0)
{
	m_versions = new int[cache_size];
	m_subcaches = new IntersectionCache*[cache_size];

	for (int i = 0; i < cache_size; ++i)
	{
		m_versions[i] = -1;
		m_subcaches[i] = 0;
	}
}

IntersectionCache::~IntersectionCache()
{
	delete[] m_versions;
	for (int i = 0; i < m_cache_size; ++i)
		if (m_subcaches[i])
			delete m_subcaches[i];
}

IntersectionCache*
IntersectionCache::get_subcache(
		const int id,
		const int cache_size)
{
	if (!m_subcaches[id])
		m_subcaches[id] = new IntersectionCache(cache_size);
	return m_subcaches[id];
}

void
IntersectionCache::increment_cache_version()
{
	m_cache_version++;
}

bool
IntersectionCache::test_id(int id)
{
	if (id >= 0 && m_versions[id] < m_cache_version)
	{
		m_versions[id] = m_cache_version;
		return true;
	}
	return false;
}


/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
///////////////// Intersection Strategy /////////////////
/////////////////////////////////////////////////////////


IntersectionStrategy::IntersectionStrategy() :
	m_object_count(0)
{
}

IntersectionStrategy::~IntersectionStrategy()
{
}

void
IntersectionStrategy::get_intersection(const Ray *ray, Intersection *i)
{
	i->m_vec->resize(0);
	get_intersections(ray, *i->m_vec, i);

	std::sort(i->m_vec->begin(), i->m_vec->end());

	if (i->m_vec->size() > 0)
	{
		for (std::vector<IntersectionData>::iterator it = i->m_vec->begin();
				it != i->m_vec->end(); ++it)
		{
			if (it->t > i->m_eps)
			{
				i->update_intersection(*it);
				break;
			}
		}
	}
}

void
IntersectionStrategy::add_object(SceneObject *object)
{
	// TODO - assign the object an ID (?)
	m_object_count++;
	do_add_object(object);
}



///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
///////////////// Spatial Subdivision /////////////////
///////////////////////////////////////////////////////


IntersectionCache*
SpatialSubdivisionStrategy::create_intersection_cache()
{
	return new IntersectionCache(get_object_count());
}


void
SpatialSubdivisionStrategy::get_intersections(const Ray *ray,
		std::vector<IntersectionData> &intersections,
		Intersection *i)
{
	// TODO - use the grid regions to find a nice intersection..
}


void
SpatialSubdivisionStrategy::do_add_object(SceneObject *object)
{
	// TODO - add it to the appropriate regions
}



///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////// Brute Force /////////////////
///////////////////////////////////////////////


void
BruteForceStrategy::get_intersections(const Ray *ray,
		std::vector<IntersectionData> &intersections,
		Intersection *inter)
{
	for (std::vector<SceneObject*>::iterator i = m_objects.begin();
			i != m_objects.end(); ++i)
	{
		inter->get_intersections(ray, *i, intersections);
	}
	// TODO - potentially a way to break out of the loop early
}

void
BruteForceStrategy::do_add_object(SceneObject *object)
{
	m_objects.push_back(object);
}

