attribute vec4 vPosition;
attribute vec2 inputTextureCoordinate;
uniform mat4 uMVPMatrix;
varying vec2 textureCoordinate;

void main() {
    gl_Position = uMVPMatrix * vPosition;
    textureCoordinate = inputTextureCoordinate;
}