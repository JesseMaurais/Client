// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "err.hpp"
#include "dir.hpp"
#include "type.hpp"
#include "env.hpp"
#include "usr.hpp"
#include "arg.hpp"
#include "sys.hpp"
#include "dig.hpp"
#include "sync.hpp"
#include <regex>
#include <stack>

#ifdef _WIN32
#include "win/file.hpp"
#else
#include "uni/dirent.hpp"
#include "uni/fcntl.hpp"
#endif

namespace fmt::path
{
	vector split(view u)
	{
		return fmt::split(u, sys::tag::path);
	}

	string join(span p)
	{
		return fmt::join(p, sys::tag::path);
	}

	string join(init n)
	{
		return fmt::join(fwd::to_span(n));
	}
}

namespace fmt::dir
{
	vector split(view u)
	{
		return fmt::split(u, sys::tag::dir);
	}

	string join(span p)
	{
		return fmt::join(p, sys::tag::dir);
	}

	string join(init n)
	{
		return fmt::join(fwd::to_span(n));
	}
}

namespace fmt::file
{
	vector split(view u)
	{
		return fmt::split(u, tag::dot);
	}

	string join(span p)
	{
		return fmt::join(p, tag::dot);
	}

	string join(init n)
	{
		return fmt::join(fwd::to_span(n));
	}

	string join(div p)
	{
		auto part = fmt::span(p.second);
		auto last = fmt::path::join(part);
		p.first.emplace_back(last);
		part = fmt::span(p.first);
		return fmt::dir::join(part);
	}

	div path(view u)
	{
		auto d = fmt::dir::split(u);
		auto n = d.back();
		auto b = fmt::file::split(n);
		d.pop_back();
		return { d, b };
	}
}

namespace env::file
{
	order paths()
	{
		return { env::pwd(), env::path() };
	}

	order config()
	{
		return { env::usr::config_home(), env::usr::config_dirs() };
	}

	order data()
	{
		return { env::usr::data_home(), env::usr::data_dirs() };
	}

	bool find(view path, entry check)
	{
		if (not fmt::terminated(path))
		{
			const auto buf = fmt::to_string(path);
			return find(view(buf), check);
		}
		return fwd::any_of(sys::files(path.data()), check);
	}

	bool find(span paths, entry check)
	{
		return fwd::any_of(paths, [check](auto path)
		{
			return find(path, check);
		});
	}

	bool find(order paths, entry check)
	{
		return find(paths.first, check) or find(paths.second, check);
	}

	entry mask(mode mask)
	{
		return [mask](view u)
		{
			return not env::file::fail(u, mask);
		};
	}

	entry regex(view u)
	{
		const auto s = fmt::to_string(u);
		const auto x = std::regex(s);
		return [x](view u)
		{
			std::cmatch m;
			const auto s = fmt::to_string(u);
			return std::regex_search(s.data(), m, x);
		};
	}

	entry to(string::vector& t)
	{
		return [&](view u)
		{
			auto s = fmt::to_string(u);
			t.emplace_back(std::move(s));
			return success;
		};
	}

	entry to(string& s)
	{
		return [&](view u)
		{
			s = fmt::to_string(u);
			return success;
		};
	}

	entry all(view u, mode m, entry e)
	{
		return mask(m) || regex(u) || e;
	}

	entry any(view u, mode m, entry e)
	{
		return all(u, m, e);
	}

/*
	watch::watch(view path, mode mask)
	{
		string buf;
		if (not fmt::terminated(path))
		{
			buf = fmt::to_string(path);
			path = buf;
		}

	#ifdef _WIN32

		thread_local struct : fwd::no_copy
		{
			FILE_NOTIFY_INFORMATION info[1];
		private:
			CHAR pad[MAX_PATH];
		} local;

		constexpr auto size = sizeof local;
		auto that = std::addressof(local);

		sys::win::handle file = CreateFile
		(
			path.data(),
			FILE_LIST_DIRECTORY,
			FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
			nullptr,
			OPEN_EXISTING,
			FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
			nullptr
		);

		if (sys::win::fail(file))
		{
			sys::win::err(here, "CreateFile");
			return;
		}

		static auto check = [](DWORD dw, DWORD sz, LPOVERLAPPED lp)
		{
			auto err = sys::win::strerr(dw);
			sys::warn(here, err, sz, lp);
		};

		next = [=]
		{
			DWORD dw = 0;
			if (mask & mk) dw |= FILE_NOTIFY_CHANGE_CREATION;
			if (mask & fl) dw |= FILE_NOTIFY_CHANGE_ATTRIBUTES;
			if (mask & mv) dw |= FILE_NOTIFY_CHANGE_FILE_NAME;
			if (mask & rd) dw |= FILE_NOTIFY_CHANGE_LAST_ACCESS;
			if (mask & wr) dw |= FILE_NOTIFY_CHANGE_LAST_WRITE;
			if (ReadDirectoryChangesW(file, that->info, fmt::to<DWORD>(size), true, dw, nullptr, nullptr, check))
			{
				ZeroMemory(that, sizeof(FILE_NOTIFY_INFORMATION));
			}
		};

		value = [=]
		{
			mode m = 0;
			if (that->info->Action & FILE_ACTION_ADDED) m |= mk;
			if (that->info->Action & FILE_ACTION_REMOVED) m |= rm;
			if (that->info->Action & FILE_ACTION_MODIFIED) m |= fl;
			if (that->info->Action & FILE_ACTION_RENAMED_OLD_NAME) m |= mv;
			if (that->info->Action & FILE_ACTION_RENAMED_NEW_NAME) m |= to;
			fmt::wide wide { that->info->FileName, that->info->FileNameLength / sizeof(WCHAR) };
			if (wide.empty()) utf.clear(); else buf = fmt::to_string(wide);
			return { buf, m };
		};

	#elif defined(SYS_INOTIFY)

		thread_local struct : fwd::no_copy
		{
			inotify_event ev[1];
		private:
			char pad[PATH_MAX];
		} local;

		constexpr auto size = sizeof local;
		auto that = std::addressof(local);

		uint32_t in = 0;
		if (mask & mk) in |= IN_CREATE | IN_DELETE;
		if (mask & fl) in |= IN_ATTRIB;
		if (mask & mv) in |= IN_MOVED_FROM | IN_MOVED_TO;
		if (mask & rd) in |= IN_ACCESS;
		if (mask & wr) in |= IN_MODIFY;

		sys::uni::filed wd = inotify_init();
		if (sys::fail(wd))
		{
			sys::err(here, "inotify_init");
			return;
		}

		sys::uni::filed fd = inotify_add_watch(wd, path.data(), mask);
		if (sys::fail(fd))
		{
			sys::err(here, "inotify_add_watch");
			return;
		}

		next = [=]
		{
			if (sys::fail(sys::read(wd, that->ev, size)))
			{
				sys::err(here, "inotify_event");
				std::memset(that->ev, 0, size);
			}
		};

		value = [=]
		{
			mode m = 0;
			if (this->ev->mask & IN_CREATE) m |= mk;
			if (this->ev->mask & IN_DELETE) m |= rm;
			if (this->ev->mask & IN_ATTRIB) m |= fl;
			if (this->ev->mask & IN_ACCESS) m |= rd;
			if (this->ev->mask & IN_MODIFY) m |= wr;
			if (this->ev->mask & IN_MOVED_FROM) m |= mv;
			if (this->ev->mask & IN_MOVED_TO) m |= to;
			fmt::view u { that->ev->name, that->ev->len };
			return { u , m };
		};

	#elif defined(SYS_EVENT)

		thread_local struct : fwd::no_copy
		{
			struct kevent change[1], event[1];
		} local;

		constexpr auto size = sizeof local;
		auto that = std::addressof(local);

		sys::uni::filed kq = kqueue();
		if (sys::fail(kq))
		{
			sys::err(here, "kqueue");
			return;
		}

		sys::uni::filed fd = sys::open(path.data(), convert(mask));
		if (sys::fail(fd))
		{
			sys::err(here, "open");
			return;
		}

		next = [=]
		{
			if (sys::fail(kevent(kq, that->change, 1, that->event, 1, nullptr)))
			{
				sys::err(here, "kevent");
			}
		};

		value = [=]
		{
	 		mode m = 0;
			if (that->event->fflags & NOTE_OPEN) m |= mk;
			if (that->event->fflags & NOTE_DELETE) m |= rm;
			if (that->event->fflags & NOTE_ATTRIB) m |= at;
			if (that->event->fflags & NOTE_READ) m |= rd;
			if (that->event->fflags & NOTE_WRITE) m |= wr;
			if (that->event->fflags & NOTE_RENAME) m |= mv;
			buf.resize(FILENAME_MAX);
			if (sys::fail(fcntl(that->event->ident, F_GETPATH, buf.data())))
			{
				sys::err(here, "F_GETPATH", that->event->ident);
				buf.clear();
			}
			return { buf, m };
		};

		uint32_t m = 0;
		if (mask & mk) m |= NOTE_OPEN;
		if (mask & rm) m |= NOTE_DELETE;
		if (mask & at) m |= NOTE_ATTRIB;
		if (mask & rd) m |= NOTE_READ;
		if (mask & wr) m |= NOTE_WRITE;
		if (mask & mv) m |= NOTE_RENAME;
		EV_SET(that->change, fd, EVFILT_VNODE, EV_ADD | EV_CLEAR, m, 0, this);

	#else
	#error No events
	#endif
	}
*/

	string search(view name, entry check, order roots)
	{
		auto dirs = fmt::dir::split(name);
		auto first = not fwd::equal_to(fmt::tag::dots);
		auto it = fwd::find_if(dirs, first);
		auto diff = std::distance(dirs.begin(), it);
		auto sub =  fmt::span(dirs.data() + diff, dirs.size() - diff);
		// Search predicate
		string buf;
		entry visit = [&](view root)
		{
			auto folders = fmt::dir::split(root);
			if (ptrdiff_t size = folders.size(); size > diff)
			 {
				// Compound path parts
				folders.resize(size - diff);
				auto put = std::back_inserter(folders);
				fwd::copy(sub, put);
				// Compose a full string
				buf = fmt::dir::join(folders);
				return check(buf);
			 }
			return false;
		};
		// Clear if not found
		if (not find(roots, visit))
		{
			buf.clear();
		}
		return buf;
	}

	view mkdir(view path)
	{
		std::stack<view> stack;
		string buf;

		auto folders = fmt::dir::split(path);
		auto stem = path;

		while (env::file::fail(stem))
		{
			stack.push(folders.back());
			folders.pop_back();

			buf = fmt::dir::join(folders);
			stem = buf;
		}

		stem = path.substr(0, path.find(sys::tag::dir, stem.size() + 1));

		while (not stack.empty())
		{
			folders.push_back(stack.top());
			stack.pop();

			buf = fmt::dir::join(folders);
			auto const c = buf.data();
			if (sys::fail(sys::mkdir(c, S_IRWXU)))
			{
				perror("mkdir");
				stem = "";
				break;
			}
		}

		return stem;
	}

	bool rmdir(view dir)
	{
		std::deque<string> deque;
		deque.emplace_back(dir);

		for (auto it = deque.begin(); it != deque.end(); ++it)
		{
			(void) find(*it, [&](view u)
			{
				auto const path = fmt::dir::join({*it, u});
				auto const c = path.data();
				struct sys::stats st(c);
				if (sys::fail(st.ok))
				{
					perror("stat");
				}
				else
				if (S_ISDIR(st.st_mode))
				{
					if (u != "." and u != "..")
					{
						deque.emplace_back(std::move(path));
					}
				}
				else
				if (sys::fail(sys::unlink(c)))
				{
					perror("unlink");
				}
				return success;
			});
		}

		bool ok = success;
		while (not deque.empty())
		{
			dir = deque.back();
			const auto c = dir.data();
			if (sys::fail(sys::rmdir(c)))
			{
				perror("rmdir");
				ok = failure;
			}
			deque.pop_back();
		}
		return ok;
	}
}

#ifdef test_unit
test_unit(dir)
{
	assert(not env::file::fail(env::temp()));
	assert(not env::file::fail(env::pwd()));
	assert(not env::file::fail(env::home()));

	const auto path = fmt::dir::split(__FILE__);
	assert(not path.empty());
	const auto name = path.back();
	assert(not name.empty());
	const auto program = env::opt::program();
	assert(not program.empty());

	assert(env::file::find(env::pwd(), [program](auto entry)
	{
		return fmt::dir::split(entry).back() == program;
	}));

	const auto temp = fmt::dir::join({env::temp(), "my", "test", "dir"});
	if (temp.empty()) return;
	const auto stem = env::file::mkdir(temp);
//	assert(not empty(stem.first));
//	assert(not empty(stem.second));
	assert(not env::file::rmdir(stem));
}
test_unit(dir)
{
	fmt::view dir, name, path = __FILE__;
	auto pos = path.find_last_of(sys::tag::dir);
	if (auto pair = path.split(pos); pair.second.empty())
	{
		dir = ".";
		name = pair.frist;
	}
	else
	{
		dir = pair.first;
		name = pair.second;
	}
}
#endif