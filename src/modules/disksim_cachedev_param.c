#include "disksim_cachedev_param.h"
#include <libparam/bitvector.h>
#include "disksim_cachedev.h"
static int
DISKSIM_CACHEDEV_CACHE_BLOCK_SIZE_depend (char *bv)
{
  return -1;
}

static void
DISKSIM_CACHEDEV_CACHE_BLOCK_SIZE_loader (struct cache_dev *result, int i)
{
  if (!(i >= 0))
    {				// foo 
    }
  result->block_size = i;

}

static int
DISKSIM_CACHEDEV_CACHE_SET_ASSOC_depend (char *bv)
{
  return -1;
}

static void
DISKSIM_CACHEDEV_CACHE_SET_ASSOC_loader (struct cache_dev *result, int i)
{
  if (!(i >= 0))
    {				// foo 
    }
  result->assoc = i;

}

static int
DISKSIM_CACHEDEV_CACHE_SIZE_depend (char *bv)
{
  return -1;
}

static void
DISKSIM_CACHEDEV_CACHE_SIZE_loader (struct cache_dev *result, int i)
{
  if (!(i >= 0))
    {				// foo 
    }
  result->size = i;
  result->cache_numbers =
    result->size % result->block_size ==
    0 ? result->size / result->block_size : result->size /
    result->block_size + 1;
  result->caches =
    (struct cacheblock *) DISKSIM_malloc (result->cache_numbers *
					  sizeof (struct cacheblock));
  result->set_numbers =
    result->size % (result->assoc * result->block_size) ==
    0 ? result->size / (result->assoc * result->block_size) : result->size /
    (result->assoc * result->block_size) + 1;
  result->cache_sets =
    (struct cache_set *) DISKSIM_malloc (result->set_numbers *
					 sizeof (struct cache_set));

}

static int
DISKSIM_CACHEDEV_CACHE_POLICY_depend (char *bv)
{
  return -1;
}

static void
DISKSIM_CACHEDEV_CACHE_POLICY_loader (struct cache_dev *result, int i)
{
  if (!(i >= 0))
    {				// foo 
    }
  result->cache_policy = i;

}

static int
DISKSIM_CACHEDEV_MAX_REQUEST_SIZE_depend (char *bv)
{
  return -1;
}

static void
DISKSIM_CACHEDEV_MAX_REQUEST_SIZE_loader (struct cache_dev *result, int i)
{
  if (!(i >= 0))
    {				// foo 
    }
  result->maxreqsize = i;

}

static int
DISKSIM_CACHEDEV_WRITE_SCHEME_depend (char *bv)
{
  return -1;
}

static void
DISKSIM_CACHEDEV_WRITE_SCHEME_loader (struct cache_dev *result, int i)
{
  if (!(RANGE (i, CACHE_WRITE_MIN, CACHE_WRITE_MAX)))
    {				// foo 
    }
  result->writescheme = i;

}

static int
DISKSIM_CACHEDEV_FLUSH_POLICY_depend (char *bv)
{
  return -1;
}

static void
DISKSIM_CACHEDEV_FLUSH_POLICY_loader (struct cache_dev *result, int i)
{
  if (!(RANGE (i, CACHE_FLUSH_MIN, CACHE_FLUSH_MAX)))
    {				// foo 
    }
  result->flush_policy = i;

}

static int
DISKSIM_CACHEDEV_FLUSH_PERIOD_depend (char *bv)
{
  return -1;
}

static void
DISKSIM_CACHEDEV_FLUSH_PERIOD_loader (struct cache_dev *result, double d)
{
  if (!(d >= 0))
    {				// foo 
    }
  result->flush_period = d;

}

static int
DISKSIM_CACHEDEV_FLUSH_IDLE_DELAY_depend (char *bv)
{
  return -1;
}

static void
DISKSIM_CACHEDEV_FLUSH_IDLE_DELAY_loader (struct cache_dev *result, double d)
{
  if (!((d >= 0) || (d == -1.0)))
    {				// foo 
    }
  result->flush_idledelay = d;

}

static int
DISKSIM_CACHEDEV_CACHE_DEVICE_depend (char *bv)
{
  return -1;
}

static void
DISKSIM_CACHEDEV_CACHE_DEVICE_loader (struct cache_dev *result, char *s)
{
  if (!(getdevbyname (s, &result->cache_devno, 0, 0)))
    {				// foo 
    }

}

static int
DISKSIM_CACHEDEV_CACHED_DEVICE_depend (char *bv)
{
  return -1;
}

static void
DISKSIM_CACHEDEV_CACHED_DEVICE_loader (struct cache_dev *result, char *s)
{
  if (!(getdevbyname (s, &result->real_devno, 0, 0)))
    {				// foo 
    }

}

void *DISKSIM_CACHEDEV_loaders[] = {
  (void *) DISKSIM_CACHEDEV_CACHE_BLOCK_SIZE_loader,
  (void *) DISKSIM_CACHEDEV_CACHE_SET_ASSOC_loader,
  (void *) DISKSIM_CACHEDEV_CACHE_SIZE_loader,
  (void *) DISKSIM_CACHEDEV_CACHE_POLICY_loader,
  (void *) DISKSIM_CACHEDEV_MAX_REQUEST_SIZE_loader,
  (void *) DISKSIM_CACHEDEV_WRITE_SCHEME_loader,
  (void *) DISKSIM_CACHEDEV_FLUSH_POLICY_loader,
  (void *) DISKSIM_CACHEDEV_FLUSH_PERIOD_loader,
  (void *) DISKSIM_CACHEDEV_FLUSH_IDLE_DELAY_loader,
  (void *) DISKSIM_CACHEDEV_CACHE_DEVICE_loader,
  (void *) DISKSIM_CACHEDEV_CACHED_DEVICE_loader
};

lp_paramdep_t DISKSIM_CACHEDEV_deps[] = {
  DISKSIM_CACHEDEV_CACHE_BLOCK_SIZE_depend,
  DISKSIM_CACHEDEV_CACHE_SET_ASSOC_depend,
  DISKSIM_CACHEDEV_CACHE_SIZE_depend,
  DISKSIM_CACHEDEV_CACHE_POLICY_depend,
  DISKSIM_CACHEDEV_MAX_REQUEST_SIZE_depend,
  DISKSIM_CACHEDEV_WRITE_SCHEME_depend,
  DISKSIM_CACHEDEV_FLUSH_POLICY_depend,
  DISKSIM_CACHEDEV_FLUSH_PERIOD_depend,
  DISKSIM_CACHEDEV_FLUSH_IDLE_DELAY_depend,
  DISKSIM_CACHEDEV_CACHE_DEVICE_depend,
  DISKSIM_CACHEDEV_CACHED_DEVICE_depend
};
