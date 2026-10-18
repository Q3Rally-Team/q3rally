// Q3Rally: FXAA 3.11 (Timothy Lottes, NVIDIA), PC quality path with 8
// search steps per side, on the final LDR image. u_InvTexRes = 1 / size.
uniform sampler2D u_DiffuseMap;
uniform vec2      u_InvTexRes;

varying vec2      var_Tex1;

#define FXAA_SUBPIX          0.75
#define FXAA_EDGE_THRESHOLD  0.166
#define FXAA_EDGE_MIN        0.0625

float Luma(vec3 c)
{
	return dot(c, vec3(0.299, 0.587, 0.114));
}

float LumaAt(vec2 p)
{
	return Luma(texture2D(u_DiffuseMap, p).rgb);
}

float StepSize(int i)
{
	if (i == 0) return 1.5;
	if (i < 5) return 2.0;
	if (i == 5) return 4.0;
	return 8.0;
}

void main()
{
	vec2 rcp = u_InvTexRes;
	vec2 posM = var_Tex1;
	vec4 rgbyM = texture2D(u_DiffuseMap, posM);
	float lumaM = Luma(rgbyM.rgb);

	float lumaS = LumaAt(posM + vec2( 0.0,  1.0) * rcp);
	float lumaE = LumaAt(posM + vec2( 1.0,  0.0) * rcp);
	float lumaN = LumaAt(posM + vec2( 0.0, -1.0) * rcp);
	float lumaW = LumaAt(posM + vec2(-1.0,  0.0) * rcp);

	float maxSM = max(lumaS, lumaM);
	float minSM = min(lumaS, lumaM);
	float maxESM = max(lumaE, maxSM);
	float minESM = min(lumaE, minSM);
	float maxWN = max(lumaN, lumaW);
	float minWN = min(lumaN, lumaW);
	float rangeMax = max(maxWN, maxESM);
	float rangeMin = min(minWN, minESM);
	float range = rangeMax - rangeMin;

	if (range < max(FXAA_EDGE_MIN, rangeMax * FXAA_EDGE_THRESHOLD))
	{
		gl_FragColor = rgbyM;
		return;
	}

	float lumaNW = LumaAt(posM + vec2(-1.0, -1.0) * rcp);
	float lumaSE = LumaAt(posM + vec2( 1.0,  1.0) * rcp);
	float lumaNE = LumaAt(posM + vec2( 1.0, -1.0) * rcp);
	float lumaSW = LumaAt(posM + vec2(-1.0,  1.0) * rcp);

	float lumaNS = lumaN + lumaS;
	float lumaWE = lumaW + lumaE;
	float subpixRcpRange = 1.0 / range;
	float subpixNSWE = lumaNS + lumaWE;
	float edgeHorz1 = -2.0 * lumaM + lumaNS;
	float edgeVert1 = -2.0 * lumaM + lumaWE;

	float lumaNESE = lumaNE + lumaSE;
	float lumaNWNE = lumaNW + lumaNE;
	float edgeHorz2 = -2.0 * lumaE + lumaNESE;
	float edgeVert2 = -2.0 * lumaN + lumaNWNE;

	float lumaNWSW = lumaNW + lumaSW;
	float lumaSWSE = lumaSW + lumaSE;
	float edgeHorz4 = abs(edgeHorz1) * 2.0 + abs(edgeHorz2);
	float edgeVert4 = abs(edgeVert1) * 2.0 + abs(edgeVert2);
	float edgeHorz3 = -2.0 * lumaW + lumaNWSW;
	float edgeVert3 = -2.0 * lumaS + lumaSWSE;
	float edgeHorz = abs(edgeHorz3) + edgeHorz4;
	float edgeVert = abs(edgeVert3) + edgeVert4;

	float subpixNWSWNESE = lumaNWSW + lumaNESE;
	float lengthSign = rcp.x;
	bool horzSpan = edgeHorz >= edgeVert;
	float subpixA = subpixNSWE * 2.0 + subpixNWSWNESE;

	if (!horzSpan) lumaN = lumaW;
	if (!horzSpan) lumaS = lumaE;
	if (horzSpan) lengthSign = rcp.y;
	float subpixB = (subpixA * (1.0 / 12.0)) - lumaM;

	float gradientN = lumaN - lumaM;
	float gradientS = lumaS - lumaM;
	float lumaNN = lumaN + lumaM;
	float lumaSS = lumaS + lumaM;
	bool pairN = abs(gradientN) >= abs(gradientS);
	float gradient = max(abs(gradientN), abs(gradientS));
	if (pairN) lengthSign = -lengthSign;
	float subpixC = clamp(abs(subpixB) * subpixRcpRange, 0.0, 1.0);

	vec2 posB = posM;
	vec2 offNP;
	offNP.x = (!horzSpan) ? 0.0 : rcp.x;
	offNP.y = ( horzSpan) ? 0.0 : rcp.y;
	if (!horzSpan) posB.x += lengthSign * 0.5;
	if ( horzSpan) posB.y += lengthSign * 0.5;

	vec2 posN = posB - offNP;
	vec2 posP = posB + offNP;
	float subpixD = -2.0 * subpixC + 3.0;
	float lumaEndN = LumaAt(posN);
	float subpixE = subpixC * subpixC;
	float lumaEndP = LumaAt(posP);

	if (!pairN) lumaNN = lumaSS;
	float gradientScaled = gradient * 0.25;
	float lumaMM = lumaM - lumaNN * 0.5;
	float subpixF = subpixD * subpixE;
	bool lumaMLTZero = lumaMM < 0.0;

	lumaEndN -= lumaNN * 0.5;
	lumaEndP -= lumaNN * 0.5;
	bool doneN = abs(lumaEndN) >= gradientScaled;
	bool doneP = abs(lumaEndP) >= gradientScaled;

	for (int i = 0; i < 7; i++)
	{
		if (doneN && doneP)
			break;

		float s = StepSize(i);
		if (!doneN)
		{
			posN -= offNP * s;
			lumaEndN = LumaAt(posN) - lumaNN * 0.5;
			doneN = abs(lumaEndN) >= gradientScaled;
		}
		if (!doneP)
		{
			posP += offNP * s;
			lumaEndP = LumaAt(posP) - lumaNN * 0.5;
			doneP = abs(lumaEndP) >= gradientScaled;
		}
	}

	float dstN = posM.x - posN.x;
	float dstP = posP.x - posM.x;
	if (!horzSpan) dstN = posM.y - posN.y;
	if (!horzSpan) dstP = posP.y - posM.y;

	bool goodSpanN = (lumaEndN < 0.0) != lumaMLTZero;
	bool goodSpanP = (lumaEndP < 0.0) != lumaMLTZero;
	float spanLength = dstP + dstN;
	float spanLengthRcp = 1.0 / spanLength;

	bool directionN = dstN < dstP;
	float dst = min(dstN, dstP);
	bool goodSpan = directionN ? goodSpanN : goodSpanP;
	float subpixG = subpixF * subpixF;
	float pixelOffset = (dst * (-spanLengthRcp)) + 0.5;
	float subpixH = subpixG * FXAA_SUBPIX;

	float pixelOffsetGood = goodSpan ? pixelOffset : 0.0;
	float pixelOffsetSubpix = max(pixelOffsetGood, subpixH);
	if (!horzSpan) posM.x += pixelOffsetSubpix * lengthSign;
	if ( horzSpan) posM.y += pixelOffsetSubpix * lengthSign;

	gl_FragColor = vec4(texture2D(u_DiffuseMap, posM).rgb, 1.0);
}
