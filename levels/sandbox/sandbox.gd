extends Node

const ChunkBody2D = preload("res://landscape/chunk_body_2d.gdns")

func _unhandled_input(event):
	if event is InputEventMouseButton and event.pressed:
		var xform = get_viewport().canvas_transform.affine_inverse()
		var pos = xform.xform(event.position)
		var hole = _create_circle(pos, 64)
		_blit_path(hole, self)

func _blit_path(path, from):
	for node in from.get_children():
		if node.get_script() == ChunkBody2D:
			var xform = node.global_transform.affine_inverse()
			node.add_path(path, xform)
			node.rebuild()
		
		_blit_path(path, node)

func _create_circle(center : Vector2, radius : float, segments: int = 12):
	var points = PoolVector2Array()
	points.resize(segments)
	
	var theta = TAU / segments
	
	for i in range(segments):
		points[i] = center + Vector2(
			cos(-theta * i) * radius,
			sin(-theta * i) * radius
		)
	
	return points
