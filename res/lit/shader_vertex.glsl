#version 450 core

layout(location=0) in vec3 aModelVtx;
layout(location=1) in vec4 aColor[3]; 
layout(location=4) in mat4 aTransform;

out vec4 mColor;
uniform mat4 uProjection; 

void main(void) { 
  gl_Position = uProjection * aTransform *  vec4(aModelVtx, 1.0);
  mColor = aColor[gl_VertexID];
}

