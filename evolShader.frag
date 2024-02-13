#version 400

uniform float fracHeight;
uniform vec3 a;
uniform vec3 b;
uniform vec3 c;
uniform vec3 d;

uniform sampler2D tex;

// Maps a value in the interval [a,b] to the interval [c,d]
float linearMap(float var, float bound1, float bound2, float outBound1, float outBound2){
    return outBound1 + (var-bound1)/(bound2-bound1)*(outBound2-outBound1);
}

void main(){
	// System bounds;
    float pBound = 1;
    float pi = 3.141592653589793238462643;

    // Retrieves system state from texture data and maps it to system bounds
    vec4 texelValue = texelFetch(tex, ivec2(gl_FragCoord.x-0.5,fracHeight-gl_FragCoord.y-0.5),0);
    vec2 inRG = vec2(linearMap(texelValue.x,0,pBound,-pi,pi),linearMap(texelValue.y,0,pBound,-pi,pi));
   
    // Maps the state radius (normalized to [0,1]) from the center to a pallete specified by [a,b,c,d]
    gl_FragColor = vec4(a + b*cos(6.28318*(c*(sqrt(pow(inRG.x,2.0)+pow(inRG.y,2.0))/4.442882938158366)+d)), 1.0);


}