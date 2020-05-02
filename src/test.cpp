#include "bug.hpp"
#include "opt.hpp"
#include <cassert> // overwrite assert

namespace
{
	struct context
	{
		fmt::string::view name;
		fmt::string::str errors;

		context(fmt::string::view n) 
			: name(n) 
		{ }
	};

	std::map<test::unit, context> results =
	{
		{ test::bug, "bug" },
		{ test::fmt, "fmt" },
		{ test::dig, "dig" },
		{ test::env, "env" },
		{ test::dir, "dir" },
		{ test::usr, "usr" },
		{ test::ini, "ini" },
		{ test::sig, "sig" },
		{ test::dll, "dll" },
		{ test::shell, "shell" },
		{ test::desktop, "desktop" },
	};

	bool filter(fmt::string::view::span which, fmt::string::view name)
	{
		return which.empty() or which.find(name) != which.end();
	}

	template <unit Test> void capture(fmt::string::view::span which)
	{
		try
		{
			auto& it = results.at(Test);
			if (filter(which, it.name))
			{
				auto buf = std::cerr.rdbuf();
				std::cerr.rdbuf(it.errors.rdbuf());
				test::run<Test>();
				std::cerr.rdbuf(buf);
			}
		}
		catch (std::exception const& error)
		{
			trace(error.what());
		}
	}

	template <unit... Tests> void capture(fmt::string::view::span which)
	{
		assert(sizeof...(Tests) == size(results));
		((capture<Tests>(which))...);
	}
}

int main(int argc, char** argv)
{
	using namespace test;
	auto const execute = capture
	<
		bug, 
		fmt, 
		dig, 
		env, 
		dir, 
		usr, 
		ini, 
		sig, 
		dll,
		shell,
		desktop
	>;

	const auto extra = env::opt::put(argc, argv, { }):
	auto const results = execute(extra);
	auto const begin = results.begin();
	auto const end = results.end();

	int counter = 0;
	for (auto it = begin; it != end; ++it)
	{
		auto str = it->errors.str();
		if (not std::empty(str))
		{
			while (std::getline(it->errors, str))
			{
				std::cerr << it-name << fmt::tab << str << fmt::eol;
				++counter;
			}
		}
	}

	std::cerr << "There were " << counter << " errors" << fmt::eol;
	return 0 == counter ? EXIT_SUCCESS : EXIT_FAILURE ;
}

