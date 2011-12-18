//#define NUMBER_OF_BORNCOLORS 7
#define NUMBER_OF_BORNCOLORS 4

static const int2 g_sumIndex[8] = {
    {-1, -1},
    { 0, -1},
    { 1, -1},
    {-1,  0},
    { 1,  0},
    {-1,  1},
    { 0,  1},
    { 1,  1},
};
/*
static const float3 g_bornColors[NUMBER_OF_BORNCOLORS] = {
    {1.0f, 0.0f, 0.0f},
    {0.0f, 1.0f, 0.0f},
    {0.0f, 0.0f, 1.0f},
    {1.0f, 1.0f, 0.0f},
    {0.0f, 1.0f, 1.0f},
    {1.0f, 0.0f, 1.0f},
    {1.0f, 1.0f, 1.0f},
};
*/
static const float3 g_bornColors[NUMBER_OF_BORNCOLORS] = {
    {255.f / 255.f, 255.f / 255.f, 255.f / 255.f},
    {255.f / 255.f, 128.f / 255.f, 128.f / 255.f},
    {128.f / 255.f, 255.f / 255.f, 128.f / 255.f},
    {128.f / 255.f, 128.f / 255.f, 255.f / 255.f},
};

cbuffer CBParams : register(b0)
{
	int width;
	int height;
	int r1;
	int r2;
	int r3;
	int r4;
};

struct Data {
	int alive;
	float3 color;
};

StructuredBuffer<Data> current : register(t0);
RWStructuredBuffer<Data> next : register(t1);

[numthreads(1, 1, 1)]
void main( uint3 tid : SV_DispatchThreadID )
{
	int cx = tid.x % width;
	int cy = tid.x / width;

	int sum = 0;
	float3 colorSum = 0;
	for(int i = 0; i < 8; ++i) {
		int x = cx + g_sumIndex[i].x;
		int y = cy + g_sumIndex[i].y;
		if(0 <= x && x < width && 0 <= y && y < height) {
			int id = x + y * width;
			if(current[id].alive != 0) {
				sum++;
				colorSum += current[id].color;
			}			
		}
	}
	colorSum /= sum;

	if(current[tid.x].alive == 0 && sum >= r1 && sum <= r2) {
		next[tid.x].alive = 1;
//		next[tid.x].color = g_bornColors[tid.x % NUMBER_OF_BORNCOLORS];
		next[tid.x].color = float3(1.0f, 1.0f, 1.0f);
//		next[tid.x].color = float3(0.0f, 0.0f, 1.0f);
	}
	else if(current[tid.x].alive != 0 && (sum > r3 || sum < r4)) {
		next[tid.x].alive = 0;
		next[tid.x].color = 0.0f;
	}
	else {
		next[tid.x].alive = current[tid.x].alive;
		next[tid.x].color = current[tid.x].color;
//		if(current[tid.x].alive != 0) {
//			next[tid.x].color = saturate((colorSum + next[tid.x].color) / 2.0f);
//		}
	}
}
