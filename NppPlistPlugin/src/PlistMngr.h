#pragma once

#include <vector>
#include "plist\plist.h"

typedef std::vector<char> CharVt;

namespace plist
{
  class GuardedPlist;

  enum class ContentType { raw, xml };

  class PlistEntry
  {
  public:
    PlistEntry( CharVt&& fileBuff ):
      rawFileBuff_( fileBuff ),
      contentType_{ ContentType::raw }
    {}

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

    const CharVt& GetXML();
    const CharVt& GetBinPlist();
  private:
    ContentType contentType_;
    CharVt rawFileBuff_;
    CharVt xmlBuff_;
  };

  class GuardedPlist
  {
  public:
    GuardedPlist():
      plist_( nullptr )
    {
    }
    ~GuardedPlist()
    {
      plist_free( plist_ );
    }

    plist_t get() const { return plist_; }
    plist_t* get_ptr(){ return &plist_; }

    operator plist_t(){ return this->get(); }

  private:
    plist_t plist_;
  };

} // namespace plist

