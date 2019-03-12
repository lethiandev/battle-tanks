extends RigidBody2D

export(NodePath) var root_node

func _enter_tree():
	for child in get_parent().get_children():
		if child.get_script() == get_script():
			add_collision_exception_with(child)

func _integrate_forces(state):
	get_node(root_node)._wheel_integrate_forces(state)
