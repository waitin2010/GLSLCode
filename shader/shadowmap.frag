#version 400
uniform sampler2D tex;
out vec4 color;
in vec2 UV;

void main(){
	vec4 temp = texture2D(tex, UV);
	color = temp;
}
