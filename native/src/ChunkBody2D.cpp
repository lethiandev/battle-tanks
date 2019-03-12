#include <cmath>
#include <thread>
#include <VisualServer.hpp>
#include <Physics2DServer.hpp>
#include <SceneTree.hpp>
#include <CollisionPolygon2D.hpp>
#include "ChunkBody2D.hpp"

#ifdef _DEBUG
#define PRINT_DEBUG(...) godot::Godot::print(__VA_ARGS__);
#else
#define PRINT_DEBUG(...)
#endif

ChunkBody2D::ChunkBody2D()
	: _clipper(ClipperLib::ioStrictlySimple)
{
}

void ChunkBody2D::_init()
{
	_mesh = godot::VisualServer::get_singleton()->mesh_create();

#ifdef _DEBUG
	_debug_mesh = godot::VisualServer::get_singleton()->mesh_create();
#endif
}

void ChunkBody2D::_ready()
{
	PRINT_DEBUG("{0} called by {1}", __FUNCTION__, get_name());

	for (int i = 0, len = get_child_count(); i < len; ++i)
	{
		auto node = get_child(i);

		if (auto poly2d = godot::Object::cast_to<godot::CollisionPolygon2D>(node))
		{
			auto polygon = poly2d->get_polygon();
			auto xform = poly2d->get_transform();

			add_path(polygon, xform);

			_draw_cache.add_canvas_item(poly2d->get_canvas_item());
		}
	}

	auto bounds = _clipper.GetBounds();

	_bounds = godot::Rect2 {
		static_cast<decltype(godot::Vector2::x)>(bounds.left),
		static_cast<decltype(godot::Vector2::y)>(bounds.top),
		static_cast<decltype(godot::Vector2::x)>(bounds.right - bounds.left),
		static_cast<decltype(godot::Vector2::y)>(bounds.bottom - bounds.top),
	};

	_draw_cache.set_transform(godot::Transform2D().translated(-_bounds.position));
	_draw_cache.set_size(_bounds.size);
	_draw_cache.request_update();

	godot::VisualServer::get_singleton()->request_frame_drawn_callback(this, "_frame_drawn", godot::Variant());

	rebuild();
}

void ChunkBody2D::_draw() const noexcept
{
	PRINT_DEBUG("{0} called by {1}", __FUNCTION__, get_name());

	godot::VisualServer *const vs = godot::VisualServer::get_singleton();

	godot::RID item = get_canvas_item();

	const godot::RID &cachetex = _draw_cache.get_texture();
	vs->canvas_item_add_mesh(item, _mesh, cachetex);

#ifdef _DEBUG
	if (get_tree()->is_debugging_collisions_hint())
	{
		vs->canvas_item_add_mesh(item, _debug_mesh, godot::RID());

		const godot::Vector2 p1 = _bounds.position;
		const godot::Vector2 p2 = _bounds.position + _bounds.size;
		const godot::Color bounds_color { 0.75f, 0.0f, 0.6f, 0.75f };

		vs->canvas_item_add_line(item, godot::Vector2(p1.x, p1.y), godot::Vector2(p2.x, p1.y), bounds_color);
		vs->canvas_item_add_line(item, godot::Vector2(p2.x, p1.y), godot::Vector2(p2.x, p2.y), bounds_color);
		vs->canvas_item_add_line(item, godot::Vector2(p2.x, p2.y), godot::Vector2(p1.x, p2.y), bounds_color);
		vs->canvas_item_add_line(item, godot::Vector2(p1.x, p2.y), godot::Vector2(p1.x, p1.y), bounds_color);
	}
#endif
}

void ChunkBody2D::_notification(const int what) noexcept
{
	if (what == godot::Object::NOTIFICATION_PREDELETE)
	{
		_free();
	}
}

void ChunkBody2D::_frame_drawn(godot::Variant userdata)
{
	for (int i = 0, len = get_child_count(); i < len; ++i)
	{
		if (godot::Object::cast_to<godot::CollisionPolygon2D>(get_child(i)))
		{
			get_child(i)->queue_free();
		}
	}
}

void ChunkBody2D::add_path(const godot::PoolVector2Array points, const godot::Transform2D xform)
{
	ClipperLib::Path path;
	path.reserve(points.size());

	auto poolread = points.read();
	auto poolptr = poolread.ptr();

	for (int i = 0, len = points.size(); i < len; ++i, ++poolptr)
	{
		const auto point = xform.xform(*poolptr);
		path.emplace_back(ClipperLib::IntPoint {
			static_cast<ClipperLib::cInt>(point.x),
			static_cast<ClipperLib::cInt>(point.y),
		});
	}

	ClipperLib::PolyType type = ClipperLib::ptSubject;
	if (!ClipperLib::Orientation(path))
	{
		type = ClipperLib::ptClip;
	}

	_clipper.AddPath(path, type, true);
}

void ChunkBody2D::rebuild()
{
	if (!_clipper.Execute(ClipperLib::ctDifference, _solution, ClipperLib::pftNonZero))
	{
		PRINT_DEBUG("Clipper failed to execute");
	}

	_clipper.Clear();
	_clipper.AddPaths(_solution, ClipperLib::ptSubject, true);

	TPPLPolyList polygons;

	for (const auto &polygon : _solution)
	{
		polygons.emplace_back();
		TPPLPoly &poly = polygons.back();
		poly.Init(polygon.size());

		TPPLPoint *ptr = poly.GetPoints();
		for (const auto &point : polygon)
		{
			*ptr++ = TPPLPoint {
				static_cast<decltype(TPPLPoint::x)>(point.X),
				static_cast<decltype(TPPLPoint::y)>(point.Y),
			};
		}

		poly.SetHole(poly.GetOrientation() == TPPL_CW);
	}

	_triangles.clear();
	_partition.Triangulate_EC(&polygons, &_triangles);

	_convex_shapes.clear();
	_partition.ConvexPartition_HM(&polygons, &_convex_shapes);

	_build_mesh();
	_build_shapes();
}

void ChunkBody2D::_register_methods() noexcept
{
	PRINT_DEBUG("{0} called", __FUNCTION__);

	godot::register_method("_init", &ChunkBody2D::_init);
	godot::register_method("_ready", &ChunkBody2D::_ready);
	godot::register_method("_draw", &ChunkBody2D::_draw);
	godot::register_method("_notification", &ChunkBody2D::_notification);
	godot::register_method("_frame_drawn", &ChunkBody2D::_frame_drawn);
	godot::register_method("add_path", &ChunkBody2D::add_path);
	godot::register_method("rebuild", &ChunkBody2D::rebuild);

	godot::register_method("get_bounds", &ChunkBody2D::get_bounds);
	godot::register_method("get_texture", &ChunkBody2D::get_texture);
}

void ChunkBody2D::_build_mesh()
{
	godot::VisualServer::get_singleton()->mesh_clear(_mesh);

	godot::PoolVector2Array points, uvs;
	godot::PoolColorArray colors;

	const godot::Color color { 1.0f, 1.0f, 1.0f };

	const godot::Vector2 invbounds {
		1.0f / _bounds.size.x,
		1.0f / _bounds.size.y
	};

	for (auto &triangle : _triangles)
	{
		auto ptr = triangle.GetPoints();
		for (int i = 0; i < 3; ++i, ++ptr)
		{
			const godot::Vector2 point {
				static_cast<decltype(godot::Vector2::x)>(ptr->x),
				static_cast<decltype(godot::Vector2::y)>(ptr->y),
			};

			points.push_back(point);
			uvs.push_back((point - _bounds.position) * invbounds);
			colors.push_back(color);
		}
	}

	godot::Array meshdata;
	meshdata.resize(godot::VisualServer::ARRAY_MAX);
	meshdata[godot::VisualServer::ARRAY_VERTEX] = points;
	meshdata[godot::VisualServer::ARRAY_TEX_UV] = uvs;
	meshdata[godot::VisualServer::ARRAY_COLOR] = colors;

	godot::VisualServer::get_singleton()->mesh_add_surface_from_arrays(
		_mesh, godot::VisualServer::PRIMITIVE_TRIANGLES, meshdata
	);

#ifdef _DEBUG
	_build_debug_mesh();
#endif
}

void ChunkBody2D::_build_shapes()
{
	godot::Physics2DServer *const p2ds = godot::Physics2DServer::get_singleton();

	while (_shapes.size() < _convex_shapes.size())
	{
		godot::RID shape = p2ds->convex_polygon_shape_create();
		p2ds->body_add_shape(get_rid(), shape);
		_shapes.emplace_back(shape);
	}

	while (_shapes.size() > _convex_shapes.size())
	{
		p2ds->free_rid(_shapes.back());
		_shapes.pop_back();
	}

	if (!_shapes.size())
	{
		return;
	}

	auto shapeptr = &_shapes[0];
	for (auto &convexhull : _convex_shapes)
	{
		godot::PoolVector2Array points;
		points.resize(convexhull.GetNumPoints());

		auto poolwrite = points.write();
		auto poolptr = poolwrite.ptr();

		auto ptr = convexhull.GetPoints();
		for (int i = 0, len = convexhull.GetNumPoints(); i < len; ++i, ++ptr)
		{
			*poolptr++ = godot::Vector2 {
				static_cast<decltype(godot::Vector2::x)>(ptr->x),
				static_cast<decltype(godot::Vector2::y)>(ptr->y),
			};
		}

		p2ds->shape_set_data(*shapeptr++, points);
	}
}

void ChunkBody2D::_free() noexcept
{
	PRINT_DEBUG("{0} called by {1}", __FUNCTION__, get_name());

	godot::VisualServer::get_singleton()->free_rid(_mesh);

#ifdef _DEBUG
	godot::VisualServer::get_singleton()->free_rid(_debug_mesh);
#endif
}

#ifdef _DEBUG
void ChunkBody2D::_build_debug_mesh()
{
	godot::VisualServer::get_singleton()->mesh_clear(_debug_mesh);

	godot::PoolVector2Array points;
	godot::PoolColorArray colors;

	godot::Color color { 0.4, 0.9, 0.1 };

	for (const auto &convex : _convex_shapes)
	{
		TPPLPolyList triangles;
		_partition.Triangulate_EC(const_cast<TPPLPoly*>(&convex), &triangles);

		color.set_hsv(fmodf(color.get_h() + 0.738, 1), color.get_s(), color.get_v(), 0.5);

		for (auto &triangle : triangles)
		{
			auto ptr = triangle.GetPoints();
			for (int i = 0; i < 3; ++i, ++ptr)
			{
				points.push_back(godot::Vector2 {
					static_cast<decltype(godot::Vector2::x)>(ptr->x),
					static_cast<decltype(godot::Vector2::y)>(ptr->y),
				});

				colors.push_back(color);
			}
		}
	}

	godot::Array meshdata;
	meshdata.resize(godot::VisualServer::ARRAY_MAX);
	meshdata[godot::VisualServer::ARRAY_VERTEX] = points;
	meshdata[godot::VisualServer::ARRAY_COLOR] = colors;

	godot::VisualServer::get_singleton()->mesh_add_surface_from_arrays(
		_debug_mesh, godot::VisualServer::PRIMITIVE_TRIANGLES, meshdata
	);

	_build_outline_mesh();
}

void ChunkBody2D::_build_outline_mesh()
{
	godot::PoolVector2Array points;
	godot::PoolColorArray colors;
	godot::PoolIntArray indices;

	const godot::Color line_color { 1.0, 0.0, 0.0 };

	for (const auto &polygon : _solution)
	{
		const int index = points.size();

		for (int i = 0, len = polygon.size(); i < len; ++i)
		{
			const godot::Vector2 point {
				static_cast<decltype(godot::Vector2::x)>(polygon[i].X),
				static_cast<decltype(godot::Vector2::x)>(polygon[i].Y),
			};

			points.push_back(point);
			colors.push_back(line_color);
			indices.push_back(index + i);
			indices.push_back(index + ((i + 1) % len));
		}
	}

	godot::Array meshdata;
	meshdata.resize(godot::VisualServer::ARRAY_MAX);
	meshdata[godot::VisualServer::ARRAY_VERTEX] = points;
	meshdata[godot::VisualServer::ARRAY_COLOR] = colors;
	meshdata[godot::VisualServer::ARRAY_INDEX] = indices;

	godot::VisualServer::get_singleton()->mesh_add_surface_from_arrays(
		_debug_mesh, godot::VisualServer::PRIMITIVE_LINES, meshdata
	);
}
#endif
