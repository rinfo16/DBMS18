#include "storage/loader.h"
#include "common/minicsv.h"
#include "common/config.h"
#include "storage_service.h"
#include <iostream>

namespace storage {

Loader::Loader(const std::string & path, const std::string & rel_name)
    : batch_(NULL),
      csv_(path),
      rel_name_(rel_name),
      is_(path.c_str()),
      tuple_(NULL) {
}

Loader::~Loader() {
  free(tuple_);
  storage::Storage::instance().DeleteIOObject(batch_);
}

bool Loader::Prepare() {
  return true;
}

bool Loader::Load() {
  is_.set_delimiter(',', "$$");
  is_.enable_trim_quote_on_str(true, '\"');
  if (!is_.is_open()) {
    return false;
  }
  MetaDataManagerInterface *meta_data = storage::Storage::instance()
      .GetMetaDataManager();
  Relation *rel = meta_data->GetRelationByName(rel_name_);
  if (rel == NULL) {
    return false;
  }
  TupleDesc desc = rel->ToTupleDesc();
  batch_ = storage::Storage::instance().NewWriteBatch(rel_name_);
  if (batch_ == NULL) {
    return false;
  }

  tuple_ = (Tuple*) malloc(config::Setting::instance().page_size_);

  int i = 0;
  uint32_t slot_length = sizeof(Slot) * desc.column_count_;
  while (is_.read_line()) {
    try {
      uint32_t off = slot_length;
      union DataValue value;
      std::string str;
      for (int i = 0; i < desc.column_count_; i++) {
        Slot *slot = tuple_->GetSlot(i, &desc);
        switch (desc.data_type_[desc.mapping_[i]]) {
          case kDTInteger:
            is_ >> value.integer_;
            slot->offset_ = off;
            tuple_->SetValue(off, &value.integer_, sizeof(value.integer_));
            off += sizeof(value.integer_);
            break;
          case kDTFloat:
            is_ >> value.float_;
            slot->offset_ = off;
            tuple_->SetValue(off, &value.float_, sizeof(value.float_));
            off += sizeof(value.float_);
            break;
          case kDTVarchar:
            is_ >> str;
            slot->offset_ = off;
            tuple_->SetValue(off, str.c_str(), str.size());
            off += str.size();
            break;
          case kDTDate:
            is_ >> str;
            // TODO ...
            slot->offset_ = off;
            tuple_->SetValue(off, &value.date_, sizeof(value.date_));
            off += sizeof(value.date_);
            break;
          default:
            assert(false);
            break;
        }
      }
      TupleWarpper t(tuple_, off);
      batch_->Put(&t);
#if OUTPUT
      std::string tuple_string((const char*)t.Data() + slot_length, t.Size() - slot_length);
      std::cout << tuple_string << std::endl;
#endif
      if (i != 0 && i % 10000 == 0) {
        std::cout << "write " << i << " rows ..." << std::endl;
      }

      i++;
    } catch (std::runtime_error& e) {
      std::cerr << e.what() << std::endl;
      return false;
    }
  }
  return true;
}

}  // namespace storage
