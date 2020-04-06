#ifndef opt_hpp
#define opt_hpp "Environment Options"

#include "env.hpp"

namespace env::opt
{
	using fmt::string;
	using string::view;
	using view::span;
	using view::vector;
	using view::pair;
	using string::in;
	using string::out;
	using word = long long;
	using quad = long double;

	extern view const application;
	extern env::span::ref arguments;
	extern env::view::ref program;
	extern env::view::ref config;
	extern env::view::ref cache;
	extern env::view::ref rundir;

	inline auto arg(size_t argn)
	{
		span const argv = arguments;
		return argv[argn];
	}

	struct description
	{
		word argn; // required arguments (or -1 for any number)
		view dash; // short name with one dash
		view name; // long name with dual dash
		view text; // descriptive text for users
	};

	using commands = std::vector<description>;

	void set(int argc, char** argv);
	vector put(commands const& cmd);

	bool got(view key);
	view get(view key);
	bool set(view key, view value);
	bool put(view key, view value);
	bool cut(view key);
	bool got(pair key);
	view get(pair key);
	bool set(pair key, view value);
	bool put(pair key, view value);
	bool cut(pair key);

	bool get(view key, bool value);
	bool get(pair key, bool value);
	bool put(view key, bool value);
	bool put(pair key, bool value);
	word get(view key, word value, int base = 10);
	word get(pair key, word value, int base = 10);
	bool put(view key, word value, int base = 10);
	bool put(pair key, word value, int base = 10);
	quad get(view key, quad value);
	quad get(pair key, quad value);
	bool put(view key, quad value);
	bool put(pair key, quad value);
	vector get(view key, span value);
	vector get(pair key, span value);
	bool put(view key, span value);
	bool put(pair key, span value);

	template <typename T> struct meme
	{
		meme(pair k, T v) : key(k)
		{
			value = get(key, v);
		}

		~meme()
		{
			(void) put(key, value);
		}

		operator T() const
		{
			return value;
		}

		auto operator=(T v)
		{
			return value = v;
		}

	private:

		pair key;
		T value;
	};

	in::ref get(in::ref);
	out::ref put(out::ref);

	string directory(view);
	string initials(view);
};

#endif // file
