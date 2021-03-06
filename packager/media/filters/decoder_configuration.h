// Copyright 2016 Google Inc. All rights reserved.
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#ifndef MEDIA_FILTERS_DECODER_CONFIGURATION_H_
#define MEDIA_FILTERS_DECODER_CONFIGURATION_H_

#include <vector>

#include "packager/base/logging.h"
#include "packager/base/macros.h"
#include "packager/media/filters/nalu_reader.h"

namespace edash_packager {
namespace media {

// Defines a base class for decoder configurations.
class DecoderConfiguration {
 public:
  virtual ~DecoderConfiguration();

  /// Parses input to extract decoder configuration data.  This will make and
  /// store a copy of the data for Nalu access.
  /// @return false if there are parsing errors.
  bool Parse(const std::vector<uint8_t>& data) {
    return Parse(data.data(), data.size());
  }

  /// Parses input to extract decoder configuration data.  This will make and
  /// store a copy of the data for Nalu access.
  /// @return false if there are parsing errors.
  bool Parse(const uint8_t* data, size_t data_size);

  /// @return The size of the NAL unit length field.
  uint8_t nalu_length_size() const { return nalu_length_size_; }

  /// @return The number of Nalu in the configuration.
  size_t nalu_count() const { return nalu_.size(); }

  /// @return The nalu at the given index.  The Nalu is only valid for the
  ///         lifetime of this object, even if copied.
  const Nalu& nalu(size_t i) const { return nalu_[i]; }

 protected:
  DecoderConfiguration();

  /// Adds the given Nalu to the configuration.
  void AddNalu(const Nalu& nalu);

  /// @return a pointer to the copy of the data.
  const uint8_t* data() const { return data_.data(); }

  /// @return the size of the copy of the data.
  size_t data_size() const { return data_.size(); }

  /// Sets the size of the NAL unit length field.
  void set_nalu_length_size(uint8_t nalu_length_size) {
    DCHECK(nalu_length_size <= 2 || nalu_length_size == 4);
    nalu_length_size_ = nalu_length_size;
  }

 private:
  // Performs the actual parsing of the data.
  virtual bool ParseInternal() = 0;

  // Contains a copy of the data.  This manages the pointer lifetime so the
  // extracted Nalu can accessed.
  std::vector<uint8_t> data_;
  std::vector<Nalu> nalu_;
  uint8_t nalu_length_size_;

  DISALLOW_COPY_AND_ASSIGN(DecoderConfiguration);
};

}  // namespace media
}  // namespace edash_packager

#endif  // MEDIA_FILTERS_DECODER_CONFIGURATION_H_
