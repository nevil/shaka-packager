// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef PACKAGER_MEDIA_BASE_TEXT_TRACK_CONFIG_H_
#define PACKAGER_MEDIA_BASE_TEXT_TRACK_CONFIG_H_

#include <string>

namespace edash_packager {
namespace media {

// Specifies the varieties of text tracks.
enum TextKind {
  kTextSubtitles,
  kTextCaptions,
  kTextDescriptions,
  kTextMetadata,
  kTextNone
};

class TextTrackConfig {
 public:
  TextTrackConfig();
  TextTrackConfig(TextKind kind,
                  const std::string& label,
                  const std::string& language,
                  const std::string& id);

  // Returns true if all fields in |config| match this config.
  bool Matches(const TextTrackConfig& config) const;

  TextKind kind() const { return kind_; }
  const std::string& label() const { return label_; }
  const std::string& language() const { return language_; }
  const std::string& id() const { return id_; }

 private:
  TextKind kind_;
  std::string label_;
  std::string language_;
  std::string id_;
};

}  // namespace media
}  // namespace edash_packager

#endif  // PACKAGER_MEDIA_BASE_TEXT_TRACK_H_
