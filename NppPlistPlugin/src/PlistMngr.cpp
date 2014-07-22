#include "PlistMngr.h"
#include <stdexcept>


namespace plist
{
  const CharVt& PlistEntry::GetXML()
  {
    if ( xmlBuff_.empty() )
    {
      GuardedPlist plist;
      plist_from_bin( rawFileBuff_.data(), rawFileBuff_.size(), plist.get_ptr() );

      size_t cbXML;
      char* pXML_ = nullptr;
      plist_to_xml( plist, &pXML_, &cbXML );

      if ( cbXML > 0 )
      {
        xmlBuff_.assign( pXML_, pXML_ + cbXML );
        // free mem allocated by plist_to_xml
        free( pXML_ );
      }

      if ( xmlBuff_.empty() )
      {
        throw std::runtime_error( "error converting bplist to xml" );
      }
    }

    return xmlBuff_;
  }

  const CharVt& PlistEntry::GetBinPlist()
  {
    if ( rawFileBuff_.empty() )
    {
      GuardedPlist plist;
      plist_from_xml( xmlBuff_.data(), xmlBuff_.size(), plist.get_ptr() );

      size_t cbBinXML;
      char* pBinXML{};
      plist_to_bin( plist, &pBinXML, &cbBinXML );

      if ( cbBinXML > 0 )
      {
        rawFileBuff_.assign( pBinXML, pBinXML + cbBinXML );
        free( pBinXML );
      }

      if ( rawFileBuff_.empty() )
      {
        throw std::runtime_error( "error converting xml to bplist" );
      }
    }

    return rawFileBuff_;
  }
} // namespace plist