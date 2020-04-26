#ifndef opt_hpp
#define opt_hpp "Program Options"

#include "fmt.hpp"

namespace env::opt
{
	using fmt::string;
	using string::view;
	using view::in;
	using view::out;

	using word = long long;
	using quad = long double;
	using pair = fwd::pair<word>;
	using line = fwd::vector<word>;

	in::ref get(in::ref); // read options in
	out::ref put(out::ref); // write options out

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

	template <class Key, class Type> auto set(Key key, span<Type> value)
	{
		line out;
		for (const Type t : s) out.emplace_back
		return out;
	}

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
