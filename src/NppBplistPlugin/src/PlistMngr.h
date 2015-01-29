#pragma once

#include <vector>
#include "plist\plist.h"

typedef std::vector<char> CharVt;

namespace bplist
{
  class GuardedPlist;

  enum class ContentType { raw, xml, corrupted };

  class PlistEntry
  {
  public:
    PlistEntry():
      contentType_{ ContentType::corrupted }
    {}

    /*
    // update Entry data with XML text in runtime
    void UpdateXML( CharVt&& xmlBuff )
    {
      xmlBuff_ = xmlBuff;
      rawFileBuff_.clear();

      contentType_ = ContentType::xml;
    }

    // update binary data in runtime ( affected XML too )
    void UpdateBPlist( CharVt&& BplistBuff )
    {
      rawFileBuff_ = BplistBuff;
      xmlBuff_.clear();

      contentType_ = ContentType::raw;
    }
    */

    ContentType GetContentType() const _NOEXCEPT { return contentType_; }

    const CharVt& GetXML( CharVt&& BplistBuff );
    const CharVt& GetBinPlist( CharVt&& xmlBuff );
    const CharVt& GetXML()
    {
      return GetXML( std::move( rawFileBuff_ ) );
    };
    const CharVt& GetBinPlist()
    {
      return GetBinPlist( std::move( xmlBuff_ ) );
    };
  private:
    ContentType contentType_;
    CharVt rawFileBuff_;
    CharVt xmlBuff_;
  };


  class GuardedPlist
  {
  public:
    GuardedPlist():
      plist_{ nullptr }
    {
    }
    ~GuardedPlist()
    {
      plist_free( plist_ );
    }

    plist_t get() const _NOEXCEPT { return plist_; }
    plist_t* get_ptr() _NOEXCEPT { return &plist_; }

    operator plist_t() _NOEXCEPT { return this->get(); }

  private:
    plist_t plist_;
  };

} // namespace bplist

