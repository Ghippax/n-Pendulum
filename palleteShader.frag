#version 400

uniform float width;
uniform vec3 a;
uniform vec3 b;
uniform vec3 c;
uniform vec3 d;

vec3 palette(float t, vec3 a, vec3 b, vec3 c, vec3 d){
    return a + b*cos(6.28318*(c*t+d));
}

void main(){
   	gl_FragColor = vec4(palette((gl_FragCoord.x-0.5)/width,a,b,c,d).xyz,1.0);

}