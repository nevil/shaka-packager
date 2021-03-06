// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_FORMATS_MP2T_ES_PARSER_H264_H_
#define MEDIA_FORMATS_MP2T_ES_PARSER_H264_H_

#include <stdint.h>

#include "packager/base/callback.h"
#include "packager/base/memory/scoped_ptr.h"
#include "packager/media/formats/mp2t/es_parser_h26x.h"

namespace edash_packager {
namespace media {

class H264Parser;

namespace mp2t {

// Remark:
// In this h264 parser, frame splitting is based on AUD nals.
// Mpeg2 TS spec: "2.14 Carriage of Rec. ITU-T H.264 | ISO/IEC 14496-10 video"
// "Each AVC access unit shall contain an access unit delimiter NAL Unit;"
//
class EsParserH264 : public EsParserH26x {
 public:
  EsParserH264(uint32_t pid,
               const NewStreamInfoCB& new_stream_info_cb,
               const EmitSampleCB& emit_sample_cb);
  ~EsParserH264() override;

  // EsParserH26x implementation override.
  void Reset() override;

 private:
  // Processes a NAL unit found in ParseInternal.  The @a pps_id_for_access_unit
  // value will be passed to UpdateVideoDecoderConfig.
  bool ProcessNalu(const Nalu& nalu,
                   bool* is_key_frame,
                   int* pps_id_for_access_unit) override;

  // Update the video decoder config based on an H264 SPS.
  // Return true if successful.
  bool UpdateVideoDecoderConfig(int sps_id) override;

  // Callback to pass the stream configuration.
  NewStreamInfoCB new_stream_info_cb_;

  scoped_refptr<StreamInfo> last_video_decoder_config_;
  bool decoder_config_check_pending_;

  scoped_ptr<H264Parser> h264_parser_;
};

}  // namespace mp2t
}  // namespace media
}  // namespace edash_packager

#endif
