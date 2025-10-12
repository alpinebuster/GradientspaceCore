// Copyright Gradientspace Corp. All Rights Reserved.
#include "Containment/ContainCircle2.h"

#include "Math/GSIndex3.h"
#include "Core/dynamic_buffer.h"
#include "Core/gs_iterator.h"

using namespace GS;


// this code is ported from geometry3Sharp ContMinCircle2.cs, which
// is ported from WildMagic4 by David Eberly (https://www.geometrictools.com/).
// Both libraries were released under the Boost Software License.


namespace local
{
	struct Circle {
		Vector2d Center;
		double RadiusSqr;		// actually radius squared...
		Circle() { Center = Vector2d::Zero(); RadiusSqr = GS::Mathd::SafeMaxValue(); }
		Circle(Vector2d c, double radius) { Center = c; RadiusSqr = radius; }

		inline bool Contains(const Vector2d& point, double& distDiff) const {
			Vector2d diff = point - Center;
			double test = diff.SquaredLength();
			distDiff = test - RadiusSqr;
			return distDiff <= 0;
		}
	};

	struct Support
	{
		int Quantity;
		Index3i Index;

		bool Contains(int index, const Vector2d* Points, const int* permutation, double epsilon)
		{
			for (int i = 0; i < Quantity; ++i) {
				Vector2d diff = Points[permutation[index]] - Points[permutation[Index[i]]];
				if (diff.SquaredLength() < epsilon) {
					return true;
				}
			}
			return false;
		}
	};


	static Circle ExactCircle2(const Vector2d& P0, const Vector2d& P1) {
		return Circle(0.5 * (P0 + P1), 0.25 * DistanceSquared(P1, P0));
	}
	static Circle ExactCircle3(const Vector2d& P0, const Vector2d& P1, const Vector2d& P2, double epsilon)
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

			return Circle(P0 + Q, Q.SquaredLength());
		}
		else {
			return Circle(Vector2d::Zero(), GS::Mathd::SafeMaxValue());
		}
	}

	static Circle UpdateSupport1(const Vector2d* Points, int i, const int* permutation, Support& support, double epsilon)
	{
		Vector2d P0 = Points[permutation[support.Index[0]]];
		Vector2d P1 = Points[permutation[i]];
		Circle minimal = ExactCircle2(P0, P1);
		support.Quantity = 2;
		support.Index[1] = i;
		return minimal;
	}

	static const int type2_2[2][2] = { { 0, /*2*/ 1 }, { 1, /*2*/ 0 } };

	static Circle UpdateSupport2(const Vector2d* Points, int i, const int* permutation, Support& support, double epsilon)
	{
		Vector2d point[2] = {
			Points[permutation[support.Index[0]]],  // P0
			Points[permutation[support.Index[1]]]   // P1
		};
		Vector2d P2 = Points[permutation[i]];

		// Permutations of type 2, used for calling ExactCircle2(..).
		int numType2 = 2;

		Circle circle[6];
		int indexCircle = 0;
		double minRSqr = GS::Mathd::SafeMaxValue();
		int indexMinRSqr = -1;
		double distDiff = 0, minDistDiff = GS::Mathd::SafeMaxValue();
		int indexMinDistDiff = -1;

		// Permutations of type 2.
		int j;
		for (j = 0; j < numType2; ++j, ++indexCircle)
		{
			circle[indexCircle] = ExactCircle2(point[type2_2[j][0]], P2);
			if (circle[indexCircle].RadiusSqr < minRSqr)
			{
				if (circle[indexCircle].Contains(point[type2_2[j][1]], distDiff)) {
					minRSqr = circle[indexCircle].RadiusSqr;
					indexMinRSqr = indexCircle;
				}
				else if (distDiff < minDistDiff)
				{
					minDistDiff = distDiff;
					indexMinDistDiff = indexCircle;
				}
			}
		}

		// Permutations of type 3.
		circle[indexCircle] = ExactCircle3(point[0], point[1], P2, epsilon);
		if (circle[indexCircle].RadiusSqr < minRSqr) {
			minRSqr = circle[indexCircle].RadiusSqr;
			indexMinRSqr = indexCircle;
		}

		// Theoreticaly, indexMinRSqr >= 0, but floating-point round-off errors
		// can lead to indexMinRSqr == -1.  When this happens, the minimal sphere
		// is chosen to be the one that has the minimum absolute errors between
		// the sphere and points (barely) outside the sphere.
		if (indexMinRSqr == -1)
		{
			indexMinRSqr = indexMinDistDiff;
		}

		Circle minimal = circle[indexMinRSqr];
		switch (indexMinRSqr)
		{
		case 0:
			support.Index[1] = i;
			break;
		case 1:
			support.Index[0] = i;
			break;
		case 2:
			support.Quantity = 3;
			support.Index[2] = i;
			break;
		}

		return minimal;
	}

	static const int type2_3[3][3] = { { 0, /*3*/ 1, 2}, { 1, /*3*/ 0, 2}, { 2, /*3*/ 0, 1} };
	static const int type3_3[3][3] = { {0, 1, /*3*/ 2}, {0, 2, /*3*/ 1}, {1, 2, /*3*/ 0} };

	Circle UpdateSupport3(const Vector2d* Points, int i, const int* permutation, Support& support, double epsilon)
	{
		Vector2d point[3] = {
			Points[permutation[support.Index[0]]],  // P0
			Points[permutation[support.Index[1]]],  // P1
			Points[permutation[support.Index[2]]]   // P2
		};
		Vector2d P3 = Points[permutation[i]];

		// Permutations of type 2, used for calling ExactCircle2(..).
		int numType2 = 3;

		// Permutations of type 2, used for calling ExactCircle3(..).
		int numType3 = 3;

		Circle circle[6];
		int indexCircle = 0;
		double minRSqr = GS::Mathd::SafeMaxValue();
		int indexMinRSqr = -1;
		double distDiff = 0, minDistDiff = GS::Mathd::SafeMaxValue();
		int indexMinDistDiff = -1;

		// Permutations of type 2.
		int j;
		for (j = 0; j < numType2; ++j, ++indexCircle) {
			circle[indexCircle] = ExactCircle2(point[type2_3[j][0]], P3);
			if (circle[indexCircle].RadiusSqr < minRSqr)
			{
				if (circle[indexCircle].Contains(point[type2_3[j][1]], distDiff)
					&& circle[indexCircle].Contains(point[type2_3[j][2]], distDiff))
				{
					minRSqr = circle[indexCircle].RadiusSqr;
					indexMinRSqr = indexCircle;
				}
				else if (distDiff < minDistDiff)
				{
					minDistDiff = distDiff;
					indexMinDistDiff = indexCircle;
				}
			}
		}

		// Permutations of type 3.
		for (j = 0; j < numType3; ++j, ++indexCircle) {
			circle[indexCircle] = ExactCircle3(point[type3_3[j][0]], point[type3_3[j][1]], P3, epsilon);
			if (circle[indexCircle].RadiusSqr < minRSqr)
			{
				if (circle[indexCircle].Contains(point[type3_3[j][2]], distDiff))
				{
					minRSqr = circle[indexCircle].RadiusSqr;
					indexMinRSqr = indexCircle;
				}
				else if (distDiff < minDistDiff)
				{
					minDistDiff = distDiff;
					indexMinDistDiff = indexCircle;
				}
			}
		}

		if (indexMinRSqr == -1)
		{
			indexMinRSqr = indexMinDistDiff;
		}

		Circle minimal = circle[indexMinRSqr];
		switch (indexMinRSqr)
		{
		case 0:
			support.Quantity = 2;
			support.Index[1] = i;
			break;
		case 1:
			support.Quantity = 2;
			support.Index[0] = i;
			break;
		case 2:
			support.Quantity = 2;
			support.Index[0] = support.Index[2];
			support.Index[1] = i;
			break;
		case 3:
			support.Index[2] = i;
			break;
		case 4:
			support.Index[1] = i;
			break;
		case 5:
			support.Index[0] = i;
			break;
		}

		return minimal;
	}
}





Circle2d GS::MinContainingCircle2(const Vector2d* Points, int NumPoints, double epsilon)
{
	using namespace local;

	if (NumPoints <= 1) {
		return (NumPoints == 0) ? Circle2d() : Circle2d(Points[0], 0.0);
	}

	typedef Circle(*UpdateSupportFunc)(const Vector2d* Points, int i, const int* permutation, Support& support, double epsilon);
	UpdateSupportFunc mUpdate[4] = { nullptr, &UpdateSupport1, &UpdateSupport2, &UpdateSupport3 };

	Circle minimal;

	Support support;
	double distDiff = 0;

	// todo could we get rid of this permutation?
	dynamic_buffer<int> permutation;

	// create permutation that shuffles the point set
	permutation.resize(NumPoints);
	modulo_iteration iter(NumPoints);
	int ii = 0, idx = 0;
	while (iter.next_index(idx))
		permutation[ii++] = idx;

	minimal = local::Circle(Points[permutation[0]], 0);
	support.Quantity = 1;
	support.Index[0] = 0;

	for (int i = 1 % NumPoints, n = 0; i != n; i = (i + 1) % NumPoints) {
		if (!support.Contains(i, Points, permutation.raw_pointer(), epsilon)) {
			if (!minimal.Contains(Points[permutation[i]], distDiff)) {
				auto updateF = mUpdate[support.Quantity];
				Circle circle = updateF(Points, i, permutation.raw_pointer(), support, epsilon);
				if (circle.RadiusSqr > minimal.RadiusSqr) {
					minimal = circle;
					n = i;
				}
			}
		}
	}

	return Circle2d(minimal.Center, GS::Sqrt(minimal.RadiusSqr));

}