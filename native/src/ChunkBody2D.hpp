#ifndef CHUNKBODY2D_HPP
#define CHUNKBODY2D_HPP

#include <vector>
#include <clipper.hpp>
#include <polypartition.hpp>
#include <Godot.hpp>
#include <PhysicsBody2D.hpp>
#include "DrawCache.hpp"

class ChunkBody2D : public godot::PhysicsBody2D
{
	GODOT_SUBCLASS(ChunkBody2D, PhysicsBody2D);

public:
	ChunkBody2D();

	void _init();
	void _ready();

	void _draw() const noexcept;

	void _notification(const int what) noexcept;

	void _frame_drawn(godot::Variant userdata);

	void add_path(const godot::PoolVector2Array points, const godot::Transform2D xform = {});
	void rebuild();

	godot::Rect2 get_bounds() const noexcept
	{
		return _bounds;
	}

	godot::RID get_texture() const noexcept
	{
		return _draw_cache.get_texture();
	}

	static void _register_methods() noexcept;

private:
	ClipperLib::Clipper _clipper;
	ClipperLib::Paths _solution;

	TPPLPartition _partition;
	TPPLPolyList _convex_shapes;
	TPPLPolyList _triangles;

	godot::Rect2 _bounds;

	DrawCache _draw_cache;

	godot::RID _mesh;

	std::vector<godot::RID> _shapes;

	void _build_mesh();
	void _build_shapes();
	void _free() noexcept;

#ifdef _DEBUG
	godot::RID _debug_mesh;

	void _build_debug_mesh();
	void _build_outline_mesh();
#endif
};

#endif
