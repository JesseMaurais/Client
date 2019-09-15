#ifndef uni_semaphore_hpp
#define uni_semaphore_hpp

#include "uni.hpp"
#include <semaphore.h>

namespace sys::uni
{
	struct semaphore : sem_t
	{
		operator auto()
		{
			return this;
		}

		semaphore(unsigned int value, bool shared = true)
		{
			if (sys::fail(sem_init(this, shared, value)))
			{
				err(here, value);
			}
		}

		~semaphore()
		{
			if (sys::fail(sem_destroy(this)))
			{
				err(here);
			}
		}
	};

	namespace sem
	{
		inline auto open(char const* name, int flags)
		{
			auto const ptr = sem_open(name, flags);
			if (SEM_FAILED == ptr)
			{
				err(here);
			}

			return make_ptr(ptr, [](auto ptr)
			{
				if (SEM_FAILED != ptr)
				{
					if (fail(sem_close(ptr)))
					{
						err(here, ptr);
					}
				}
			});
		}

		inline bool wait(sem_t* sem)
		{
			return fail(sem_wait(sem)) and err(here);
		}

		inline bool post(sem_t* sem)
		{
			return fail(sem_post(sem)) and err(here);
		}

		inline int value(sem_t* sem, int def = invalid)
		{
			if (fail(sem_getvalue(sem, &def)))
			{
				err(here);
			}
			return def;
		}
	}
}

namespace sys
{
	struct semaphore : uni::semaphore
	{
		using base = uni::semaphore;

		semaphore(unsigned most) : base(most)
		{ 
			assert(0 < most);
		}

		auto wait()
		{
			class post
			{
				base* ptr;

			public:

				post(base* that) : ptr(that)
				{
					if (fail(uni::sem::wait(ptr)))
					{
						ptr = nullptr;
					}
				}

				~post()
				{
					if (nullptr != ptr)
					{
						uni::sem::post(ptr);
					}
				}
			};
			return post(this);
		}
	};
}

#endif // file
