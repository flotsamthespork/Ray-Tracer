
#include <cassert>
#include <vector>
#include <float.h>
#include "job.hpp"
#include "bounds.hpp"

#include "intersection.hpp"


IntersectionStrategy *get_strategy(const IntersectionStrategyParams &params)
{
	if (params.type == BRUTE_FORCE)
		return new BruteForceStrategy(params);
	if (params.type == SUBDIVISION)
	{
		return new SpatialSubdivisionStrategy(params);
	}

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
	if (id < 0)
		return true;
	if (m_versions[id] < m_cache_version)
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


IntersectionStrategy::IntersectionStrategy(
		const IntersectionStrategyParams &params) :
	m_params(params),
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


SpatialSubdivisionStrategy::SpatialSubdivisionStrategy(
		const IntersectionStrategyParams &params) :
	IntersectionStrategy(params),
	m_nx(1), m_ny(1), m_nz(1),
	m_grid(0)
{
}

SpatialSubdivisionStrategy::~SpatialSubdivisionStrategy()
{
	if (m_grid)
	{
		for (int x = 0; x < m_nx; ++x)
		{
			for (int y = 0; y < m_ny; ++y)
			{
				for (int z = 0; z < m_nz; ++z)
				{
					if (m_grid[x][y][z])
						delete m_grid[x][y][z];
				}
				delete[] m_grid[x][y];
			}
			delete[] m_grid[x];
		}
		delete[] m_grid;
	}
}

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
	double tx, ty, tz;
	const Point3D &ray_pos = ray->get_pos();
	const Vector3D &ray_dir = ray->get_dir();

	const int stepx = (ray_dir[0] < 0 ? -1 : 1);
	const int stepy = (ray_dir[1] < 0 ? -1 : 1);
	const int stepz = (ray_dir[2] < 0 ? -1 : 1);

	const double left =	m_bounds.get_left();
	const double bottom =	m_bounds.get_bottom();
	const double front =	m_bounds.get_front();

	int ix = (int)(m_nx*((ray_pos[0]-left)/m_xs));
	int iy = (int)(m_ny*((ray_pos[1]-bottom)/m_ys));
	int iz = (int)(m_nz*((ray_pos[2]-front)/m_zs));

	if (ix < 0 || ix >= m_nx ||
			iy < 0 || iy >= m_ny ||
			iz < 0 || iz >= m_nz)
	{
		const double right =	m_bounds.get_right();
		const double top =	m_bounds.get_top();
		const double back =	m_bounds.get_back();

		std::vector<double> t_values;

		t_values.push_back((left-ray_pos[0])/ray_dir[0]);
		t_values.push_back((right-ray_pos[0])/ray_dir[0]);
		t_values.push_back((top-ray_pos[1])/ray_dir[1]);
		t_values.push_back((bottom-ray_pos[1])/ray_dir[1]);
		t_values.push_back((front-ray_pos[2])/ray_dir[2]);
		t_values.push_back((back-ray_pos[2])/ray_dir[2]);
//		t_values.push_back((left-ray_pos[0])/ray_dir[0]);
//		t_values.push_back((right-ray_pos[0])/ray_dir[0]);
//		t_values.push_back((top-ray_pos[1])/ray_dir[1]);
//		t_values.push_back((bottom-ray_pos[1])/ray_dir[1]);
//		t_values.push_back((front-ray_pos[2])/ray_dir[2]);
//		t_values.push_back((back-ray_pos[2])/ray_dir[2]);

		std::sort(t_values.begin(), t_values.end());

		bool success = false;
		for (int i = 0; i < t_values.size(); ++i)
		{
			double t = t_values[i];
			if (t_values[i] < 0)
				continue;
			Point3D pt = ray_pos + (t+0.0000001)*ray_dir;
			ix = (int)(m_nx*((pt[0]-left)/m_xs));
			iy = (int)(m_ny*((pt[1]-bottom)/m_ys));
			iz = (int)(m_nz*((pt[2]-front)/m_zs));
			if (ix >= 0 && ix < m_nx &&
					iy >= 0 && iy < m_ny &&
					iz >= 0 && iz < m_nz)
			{
				success = true;
				break;
			}
		}

		if (!success)
			return;
		// TODO - find new initial ix, iy, iz
	}

	while (true)
	{
		const int nix = ix+stepx;
		const int niy = iy+stepy;
		const int niz = iz+stepz;

		const bool out_x = nix < 0 || nix >= m_nx;
		const bool out_y = niy < 0 || niy >= m_ny;
		const bool out_z = niz < 0 || niz >= m_nz;

		if (out_x) tx = DBL_MAX;
		else if (stepx < 0)	tx = -(ray_pos[0]-(left+(m_xs*ix)/m_nx))/ray_dir[0];
		else			tx = -(ray_pos[0]-(left+(m_xs*nix)/m_nx))/ray_dir[0];
		if (out_y) ty = DBL_MAX;
		else if (stepy < 0)	ty = -(ray_pos[1]-(bottom+(m_ys*iy)/m_ny))/ray_dir[1];
		else			ty = -(ray_pos[1]-(bottom+(m_ys*niy)/m_ny))/ray_dir[1];
		if (out_z) tz = DBL_MAX;
		else if (stepz < 0)	tz = -(ray_pos[2]-(front+(m_zs*iz)/m_nz))/ray_dir[2];
		else			tz = -(ray_pos[2]-(front+(m_zs*niz)/m_nz))/ray_dir[2];

		IntersectionStrategy *is = m_grid[ix][iy][iz];
		if (is)
		{
			is->get_intersections(ray, intersections, i);
		}

		if (tx <=ty && tx <= tz)
		{
			if (out_x)
				break;
			ix = nix;
		}
		else if (ty <= tz)
		{
			if (out_y)
				break;
			iy = niy;
		}
		else
		{
			if (out_z)
				break;
			iz = niz;
		}

	}
}

void
SpatialSubdivisionStrategy::finish()
{
	std::vector<Bounds> bounds;

	for (int i = 0; i < m_all_objects.size(); ++i)
	{
		Bounds b;
		m_all_objects[i]->get_bounds(b);
		bounds.push_back(b);
		m_bounds.merge(b);
	}

	if (m_bounds.get_type() == UNSET)
		return;
	assert(m_bounds.get_type() == BOX);

	setup_grid();
	
	for (int i = 0; i < bounds.size(); ++i)
	{
		if (bounds[i].get_type() == UNSET)
			continue;

		const double left =	(bounds[i].get_left() - m_bounds.get_left()) / m_xs;
		const double right =	(bounds[i].get_right() - m_bounds.get_left()) / m_xs;
		const double top =	(bounds[i].get_top() - m_bounds.get_bottom()) / m_ys;
		const double bottom =	(bounds[i].get_bottom() - m_bounds.get_bottom()) / m_ys;
		const double front =	(bounds[i].get_front() - m_bounds.get_front()) / m_zs;
		const double back =	(bounds[i].get_back() - m_bounds.get_front()) / m_zs;

		const int xi = std::max(0,	(int)floor(left*m_nx));
		const int xf = std::min(std::max(xi+1,	(int)ceil(right*m_nx)),m_nx);
		const int yi = std::max(0,	(int)floor(bottom*m_ny));
		const int yf = std::min(std::max(yi+1,	(int)ceil(top*m_ny)), m_ny);
		const int zi = std::max(0,	(int)floor(front*m_nz));
		const int zf = std::min(std::max(zi+1,	(int)ceil(back*m_nz)), m_nz);

		for (int x = xi; x < xf; ++x)
			for (int y = yi; y < yf; ++y)
				for (int z = zi; z < zf; ++z)
				{
					if (!m_grid[x][y][z])
						m_grid[x][y][z] = new BruteForceStrategy(m_params);

					m_grid[x][y][z]->add_object(m_all_objects[i]);
				}
	}

	for (int x = 0; x < m_nx; ++x)
		for (int y = 0; y < m_ny; ++y)
			for (int z = 0; z < m_nz; ++z)
				if (m_grid[x][y][z])
					m_grid[x][y][z]->finish();


	// TODO - check that bounds[i].m_type != UNSET
	//	* if it is unset we don't even have to add it!
}

void
SpatialSubdivisionStrategy::setup_grid()
{
	const double left =	m_bounds.get_left();
	const double right =	m_bounds.get_right();
	const double top =	m_bounds.get_top();
	const double bottom =	m_bounds.get_bottom();
	const double front =	m_bounds.get_front();
	const double back =	m_bounds.get_back();

	double xs = m_xs = right - left;
	double ys = m_ys = top - bottom;
	double zs = m_zs = back - front;

	if (m_xs == 0) xs = m_xs = 0.1;
	if (m_ys == 0) ys = m_ys = 0.1;
	if (m_zs == 0) zs = m_zs = 0.1;

	// TODO - find some optimal value for this... (for the cow a multiple speeds it up by up to 4x)
	const int desired_divisions = (int) ceil(sqrt((double)m_all_objects.size()));
//	const int desired_divisions = m_all_objects.size()*100;

	while (m_nx*m_ny*m_nz < desired_divisions)
	{
		if (xs >= ys && xs >= zs)
		{
			m_nx += 1;
			xs = m_xs/m_nx;
		}
		else if (zs >= ys)
		{
			m_nz += 1;
			zs = m_zs/m_nz;
		}
		else
		{
			m_ny += 1;
			ys = m_ys/m_ny;
		}
	}

	m_grid = new IntersectionStrategy***[m_nx];
	for (int x = 0; x < m_nx; ++x)
	{
		m_grid[x] = new IntersectionStrategy**[m_ny];
		for (int y = 0; y < m_ny; ++y)
		{
			m_grid[x][y] = new IntersectionStrategy*[m_nz];
			for (int z = 0; z < m_nz; ++z)
			{
				m_grid[x][y][z] = 0;
			}
		}
	}
}

void
SpatialSubdivisionStrategy::do_add_object(SceneObject *object)
{
	m_all_objects.push_back(object);
}



///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////// Brute Force /////////////////
///////////////////////////////////////////////

BruteForceStrategy::BruteForceStrategy(
		const IntersectionStrategyParams &params) :
	IntersectionStrategy(params)
{
}

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
BruteForceStrategy::finish()
{
}

void
BruteForceStrategy::do_add_object(SceneObject *object)
{
	m_objects.push_back(object);
}

