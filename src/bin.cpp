#include "bin.hpp"
#include "err.hpp"

#ifdef test

namespace
{
	struct data
	{
		int i = 1;
		float f = 2.0f;
		char c = 'x';
		short s = 3;
		long l = 4L;
		double d = 5.0;
		long long ll = 6LL;
		long double ld = 7.0;
		wchar_t w = L'X';
		wint_t u = EOF;
	};
}

namespace fmt
{
	template <> auto make_series<data>()
	{
		return std::make_tuple
		(
			&data::c, 
			&data::d, 
			&data::f,
			&data::i,
			&data::l,
			&data::ll,
			&data::ld,
			&data::u,
			&data::w
		);
	}
}

test(bin)
{
	//std::stringstream ss;
	data raw;
	sys::out << fmt::write(raw);
}

#endif
