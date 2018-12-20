#pragma once
#include "vector.hpp"

namespace dmc
{
	template <class Scalar>
	class vertex
	{
	public:
		typedef Scalar scalar_type;
		typedef vector<scalar_type, 3> vector_type;

		vertex(const vector_type& position, scalar_type offset)
			: position_(position)
			, offset_(offset)
			, sign_(offset < static_cast<scalar_type>(0.0))
		{
		}

		const vector_type& position() const
		{
			return position_;
		}

		scalar_type offset() const
		{
			return offset_;
		}

		bool sign() const
		{
			return sign_;
		}

	private:
		vector_type position_;
		scalar_type offset_;
		bool sign_;
	};
}
