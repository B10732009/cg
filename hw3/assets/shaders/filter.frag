#version 430

out vec4 color;
in vec2 TexCoord;

uniform sampler2D colorBuffer;
uniform int enableEdgeDetection;
uniform int eanbleGrayscale;

const float offset = 1.0 / 300;  
const vec2 offsets[9] = vec2[](
        vec2(-offset, offset),  // top-left
        vec2(0.0f,    offset),  // top-center
        vec2(offset,  offset),  // top-right
        vec2(-offset, 0.0f),    // center-left
        vec2(0.0f,    0.0f),    // center-center
        vec2(offset,  0.0f),    // center-right
        vec2(-offset, -offset), // bottom-left
        vec2(0.0f,    -offset), // bottom-center
        vec2(offset,  -offset)  // bottom-right
    );

float kernel[9] = float[](
    -1, -1, -1,
    -1,  8, -1,
    -1, -1, -1
);

// TODO#3-2: apply filter to color
// For grayscale, you need to mix rgb color with ratio (0.2126:0.7152:0.0722)
// For edge detection, you need to apply kernel matrix and sampling offset provided (offsets)

void main() {             
    // get color from color buffer
    color = texture(colorBuffer, TexCoord);

    // edge detection
    if (enableEdgeDetection == 1) {
        color = vec4(0.0f, 0.0f, 0.0f, 1.0f);
        vec3 kernel_color[9];
        for (int i = 0; i < 9; i++) {
            vec3 sample_color = vec3(texture(colorBuffer, TexCoord + offsets[i]));
            color += vec4(sample_color * kernel[i], 0.0);
        }
    }

    // gray scale
    if (eanbleGrayscale == 1) {
        float gray = color.r * 0.2126 + color.g * 0.7152 + color.b * 0.0722;
        color.r = gray;
        color.g = gray;
        color.b = gray;
    }
}