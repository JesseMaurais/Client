#ifndef sax_hpp
#define sax_hpp

#include "del.hpp"

namespace xml
{
	template <class Char> class basic_sax : public fmt::basic_sequence<Char>
	{
		using char_type = Char;
		using span_view = fmt::basic_span_view<char_type>;
		using sequence = fmt::basic_sequence<char_type>;
		using iterator = typename sequence::iterator;

		template <class Char>
		friend bool is_element(iterator const& it);

		static constexpr char_type quot [] = { '"', '\0' };
		static constexpr char_type amp  [] = { '&', '\0' };
		static constexpr char_type lt   [] = { '<', '\0' };
		static constexpr char_type gt   [] = { '>', '\0' };

	public:

		basic_sax(string_view view)
		: basic_sequence(view, { lt, gt })
		{ }
	};

	template <class Char>
	inline bool is_element(basic_sax<Char>::iterator const& it)
	{
		return fmt::same(it, basic_sax<Char>::gt);
	}

	template <class Char>
	inline bool is_closing(basic_sax<Char>::iterator const& it)
	{
		return it.starts_with("/");
	}

	template <class Char>
	inline bool is_self_closing(basic_sax<Char>::iterator const& it)
	{
		return it.ends_with("/");
	}

	template <class Char>
	inline bool is_comment(basic_sax<Char>::iterator const& it)
	{
		return it.starts_with("!--") and it.ends_with("--");
	}

	template <class Char>
	inline bool is_prolog(basic_sax<Char>::iterator const& it)
	{
		return it.starts_with("?xml ") and it.ends_with("?");
	}

	using sax = basic_sax<char>;
	using wsax = basic_sax<wchar_t>;
}

#endif // file
