shader_type canvas_item;

void vertex() {
	UV = vec2(UV.y, UV.x);
}
