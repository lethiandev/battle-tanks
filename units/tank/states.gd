extends "res://units/state_machine/state_machine.gd"

const STATE_IDLE = "idle"
const STATE_STEER = "steer"

func state_idle_enter():
	pass

func state_steer_enter():
	Debug.put("%s::Engine on" % host.name)

func state_steer_process(delta):
	if Input.is_action_pressed("move_right"):
		host.accelerate = host.ACCELERATE_RIGHT
	elif Input.is_action_pressed("move_left"):
		host.accelerate = host.ACCELERATE_LEFT
	
	if Input.is_action_pressed("rotate_left"):
		host.rotate = host.ROTATE_DESCEND
	elif Input.is_action_pressed("rotate_right"):
		host.rotate = host.ROTATE_ASCEND
	
	if Input.is_action_pressed("fire"):
		host.fire_power += delta * 500
	if Input.is_action_just_released("fire"):
		host.fire()

func state_steer_exit():
	Debug.put("%s::Engine off" % host.name)
