CacheTable is a C++ template library that implements a very fast hash map container.

Its particularity, compared with [std::hash\_map](http://www.sgi.com/tech/stl/hash_map.html) and [google::sparse\_hash\_map](http://goog-sparsehash.sourceforge.net/) it's that it has a fixed size (you decide it when you create it) and a random item replacement policy:
in case of an 'hash collision' the older item is 'discarded' and replaced with the new one.

This container is ideal for implementing caching system, when you want super fast item insertion and retrieval and you know 'a priori' the memory amount you want to dedicate.
It is also possible to use it in conjunction to a 2nd level cache, passing discarded item to a slower and bigger container.

The interfaces of the containers are compatible with STL, and to get a glimpse you can check out the
[examples](http://cache-table.googlecode.com/svn/trunk/examples/) provided.


## Released CacheTable 0.2 ##

  * Fixed a bug with deletion of iterator range
  * More complete documentation
