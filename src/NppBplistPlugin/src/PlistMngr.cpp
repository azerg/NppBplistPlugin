#include "PlistMngr.h"
#include <stdexcept>
#include <system_error>


namespace bplist
{
  //
  // If plist_from_bin will fail, xmlBuff_ will remain unchanged, pointing to PREVIOUS-SAVED xml buffer.
  //
  const CharVt& PlistEntry::GetXML(CharVt&& BplistBuff)
  {
    GuardedPlist plist;
    try
    {
      plist_from_bin(BplistBuff.data(), BplistBuff.size(), plist.get_ptr());
    }
    catch (...)
    {
      // catch SEH exceptions (/withSEH)
      throw std::system_error(std::error_code(EFAULT, std::generic_category()), "Invalid bplist file! Cant parse it");
    }

    size_t cbXML{};
    char* pXML_{};
    plist_to_xml( plist, &pXML_, &cbXML );

    if ( cbXML > 0 )
    {
      xmlBuff_.assign( pXML_, pXML_ + cbXML );
      // free mem allocated by plist_to_xml
      free( pXML_ );
    }

    if ( xmlBuff_.empty() )
    {
      contentType_ = ContentType::corrupted;
      throw std::runtime_error( "error converting bplist to xml" );
    }

    contentType_ = ContentType::xml;

    return xmlBuff_;
  }

  const CharVt& PlistEntry::GetBinPlist(CharVt&& xmlBuff)
  {
    GuardedPlist plist;
    plist_from_xml( xmlBuff.data(), xmlBuff.size(), plist.get_ptr() );

    size_t cbBinXML{};
    char* pBinXML{};
    plist_to_bin( plist, &pBinXML, &cbBinXML );

    if ( cbBinXML > 0 )
    {
      rawFileBuff_.assign( pBinXML, pBinXML + cbBinXML );
      free( pBinXML );
    }
    else
    {
      rawFileBuff_.clear();
    }

    if ( rawFileBuff_.empty() )
    {
      contentType_ = ContentType::corrupted;
      throw std::runtime_error( "error converting xml to bplist - bplist will be restored." );
    }

    contentType_ = ContentType::raw;

    return rawFileBuff_;
  }
} // namespace bplist