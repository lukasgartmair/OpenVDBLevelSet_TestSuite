#include "../openvdb/openvdb/openvdb.h"
#include "../openvdb/openvdb/math/Maps.h" 
#include "../openvdb/openvdb/math/Maps.cc"
#include "../openvdb/openvdb/MetaMap.h" 
#include "../openvdb/openvdb/MetaMap.cc"
#include "../openvdb/openvdb/Grid.h" 
#include "../openvdb/openvdb/tools/MeshToVolume.h" 
#include "../openvdb/openvdb/tools/VolumeToMesh.h" 

#include "../openvdb/openvdb/tools/Composite.h"



// compile sudo g++ -std=c++11 -I/usr/include/openvdb  TestRunner.cpp -lcppunit -lopenvdb -ltbb -lHalf -o RunTests
// sudo g++ -std=c++11 level_set_test3.cpp -I/home/lukas/openvdb/openvdb -Wl,-rpath,/home/lukas/openvdb/openvdb

/*
nearly:

sudo g++ -std=c++11 level_set_test3.cpp /home/lukas/openvdb/openvdb/Metadata.cc -I/home/lukas/openvdb/openvdb  -ltbb -Wl,-rpath,/home/lukas/openvdb/openvdb -L/home/lukas/openvdb/openvdb -lopenvdb (without so it finds the file)

sudo g++ -std=c++11 level_set_test3.cpp /home/lukas/openvdb/openvdb/Metadata.cc -I/home/lukas/openvdb/openvdb  -ltbb -Wl,-rpath,/home/lukas/openvdb/openvdb -L/home/lukas/openvdb/openvdb /home/lukas/openvdb/openvdb/libopenvdb.so.4.0.1

works:
sudo g++ -std=c++11 level_set_test3.cpp /home/lukas/openvdb/openvdb/Metadata.cc -I/home/lukas/openvdb/openvdb  -ltbb -lHalf -Wl,-rpath,/home/lukas/openvdb/openvdb -L/home/lukas/openvdb/openvdb /home/lukas/openvdb/openvdb/libopenvdb.so.4.0.1 -lHalf -lboost_iostreams

*/

int main()
{

	openvdb::initialize();

	// Create a VDB file object.
	openvdb::io::File file("utahteapot.vdb");
	// Open the file.  This reads the file header, but not any grids.
	file.open();
	// Loop over all grids in the file and retrieve a shared pointer
	// to the one named "LevelSetSphere".  (This can also be done
	// more simply by calling file.readGrid("LevelSetSphere").)
	openvdb::GridBase::Ptr basegrid;
	for (openvdb::io::File::NameIterator nameIter = file.beginName();
		nameIter != file.endName(); ++nameIter)
		{
		// Read in only the grid we are interested in.
		if (nameIter.gridName() == "ls_utahteapot") {
			basegrid = file.readGrid(nameIter.gridName());
		} else {
			std::cout << "skipping grid " << nameIter.gridName() << std::endl;
		}
	}
	file.close();

	openvdb::FloatGrid::Ptr grid = openvdb::gridPtrCast<openvdb::FloatGrid>(basegrid);

	float minVal = 0.0;
	float maxVal = 0.0;

	double isovalue = 0.1;
	double adaptivity = 0;

	std::vector<openvdb::Vec3s> points;
	std::vector<openvdb::Vec3I> triangles;
	std::vector<openvdb::Vec4I> quads;
	// change the grid here to be extracted
	openvdb::tools::volumeToMesh<openvdb::FloatGrid>(*grid, points, triangles, quads, isovalue, adaptivity);

	std::cout << points.size() << std::endl;

	const float bandwidth = 30;
	const float in_bandwidth = 10;
	const float ex_bandwidth = 10;

	const double voxelsize = 1.0;

	openvdb::math::Transform::Ptr transform = openvdb::math::Transform::createLinearTransform(voxelsize);
	openvdb::FloatGrid::Ptr sdf = openvdb::tools::meshToSignedDistanceField<openvdb::FloatGrid>(*transform, points, triangles, quads, ex_bandwidth, in_bandwidth);

	//openvdb::FloatGrid::Ptr cutter = openvdb::tools::MeshToVolume::meshToLevelSet<openvdb::FloatGrid>(*transform, points, triangles, quads, ex_bandwidth, in_bandwidth);
	//cutter->setTransform(openvdb::math::Transform::createLinearTransform(voxelsize));

/*


	openvdb::tools::LevelSetFracture::fracture(sdf,cutter); 	



	sdf->evalMinMax(minVal,maxVal);
	std::cout << " eval min max grid" << " = " << minVal << " , " << maxVal << std::endl;
	std::cout << " active voxel count" << " = " << sdf->activeVoxelCount() << std::endl;

	openvdb::io::File file_out("sdf.vdb");
	openvdb::GridPtrVec grids_out;
	grids_out.push_back(sdf);

	file_out.write(grids_out);
	file_out.close();
*/

}

