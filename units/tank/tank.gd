extends RigidBody2D

enum {
	ACCELERATE_NONE = 0,
	ACCELERATE_RIGHT = 1,
	ACCELERATE_LEFT = -1
}

enum {
	ROTATE_NONE = 0,
	ROTATE_ASCEND = 1,
	ROTATE_DESCEND = -1
}

const SPEED_FACTOR = 0.5
const SPEED_MAX = 10

const AIR_FACTOR = 2.5

const FIRE_POWER_MIN = 200
const FIRE_POWER_MAX = 1000

var accelerate = ACCELERATE_NONE
var rotate = ROTATE_NONE

var fire_power = 0 setget set_fire_power, get_fire_power

func _ready():
	$StateMachine.state = $StateMachine.STATE_STEER
	set_fire_power(0)

func _process(delta):
	accelerate = ACCELERATE_NONE
	
	$Body/Barrel.rotate(rotate * delta)
	rotate = ROTATE_NONE

func _integrate_forces(state):
	state.angular_velocity += AIR_FACTOR * state.step * -accelerate

func _wheel_integrate_forces(state):
	if accelerate == ACCELERATE_NONE:
		state.angular_velocity = 0
		return
	
	var d_speed = SPEED_MAX * TAU - abs(state.angular_velocity) 
	var d_av = max(0, d_speed * SPEED_FACTOR * state.step)
	
	state.angular_velocity += d_av * accelerate

func set_fire_power(value):
	fire_power = clamp(value, FIRE_POWER_MIN, FIRE_POWER_MAX)
	
	var barrel = $Body/Barrel/Position2D
	
	if fire_power == FIRE_POWER_MIN:
		barrel.visible = false
	else:
		barrel.visible = true
		barrel.set_power(fire_power)
		
		var fmax = FIRE_POWER_MAX - FIRE_POWER_MIN
		var f = (fire_power - FIRE_POWER_MIN) / fmax
		barrel.modulate = Color(f, 1.0 - f, 0.0)

func get_fire_power():
	return fire_power

func fire():
	set_fire_power(0)
