#version 410

uniform bool black_overide = false;
uniform bool show_back_facing = false;

// Light Sources
uniform vec3 sun_direction;
uniform vec3 sun_intensity;
uniform vec3 lamps[12];
uniform vec3 lamp_intensity;

// Material Shading Information
uniform vec3 La;
uniform vec3 Le;
uniform vec3 Ka;
uniform vec3 Kd;
uniform vec3 Ks;
uniform float shine;

// Vertex Shading Information
in vec3 normal;
in vec3 tangent;
in vec3 eyepos; //position in eye coordinates

//Textures and Normal Maps
in vec2 itex_coord;
uniform bool enable_normal_map = false;
uniform sampler2D normal_map;

out vec4 fragColor;

void main() {
    if(!gl_FrontFacing && show_back_facing) {
        fragColor = vec4(1,0,0,1);
        return;
    }
    if(black_overide) {
        fragColor = vec4(0, 0, 0, 1);
        return;
    }

    vec3 n = normalize(normal);
    if(enable_normal_map) {
        vec3 t = normalize(tangent);
        vec3 b = normalize(cross(t, n));

        // Matrix to transform from tangent space to cameraspace
        mat3 tanspace = mat3(t, b, n);

        n = tanspace * normalize(texture(normal_map, itex_coord).rgb * 2.0 - 1.0);

        //fragColor = texture(normal_map, itex_coord);
        //return;
    }

    vec3 light_sum = vec3(0); // sum of all light
    vec3 v = normalize( -eyepos ); //camera is at 0,0,0

    for(int x = 0; x < 12; ++x) {
        vec3 l = normalize( lamps[x] - eyepos );
        vec3 h = normalize( l + v );

        vec3 tmp = Kd * max(dot(n, l), 0); //diffuse
        tmp += Ks * pow(max(dot(h, n), 0), shine); //specular
        light_sum += tmp * (lamp_intensity / pow(length(lamps[x] - eyepos), 2)); //intensity / distance-squared
    }
    light_sum += Ka*La; // ambient light
    light_sum += Le; // emmission

    // Sun
    vec3 l = normalize( sun_direction );
    vec3 h = normalize( l + v );
    vec3 tmp = Kd * max(dot(n, l), 0); //diffuse
    tmp += Ks * pow(max(dot(h, n), 0), shine); //specular
    light_sum += tmp * sun_intensity;

    fragColor = vec4(light_sum, 1); //ambient + diffuse + Specular
}
