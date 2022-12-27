attribute vec4 qt_Vertex;
attribute vec2 qt_TexCoord;
varying vec2 TexCoord;

void main(void)
{
    gl_Position = qt_Vertex;
    TexCoord = qt_TexCoord;
}
