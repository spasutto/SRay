#pragma once

#include "../SRay/Common.h"
#include "Utility.h"

#define DSFMT_DO_NOT_USE_OLD_NAMES
#include "./MersenneTwister/dSFMT.h"

using namespace SRay::Utility::MersenneTwister;

namespace SRay
{
	namespace Utility
	{
		class Random
		{
		public:
			static Random& Instance(uint seed = 0)
			{
				return m_instance;
			}

			static double Rand()
			{
				if (++m_instance.cur_nbr >= NBR_RAND)
					m_instance.genRandoms();

				return m_instance.randoms[m_instance.cur_nbr];
			}
		protected:
			void genRandoms(uint seed = 12345)
			{
				int size;
				dsfmt_t dsfmt;

				//debug_log("genRandoms(%d)\n", seed);
				seed = 12345;
				size = dsfmt_get_min_array_size();
				if (size < NBR_RAND)
					size = NBR_RAND;
				if (randoms == NULL)
					randoms = new double[size];// (double*)malloc(sizeof(double)* size);
				if (randoms == NULL)
					throw std::runtime_error("Impossible d'allouer la mémoire pour le générateur de nombres aléatoires");
				dsfmt_init_gen_rand(&dsfmt, seed);
				dsfmt_fill_array_close_open(&dsfmt, randoms, size);

				m_instance.cur_nbr = 0;
			}

		private:
			Random()
			{
				m_instance.genRandoms();
			}
			~Random()
			{
				if (randoms != NULL)
					free(randoms);
			}
			Random& operator= (const Random&){}
			Random(const Random&){}
			static Random m_instance;

			uint cur_nbr = NBR_RAND;
			double *randoms = NULL;// [NBR_RAND];
			dsfmt_t dsfmt;
		};
	}
}