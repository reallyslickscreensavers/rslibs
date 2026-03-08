/*
 * Copyright (C) 2026  Really Slick Screensavers Contributors
 *
 * Catmull-Rom spline interpolation and distance enforcement utilities.
 */

#ifndef CATMULLROM_H
#define CATMULLROM_H

#include <math.h>

// Catmull-Rom spline interpolation between p1 and p2.
// p0 and p3 are neighboring control points for tangent calculation.
// t in [0,1] interpolates between p1 (t=0) and p2 (t=1).
// Values outside [0,1] extrapolate.
inline float catmullRom(float p0, float p1, float p2, float p3, float t){
	return 0.5f * ((2.0f * p1) +
		(-p0 + p2) * t +
		(2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * t * t +
		(-p0 + 3.0f * p1 - 3.0f * p2 + p3) * t * t * t);
}

// Ensure pt is at least minDist away from prev.
// If distance is zero, uses fallback direction (minDist, 0, 0).
// If distance is nonzero but less than minDist, scales offset to minDist.
inline void ensureMinDist(float* pt, const float* prev, float minDist){
	float dx = pt[0] - prev[0];
	float dy = pt[1] - prev[1];
	float dz = pt[2] - prev[2];
	float dist = sqrtf(dx * dx + dy * dy + dz * dz);
	if(dist < minDist){
		if(dist < 1e-8f){
			// Zero offset - use fallback direction
			pt[0] = prev[0] + minDist;
			pt[1] = prev[1];
			pt[2] = prev[2];
		} else {
			// Scale offset to minDist
			float scale = minDist / dist;
			pt[0] = prev[0] + dx * scale;
			pt[1] = prev[1] + dy * scale;
			pt[2] = prev[2] + dz * scale;
		}
	}
}

#endif // CATMULLROM_H
