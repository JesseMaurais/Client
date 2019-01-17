#ifndef wcs_hpp
#define wcs_hpp

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
				return it.view != view;
			}

			wchar_t operator*() const
			{
				return wide;
			}

			iterator operator++()
			{
				convert();
				return *this;
			}

		private:

			iterator(fmt::string_view u = "")
			: view(u), size(0), wide(L'\0')
			{
				std::memset(&state, 0, sizeof state);
				convert();
			}

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

	struct narrow
	{
		struct iterator
		{
			friend struct narrow;

			bool operator!=(iterator const& it) const
			{
				return it.view != view;
			}

			char operator*() const
			{
				return bytes[off];
			}

			iterator operator++()
			{
				if (++off >= size) convert();
				return *this;
			}

		private:

			iterator(fmt::wstring_view w = L"")
			: view(w), size(0), bytes(MB_CUR_MAX, '\0')
			{
				std::memset(&state, 0, sizeof state);
				convert();
			}

			void convert()
			{
				off = 0;
				if (0 < size) view = view.substr(1);
				size = std::wcrtomb(bytes.data(), view.front(), &state);
				//std::size_t value;
				//::wcrtomb_s(&value, bytes.data(), bytes.size(), view.front(), &state);
				//size = value;
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
