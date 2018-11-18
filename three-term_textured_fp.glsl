// Illumination based on the traditional three-term model

#version 130

// Attributes passed from the vertex shader
in vec3 position_interp;
in vec3 normal_interp;
in vec4 color_interp;
in vec2 uv_interp;
in vec3 light_pos[2];
in vec3 camera_pos;

// Material attributes (constants)
vec4 ambient_color = vec4(0.1, 0.1, 0.1, 1.0);
vec4 diffuse_color = vec4(0.5, 0.5, 0.5, 1.0);
vec4 specular_color = vec4(0.8, 0.8, 0.9, 1.0);
float phong_exponent = 128.0;

// Uniform (global) buffer
uniform sampler2D texture_map;



void main() 
{
	// Retrieve texture value
    vec4 pixel = texture(texture_map, uv_interp);

    // Blinn-Phong shading

    vec3 N, // Interpolated normal for fragment
         L, // Light-source direction
         V, // View direction
         H; // Half-way vector
	for(int i = 0; i < light_pos.length; i++){

    // Compute Lambertian lighting Id
    N = normalize(normal_interp);

    L = (light_pos[i] - position_interp);
    L = normalize(L);

    float Id = max(dot(N, L), 0.0);
    
    // Compute specular term for Blinn-Phong shading
    // V = (eye_position - position_interp);
    V = - position_interp; // Eye position is (0, 0, 0) in view coordinates
    V = normalize(V);

    //H = 0.5*(V + L); // Halfway vector
    H = (V + L); // Halfway vector
    H = normalize(H);

    float spec_angle_cos = max(dot(N, H), 0.0);
    float Is = pow(spec_angle_cos, phong_exponent);
        
    // Assign light to the fragment
    gl_FragColor = gl_FragColor + pixel*(ambient_color/light_pos.length + Id*diffuse_color + Is*specular_color);
	}
                    
    // For debug, we can display the different values
    //gl_FragColor = ambient_color;
    //gl_FragColor = diffuse_color;
    //gl_FragColor = specular_color;
    //gl_FragColor = color_interp;
    //gl_FragColor = vec4(N.xyz, 1.0);
    //gl_FragColor = vec4(L.xyz, 1.0);
    //gl_FragColor = vec4(V.xyz, 1.0);
}
