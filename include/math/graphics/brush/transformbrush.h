#pragma once
constexpr mat3x3 brushFixTransform = mat3x3::cross(
	mat3x3::translate(cvec2(1.0 / (fp)0x2000)), //translate the point a bit forward so when it rotates it won't access the end of the array (it's like the pixels are in the center)
	mat3x3::scale(cvec2(0xfff / (fp)0x1000)) //scales a bit so the last
);
//transforms a point (after offsetting it a bit for rotation) and then calls a base brush
template<typename brush0Type>
struct transformBrush final : public colorBrush
{
//private:
	cmat3x3 transform;
	const brush0Type& baseBrush;
	transformBrush(const mat3x3& transform, const brush0Type& baseBrush) :
		transform(mat3x3::cross(brushFixTransform, transform)),
		baseBrush(baseBrush) {}

	inline color getValue(cvec2& pos) const final
	{
		return baseBrush.getValue(transform.multPointMatrix(pos));
	}
};