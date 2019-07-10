#ifndef it_hpp
#define it_hpp

#include <cwchar>
#include <cstring>
#include "str.hpp"

namespace fmt
{
	struct widen
	{
		struct iterator
		{
			friend struct widen;

			bool operator!=(iterator const& it) const
			{
				return not same(it.view, view);
			}

			wchar_t operator*() const
			{
				return wide;
			}

			iterator operator++()
			{
				iterate();
				return *this;
			}

		private:

			iterator(fmt::string_view u = "")
			: view(u), size(0), wide(L'\0')
			{
				std::memset(&state, 0, sizeof state);
				iterate();
			}

			void iterate()
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

	struct narrow
	{
		struct iterator
		{
			friend struct narrow;

			bool operator!=(iterator const& it) const
			{
				return not same(it.view, view);
			}

			char operator*() const
			{
				return bytes[off];
			}

			iterator operator++()
			{
				if (++off >= size) iterate();
				return *this;
			}

		private:

			iterator(fmt::wstring_view w = L"")
			: view(w), size(0), bytes(MB_CUR_MAX, '\0')
			{
				std::memset(&state, 0, sizeof state);
				iterate();
			}

			void iterate()
			{
				off = 0;
				if (0 < size) view = view.substr(1);
				size = std::wcrtomb(bytes.data(), view.front(), &state);
			}

			fmt::wstring_view view;
			std::mbstate_t state;
			std::ptrdiff_t size;
			std::ptrdiff_t off;
			std::string bytes;
		};

		narrow(fmt::wstring_view w)
		: view(w)
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

		fmt::wstring_view view;
	};
}

#endif // file
