#pragma once

#include <string>

namespace Fluidic
{
	class Fluid2DPrograms
	{
		static const std::string SimpleAdvect = "
/**
 * Simple advection
 * Advects the 'data' texture by the velocity given over the timestep given.
 * WARNING: Not working satisfactorily
 *
 * @param coords coordinates of the current pixel
 * @param data the data texture - will be advected
 * @param velocity the velocity texture - will be the direction of advection
 * @param boundary texture containing the fluid boundaries
 * @param scale the scale between the velocity resolution and the data resolution
 * @param d (dx, dy, 0, dt)
 * @return the new data for this coordiate
 */
half4 simpleAdvect (
			float2 coords : TEXCOORD0,
			uniform samplerRECT data,
			uniform samplerRECT velocity,
			uniform samplerRECT boundary,
			uniform float2 scale,
			uniform float4 d) : COLOR
{
	half4 returnVal;
	half2 currVel;
	half2 stepBackCoords;
	half2 alpha = d.w/d.xy;
	
	//do a texture lookup to get the velocity
	currVel = (texRECT(velocity, coords*scale)).xy;
	
	//step back a timestep to get the previous velocity
	stepBackCoords = coords*scale - alpha * currVel;

	//if it's a boundary, return base value
	float bnd = texRECT(boundary, stepBackCoords);
	
	if (bnd > 0) {
		returnVal = texRECT(data, coords);
	} else {
		//interpolate the step back position and return
		returnVal = f4texRECTbilerp(data, stepBackCoords/scale);
	}
	//returnVal = texRECT(data, stepBackCoords);
	return returnVal;
}
";
	}
}