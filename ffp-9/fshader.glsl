varying vec2 TexCoord;

uniform sampler2D qt_Texture;
uniform sampler2D qt_YTex;
uniform sampler2D qt_UTex;
uniform sampler2D qt_VTex;

void main(void)
{
    vec3 yuv;
    vec3 rgb;

    yuv.x=texture2D(qt_YTex,TexCoord.xy).r;
    yuv.y=texture2D(qt_UTex,TexCoord.xy).r-0.5;
    yuv.z=texture2D(qt_VTex,TexCoord.xy).r-0.5;

    rgb=mat3(1.0,1.0,1.0,
             0.0,-0.39465,2.03211,
             1.13983,-0.58060,0.0)*yuv;

//    gl_FragColor=vec4(1.0,0.0,0.0,1.0);
//    gl_FragColor=texture2D(qt_Texture,TexCoord.xy);
    gl_FragColor=vec4(rgb,1.0);
}
