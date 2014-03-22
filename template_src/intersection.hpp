#ifndef __INTERSECTION_H__
#define __INTERSECTION_H__

#include "scene.hpp"
#include "ray.hpp"

class Material;
class Primitive;

enum StrategyType {
	BRUTE_FORCE,
	REGULAR_GRID
};


class IntersectionCache {
private:
	const int m_cache_size;
	int m_cache_version;
	int *m_versions;
	IntersectionCache **m_subcaches;

public:
	IntersectionCache(int cache_size);
	virtual ~IntersectionCache();

	IntersectionCache *get_subcache(
			const int id,
			const int cache_size);

	void increment_cache_version();

	bool test_id(int id);
};


class Intersection {
private:
	// TODO - woof woof woof
	// TODO - all the necessary variables.

	// TODO - UV coordinates, which cache the first time they
	//	  are calculated
	bool m_found;

	IntersectionData m_intersection;

	IntersectionCache *m_cache;
public:
	Intersection(IntersectionCache *cache);

	bool intersects() const;

	void get_intersection(const Ray *ray, SceneObject *object);
	IntersectionCache *swap_cache(IntersectionCache *cache);

	void update_intersection(IntersectionData &i);

	// TODO - for caches,
	//	old_cache = swap(new_cache);
	//	DO_STUFF();
	//	swap(old_cache);
};

struct IntersectionStrategyParams {
	StrategyType type;
};

class IntersectionStrategy {
private:
	int m_object_count;
public:
	IntersectionStrategy();
	virtual ~IntersectionStrategy();
	virtual IntersectionCache *create_intersection_cache()
	{
		return 0;
	}

	virtual void get_intersection(const Ray *ray, Intersection *intersection) = 0;

	// TODO - Not sure if this is actually necessary...
	// it may be(?)
	virtual bool requires_cache()
	{
		return true;
	}

	void add_object(SceneObject *object);

	int get_object_count()
	{
		return m_object_count;
	}

protected:
	virtual void do_add_object(SceneObject *object) = 0;
};


class SpatialSubdivisionStrategy : public IntersectionStrategy {
	// TODO - For "oct-tree" implementation we have one cache
	//	- which is shared amongst all the regions
public:
	virtual IntersectionCache *create_intersection_cache();

	virtual void get_intersection(const Ray *ray, Intersection *intersection);

protected:
	virtual void do_add_object(SceneObject *object);
};



class BruteForceStrategy : public IntersectionStrategy {
private:
	std::vector<SceneObject*> m_objects;
public:
	virtual void get_intersection(const Ray *ray, Intersection *intersection);
protected:
	virtual void do_add_object(SceneObject *object);
};

IntersectionStrategy *get_strategy(IntersectionStrategyParams &params);

#endif
