[![Build Status](https://travis-ci.org/google/edash-packager.svg?branch=master)](https://travis-ci.org/google/edash-packager)

Media packaging SDK intended for C++ programmers writing DASH packager applications with common encryption support, Widevine DRM support, Live, and Video-On-Demand.

This document provides the information needed to create a DASH packager that is able to remux and encrypt a video into fragmented ISO BMFF format with common encryption (CENC) support. The DASH packaging API is also designed in such a way for easy extension to more source and destination formats.

Current supported codecs:

|      Codecs       |   ISO-BMFF   |     WebM     |   MPEG2-TS   |     WVM     |
|:-----------------:|:------------:|:------------:|:------------:|:-----------:|
|    H264 (AVC)     |    I / O     |      -       |       I      |      I      |
|    H265 (HEVC)    |    I / O     |      -       |       I      |      -      |
|       VP8         |    I / O     |    I / O     |       -      |      -      |
|       VP9         |    I / O     |    I / O     |       -      |      -      |
|       AAC         |    I / O     |      -       |       I      |      I      |
|  Dolby AC3/EAC3   |    I / O     |      -       |       -      |      -      |
|       DTS         |    I / O     |      -       |       -      |      -      |
|       Opus        |      -       |    I / O     |       -      |      -      |
|      Vorbis       |      -       |    I / O     |       -      |      -      |
** I for input and O for output.
** We will update this table once new formats are supported.

Right now this project is supported directly on Linux and MacOSX platforms only. One option to run edash-packager on other platforms is using [docker] (#Using docker for testing /development).


# Mailing list #

We have a [public mailing list](https://groups.google.com/forum/#!forum/edash-users) for discussion and announcements. To receive notifications about new versions, please join the list. You can also use the list to ask questions or discuss eDash Packager developments.

# Setting up for development #

1. Packager source is managed by Git at https://www.github.com/google/edash-packager. We use gclient tool from Chromium to manage third party libraries. You will need Git (v1.7.5 or above) installed on your machine to access the source code.

2. Install Chromium depot tools which contains gclient and ninja

  See http://www.chromium.org/developers/how-tos/install-depot-tools for details.

3. Get the source

  ```Shell
  mkdir edash_packager
  cd edash_packager
  gclient config https://www.github.com/google/edash-packager.git --name=src
  gclient sync
  ```
  To sync to a particular commit or version, use 'gclient sync -r \<revision\>', e.g.
  ```Shell
  # Sync to commit 4cb5326355e1559d60b46167740e04624d0d2f51
  gclient sync -r 4cb5326355e1559d60b46167740e04624d0d2f51
  # Sync to version 1.2.0
  gclient sync -r v1.2.0
  ```

4. Build

  We use ninja, which is much faster than make, to build our code:
  ```Shell
  cd src
  ninja -C out/{Debug,Release} [Module]
  ```
  Module is optional. If not specified, build all, e.g.
  ```Shell
  ninja -C out/Debug                     # build all modules in Debug mode
  ninja -C out/Release                   # build all modules in Release mode
  ninja -C out/Release mp4               # build mp4 module in Release mode
  ```
  Refer to ninja manual for details.

  We also provide a mechanism to change build configurations, for example, developers can change build system to “make” by overriding *GYP_GENERATORS*.
  ```Shell
  GYP_GENERATORS='make' gclient runhooks
  ```
  Another example, developers can also enable clang by overriding *GYP_DEFINE*.
  ```Shell
  GYP_DEFINES='clang=1' gclient runhooks
  ```

5. Updating the code

  Update your current branch with *git pull* followed by *gclient sync*. Note that if you are not on a branch, *git pull* will not work, and you will need to use *git fetch* instead.

6. Contributing

  See https://github.com/google/edash-packager/blob/master/CONTRIBUTING.md for details.


# Using docker for testing / development #

[Docker](https://www.docker.com/whatisdocker) is a tool that can package an application and its dependencies in a virtual container to run on different host operating systems.

1. Install [Docker](https://docs.docker.com/installation/).

2. Pull prebuilt image from Dockerhub or build an image locally

  2.a. Pull prebuilt image from Dockerhub

    ```Shell
    docker pull google/edash-packager
    ```

  2.b. Build an image locally

    ```Shell
    docker build -t google/edash-packager github.com/google/edash-packager.git
    ```

3. Run the container (`your_media_path` should be your media folder)

  ```Shell
  docker run -v /your_media_path/:/media -it --rm google/edash-packager
  ```

4. Testing

  ```Shell
  # Make sure you run step 3 and you're inside the container.
  cd /media

  # VOD: mp4 --> dash
  packager input=/media/example.mp4,stream=audio,output=audio.mp4 \
           input=/media/example.mp4,stream=video,output=video.mp4 \
           --profile on-demand --mpd_output example.mpd

  # Leave the container.
  exit
  ```
  Outputs are available in your media folder `your_media_path`.


# Design overview #

Major modules are described below:

Demuxer is responsible for extracting elementary stream samples from a multimedia file, e.g. an ISO BMFF file. The demuxed streams can be fed into a muxer to generate multimedia files. An optional KeySource can be provided to Demuxer to decrypt CENC and WVM source content.

Demuxer reads from source through the File interface. A concrete LocalFile class is already implemented. The users may also implement their own File class if they want to read/write using a different kinds of protocol, e.g. network storage, http etc.

Muxer is responsible for taking elementary stream samples and producing media segments. An optional KeySource can be provided to Muxer to generate encrypted outputs. Muxer writes to output using the same File interface as Demuxer.

Demuxer and Muxer are connected using MediaStream. MediaStream wraps the elementary streams and is responsible for the interaction between Demuxer and Muxer. A demuxer can transmits multiple MediaStreams; similarly, A muxer is able to accept and mux multiple MediaStreams, not necessarily from the same Demuxer.

MpdBuilder is responsible for the creation of Media Presentation Description as specified in ISO/IEC 23009-1 DASH MPD spec.

Refer to [Design](docs/design.md), [API](https://google.github.io/edash-packager/docs) for details.


# DASH-IF IOP Compliance #

We try out best to be compliant to [Guidelines for Implementation: DASH-IF Interoperability Points](http://dashif.org/wp-content/uploads/2015/04/DASH-IF-IOP-v3.0.pdf).

We are already compliant to most of the requirements specified by the document, with two exceptions:
- ContentProtection elements are still put under Representation element instead of AdaptationSet element;
- Representations encrypted with different keys are still put under the same AdaptationSet.

We created a flag '--generate_dash_if_iop_compliant_mpd', if enabled,
- ContentProtection elements will be moved under AdaptationSet;
- Representations encrypted with different keys will be put under different AdaptationSets, grouped by `@group` attribute.

Users can enable the flag '--generate_dash_if_iop_compliant_mpd' to have these features. This flag will be enabled by default in a future release.

Please feel free to file a bug or feature request if there are any incompatibilities with DASH-IF IOP or other standards / specifications.


# Driver Program Sample Usage #

Sample driver programs **packager** and **mpd_generator** are written using the SDK.

Some sample usages:

Run the program without arguments will display the help page with the list of command line arguments:
```Shell
packager
```

Dump stream info:
```Shell
packager input=sintel.mp4 --dump_stream_info
```

Demux audio from the input and generate a fragmented mp4:
```Shell
packager input=sintel.mp4,stream=audio,output=fragmented_sintel.mp4
```

Demux streams from the input and generates a mpd with on-demand profile along with fragmented mp4:
```Shell
packager \
  input=sintel.mp4,stream=audio,output=sintel_audio.mp4 \
  input=sintel.mp4,stream=video,output=sintel_video.mp4 \
--profile on-demand \
--mpd_output sintel_vod.mpd
```

Includes a subtitle input from webvtt:
```Shell
packager \
  input=sintel.mp4,stream=audio,output=sintel_audio.mp4 \
  input=sintel.mp4,stream=video,output=sintel_video.mp4 \
  input=sintel_english_input.vtt,stream=text,output=sintel_english.vtt \
--profile on-demand \
--mpd_output sintel_vod.mpd
```


You may also generate mpd with live profile. Here is an example with IPTV input streams:
```Shell
packager \
  'input=udp://224.1.1.5:5003,stream=audio,init_segment=live-audio.mp4,segment_template=live-audio-$Number$.mp4,bandwidth=130000'  \
  'input=udp://224.1.1.5:5003,stream=video,init_segment=live-video-sd.mp4,segment_template=live-video-sd-$Number$.mp4,bandwidth=2000000' \
  'input=udp://224.1.1.5:5002,stream=video,init_segment=live-video-hd.mp4,segment_template=live-video-hd-$Number$.mp4,bandwidth=5000000' \
--profile live \
--mpd_output live.mpd
```

Demux video from the input and generate an encrypted fragmented mp4 using Widevine encryption with RSA signing key file *widevine_test_private.der*:
```Shell
packager input=sintel.mp4,stream=video,output=encrypted_sintel.mp4 \
--enable_widevine_encryption \
--key_server_url "https://license.uat.widevine.com/cenc/getcontentkey/widevine_test" \
--content_id "3031323334353637" \
--signer "widevine_test" \
--rsa_signing_key_path "widevine_test_private.der"
```

The program also supports AES signing. Here is an example with encryption key rotates every 1800 seconds:
```Shell
packager \
  'input=udp://224.1.1.5:5003,stream=audio,init_segment=live-audio.mp4,segment_template=live-audio-$Number$.mp4,bandwidth=130000'  \
  'input=udp://224.1.1.5:5003,stream=video,init_segment=live-video-sd.mp4,segment_template=live-video-sd-$Number$.mp4,bandwidth=2000000' \
  'input=udp://224.1.1.5:5002,stream=video,init_segment=live-video-hd.mp4,segment_template=live-video-hd-$Number$.mp4,bandwidth=5000000' \
--profile live \
--mpd_output live.mpd \
--enable_widevine_encryption \
--key_server_url "https://license.uat.widevine.com/cenc/getcontentkey/widevine_test" \
--content_id "3031323334353637" \
--signer "widevine_test" \
--aes_signing_key "1ae8ccd0e7985cc0b6203a55855a1034afc252980e970ca90e5202689f947ab9" \
--aes_signing_iv "d58ce954203b7c9a9a9d467f59839249" \
--crypto_period_duration 1800
```
Note that key rotation is only supported for live profile.

Demux and decrypt video from a WVM container, and generate encrypted fragmented mp4 using Widevine encryption with RSA signing key file *widevine_test_private.der*:
```Shell
packager input=sintel.wvm,stream=video,output=encrypted_sintel.mp4 \
--enable_widevine_decryption \
--enable_widevine_encryption \
--key_server_url "https://license.uat.widevine.com/cenc/getcontentkey/widevine_test" \
--content_id "3031323334353637" \
--signer "widevine_test" \
--rsa_signing_key_path "widevine_test_private.der"
```

The program can be told to generate MediaInfo files, which can be fed to **mpd_generate** to generate the mpd file.
```Shell
packager \
  input=sintel.mp4,stream=video,output=sintel_video.mp4 \
  input=sintel.mp4,stream=audio,output=sintel_audio.mp4 \
--output_media_info

mpd_generator \
--input "sintel_video.mp4.media_info,sintel_audio.mp4.media_info" \
--output "sintel.mpd"
```
