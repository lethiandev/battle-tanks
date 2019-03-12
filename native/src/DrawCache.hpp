#ifndef DRAWCACHE_HPP
#define DRAWCACHE_HPP

#include <cstdint>
#include <Godot.hpp>

class DrawCache
{
public:
	DrawCache();
	DrawCache(const godot::Size2 size);

	~DrawCache() noexcept;

	void request_update() const noexcept;

	void add_canvas_item(godot::RID item);

	void set_size(const godot::Size2 size) noexcept
	{
		_set_viewport_size(size);
		_size = size;
	}

	const godot::Size2& get_size() const noexcept
	{
		return _size;
	}

	void set_transform(const godot::Transform2D xform) noexcept
	{
		_set_viewport_transform(xform);
		_transform = xform;
	}

	const godot::Transform2D& get_transform() const noexcept
	{
		return _transform;
	}

	const godot::RID& get_viewport() const noexcept
	{
		return _viewport;
	}

	const godot::RID& get_texture() const noexcept
	{
		return _texture;
	}

	const godot::RID& get_canvas() const noexcept
	{
		return _canvas;
	}

private:
	godot::RID _viewport;
	godot::RID _texture;
	godot::RID _canvas;

	godot::Size2 _size;
	godot::Transform2D _transform;

	void _set_viewport_size(const godot::Size2 &size) noexcept;
	void _set_viewport_transform(const godot::Transform2D &xform) noexcept;
};

#endif
