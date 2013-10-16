#include "vector_tile_datasource.hpp"
#include "vector_tile_compression.hpp"
#include "vector_tile_projection.hpp"

#include <stdio.h>

#include <mapnik/box2d.hpp>

#include <Python.h>
#include <boost/python.hpp>

namespace mapnik { namespace vector {

typedef boost::shared_ptr<tile_datasource> tile_datasource_ptr;

class vector_tile 
{
public:
    vector_tile(int x, int y, int z, int tilesize);
    int layers_size();
    tile_datasource_ptr layer_datasource(int layer_index);
    std::string layer_name(int layer_index);
    void parse_from_buffer(std::string const& buffer);
    mapnik::box2d<double> get_bbox();
private:
    tile tile_;
    int x_, y_, z_;
    int tilesize_;
    double minx_, miny_, maxx_, maxy_;
    mapnik::box2d<double> bbox_;
    spherical_mercator merc_;
};

inline vector_tile::vector_tile(int x, int y, int z, int tilesize)
: x_(x),
  y_(y),
  z_(z),
  tilesize_(tilesize),
  merc_(tilesize) {
    merc_.xyz(x_, y_, z_, minx_, miny_, maxx_, maxy_);
    bbox_.init(minx_, miny_, maxx_, maxy_);
}

inline void vector_tile::parse_from_buffer(std::string const& buffer) {
    tile_.ParseFromString(buffer);
}

inline int vector_tile::layers_size() {
    return tile_.layers_size();
}

inline tile_datasource_ptr vector_tile::layer_datasource(int layer_index) {
    tile_layer const& layer = tile_.layers(layer_index);
    tile_datasource_ptr ds = boost::make_shared<tile_datasource>(layer, x_, y_, z_, tilesize_);
    ds->set_envelope(bbox_);
    return ds;
}

inline std::string vector_tile::layer_name(int layer_index) {
    tile_layer const& layer = tile_.layers(layer_index);
    return layer.name();
}

inline mapnik::box2d<double> vector_tile::get_bbox() {
    return bbox_;
}
    
using namespace boost::python;

BOOST_PYTHON_MODULE(mapnik_vector_tiles)
{
    
    class_<vector_tile>("VectorTile", init<int, int, int, int>(
                        ( arg("x"), arg("y"), arg("z"), arg("tilesize") ),
                        "Enables the creation a datasource or set of datasources\n"
                        "from a mapnik vector tile protobuffer.\n"
                        "\n"
                        "Usage:\n"
                        ">>> from mapnik_vector_tile import VectorTile\n"
                        ">>> mvt = VectorTile(0,0,0, 4096)\n"))
        .def("parse_from_buffer", &vector_tile::parse_from_buffer,
             (arg("buffer")), 
             "Load a protobuffer as an uncompressed buffer\n"
             "string into the VectorTile object.\n"
             "\n"
             "Usage:\n"
             ">>> mvt.parse_from_buffer(buffer)\n")
        .def("layers_size", &vector_tile::layers_size,
             "Determine how many layers are in the VectorTile.\n"
             "\n"
             "Usage:\n"
             ">>> mvt.layer_size()\n"
             "1")
        .def("layer_datasource", &vector_tile::layer_datasource,
             (arg("layer_index")),
             "Retrieve a datasource for a specific layer index.\n",
             "\n"
             "Usage:\n"
             ">>> mvt.layer_datasource(0)\n")
        .def("layer_name", &vector_tile::layer_name,
             (arg("layer_index")),
             "Retrieve the name for a layer by its index.\n",
             "\n"
             "Usage:\n"
             ">>> mvt.layer_name(0)\n")
        .def("get_bbox", &vector_tile::get_bbox,
             "Retrieve the bounding box used by the vector tile.")
        ;
 
    class_<tile_datasource,
        bases<datasource>, boost::shared_ptr<tile_datasource>,
        boost::noncopyable>("TileDatasource", no_init);
    implicitly_convertible<boost::shared_ptr<tile_datasource>,boost::shared_ptr<datasource> >();
}

}} //end ns
