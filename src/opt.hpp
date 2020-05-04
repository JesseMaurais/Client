#ifndef opt_hpp
#define opt_hpp "Program Options"

#include "fwd.hpp"

namespace env::opt
{
	using word = long long;
	using quad = long double;
	using pair = fwd::pair<word>;
	using vector = fwd::vector<word>;

	// Boolean
	bool get(word, bool);
	bool set(word, bool);
	bool get(pair, bool);
	bool set(pair, bool);

	// Integer
	word get(word, word, int base = 10);
	bool set(word, word, int base = 10);
	word get(pair, word, int base = 10);
	bool set(pair, word, int base = 10);

	// Pointed Decimal
	quad get(word, quad);
	bool set(word, quad, int digits = 6);
	quad get(pair, quad);
	bool set(pair, quad, int digits = 6);

	template <class Type> class meme
	// Mnemonic device for options
	{
		pair key;
		Type value;

	public:

		meme(pair k, Type v) : key(k)
		{
			value = get(k, v);
		}

		~meme()
		{
			put(key, value);
		}

		operator Type() const
		{
			return value;
		}

		auto operator=(Type v)
		{
			return value = v;
		}
	};
};

#endif // file
