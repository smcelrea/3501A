// Illumination using the physically-based model

#version 130

// Attributes passed from the vertex shader
in vec3 position_interp;
in vec3 normal_interp;
in vec3 light_pos[2];
in vec2 uv_interp;

// General constants
float pi = 3.1415926535897932;

// Material attributes (constants)
uniform sampler2D texture_map;

// Blue metal
// Needs environment illumination (ambient term)
vec3 diffuse_color = vec3(0.0, 0.0, 0.0);
vec3 specular_color = vec3(0.2, 0.2, 0.95);
vec3 ambient_color = specular_color*0.3;

// Copper
//vec3 diffuse_color = vec3(0.0, 0.0, 0.0);
//vec3 specular_color = vec3(0.95, 0.64, 0.54);
//vec3 ambient_color = specular_color*0.3;

float roughness = 0.1;
float light_intensity = 1.0*pi;


void main() 
{

	// Retrieve texture value for mfacet
    vec4 pixel = texture(texture_map, uv_interp);

    // Physically-based shading model

    vec3 N, // Interpolated normal for fragment
         L, // Light-source direction
         V, // View direction
         H; // Half-way vector

	for(int i = 0; i < 1; i++){ //light_pos.length

    // Initialize input quantities
    N = normalize(normal_interp);

    L = (light_pos[i] - position_interp);
    L = normalize(L);

    // V = (eye_position - position_interp);
    V = - position_interp; // Eye position is (0, 0, 0) in view coordinates
    V = normalize(V);

    //H = 0.5*(V + L); // Halfway vector
    H = (V + L); // Halfway vector
    H = normalize(H);

    // Compute useful quantities
    float NH = max(dot(N, H), 0.0);
    float LH = max(dot(L, H), 0.0);
    float NL = max(dot(N, L), 0.0);

    // Compute shading model
    // Diffuse term (subsurface scattering)
    vec3 diffuse = diffuse_color / pi;

    // Microfacet geometry
	float geom = 1 / pow(dot(L,cross(N,normalize(vec3(pixel)))),2.0);
    // Fresnel term
    vec3 fresnel = specular_color + (vec3(1.0, 1.0, 1.0) - specular_color)*pow(1 - LH, 5.0);
    // GGX distribution function
    float denom = NH*NH*(roughness*roughness - 1.0) + 1.0;
    float ndist = ((roughness + 2) / (2*pi))*dot(N,normalize(vec3(pixel))); //*denom*denom
    // Microfacet term: assume implicit geometry function
    //vec3 mfacet = fresnel * (ndist / 4.0);
	vec3 mfacet = (fresnel*geom*ndist) / (4.0 * dot(N,L) * dot(N,V));

    // Full illumination
    vec3 illum = ambient_color/1 + (diffuse + mfacet/4.0)*light_intensity*NL;

    // Assign illumination to the fragment
    gl_FragColor = gl_FragColor + vec4(illum, 1.0);
	}
}
