extends CanvasLayer

var logs = []

func _ready():
	$FPSLabel.text = ""
	$LoggingLabel.text = ""

func put(message):
	logs.push_back(message)
	print("Log: ", message)
	
	if logs.size() > 10:
		logs.pop_front()
	
	_update_logger()

func _process(delta):
	$FPSLabel.text = "FPS: %d" % Engine.get_frames_per_second()

func _update_logger():
	$LoggingLabel.text = ""
	
	for message in logs:
		$LoggingLabel.text += str(message) + "\n"
