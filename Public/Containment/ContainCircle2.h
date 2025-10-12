// Copyright Gradientspace Corp. All Rights Reserved.
#pragma once

#include "GradientspacePlatform.h"
#include "Math/GSCircle2.h"

namespace GS
{

/**
 * Find the circle that contains the provided point set.
 */
GRADIENTSPACECORE_API
Circle2d MinContainingCircle2( const Vector2d* Points, int NumPoints, double epsilon = 1e-07 );

}