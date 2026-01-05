#version 450 core

#ifdef COMPUTE_SHADER

// In
layout(binding = 0, std430) readonly buffer InElevation { float hf[]; };
layout(binding = 1, std430) readonly buffer InStreamArea { float stream[]; };

// Out
layout(binding = 2, std430) writeonly buffer OutElevation { float out_hf[]; };
layout(binding = 3, std430) writeonly buffer OutStreamArea { float out_stream[]; };

layout(binding = 4, std430) readonly buffer Uplift { float upliftBuffer[]; };

layout(binding = 5, std430) readonly buffer Steepest { ivec2 steepestBuffer[]; };



uniform ivec2 srcPos;
uniform ivec2 size;

uniform int nx;
uniform int ny;
uniform vec2 a;
uniform vec2 b;
uniform vec2 cellDiag;

// 0: Stream power
// 1: Stream power + Hillslope (Laplacian)
// 2: Stream power + Hillslope (Laplacian) + Debris slope
uniform int erosionMode = 0;

uniform float uplift = 0.01f;
uniform float k = 0.0005f;
uniform float k_d = 10.0f;
uniform float k_h = 2.0f;
uniform float p_sa = 0.8f;
uniform float p_sl = 2.0f;
uniform float dt = 50.0f;

const ivec2 next8[8] = ivec2[8](ivec2(0, 1), ivec2(1, 1), ivec2(1, 0), ivec2(1, -1),
                                ivec2(0, -1), ivec2(-1, -1), ivec2(-1, 0), ivec2(-1, 1));


float cellSize = cellDiag.x;

int toIndex(int i, int j) { return i + nx * j; }
int toIndex(ivec2 p) { return p.x + nx * p.y; }

float slope(ivec2 p, ivec2 q) {
    if (p.x < 0 || p.x >= size.x || p.y < 0 || p.y >= size.y) return 0.0;
    if (q.x < 0 || q.x >= size.x || q.y < 0 || q.y >= size.y) return 0.0;
    if (p == q) return 0.0;
    int index_p = toIndex(p.x, p.y);
    int index_q = toIndex(q.x, q.y);
    float d = cellSize * length(vec2(p)-vec2(q));
    return (hf[index_q] - hf[index_p]) / d;
}


float laplacian_h(ivec2 p) {
    float lapl = 0.0;
    int i = p.x;
    int j = p.y;

    if (i == 0)
        lapl += (hf[toIndex(i, j)] - 2.0 * hf[toIndex(i + 1, j)] + hf[toIndex(i + 2, j)]);
    else if (i == size.x - 1)
        lapl += (hf[toIndex(i, j)] - 2.0 * hf[toIndex(i - 1, j)] + hf[toIndex(i - 2, j)]);
    else
        lapl += (hf[toIndex(i + 1, j)] - 2.0 * hf[toIndex(i, j)] + hf[toIndex(i - 1, j)]);

    if (j == 0)
        lapl += (hf[toIndex(i, j)] - 2.0 * hf[toIndex(i, j + 1)] + hf[toIndex(i, j + 2)]);
    else if (j == size.y - 1)
        lapl += (hf[toIndex(i, j)] - 2.0 * hf[toIndex(i, j - 1)] + hf[toIndex(i, j - 2)]);
    else
        lapl += (hf[toIndex(i, j + 1)] - 2.0 * hf[toIndex(i, j)] + hf[toIndex(i, j - 1)]);

    lapl /= cellSize*cellSize;

    return lapl;
}

ivec2 getOffsetToDownstream(ivec2 p) {
    ivec2 d = ivec2(0, 0);
    float maxSlope = 0.0;
    for (int i = 0; i < 8; i++) {
        float ss = slope(p + next8[i], p);
        if (ss > maxSlope) {
            maxSlope = ss;
            d = next8[i];
        }
    }
    return d;
}

float getDiffDrainageArea(ivec2 p) {
    float water = 0.0;
    for (int i = 0; i < 8; i++) {
        ivec2 q = p + next8[i];
        ivec2 fd = steepestBuffer[toIndex(q)];
        if (q + fd == p) {
            water += q.x < 0 || q.x >= size.x || q.y < 0 || q.y >= size.y ? 0.0 : stream[toIndex(q)];
        }
    }
    return water;
}

layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;
void main() {
    ivec2 p = srcPos + ivec2(gl_GlobalInvocationID.xy);
    if (p.x >= size.x || p.y >= size.y) return;

    int id = toIndex(p);

    // Border nodes are fixed to zero (elevation and drainage)
    if (p.x == 0 || p.x == size.x - 1 || p.y == 0 || p.y == size.y - 1) {
        out_hf[id] = 0.0;
        out_stream[id] = sqrt(2.0) * cellSize;
        return;
    }

    float h = hf[id];
    float da = sqrt(2.0) * cellSize + getDiffDrainageArea(p);
   
    // Erosion at p (relative to steepest)
    ivec2 downstream = p+steepestBuffer[id];
    float pslope = abs(slope(downstream, p));

    float spe = k * pow(da, p_sa) * pow(pslope, p_sl);

    
    if (erosionMode == 0)       // Stream power
        h -= dt * (spe);
    else if (erosionMode == 1)  // Stream power + Hillslope erosion (Laplacian)
        h -= dt * (spe - k_h * laplacian_h(p));
    else if (erosionMode == 2)  // Stream power + Hillslope erosion (Laplacian) + Debris flow
        h -= dt * (spe - k_h * laplacian_h(p) - k_d * pslope);
    h = max(h, hf[toIndex(downstream)]);
    h += dt * uplift * upliftBuffer[id];

    out_hf[id] = h;
    out_stream[id] = da;
}

#endif
