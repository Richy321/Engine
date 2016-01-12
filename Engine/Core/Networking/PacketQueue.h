#pragma once
#include <assert.h>
#include <list>

namespace networking
{
	struct PacketData
	{
		unsigned int sequence;	// packet sequence number
		float time;				// time offset since packet was sent or received (depending on context)
		int size;				//packet size in bytes
	};

	inline bool sequence_more_recent(unsigned int s1, unsigned int s2, unsigned int max_sequence)
	{
		return (s1 > s2) && (s1 - s2 <= max_sequence / 2) || (s2 > s1) && (s2 - s1 > max_sequence / 2);
	}

	class PacketQueue : public std::list<PacketData>
	{
	public:
		bool exists(unsigned int sequence)
		{
			for (iterator itor = begin(); itor != end(); ++itor)
			{
				if (itor->sequence == sequence)
					return true;
			}
			return false;
		}

		void insertSorted(const PacketData& p, unsigned int max_sequence)
		{
			if (empty())
			{
				push_back(p);
			}
			else if (!sequence_more_recent(p.sequence, front().sequence, max_sequence))
			{
				push_front(p);
			}
			else if (sequence_more_recent(p.sequence, back().sequence, max_sequence))
			{
				push_back(p);
			}
			else
			{
				for (PacketQueue::iterator itor = begin(); itor != end(); itor++)
				{
					assert(itor->sequence != p.sequence);
					if (sequence_more_recent(itor->sequence, p.sequence, max_sequence))
					{
						insert(itor, p);
						break;
					}
				}
			}
		}

		void verify_sorted(unsigned int max_sequence)
		{
			PacketQueue::iterator prev = end();
			for (PacketQueue::iterator itor = begin(); itor != end(); itor++)
			{
				assert(itor->sequence <= max_sequence);
				if (prev != end())
				{
					assert(sequence_more_recent(itor->sequence, prev->sequence, max_sequence));
					prev = itor;
				}
			}
		}
	};
}

