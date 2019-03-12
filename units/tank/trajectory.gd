tool
extends Node2D

export(int) var amount = 10 setget set_amount, get_amount
export(float) var size = 12 setget set_size, get_size
export(Texture) var texture setget set_texture, get_texture

var mesh

func _init():
	mesh = VisualServer.mesh_create()

func _notification(what):
	if what == NOTIFICATION_PREDELETE:
		VisualServer.free_rid(mesh)

func _enter_tree():
	_rebuild_mesh()

func _draw():
	var texrid = texture.get_rid() if texture else RID()
	VisualServer.canvas_item_add_mesh(get_canvas_item(), mesh, texrid)

func set_amount(value):
	amount = value
	_rebuild_mesh()

func get_amount():
	return amount

func set_size(value):
	size = value
	_rebuild_mesh()

func get_size():
	return size

func set_texture(value):
	texture = value
	update()

func get_texture():
	return texture

func _rebuild_mesh():
	var points = PoolVector2Array()
	var uvs = PoolVector2Array()
	var colors = PoolColorArray()
	var indices = PoolIntArray()
	
	var hsize = size * 0.5;
	
	for i in range(amount):
		points.append_array([
			Vector2(-hsize, -hsize),
			Vector2( hsize, -hsize),
			Vector2( hsize,  hsize),
			Vector2(-hsize,  hsize),
		])
		
		uvs.append_array([
			Vector2(0.0, 0.0),
			Vector2(1.0, 0.0),
			Vector2(1.0, 1.0),
			Vector2(0.0, 1.0),
		])
		
		var vidx = (1.0 / amount) * i
		colors.append_array([
			Color(1.0, 1.0, 1.0, vidx),
			Color(1.0, 1.0, 1.0, vidx),
			Color(1.0, 1.0, 1.0, vidx),
			Color(1.0, 1.0, 1.0, vidx),
		])
		
		var idx = i * 4
		indices.append_array([
			idx + 0, idx + 1, idx + 2,
			idx + 0, idx + 2, idx + 3,
		])
	
	var meshdata = Array()
	meshdata.resize(VisualServer.ARRAY_MAX)
	meshdata[VisualServer.ARRAY_VERTEX] = points
	meshdata[VisualServer.ARRAY_TEX_UV] = uvs
	meshdata[VisualServer.ARRAY_COLOR] = colors
	meshdata[VisualServer.ARRAY_INDEX] = indices
	
	VisualServer.mesh_clear(mesh)
	VisualServer.mesh_add_surface_from_arrays(mesh, VisualServer.PRIMITIVE_TRIANGLES, meshdata)
	
	if material:
		material.set_shader_param("amount", amount)

func set_power(value):
	material.set_shader_param("power", value)

func get_power():
	return material.get_shader_param("power")
