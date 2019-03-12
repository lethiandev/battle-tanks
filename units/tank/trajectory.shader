shader_type canvas_item;
render_mode unshaded, skip_vertex_transform;

uniform int amount = 0;
uniform float space = 38.0;
uniform float power = 300.0;

void vertex() {
	float idx = COLOR.a * float(amount);
	
	VERTEX = (EXTRA_MATRIX * (WORLD_MATRIX * vec4(VERTEX, 0.0, 1.0))).xy;
	
	vec2 v = normalize(WORLD_MATRIX[0].xy) * power;
	vec2 a = vec2(0.0, 720.0);
	
	float s = space * float(idx) + space * fract(TIME);
	float vl = length(v);
	float al = length(a);
	float t = (sqrt(2.0*s*al + vl*vl) - vl) / al;
	
	float scale = length(WORLD_MATRIX[0].xy);
	
	VERTEX += (v*t + 0.5*a*t*t) * scale;
	COLOR.a = min(1.0, float(amount) - idx - fract(TIME));
}

void fragment() {
	COLOR = texture(TEXTURE, UV) * COLOR;
}
