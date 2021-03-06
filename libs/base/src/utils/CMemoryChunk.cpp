/* +---------------------------------------------------------------------------+
   |                     Mobile Robot Programming Toolkit (MRPT)               |
   |                          http://www.mrpt.org/                             |
   |                                                                           |
   | Copyright (c) 2005-2014, Individual contributors, see AUTHORS file        |
   | See: http://www.mrpt.org/Authors - All rights reserved.                   |
   | Released under BSD License. See details in http://www.mrpt.org/License    |
   +---------------------------------------------------------------------------+ */

#include <mrpt/base.h>  // Precompiled headers


#include <mrpt/utils/CMemoryChunk.h>

using namespace mrpt::utils;


// This must be added to any CSerializable class implementation file.
IMPLEMENTS_SERIALIZABLE(CMemoryChunk, CSerializable, mrpt::utils)

/*---------------------------------------------------------------
						writeToStream
 ---------------------------------------------------------------*/
void  CMemoryChunk::writeToStream(CStream &out, int *out_Version) const
{
	if (out_Version)
		*out_Version = 0;
	else
	{
		out << static_cast<uint64_t>(m_bytesWritten);
		if (m_bytesWritten)
		{
			ASSERT_(m_memory.get())
			out.WriteBuffer(m_memory.get(),m_bytesWritten);
		}
	}

}

/*---------------------------------------------------------------
						readFromStream
 ---------------------------------------------------------------*/
void  CMemoryChunk::readFromStream(CStream &in, int version)
{
	switch(version)
	{
	case 0:
		{
			uint64_t   N;
			in >> N;
			resize(N);

			m_bytesWritten = N;
			m_position     = 0;
			if (N)
				in.ReadBuffer( m_memory.get(), N );

		} break;
	default:
		MRPT_THROW_UNKNOWN_SERIALIZATION_VERSION(version)

	};
}


