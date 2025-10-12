// Copyright Gradientspace Corp. All Rights Reserved.
#include "Math/GSCircle2.h"

using namespace GS;

Circle2d GS::CircleUtil::FitCircle2(const Vector2d& P0, const Vector2d& P1, const Vector2d& P2, double epsilon)
{
	Vector2d E10 = P1 - P0;
	Vector2d E20 = P2 - P0;

	double m00 = E10.X, m01 = E10.Y, m10 = E20.X, m11 = E20.Y;

	Vector2d B = Vector2d(
		((double)0.5) * E10.SquaredLength(),
		((double)0.5) * E20.SquaredLength());

	double det = m00 * m11 - m01 * m10;

	if (GS::Abs(det) > epsilon) {
		double invDet = ((double)1) / det;
		Vector2d Q;
		Q.X = (m11 * B.X - m01 * B.Y) * invDet;
		Q.Y = (m00 * B.Y - m10 * B.X) * invDet;

		return Circle2d(P0 + Q, Q.SquaredLength());
	}
	else {
		return Circle2d(Vector2d::Zero(), GS::Mathd::SafeMaxValue());
	}
}