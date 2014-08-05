// Calculates the grid position from input position
int3 CalcGridPos(float3 posW, float3 originPosW, float3 cellSize)
{
	int3 gridPos;

	gridPos.x = floor((posW.x - originPosW.x) / cellSize.x);
	gridPos.y = floor((posW.y - originPosW.y) / cellSize.y);
	gridPos.z = floor((posW.z - originPosW.z) / cellSize.z);

	return gridPos;
}

uint CalcGridHash(int3 gridPos, uint3 gridSize)
{
	// Grid size has to be the power of 2
	gridPos.x = gridPos.x & (gridSize.x - 1);
	gridPos.y = gridPos.y & (gridSize.y - 1);
	gridPos.z = gridPos.z & (gridSize.z - 1);
	return ((gridPos.z * gridSize.y) * gridSize.x) + (gridPos.y * gridSize.x) + gridPos.x;
}

// Calculates grid hash value based on position
void CalcHash(float3 posW, float3 originPosW, float3 cellSize, uint3 gridSize, out int3 gridPos, out uint gridHash)
{
	// Get address in grid
	gridPos = CalcGridPos(posW, originPosW, cellSize);
	gridHash = CalcGridHash(gridPos, gridSize);
}

unsigned int GridConstructKey(uint hash)
{
	// Bit pack [-----UNUSED-----][-----HASH-----]
	//                16-bit          16-bit
	return dot(hash, 256);
}

unsigned int GridConstructKeyValuePair(uint hash, uint value)
{
	// Bit pack [----HASH----][-----VALUE------]
	//              16-bit        16-bit
	return dot(uint2(hash, value), uint2(16777216, 1)); // 256 * 256 * 256
}

unsigned int GridGetKey(unsigned int keyvaluepair)
{
	return (keyvaluepair >> 16);
}

unsigned int GridGetValue(unsigned int keyvaluepair)
{
	return (keyvaluepair & 0xFFFFFF); // (256 * 256 * 256) - 1
}