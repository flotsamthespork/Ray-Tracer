#ifndef __INTERSECTION_H__
#define __INTERSECTION_H__

#include <vector>
#include "scene.hpp"
#include "ray.hpp"

class Material;
class Primitive;
struct JobData;

enum StrategyType {
	BRUTE_FORCE,
	SUBDIVISION,
	OCTTREE
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
	std::vector<IntersectionData> *m_vec;
	const double m_eps;

	Intersection(JobData *data,
			double eps = 0.0000001);

	bool intersects() const;

	void get_intersections(const Ray *ray, SceneObject *object,
			std::vector<IntersectionData> &intersections);
	IntersectionCache *swap_cache(IntersectionCache *cache);

	void update_intersection(IntersectionData &i);

	IntersectionCache *get_cache()
	{
		return m_cache;
	}

	IntersectionData &get_data();

	// TODO - for caches,
	//	old_cache = swap(new_cache);
	//	DO_STUFF();
	//	swap(old_cache);
};

struct IntersectionStrategyParams {
	IntersectionStrategyParams()
	{
		root = true;
	}
	StrategyType type;
	bool root;
	int max_depth;
	int ideal_size;
	Bounds bounds;
};

class IntersectionStrategy {
protected:
	IntersectionStrategyParams m_params;
private:
	int m_object_count;
public:
	IntersectionStrategy(const IntersectionStrategyParams &params);
	virtual ~IntersectionStrategy();
	virtual IntersectionCache *create_intersection_cache()
	{
		return 0;
	}

	const IntersectionStrategyParams& get_params() const
	{
		return m_params;
	}

	void get_intersection(const Ray *ray, Intersection *intersection);

	virtual void get_intersections(const Ray *ray,
			std::vector<IntersectionData> &intersections,
			Intersection *i) = 0;

	// TODO - Not sure if this is actually necessary...
	// it may be(?)
	virtual bool requires_cache()
	{
		return true;
	}

	void add_object(SceneObject *object);

	virtual void finish() = 0;

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

private:
	std::vector<SceneObject*> m_all_objects;
	Bounds m_bounds;
	int m_nx, m_ny, m_nz;
	double m_xs, m_ys, m_zs;
	IntersectionStrategy ****m_grid;

	void setup_grid();
public:
	SpatialSubdivisionStrategy(const IntersectionStrategyParams &params);
	virtual ~SpatialSubdivisionStrategy();

	virtual IntersectionCache *create_intersection_cache();

	virtual void get_intersections(const Ray *ray,
			std::vector<IntersectionData> &intersections,
			Intersection *i);

	virtual void finish();

protected:
	virtual void do_add_object(SceneObject *object);
};



class BruteForceStrategy : public IntersectionStrategy {
private:
	std::vector<SceneObject*> m_objects;
public:
	BruteForceStrategy(const IntersectionStrategyParams &params);
	virtual void get_intersections(const Ray *ray,
			std::vector<IntersectionData> &intersections,
			Intersection *i);

	virtual void finish();
protected:
	virtual void do_add_object(SceneObject *object);
};

IntersectionStrategy *get_strategy(const IntersectionStrategyParams &params,
		int object_count);

#endif
