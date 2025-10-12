// Copyright Gradientspace Corp. All Rights Reserved.
#pragma once

#include "GradientspacePlatform.h"
#include "Math/GSVector2.h"
#include "Math/GSAxisBox2.h"

#define GS_API GRADIENTSPACECORE_API

namespace GS
{


template<typename RealType>
struct Circle2
{
	Vector2<RealType> Center;
	RealType Radius;

	Circle2();
	Circle2(const Vector2<RealType>& center, RealType radius);
	Circle2(const Vector2<RealType>& PointA, const Vector2<RealType>& PointB);

	RealType Area() const;
	RealType Circumference() const;
	RealType Curvature() const;
	AxisBox2<RealType> Bounds() const;

	Vector2<RealType> PointAtAngleR(RealType AngleRad) const;
	Vector2<RealType> PointAtAngleD(RealType AngleDeg) const;
	Vector2<RealType> PointAtUnitT(RealType ParameterT) const;

	bool IsPointInside(const Vector2<RealType>& Point) const;

	RealType Distance(const Vector2<RealType>& Point) const;
	RealType DistanceSquared(const Vector2<RealType>& Point) const;
	RealType SignedDistance(const Vector2<RealType>& Point) const;

	//! distance to solid circle
	RealType DiscDistance(const Vector2<RealType>& Point) const;

	// float/double conversion cast
	template<typename RealType2>
	explicit operator Circle2<RealType2>() const {
		return Circle2<RealType2>((Vector2<RealType2>)Center, (RealType2)Radius);
	}
};
typedef Circle2<float> Circle2f;
typedef Circle2<double> Circle2d;



namespace CircleUtil
{
	// Radius of n-sided regular polygon that contains circle of radius r
	inline double BoundingPolyRadius(double CircleRadius, int NumSides) {
		double theta = (GS::Mathd::TwoPi() / (double)NumSides) / 2.0;
		return CircleRadius / GS::Cos(theta);
	}

	GS_API Circle2d FitCircle2(const Vector2d& A, const Vector2d& B, const Vector2d& C, double epsilon = 1e-07);
}




template<typename RealType>
Circle2<RealType>::Circle2()
{
	// is_trivial is false because of constructors
	static_assert(std::is_standard_layout<Circle2<RealType>>());
	static_assert(std::is_trivially_copyable<Circle2<RealType>>());
	static_assert(std::is_floating_point<RealType>());
}


template<typename RealType>
Circle2<RealType>::Circle2(const Vector2<RealType>& center, RealType radius)
	: Center(center), Radius(radius) 
{
}

template<typename RealType>
Circle2<RealType>::Circle2(const Vector2<RealType>& PointA, const Vector2<RealType>& PointB)
{
	Center = (PointA + PointB) * (RealType)0.5;
	Radius = Distance(PointA, PointB) / (RealType)2;
}


template<typename RealType>
RealType Circle2<RealType>::Area() const
{
	return GS::RealMath<RealType>::Pi() * Radius * Radius;
}

template<typename RealType>
RealType Circle2<RealType>::Circumference() const
{
	return GS::RealMath<RealType>::TwoPi() * Radius;
}

template<typename RealType>
RealType Circle2<RealType>::Curvature() const
{
	return (RealType)1 / Radius;
}

template<typename RealType>
AxisBox2<RealType> Circle2<RealType>::Bounds() const
{
	return AxisBox2<RealType>(Center, Radius);
}

template<typename RealType>
Vector2<RealType> Circle2<RealType>::PointAtAngleR(RealType AngleRad) const
{
	return Vector2<RealType>( Radius*GS::Cos(AngleRad) + Center.X, Radius*GS::Sin(AngleRad) + Center.Y );
}

template<typename RealType>
Vector2<RealType> Circle2<RealType>::PointAtAngleD(RealType AngleDeg) const
{
	return PointAtAngleR( GS::RealMath<RealType>::DegToRad() * AngleDeg );
}

template<typename RealType>
Vector2<RealType> Circle2<RealType>::PointAtUnitT(RealType ParamT) const
{
	return PointAtAngleR(GS::RealMath<RealType>::TwoPi() * ParamT);
}


template<typename RealType>
bool Circle2<RealType>::IsPointInside(const Vector2<RealType>& Point) const
{
	return DistanceSquared(Point, Center) < Radius * Radius;
}

template<typename RealType>
RealType Circle2<RealType>::Distance(const Vector2<RealType>& Point) const
{
	return GS::Abs(Distance(Point, Center) - Radius);
}

template<typename RealType>
RealType Circle2<RealType>::DistanceSquared(const Vector2<RealType>& Point) const
{
	RealType d = Distance(); return d * d;
}

template<typename RealType>
RealType Circle2<RealType>::SignedDistance(const Vector2<RealType>& Point) const
{
	return Distance(Point, Center) - Radius;
}

template<typename RealType>
RealType Circle2<RealType>::DiscDistance(const Vector2<RealType>& Point) const
{
	return GS::Max(Distance(Point, Center) - Radius, 0);
}

}

#undef GS_API