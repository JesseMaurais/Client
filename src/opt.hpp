#ifndef opt_hpp
#define opt_hpp

#include "env.hpp"
#include <iosfwd>
#include <vector>

namespace env::opt
{
	using fmt::string;
	using view = fmt::string_view;
	using span = fmt::string_view_span;
	using list = fmt::string_view_vector;
	using pair = fmt::string_view_pair;
	using word = long long;
	using quad = long double;

	extern view const application;
	extern env::span const& arguments;
	extern env::view const& program;
	extern env::view const& config;
	extern env::view const& cache;
	extern env::view const& run;
	view directory(view stem);

	struct description
	{
		word argn; // required arguments (or -1 for any number)
		view nick; // short name with one dash
		view name; // long name with dual dash
		view text; // descriptive text for users
	};

	using commands = std::vector<description>;
	void set(int argc, char** argv);
	list parse(commands const& cmd);

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
	span get(view key, span value);
	span get(pair key, span value);
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

	std::istream & get(std::istream &);
	std::ostream & put(std::ostream &);
};

#endif // file
