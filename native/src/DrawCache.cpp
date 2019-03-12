#include <VisualServer.hpp>
#include <DrawCache.hpp>

DrawCache::DrawCache()
{
	godot::VisualServer *const vs = godot::VisualServer::get_singleton();

	_viewport = vs->viewport_create();
	vs->viewport_set_usage(_viewport, godot::VisualServer::VIEWPORT_USAGE_2D);
	vs->viewport_set_update_mode(_viewport, godot::VisualServer::VIEWPORT_UPDATE_ONCE);
	vs->viewport_set_transparent_background(_viewport, true);
	vs->viewport_set_clear_mode(_viewport, godot::VisualServer::VIEWPORT_CLEAR_ALWAYS);
	vs->viewport_set_vflip(_viewport, true);
	vs->viewport_set_active(_viewport, true);

	_texture = vs->viewport_get_texture(_viewport);

	_canvas = vs->canvas_create();
	vs->viewport_attach_canvas(_viewport, _canvas);
}

DrawCache::DrawCache(const godot::Size2 size) : DrawCache()
{
	set_size(size);
}

DrawCache::~DrawCache() noexcept
{
	godot::VisualServer::get_singleton()->free_rid(_canvas);
	godot::VisualServer::get_singleton()->free_rid(_viewport);
}

void DrawCache::request_update() const noexcept
{
	godot::VisualServer::get_singleton()->viewport_set_update_mode(_viewport, godot::VisualServer::VIEWPORT_UPDATE_ONCE);
}

void DrawCache::add_canvas_item(godot::RID item)
{
	godot::VisualServer::get_singleton()->canvas_item_set_parent(item, _canvas);
}

void DrawCache::_set_viewport_size(const godot::Size2 &size) noexcept
{
	const std::int64_t width = static_cast<std::int64_t>(size.width);
	const std::int64_t height = static_cast<std::int64_t>(size.height);

	godot::VisualServer::get_singleton()->viewport_set_size(_viewport, width, height);
}

void DrawCache::_set_viewport_transform(const godot::Transform2D &xform) noexcept
{
	godot::VisualServer::get_singleton()->viewport_set_global_canvas_transform(_viewport, xform);
}
