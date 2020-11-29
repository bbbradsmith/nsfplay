// This code has not been tested with multi-track nsf[e] files as I couldn't
// easily find any on the internet.
#include <iostream>
#include <string_view>

#include <sysexits.h>

#include "nlohmann/json.hpp"
#include "../xgm/xgm.h"

using json = nlohmann::json;

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " file.nsf[e]" << std::endl;
        return EX_USAGE;
    }
    std::string_view nsf_path(argv[1]);

    xgm::NSF nsf;

    if(!nsf.LoadFile(nsf_path.data())) {
        std::cerr << "Error loading NSF file '" << nsf_path << "': "
                  << nsf.LoadError() << std::endl;
        return EXIT_FAILURE;
    }

    json nsf_json = json::array();

    for (int track = 0; track < nsf.GetSongNum(); track++) {
      json &track_json = nsf_json[track];
      nsf.SetSong(track);

      if (std::string_view title(nsf.title); !title.empty()) {
          track_json["title"] = title;
      }
      if (std::string_view artist(nsf.artist); !artist.empty()) {
          track_json["artist"] = artist;
      }
      if (std::string_view copyright(nsf.copyright); !copyright.empty()) {
          track_json["copyright"] = copyright;
      }
      if (std::string_view ripper(nsf.ripper); !ripper.empty()) {
          track_json["ripper"] = ripper;
      }
      if (std::string_view text(nsf.text, nsf.text_len); !text.empty()) {
          track_json["text"] = text;
      }
      if (int playtime = nsf.GetPlayTime(); playtime != nsf.default_playtime) {
          track_json["play_time_ms"] = playtime;
      }
      if (int fadetime = nsf.GetFadeTime(); fadetime != nsf.default_fadetime) {
          track_json["fade_ms"] = fadetime;
      }
      if (int loopnum = nsf.GetLoopNum(); loopnum != nsf.default_loopnum - 1) {
          track_json["loopnum"] = loopnum;
      }
      if (int looptime = nsf.GetLoopTime(); looptime != 0) {
          track_json["loop_time_ms"] = looptime;
      }
    }

    std::cout << nsf_json.dump(4) << std::endl;

    return EXIT_SUCCESS;
}
