attribute vec4 vPosition;

uniform mat4 model;
uniform mat4 projection;

varying vec2 TexCoords;

void main() {
    TexCoords = vPosition.zw;
    gl_Position = projection * model * vec4(vPosition.xy, 0.0, 1.0);
}