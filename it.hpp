#ifndef it_hpp
#define it_hpp

#include <locale>
#include <cwchar>
#include <cstring>
#include "str.hpp"

namespace fmt
{
	struct widen
	{
		struct iterator
		{
			iterator(fmt::string_view u = "")
			: view(u), size(0), wide(L'\0')
			{
				std::memset(&state, 0, sizeof state);
				convert();
			}

			iterator operator++()
			{
				convert();
				return *this;
			}

			wchar_t operator*() const
			{
				return wide;
			}

			bool operator==(iterator const& it) const
			{
				return it.view.empty() ? view.empty() : it.view == view and it.wide == wide;
			}

			bool operator!=(iterator const& it) const
			{
				return not this->operator==(it);
			}

		private:

			void convert()
			{
				if (0 < size) view = view.substr(size);
				size = std::mbrtowc(&wide, view.data(), view.size(), &state); 
			}

			fmt::string_view view;
			std::mbstate_t state;
			std::ptrdiff_t size;
			wchar_t wide;
		};

		widen(fmt::string_view u)
		: view(u)
		{ }

		iterator begin() const
		{
			return iterator(view);
		}

		iterator end() const
		{
			return iterator();
		}

	private:

		fmt::string_view view;
	};
}

#endif // file
