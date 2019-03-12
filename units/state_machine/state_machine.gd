extends Node

signal state_changed(from, to)
signal state_entered(state_name)
signal state_exited(state_name)

var state setget set_state, get_state
var host

var _state_enter_method
var _state_process_method
var _state_exit_method

func _enter_tree():
	host = get_parent()

func _exit_tree():
	host = null

func _process(delta):
	_callv_safe(_state_process_method, [delta])

func set_state(state_name : String):
	var old_state = state
	
	_state_exit()
	state = state_name
	
	_state_enter_method = "state_" + str(state) + "_enter"
	_state_process_method = "state_" + str(state) + "_process"
	_state_exit_method = "state_" + str(state) + "_exit"
	_state_enter()
	
	emit_signal("state_changed", old_state, state)

func get_state():
	return state

func _state_exit():
	_callv_safe(_state_exit_method)
	emit_signal("state_exited", state)

func _state_enter():
	_callv_safe(_state_enter_method)
	emit_signal("state_entered", state)

func _callv_safe(method, args = []):
	if method and has_method(method):
		callv(method, args)
